#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <fstream>
#include <deque>
#include <numeric>
#include <execution>

#include "clicker.hh"
#include "bioscience/bioparse.hpp"
#include "mpi/mpifunc.hpp"
#include "mpi/send_receive_logic.hpp"


namespace bio = bioscience;

int main(int argc, char *argv[])
{
    int myrank, numprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);



    try 
    {   
        std::cout << "[info " << myrank << "] reading file to RAM\n";
        mpiio::MpiIO factory(argv[1], argv[2]);
        factory.map_block();
        std::cout << "[info " << myrank << "] repairing blocks\n";
        factory.repair_reads();

        std::cout << "[info " << myrank << "] spread header\n";
        factory.header_spread();

        std::cout << "[info " << myrank << "] parsing block to vector\n";
        bio::io::Parser extr(factory.hdr_str_, factory.buf_str_);
        std::vector< bio::bio_read > reads_vec{};
        std::vector< bio::bam_meta > reads_vec_meta; 
        //                    ^^^^ vector of reads' meta infromation (original node, index in original node, key of read for sorting)

        while (extr.read() >= 0) {
            reads_vec.push_back(extr.get().release());
            reads_vec_meta.push_back(bio::bam_meta(myrank, reads_vec.size() - 1, *(reads_vec.back().record_)));
        }
        
        factory.buf_str_.clear();

        MPI_Status stat;
        MPI_Request req;

        //meta vectors sorting process
        for (int j = 0; j < numprocs; ++j)
        {
            for (int proc = j % 2; proc < numprocs; proc += 2)
            {
                if (proc + 1 == numprocs)
                {
                    continue;
                }

                if (myrank == proc + 1)
                {
                    unsigned long long package_size = reads_vec_meta.size();
                    MPI_Isend(&package_size, 1, MPI_UNSIGNED_LONG_LONG, proc, 0, MPI_COMM_WORLD, &req);
                    MPI_Wait(&req, &stat);

                
                    MPI_Isend(reads_vec_meta.data(), sizeof(bio::bam_meta) * reads_vec_meta.size(), MPI_BYTE, proc, 0, MPI_COMM_WORLD, &req);
                    MPI_Wait(&req, &stat);

                    MPI_Irecv(reads_vec_meta.data(), sizeof(bio::bam_meta) * reads_vec_meta.size(), MPI_BYTE, proc, 0, MPI_COMM_WORLD, &req);
                    MPI_Wait(&req, &stat);
                }
                
                if (myrank == proc)
                {
                    unsigned long long package_size = 0;
                    MPI_Irecv(&package_size, 1, MPI_UNSIGNED_LONG_LONG, proc + 1, 0, MPI_COMM_WORLD, &req);
                    MPI_Wait(&req, &stat);

                    unsigned int original_size = reads_vec_meta.size();
                    reads_vec_meta.resize(original_size + package_size);
                    MPI_Irecv(&reads_vec_meta[original_size], sizeof(bio::bam_meta) * package_size, MPI_BYTE, proc + 1, 0, MPI_COMM_WORLD, &req);
                    MPI_Wait(&req, &stat);

                    std::stable_sort(std::execution::par_unseq, reads_vec_meta.begin(), reads_vec_meta.end());

                    MPI_Isend(&reads_vec_meta[original_size], sizeof(bio::bam_meta) * package_size, MPI_BYTE, proc + 1, 0, MPI_COMM_WORLD, &req);
                    MPI_Wait(&req, &stat);
                    reads_vec_meta.resize(original_size);
                }
            }
        }

        std::cout << "[debug " << myrank << "] all meta sorted\n";

        std::vector< bam1_t > output_vector;
        output_vector.resize(reads_vec_meta.size());

        //reads' collection process
        //higher rank always sends, then receives and vice versa
        for (int j = 0; j < numprocs; ++j)
        {
            if (j > myrank)
            {
                auto positions = retreive_positions(reads_vec_meta, j);
                get_from_node(j, positions.first, positions.second, output_vector);

                send_to_node(j, reads_vec);
            }
            else if (j < myrank)
            {
                send_to_node(j, reads_vec);

                auto positions = retreive_positions(reads_vec_meta, j);
                get_from_node(j, positions.first, positions.second, output_vector);
            }
            
            
        }

        //fill output with suitable reads in this rank
        auto own_positions = retreive_positions(reads_vec_meta, myrank);
        for (int i = 0; i < own_positions.first.size(); ++i)
        {
            output_vector[own_positions.second[i]] = *(reads_vec[own_positions.first[i]].record_);
            output_vector[own_positions.second[i]].data = new uint8_t[output_vector[own_positions.second[i]].l_data];
            std::memcpy(output_vector[own_positions.second[i]].data,
                        reads_vec[own_positions.first[i]].record_->data,
                        output_vector[own_positions.second[i]].l_data);
        }

        //clear unsued input vector
        reads_vec.clear();

        std::cout << "[info] SORTING FINISHED " << myrank << '\n';

        std::clog << "[info] format vector to block\n";
        auto main_hdr = extr.header();
        if (myrank == 0) {
            factory.buf_str_.append(main_hdr.get() -> text, main_hdr.get() -> l_text);
        }

        bio::io::Formater form(main_hdr);
        for (auto &i : output_vector)
        {
            form.format(&i);
            factory.buf_str_.append(form.get());
            delete [] i.data;
        }
    
        std::clog << "[info] writing block to HDD\n";
        factory.unmap_block();
    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << "\n";
    }
    
    MPI_Finalize();
    return 0;
}

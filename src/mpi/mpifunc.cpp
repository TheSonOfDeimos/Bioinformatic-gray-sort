#include "mpifunc.hpp"
#include "mpi.h"
namespace mpiio
{
    MpiIO::MpiIO(char* fp_in, char* fp_out):
    file_path_in_(fp_in),
    file_path_out_(fp_out),
    buf_str_()
    {
        int errFlag = 0;
        if (MPI_Initialized(&errFlag)) {
            throw std::logic_error("MPI wasn't intilized");
        }
        
        MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank_);
        MPI_Comm_size(MPI_COMM_WORLD, &num_procs_);
    }

    void MpiIO::repair_reads()
    {

        if (proc_rank_ != num_procs_ - 1)
        {
            auto riter = std::find(buf_str_.rbegin(), buf_str_.rend(), '\n');

            decltype(riter.base()) iter;
            if (riter == buf_str_.rend()) {
                iter = (riter.base()) - 1;
            }
            else {
                iter = riter.base();
            }

            std::string block_to_send(iter, buf_str_.end());

            buf_str_.erase(iter, buf_str_.end());

            MPI_Status stat;
            MPI_Request req;
            MPI_Isend(block_to_send.data(), block_to_send.size(), MPI_CHAR, proc_rank_ + 1, 0, MPI_COMM_WORLD, &req);
            MPI_Request_free(&req);
        }

        if (proc_rank_ != 0)
        {
            MPI_Status stat;
            int bufsize = 0;
            MPI_Probe(proc_rank_ - 1, 0, MPI_COMM_WORLD, &stat);
            MPI_Get_count(&stat, MPI_CHAR, &bufsize);

            assert(bufsize >= 0);

            char *buf1 = new char[bufsize];
            MPI_Recv(buf1, bufsize, MPI_CHAR, proc_rank_ - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            buf_str_.insert(0, buf1, bufsize);
            delete[] buf1;
        }
    }

    void MpiIO::header_spread()
    {
        if (proc_rank_ == 0)
        {
            auto hdr = bioscience::io::Parser::read_hdr(buf_str_);
            assert(hdr != NULL);

            MPI_Request req;
            for (int i = 1; i < num_procs_; i++)
            {
                MPI_Isend(hdr->text, hdr->l_text, MPI_CHAR, i, 0, MPI_COMM_WORLD, &req);
                MPI_Request_free(&req);
            }
            hdr_str_.insert(0, hdr -> text, hdr -> l_text);
            buf_str_.erase(0, (hdr -> l_text));
            buf_str_.shrink_to_fit();

        }
        else
        {
            MPI_Status stat;
            int bufsize = 0;
            MPI_Probe(0, 0, MPI_COMM_WORLD, &stat);
            MPI_Get_count(&stat, MPI_CHAR, &bufsize);
            hdr_str_.resize(bufsize);
            MPI_Recv(const_cast< char* >(hdr_str_.data()), bufsize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    void MpiIO::map_block()
    {
        MPI_File thefile;
        MPI_Offset filesize;
        MPI_Status status;

        MPI_File_open(MPI_COMM_WORLD, file_path_in_, MPI_MODE_RDONLY, MPI_INFO_NULL, &thefile);
        MPI_File_get_size(thefile, &filesize);

        assert(filesize > 0);
        filesize = filesize / sizeof(char);

        MPI_Count bufsize = filesize / num_procs_;

        assert(bufsize > 0);
        std::clog << "[info] trying to allocate " << bufsize << " chars\n";
        MPI_File_set_view(thefile, proc_rank_ * bufsize * sizeof(char), MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);

        if (proc_rank_ == num_procs_ - 1) {
            bufsize = filesize - (bufsize * (num_procs_ - 1));
        }
        buf_str_.resize(bufsize);
        std::cout << "[info] allocated " << buf_str_.capacity() << "\n";
        assert(buf_str_.capacity() >= bufsize);
        std::cout << "[info] reading " << buf_str_.capacity() << "\n";

       
        MPI_File_read(thefile, const_cast< char* >(buf_str_.data()), bufsize, MPI_CHAR, &status);
        std::cout << "[debug] reading status " << status.MPI_ERROR << "\n";

        std::cout << "[info] reding finished " << buf_str_.capacity() << "\n";
        MPI_File_close(&thefile);
        assert(buf_str_.size() == bufsize);  
    }

    void MpiIO::unmap_block()
    {
        for (int i = proc_rank_ + 1; i < num_procs_; i++)
        {
            int size = buf_str_.size();
            MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        uint64_t offset = 0;
        for (int i = 0; i < proc_rank_; i++)
        {
            int size = 0;
            MPI_Recv(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += size;
        }
        std::cout << "OFFSET:" << offset << "\n";
        assert(offset >= 0);
        MPI_File thefile;
        MPI_File_open(MPI_COMM_WORLD, file_path_out_, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &thefile);
        MPI_File_set_view(thefile, offset * sizeof(char), MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL);
        MPI_File_write(thefile, buf_str_.data(), buf_str_.size(), MPI_CHAR, MPI_STATUS_IGNORE);
        MPI_File_close(&thefile);
    }

    std::string* MpiIO::get_buff_ptr()
    {
        return &buf_str_;
    }
}
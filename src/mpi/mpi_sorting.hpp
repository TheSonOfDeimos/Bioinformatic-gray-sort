#ifndef MPI_SORTING_HPP
#define MPI_SORTING_HPP

#include <cmath>
#include "mpi.h"
#include "bioscience/bio.hh"

template < typename T >
void mpi_proc_send_pkg(const int proc_dest, T& dataStruct)
{
    int num_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    if (proc_dest < 0) {
        return;
    }
    MPI_Status stat;
    MPI_Request req;
    MPI_Isend(dataStruct.data(), sizeof(decltype(dataStruct.back())) * dataStruct.size(), MPI_BYTE, proc_dest, 0, MPI_COMM_WORLD, &req);
    MPI_Request_free(&req);
    MPI_Recv(dataStruct.data(), sizeof(decltype(dataStruct.back())) * dataStruct.size(), MPI_BYTE, proc_dest, 0, MPI_COMM_WORLD, &stat);
}

template < typename T >
void mpi_proc_recive_pkg(const int proc_sourse, T& dataStruct)
{
    int num_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    if (proc_sourse  > num_procs - 1) {
        return;
    }

    MPI_Status stat;
    MPI_Request req;
    int datasize = 0;
    MPI_Probe(proc_sourse, 0, MPI_COMM_WORLD, &stat);
    MPI_Get_count(&stat, MPI_BYTE, &datasize);
    
    assert(datasize >= 0);

    unsigned int original_size = dataStruct.size();
    dataStruct.resize(original_size + (datasize / sizeof(decltype(dataStruct.back()))));
    MPI_Recv(&dataStruct[original_size], datasize, MPI_BYTE, proc_sourse, 0, MPI_COMM_WORLD, &stat);

    std::stable_sort(dataStruct.begin(), dataStruct.end());

    MPI_Isend(&dataStruct[original_size], datasize, MPI_BYTE, proc_sourse, 0, MPI_COMM_WORLD, &req);
    MPI_Request_free(&req);
    dataStruct.resize(original_size);
}

template < typename T >
void mpi_sort(T& data)
{
    int proc_rank;
    int num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    bool phase_indicator = 0;
    for (uint i = 0; i < num_procs; i++)
    {
        if (proc_rank == 0) {
            phase_indicator = !phase_indicator;  
        }
        MPI_Bcast(&phase_indicator, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        if (phase_indicator == 0) {
            if (proc_rank % 2 == 0) {
                mpi_proc_send_pkg(proc_rank - 1, data);
            }
            else {
                mpi_proc_recive_pkg(proc_rank + 1, data);
            }
        }
        else if (phase_indicator == 1) {
            if (proc_rank % 2 == 1) {
                mpi_proc_send_pkg(proc_rank - 1, data);
            }
            else {
                mpi_proc_recive_pkg(proc_rank + 1, data);
            }
        }
    }
}

#endif
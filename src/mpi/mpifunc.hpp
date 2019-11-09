#ifndef MPIFUNC_HPP
#define MPIFUNC_HPP

#include "mpi.h"
#include <string>
#include <algorithm>
#include <cstring>
#include <cassert>
#include <memory>
#include <exception>
#include <string_view>

#include "../bioscience/bioparse.hpp"

namespace mpiio 
{
    class MpiIO
    {
    public:
        MpiIO(char* fp_in, char* fp_out);
        void repair_reads();
        void header_spread();  
        void map_block();
        void unmap_block();
        std::string buf_str_;
        std::string hdr_str_;
        std::string* get_buff_ptr();

    private:
        char* file_path_in_;
        char* file_path_out_;
        int proc_rank_;
        int num_procs_;

        
    };
}
#endif

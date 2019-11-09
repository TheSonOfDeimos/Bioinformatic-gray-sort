#ifndef BIOPARSE_HPP
#define BIOPARSE_HPP

#include <string>
#include <iostream>
#include <algorithm>
#include <cstring>
#include "htslib/kstring.h"

#include "bio.hh"

namespace bioscience
{
    namespace io
        {
            class Parser
            {
            public:
                typedef std::unique_ptr<header_t, cleaner> header_unique_ptr_t;
                typedef std::unique_ptr<record_t, cleaner> record_unique_ptr_t;

                Parser(std::string hdr, std::string& block);

                static header_t* read_hdr(std::string &block);
                int read();
                header_unique_ptr_t header();
                record_unique_ptr_t get();

            private:
                header_unique_ptr_t bio_header_;
                record_unique_ptr_t aln_;
                std::string& block_;
                std::string::iterator iter_;
                int status_;
            };

            class Formater
            {
            public: 
                typedef std::unique_ptr<header_t, cleaner> header_unique_ptr_t;
            
                Formater(header_unique_ptr_t& hdr_ptr);
                int format(const record_t* aln);
                std::string get();
        
            private:
                header_unique_ptr_t bio_header_;
                std::string record_str_;
            };
        }
}

#endif
#include "bioparse.hpp"

namespace bioscience
{
    namespace io
    {
        Parser::Parser(std::string hdr, std::string &block): 
        aln_(nullptr),
        block_(block),
        status_(0)
        {
            bio_header_ = header_unique_ptr_t(read_hdr(hdr));
            iter_ = block_.begin();
        }

        header_t* Parser::read_hdr(std::string &block)
        {
            std::string str{};
            bam_hdr_t *h = NULL;
            int has_SQ = 0;

            auto iter = block.begin();
            auto next = std::find(iter, block.end(), '\n');

            while (*iter == '@')
            {
                std::string line(iter, next);
                if (line.size() > 3 && strncmp(line.data(), "@SQ", 3) == 0)
                {
                    has_SQ = 1;
                }
                str.append(line + '\n');
                iter = ++next;
                next = std::find(iter, block.end(), '\n');
            }
            //TODO: logic block if has_SQ == 0
            h = sam_hdr_parse(str.size(), str.data());
            h->l_text = str.size();
            h->text = (char *)malloc(sizeof(char) * h->l_text);
            std::memcpy(h->text, str.data(), str.size());
            return h; //here might be added sam_hdr_sanitise(h); for extra header checking
        }

        int Parser::read()
        {
            if (iter_ >= block_.end())
            {
                return -1;
            }
            auto next = std::find(iter_, block_.end(), '\n');
            std::string read_str(iter_, next);
            kstring_t s = {read_str.size(), read_str.size(), const_cast< char* >(read_str.data())};
            aln_ = record_unique_ptr_t(bam_init1());
            status_ = sam_parse1(&s, bio_header_.get(), aln_.get());
            iter_ = ++next;
            return 0;
        }

        Parser::header_unique_ptr_t Parser::header()
        {
            // add @HD flag
            sam_hdr_change_HD(bio_header_.get(), "SO", "coordinate" ); 
            return std::move(bio_header_);
        }

        Parser::record_unique_ptr_t Parser::get()
        {
            return std::move(aln_);
        }

        Formater::Formater(header_unique_ptr_t& hdr_ptr):
        bio_header_(std::move(hdr_ptr))
        {
        }

        int Formater::format(const record_t *aln)
        {
            char* ss = new char[aln->m_data];
            kstring_t s = {0, 0, ss};
            
            int err = sam_format1(bio_header_.get(), aln, &s);
            if (err < 0) {
                std::clog << "[error] sam_format1 " << err << "\n";
            }
            kputc('\n', &s);
            std::string rec_str(s.s, s.l);
            record_str_ = rec_str;
            delete[] s.s;
            return 0;
        }

        std::string Formater::get()
        {
            return record_str_;
        }
    } 
} 
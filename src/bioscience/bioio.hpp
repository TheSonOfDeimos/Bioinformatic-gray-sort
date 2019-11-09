#ifndef BIOIO_HPP
#define BIOIO_HPP

#include <memory>
#include <cassert>
#include <string>

#include "htslib/hts.h"
#include "htslib/bgzf.h"

#include "bio.hh"

namespace bioscience
{
  namespace io
  {
    
    struct sam_policy
    {
      typedef samFile file_t;

      file_t *open(const char *path, const char *mode);
      static int close(file_t *file)
      {
        return hts_close(file);
      }

      header_t *read(file_t *file);
      int write(file_t *file, const header_t *header);

      int read(file_t *file, header_t *header, record_t *record);
      int write(file_t *file, const header_t *header, const record_t *record);
    };
    
    struct bam_policy
    {
      typedef BGZF file_t;

      file_t *open(const char *path, const char *mode);
      static int close(file_t *file)
      {
        return bgzf_close(file);
      }

      header_t *read(file_t *file);
      int write(file_t *file, const header_t *header);

      int read(file_t *file, header_t *, record_t *record);
      int write(file_t *file, const header_t *, const record_t *record);
    };

    template <class bio_policy>
    class Extractor
    {
    public:
      typedef std::unique_ptr<header_t, cleaner> header_unique_ptr_t;
      typedef std::unique_ptr<record_t, cleaner> record_unique_ptr_t;

      Extractor(const char *const path) : policy_(),
                                          file_(policy_.open(path, "r"), bio_policy::close),
                                          bio_header_(policy_.read(file_.get())),
                                          aln_(nullptr),
                                          error_flag_(0),
                                          filename_(path)
      {
      }

      std::string filename() const
      {
        return filename_;
      }

      int read()
      {
        assert(error_flag_ >= 0);
        aln_ = record_unique_ptr_t(bam_init1());
        error_flag_ = policy_.read(file_.get(), bio_header_.get(), aln_.get());
        return status();
      }

      record_unique_ptr_t get()
      {
        return std::move(aln_);
      }

      int status() const
      {
        return error_flag_;
      }

      header_unique_ptr_t header()
      {
        return std::move(bio_header_);
      }

    private:
      typedef std::shared_ptr<typename bio_policy::file_t> file_ptr_t;

      bio_policy policy_;

      file_ptr_t file_;
      header_unique_ptr_t bio_header_;
      std::unique_ptr< record_t, cleaner > aln_;
      int error_flag_;
      std::string filename_;
    };


    template <class bio_policy>
    class Packer
    {
    public:
      typedef std::unique_ptr<header_t, cleaner> header_unique_ptr_t;

      Packer(const char *const path, header_unique_ptr_t &header) : policy_(),
                                                                   file_(policy_.open(path, "w"), bio_policy::close),
                                                                   bio_header_(std::move(header)),
                                                                   error_flag_(0)
      {
        error_flag_ = policy_.write(file_.get(), bio_header_.get());
      }

      int write(const record_t *rhs)
      {
        assert(error_flag_ >= 0);
        assert(rhs != nullptr);
        error_flag_ = policy_.write(file_.get(), bio_header_.get(), rhs);
        return status();
      }

      int status() const
      {
        return error_flag_;
      }

    private:
      bio_policy policy_;

      std::shared_ptr<typename bio_policy::file_t> file_;
      header_unique_ptr_t bio_header_;
      int error_flag_;
    };
  }
}
#endif

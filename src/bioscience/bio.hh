#ifndef BIO_HH
#define BIO_HH

#include <memory>
#include "htslib/sam.h"

namespace bioscience
{
  typedef uint32_t coordinate_t;
  typedef bam1_t record_t;
  typedef bam_hdr_t header_t;


  struct bio_coordinate
  {
    int cmp_core(const bio_coordinate& rhs) const;
    bool operator < (const bio_coordinate& rhs) const;
    bool operator == (const bio_coordinate& rhs) const;
    bool operator >= (const bio_coordinate& rhs) const;
    bool operator <= (const bio_coordinate& rhs) const;
    bool operator != (const bio_coordinate& rhs) const;

    int32_t tid_;
    int32_t pos_;
    uint16_t flag_;
  };

  struct cleaner
  {
    void operator()(header_t * hdr)
    {
      bam_hdr_destroy(hdr);
    }
    void operator()(record_t * rc)
    {
      bam_destroy1(rc);
    }
  };

  struct bio_read
  {
    typedef bio_coordinate key_t;
    bio_read();
    bio_read(record_t * record_ptr);

    bio_read(const bio_read & rhs) = delete;
    bio_read(bio_read && rhs);

    ~bio_read();

    bio_read & operator=(const bio_read &rhs) = delete;
    bio_read & operator=(bio_read &&rhs);
    
    bool operator<(const bio_read & rhs) const;

    key_t get() const;
    record_t * record_;
  };

  struct bam_meta
  {
    std::size_t origin_;
    std::size_t origin_pos_;
    std::uint64_t key;
    std::uint16_t flag_;

    bam_meta() = default;
    bam_meta(std::size_t origin, std::size_t pos, const record_t & read);
    bool operator<(const bam_meta & rhs) const;
    bool operator==(const bam_meta & rhs) const;
  };
}
#endif

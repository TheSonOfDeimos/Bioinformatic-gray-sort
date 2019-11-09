#include "bio.hh"
#include <iostream>

namespace bioscience
{
  int bio_coordinate::cmp_core(const bio_coordinate& rhs) const
  {
    uint64_t pa = (uint64_t)this -> tid_ << 32 | (this -> pos_ + 1);
    uint64_t pb = (uint64_t)rhs.tid_ << 32 | (rhs.pos_ + 1);

    if (pa == pb) {
      pa = ((this -> flag_ & BAM_FREVERSE) != 0);
      pb = ((rhs.flag_ & BAM_FREVERSE) != 0);
    }

    return pa < pb ? -1 : (pa > pb ? 1 : 0);
  }

  bool bio_coordinate::operator < (const bio_coordinate& rhs) const
  {
    return cmp_core(rhs) == -1 ? 1 : 0; 
  }

  bool bio_coordinate::operator == (const bio_coordinate& rhs) const
  {
    return cmp_core(rhs) == 0 ? 1 : 0;
  }

  bool bio_coordinate::operator >= (const bio_coordinate& rhs) const
  {
    int cmp = cmp_core(rhs);
    return cmp == 1 ? 1 : (cmp == 0 ? 1 : 0); 
  }

  bool bio_coordinate::operator <= (const bio_coordinate& rhs) const
  {
    int cmp = cmp_core(rhs);
    return cmp == -1 ? 1 : (cmp == 0 ? 1 : 0); 
  }

  bool bio_coordinate::operator != (const bio_coordinate& rhs) const
  {
    return !(*this == rhs);
  }

  bio_read::bio_read():
    record_(0)
  {
  }

  bio_read::bio_read(record_t * record_ptr):
    record_(record_ptr)
  {
  }

  bio_read::bio_read(bio_read && rhs)
  {
    this->record_ = rhs.record_;
    rhs.record_ = 0;
  }
  bio_read::~bio_read()
  {
    bam_destroy1(record_);
  }

  typename bio_read::bio_read & bio_read::operator=(bio_read &&rhs)
  {
    this->record_ = rhs.record_;
    rhs.record_ = 0;
    return *this;
  }

  bool bio_read::operator<(const bio_read & rhs) const
  {
    return this->get() < rhs.get();
  }
  
  bio_read::key_t bio_read::get() const
  {
    return {record_ -> core.tid, record_ -> core.pos, record_ -> core.flag};
  }

  bam_meta::bam_meta(std::size_t origin, std::size_t pos, const record_t & read): origin_(origin), origin_pos_(pos), flag_(read.core.flag)
  {
    key = (uint64_t)read.core.tid << 32 | (read.core.pos + 1);
  }

  bool bam_meta::operator<(const bam_meta & rhs) const
  {
    return this->key == rhs.key ? ((this -> flag_ & BAM_FREVERSE) != 0) < ((rhs.flag_ & BAM_FREVERSE) != 0) : this->key < rhs.key;
  }

  bool bam_meta::operator==(const bam_meta & rhs) const
  {
    return this->key == rhs.key ? ((this -> flag_ & BAM_FREVERSE) != 0) == ((rhs.flag_ & BAM_FREVERSE) != 0) : this->key == rhs.key;
  }
}

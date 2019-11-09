#include "bioio.hpp"

namespace bioscience
{
  namespace io
  {
    sam_policy::file_t * sam_policy::open(const char * path, const char * mode)
    {
      return hts_open(path, mode);
    }
    
    header_t * sam_policy::read(file_t * file)
    {
      return sam_hdr_read(file);
    }
    
    int sam_policy::write(file_t * file, const header_t * header)
    {
      return sam_hdr_write(file, header);
    }
    
    int sam_policy::read(file_t * file, header_t * header, record_t * record)
    {
      return sam_read1(file, header, record);
    }
    
    int sam_policy::write(file_t * file, const header_t * header, const record_t * record)
    {
      return sam_write1(file, header, record);
    }


    bam_policy::file_t * bam_policy::open(const char * path, const char * mode)
    {
      return bgzf_open(path, mode);
    }
    header_t * bam_policy::read(file_t * file)
    {
      return bam_hdr_read(file);
    }
    int bam_policy::write(file_t * file, const header_t * header)
    {
      return bam_hdr_write(file, header);
    }
    int bam_policy::read(file_t * file, header_t *, record_t * record)
    {
      return bam_read1(file, record);
    }
    int bam_policy::write(file_t * file, const header_t *, const record_t * record)
    {
      return bam_write1(file, record);
    }
  }
}

#include "clicker.hh"
#include <omp.h>

namespace yasnet
{
  Clicker::Clicker():
    time_(omp_get_wtime())
  {
  }

  void Clicker::restart()
  {
    time_ = omp_get_wtime();
  }

  double Clicker::click()
  {
    return omp_get_wtime() - time_;
  }
}

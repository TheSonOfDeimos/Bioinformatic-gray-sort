#ifndef CLICKER_HH
#define CLICKER_HH

namespace yasnet
{
  class Clicker
  {
  public:
    Clicker();
    void restart();
    double click();
  private:
    double time_;
  };
}
#endif

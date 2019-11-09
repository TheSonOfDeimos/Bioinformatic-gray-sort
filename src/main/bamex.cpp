#include <memory>
#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <string>

#include <boost/program_options.hpp>
#include <boost/sort/sort.hpp>

#include "bioscience/bioio.hpp"
#include "clicker.hh"

namespace po = boost::program_options;
namespace bio = bioscience;

void required(const po::variables_map & vm, const char * rhs)
{
  if(!vm.count(rhs))
  {
    throw std::logic_error(std::string("Option must be specified: '")
			   + rhs + "'.");
  }
}

template< class Policy >
std::unique_ptr< bio::header_t, bio::cleaner > extract(
  std::deque< bio::bio_read > & reads,
  const std::string filename)
{
  if(!filename.empty())
  {
    bio::io::Extractor< Policy > extr(filename.c_str());
    while(extr.read() >= 0)
    {
      reads.emplace_back(extr.get().release());
    }
    return extr.header();
  }
  else
  {
    throw std::logic_error(std::string("Wrong input filename"));
  }
}

using ReadIt = std::deque< bio::bio_read >::iterator;
template< class Policy >
void pack(
  ReadIt first,
  ReadIt last,
  const std::string filename,
  std::unique_ptr< bio::header_t, bio::cleaner > & header)
{
  if(!filename.empty())
  {
    bio::io::Packer< Policy > pack(filename.c_str(), header);
    std::for_each(first, last, [&](bio::bio_read &read)
    {
      pack.write(read.record_);
    });
  }
  else
  {
    throw std::logic_error(std::string("Wrong input filename"));
  }
}

int main(int argc, char ** argv)
{
  std::string input;
  std::string output;
  std::string iform;
  std::string oform;
  
  try
  {
    std::string version("SAM/BAM processing. v1.0.0");
    po::options_description general("General options");
    general.add_options()
      ("help,h", "Help screen")
      ("version,v", "Version information");

    po::options_description format("BIO format options");
    format.add_options()
      ("in-format", po::value< std::string >(&iform)->default_value("sam"), "Input format")
      ("out-format", po::value< std::string >(&oform)->default_value("sam"), "Output format");

    po::options_description data("Data options");
    data.add_options()
      ("input-file,i", po::value< std::string >(&input), "Input filename")
      ("output-file,o", po::value< std::string >(&output), "Output filename");

    po::options_description visible("Allowed options");
    visible.add(general).add(format).add(data);

    po::options_description all("Allowed options");
    all.add(general).add(format).add(data);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
      std::cout << visible;
      return 0;
    }
    if(vm.count("version"))
    {
      std::cout << version << std::endl;
      return 0;
    }

    required(vm, "input-file");
    required(vm, "output-file");
  }
  catch(std::exception & e)
  {
    std::cerr << e.what() << "\n";
    return 0;
  }

  std::clog << "[Daia input] Reading data from file " << input << std::endl;
  yasnet::Clicker timer;

  std::deque< bio::bio_read > reads;
  std::unique_ptr< bio::header_t, bio::cleaner > header;
  try
  {
    header = iform == "bam" ?
      extract< bio::io::bam_policy >(reads, input):
      extract< bio::io::sam_policy >(reads, input);
  }
  catch(std::exception & e)
  {
    std::cerr << e.what() << "\n";
    return 0;
  }
  std::clog << "[Data] Total reads: " << reads.size() << " in ";
  std::clog << timer.click() << std::endl;

  std::clog << "[Data] Sorting data..." << std::endl;
  timer.restart();
  boost::sort::block_indirect_sort(std::begin(reads), std::end(reads),
                                   [&](const bio::bio_read &lhs, const bio::bio_read &rhs) {
                                     return lhs.get() < rhs.get();
                                   });
  std::clog << "[Data] Data sorted in " << timer.click() << std::endl;

  std::clog << "[Data output] Writing data to file " << output << std::endl;
  timer.restart();
  try
  {
    oform == "bam" ?
      pack< bio::io::bam_policy >(reads.begin(), reads.end(), output, header):
      pack< bio::io::sam_policy >(reads.begin(), reads.end(), output, header);
  }
  catch(std::exception & e)
  {
    std::cerr << e.what() << "\n";
    return 0;
  }
  std::clog << "[Data output] Data wrote in " << timer.click() << std::endl;
  return 0;
}

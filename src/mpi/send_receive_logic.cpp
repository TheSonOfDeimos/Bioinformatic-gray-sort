#include "send_receive_logic.hpp"
#include <numeric>
#include <memory>
#include <iostream>
#include <cstring>
#include <cassert>
#include "mpi.h"

std::pair<std::vector<std::size_t>, std::vector<std::size_t>> retreive_positions(const std::vector<bioscience::bam_meta> &meta, unsigned proc)
{
  std::vector<std::size_t> positions_from_node{};
  std::vector<std::size_t> positions_this_node{};

  for (auto i = meta.begin(); i != meta.end(); ++i)
  {
    if (i->origin_ == proc)
    {
      positions_from_node.push_back(i->origin_pos_);
      positions_this_node.push_back(i - meta.begin());
    }
  }
  return {std::move(positions_from_node), std::move(positions_this_node)};
}

void get_from_node(unsigned node, std::vector<std::size_t> &positions_to_get, const std::vector<std::size_t> &positions_to_put, std::vector<bam1_t> &bam_storage)
{
  MPI_Status stat;
  MPI_Request req;

  unsigned long long package_size = positions_to_get.size();

  MPI_Isend(&package_size, 1, MPI_UNSIGNED_LONG_LONG, node, 0, MPI_COMM_WORLD, &req);
  MPI_Wait(&req, &stat);

  if (package_size != 0)
  {
    MPI_Isend(positions_to_get.data(), sizeof(std::size_t) * positions_to_get.size(), MPI_BYTE, node, 0, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);
    positions_to_get.clear();

    unsigned long long receive_size = 0;
    MPI_Irecv(&receive_size, 1, MPI_UNSIGNED_LONG_LONG, node, 0, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);

    std::unique_ptr<char[]> receive_buffer = std::make_unique<char[]>(receive_size);

    MPI_Irecv(receive_buffer.get(), receive_size, MPI_BYTE, node, 0, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);

    unsigned int i = 0;
    unsigned int j = 0;
    while (i < receive_size)
    {
      if (bam_storage[positions_to_put[j]].data)
      {
        delete[] bam_storage[positions_to_put[j]].data;
      }
      std::memcpy(&bam_storage[positions_to_put[j]], receive_buffer.get() + i, 64);
      i += 64;
      bam_storage[positions_to_put[j]].data = new uint8_t[bam_storage[positions_to_put[j]].l_data];
      std::memcpy(bam_storage[positions_to_put[j]].data, receive_buffer.get() + i, bam_storage[positions_to_put[j]].l_data);
      i += bam_storage[positions_to_put[j]].l_data;
      ++j;
    }
  }
}

void send_to_node(unsigned node, const std::vector<bioscience::bio_read> &bam_storage)
{
  MPI_Status stat;
  MPI_Request req;

  unsigned long long  package_size = 0;

  MPI_Irecv(&package_size, 1, MPI_UNSIGNED_LONG_LONG, node, 0, MPI_COMM_WORLD, &req);
  MPI_Wait(&req, &stat);

  if (package_size != 0)
  {

    std::vector<std::size_t> requested_positions(package_size, 0);

    MPI_Irecv(requested_positions.data(), sizeof(std::size_t) * package_size, MPI_BYTE, node, 0, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);

    unsigned long all_reads_byte = std::accumulate(requested_positions.begin(), requested_positions.end(), 0, [&](int a, std::size_t &rhs) {
      return a + 64 + bam_storage[rhs].record_->l_data;
    });

    std::unique_ptr<char[]> send_buffer = std::make_unique<char[]>(all_reads_byte);
    unsigned int i = 0;
    unsigned int j = 0;
    while (i < all_reads_byte)
    {
      std::memcpy(send_buffer.get() + i, bam_storage[requested_positions[j]].record_, 64);
      i += 64;
      std::memcpy(send_buffer.get() + i, bam_storage[requested_positions[j]].record_->data, bam_storage[requested_positions[j]].record_->l_data);
      i += bam_storage[requested_positions[j]].record_->l_data;
      ++j;
    }

    MPI_Isend(&all_reads_byte, 1, MPI_INT, node, 0, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);

    MPI_Isend(send_buffer.get(), all_reads_byte, MPI_BYTE, node, 0, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);
  }
}

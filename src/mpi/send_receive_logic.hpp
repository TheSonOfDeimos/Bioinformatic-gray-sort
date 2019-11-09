#ifndef SEND_RECEIVE_LOGIC
#define SEND_RECEIVE_LOGIC

#include <utility>
#include <vector>
#include "htslib/sam.h"
#include "../bioscience/bio.hh"

std::pair<std::vector<std::size_t>, std::vector<std::size_t>> retreive_positions(const std::vector<bioscience::bam_meta> &meta, unsigned proc);

void get_from_node(unsigned node, std::vector<std::size_t> &positions_to_get, const std::vector<std::size_t> &positions_to_put, std::vector<bam1_t> &bam_storage);

void send_to_node(unsigned node, const std::vector<bioscience::bio_read> &bam_storage);

#endif
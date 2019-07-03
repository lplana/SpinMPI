// ==========================================================================
//                                  SpinMPI
// ==========================================================================
// This file is part of SpinMPI.
//
// SpinMPI is Free Software: you can redistribute it and/or modify it
// under the terms found in the LICENSE[.md|.rst] file distributed
// together with this file.
//
// SpinMPI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// ==========================================================================
// Autor: Francesco Barchi <francesco.barchi@polito.it>
// ==========================================================================
// SpinMPI.cpp: Main File for SpinMPI
// ==========================================================================

#include "_mpi.h"

struct mpi_communicator {
  struct mpi_processor *processors;
  bool *processors_enabled;
  uint rank_max;
};



// --- Private Functions ---


// --- Global Variables ---
mpi_communicator_t comm_world;
mpi_processor_t *processor = NULL;

// --- Functions ---
void context_init(uint32_t rank, uint32_t rank_max) {
  uint32_t i;

  comm_world.rank_max = rank_max;
  comm_world.processors = sark_alloc(rank_max, sizeof(struct mpi_communicator));
  comm_world.processors_enabled = sark_alloc(rank_max, sizeof(bool));

  for (i = 0; i < rank_max; i++)
    comm_world.processors_enabled[i] = false;

  processor = &comm_world.processors[rank];
  processor->x = get_chip_x();
  processor->y = get_chip_y();
  processor->p = get_chip_p();
  processor->rank = rank;
  comm_world.processors_enabled[rank] = true;

  debug_printf("[MPI-CORE] Obtained rank %d/%d\n", rank, rank_max - 1);
}

void context_add_node(uint16_t varcode, uint8_t *data, uint16_t length) {
  mpi_processor_t *p = (mpi_processor_t *) data;

  if (processor != NULL &&
      p->rank < comm_world.rank_max &&
      p->rank != processor->rank) {

    comm_world.processors[p->rank] = *p;
    comm_world.processors_enabled[p->rank] = true;
    acp_variable_register(MPI_ACP_VARCODE_CONTEXT + p->rank,
                          MPI_ACP_BUFFER_SIZE_MAX, NULL, NULL);
  }
  return;
}

bool context_check() {
  uint32_t i;
  bool r = true;
  if (comm_world.processors_enabled != NULL) {
    for (i = 0; i < comm_world.rank_max; i++) {
      if (!comm_world.processors_enabled[i])
        return false;
    }
    return true;
  }
  return false;
}

uint32_t get_rank() {
  return processor->rank;
}

uint8_t get_chip_x() {
  return (uint8_t)(((uint32_t) sv->p2p_addr) >> 8);
}

uint8_t get_chip_y() {
  return (uint8_t)(((uint32_t) sv->p2p_addr) & 0xFF);
}

uint8_t get_chip_p() {
  return processor->p = (uint8_t) sark.virt_cpu;
}

uint32_t get_comm_size(uint32_t comm) {
  if (comm == MPI_COMM_WORLD)
    return comm_world.rank_max;
  return 0;
}

uint16_t get_comm_acp_address(uint32_t comm, uint32_t rank) {
  uint16_t w;

  if (comm == MPI_COMM_WORLD) {
    debug_printf("[MPI-CORE] Get address from rank:\n");
    debug_printf("  (rank) %04x -> (x:y:p) %02x : %02x : %02x\n",
                 rank,
                 comm_world.processors[rank].x,
                 comm_world.processors[rank].y,
                 comm_world.processors[rank].p);
    w = ((comm_world.processors[rank].x & 0xFF) << 8) |
        (comm_world.processors[rank].y & 0xFF);
    debug_printf("  --> %04x\n", w);
    return w;
  } else {
    debug_printf("[MPI-CORE] Communicators not supported");
    rt_error(RTE_ABORT);
  }
  return 0;
}

uint8_t get_comm_acp_port(uint32_t comm, uint32_t rank) {
  uint8_t w;

  if (comm == MPI_COMM_WORLD) {
    debug_printf("[MPI-CORE] Get port from rank:\n");
    debug_printf("  (rank) %04x -> (port : processor) %01x : %02x \n",
                 ACP_PORT,
                 comm_world.processors[rank].p);
    w = ((ACP_PORT & 0b111) << 5) |
        (comm_world.processors[rank].p & 0b11111);
    debug_printf("  --> %02x\n", w);
    return w;
  }
  return 0;
}

uint32_t get_comm_rank(uint32_t comm) {
  if (comm == MPI_COMM_WORLD)
    return processor->rank;
  return 0;
}

uint32_t byteof(MPI_Datatype d) {
  if (d == MPI_BYTE ||
      d == MPI_CHARACTER ||
      d == MPI_LOGICAL ||
      d == MPI_INT8_T ||
      d == MPI_UINT8_T)
    return sizeof(uint8_t);

  else if (
      d == MPI_INT16_T ||
          d == MPI_UINT16_T)
    return sizeof(uint16_t);

  else if (
      d == MPI_INTEGER ||
          d == MPI_REAL ||
          d == MPI_INT32_T ||
          d == MPI_UINT32_T)
    return sizeof(uint32_t);

  else if (
      d == MPI_DOUBLE_PRECISION ||
          d == MPI_INT64_T,
          d == MPI_UINT64_T)
    return sizeof(uint64_t);

  return 0;
}


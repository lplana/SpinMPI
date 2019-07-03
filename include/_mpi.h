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

#ifndef __SPINN_MPI_PRIVATE_H__
#define __SPINN_MPI_PRIVATE_H__

#include "mpi.h"
#include "acp.h"

#define MPI_ACP_VARCODE_RANK 0x00
#define MPI_ACP_VARCODE_RANK_MAX 0x01
#define MPI_ACP_VARCODE_PROCESSOR 0x0A
#define MPI_ACP_VARCODE_CONTEXT 0x10

#define MPI_ACP_VARCODE_SYNC1 0x05
#define MPI_ACP_VARCODE_SYNC2 0x06
#define MPI_ACP_VARCODE_SYNC3 0x07

#define MPI_ACP_BUFFER_SIZE_MAX 64


#ifdef MPI_DEBUG
#define debug_printf(...) \
  do { io_printf(IO_BUF, __VA_ARGS__); } while (0)
#else
#define debug_printf(...) \
  do { } while (0)
#endif

// --- Struct ---
struct mpi_processor {
  uint8_t x;
  uint8_t y;
  uint8_t p;
  uint32_t rank;
};

struct mpi_message {
  uint8_t bytes[MPI_ACP_BUFFER_SIZE_MAX];
};

// --- Typedefs ---
typedef uint8_t *uint8_p;

typedef struct mpi_processor mpi_processor_t;
typedef struct mpi_communicator mpi_communicator_t;
typedef struct mpi_message mpi_message_t;

// --- Functions ---
void context_init(uint32_t rank, uint32_t rank_max);
bool context_check();
void context_add_node(uint16_t varcode, uint8_t *data, uint16_t length);

uint8_t get_chip_x();
uint8_t get_chip_y();
uint8_t get_chip_p();
uint32_t get_rank();
uint32_t get_comm_size(uint32_t comm);
uint32_t get_comm_rank(uint32_t comm);
uint16_t get_comm_acp_address(uint32_t comm, uint32_t rank);
uint8_t get_comm_acp_port(uint32_t comm, uint32_t rank);

mpi_message_t _get_message();

bool send_stop_to_host();
uint32_t byteof(MPI_Datatype d);

#endif //__SPINN_MPI_PRIVATE_H__

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
// mpi_bcast_mc.c: Broadcast-over-Multicast implementation for SpinMPI
// ==========================================================================

#include "_mpi.h"

#define COMM_BUFFER 60

void _inline_send_all(uint32_t mpi_rank_src, uint8_t *buffer, uint16_t buffer_length);
void _inline_recv(uint32_t mpi_rank_src, uint8_t *buffer, uint16_t buffer_length);


/**
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
  uint32_t i, mpi_rank, mpi_size;
  uint16_t length = count * byteof(datatype);

  mpi_rank = get_rank();
  mpi_size = get_comm_size(comm);

  spin2_mc_wfs();
  if (mpi_rank == root) {
      _inline_send_all(mpi_rank, buffer, length);
  } else {
      _inline_recv(root, buffer, length);
  }

  return MPI_SUCCESS;
}

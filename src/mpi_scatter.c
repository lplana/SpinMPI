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
// mpi_scatter.c: MPI Scatter implementation for SpinMPI
// ==========================================================================

#include "_mpi.h"

/**
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int MPI_Scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                void *recvbuf, int recvcount, MPI_Datatype recvtype,
                int root, MPI_Comm comm) {

  uint32_t i, mpi_rank, mpi_size;
  uint8_p data = (uint8_p) sendbuf;
  uint16_t send_length = sendcount * byteof(sendtype);
  uint16_t recv_length = recvcount * byteof(recvtype);

  mpi_rank = get_rank();
  mpi_size = get_comm_size(comm);

  if (mpi_rank == root) {
    for (i = 0; i < mpi_size; i++) {
      if (i != root) {
        acp_net_variable_write(MPI_ACP_VARCODE_CONTEXT + mpi_rank,
                               data, send_length,
                               get_comm_acp_address(MPI_COMM_WORLD, i),
                               get_comm_acp_port(MPI_COMM_WORLD, i));
      }
      data += send_length;
    }
  } else {
    acp_variable_read_after_write(MPI_ACP_VARCODE_CONTEXT + root,
                                  recvbuf, &recv_length);
  }

  return MPI_SUCCESS;
}
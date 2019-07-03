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

inline void _inline_send(uint32_t mpi_rank_src, uint32_t mpi_rank_dest,
                         uint8_t *buffer, uint16_t buffer_length,
                         MPI_Comm comm);
inline void _inline_recv(uint32_t mpi_rank_src,
                         uint8_t *buffer, uint16_t *buffer_length);

/**
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int MPI_Allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                  void *recvbuf, int recvcount, MPI_Datatype recvtype,
                  MPI_Comm comm) {

  uint32_t mpi_rank, mpi_size;
  uint8_p data = (uint8_p) recvbuf;
  uint16_t send_length = sendcount * byteof(sendtype);
  uint16_t recv_length = recvcount * byteof(recvtype);

  mpi_rank = get_rank();
  mpi_size = get_comm_size(comm);

  for (int i = 0; i < mpi_size; i++) {
    if (i != mpi_rank) {
      if (i > mpi_rank) {
        _inline_send(mpi_rank, i, sendbuf, send_length, comm);
        _inline_recv(i, data, &recv_length);
      } else {
        _inline_recv(i, data, &recv_length);
        _inline_send(mpi_rank, i, sendbuf, send_length, comm);
      }
    } else {
      sark_mem_cpy(data, sendbuf, send_length);
    }
    //sark_delay_us(300);
    data += recv_length;
  }
  return MPI_SUCCESS;
}

void _inline_send(uint32_t mpi_rank_src, uint32_t mpi_rank_dest,
                  uint8_t *buffer, uint16_t buffer_length,
                  MPI_Comm comm) {
  acp_net_variable_write(MPI_ACP_VARCODE_CONTEXT + mpi_rank_src,
                         buffer, buffer_length,
                         get_comm_acp_address(comm, mpi_rank_dest),
                         get_comm_acp_port(comm, mpi_rank_dest));
  return;
}

void _inline_recv(uint32_t mpi_rank_src,
                  uint8_t *buffer, uint16_t *buffer_length) {
  acp_variable_read_after_write(MPI_ACP_VARCODE_CONTEXT + mpi_rank_src, buffer, buffer_length);
}
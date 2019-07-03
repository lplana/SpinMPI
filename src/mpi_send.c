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

/**
 * TODO
 * MPI_Send
 *
 * @param buf
 * @param count
 * @param datatype
 * @param dest
 * @param tag
 * @param comm
 * @return
 */
int MPI_Send(const void *buf,
             int count,
             MPI_Datatype datatype,
             int dest,
             int tag,
             MPI_Comm comm) {
  debug_printf("[MPI-SEND] Start\n");

  uint32_t length = count * byteof(datatype);

  // TODO: support more than 256
  if (length > MPI_ACP_BUFFER_SIZE_MAX) {
    debug_printf("[MPI-SEND] Error! Message too long %d\n", length);
    rt_error(RTE_ABORT);
  } else if (length == 0) {
    debug_printf("[MPI-SEND] Error! Message type not supported\n");
    rt_error(RTE_ABORT);
  }

  // TODO: support tag of communication
  if (tag != 0) {
    debug_printf("[MPI-SEND] Error! Tag not supported\n");
    rt_error(RTE_ABORT);
  }

  // TODO: support runtime communicators
  if (comm != MPI_COMM_WORLD) {
    debug_printf("[MPI-SEND] Error! Communicators not supported\n");
  }

  // Send
  acp_net_variable_write(MPI_ACP_VARCODE_CONTEXT + get_rank(),
                         (uint8_p) buf, length,
                         get_comm_acp_address(MPI_COMM_WORLD, dest),
                         get_comm_acp_port(MPI_COMM_WORLD, dest));

  return MPI_SUCCESS;
}

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
 * MPI_Recv
 *
 * @param buf
 * @param count
 * @param datatype
 * @param source
 * @param tag
 * @param comm
 * @param status
 * @return
 */
int MPI_Recv(void *buf,
             int count,
             MPI_Datatype datatype,
             int source,
             int tag,
             MPI_Comm comm,
             MPI_Status *status) {
  debug_printf("[MPI-RECV] Start\n");

  uint16_t length = count * byteof(datatype);

  // TODO: support more than 256
  if (length > MPI_ACP_BUFFER_SIZE_MAX) {
    debug_printf("[MPI-RECV] Error! Message too long");
    rt_error(RTE_ABORT);
  } else if (length == 0) {
    debug_printf("[MPI-RECV] Error! Message type not supported");
    rt_error(RTE_ABORT);
  }

  // TODO: support tag of communication
  if (tag != 0) {
    debug_printf("[MPI-RECV] Error! Tag not supported");
    rt_error(RTE_ABORT);
  }

  acp_variable_read_after_write(MPI_ACP_VARCODE_CONTEXT + source,
                                (uint8_t *) buf,
                                &length);

  return MPI_SUCCESS;
}
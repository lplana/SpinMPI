#include "_mpi.h"

/**
 * @brief The SpiNNaker MPI_Recv
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
int MPI_Recv(
    void *buf, 
    int count, 
    MPI_Datatype datatype,
    int source, 
    int tag, 
    MPI_Comm comm, 
    MPI_Status *status) {
  
  uint32_t length;
  uint8_t *data;

  debug_printf("[MPI-RECV] Start\n");

  // Check Lenght
  length = count * byteof(datatype);
  if (length == 0) {
    debug_printf("[MPI-RECV] Error! Message type not supported");
    rt_error(RTE_ABORT);
  }

  // Tag not supported
  if (tag != 0) {
    debug_printf("[MPI-RECV] Error! Tag not supported");
    rt_error(RTE_ABORT);
  }

  // Runtime communicators not supported
  if (comm != MPI_COMM_WORLD) {
    debug_printf("[MPI-RECV] Error! Communicators not supported\n");
  }

  data = (uint8_t *)buf;
  
  _recv_ucast(source, data, length, true);

  return MPI_SUCCESS;
}
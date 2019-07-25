#include "_mpi.h"

/**
 * @brief The SpiNNaker MPI_Send
 *
 * @param buf
 * @param count
 * @param datatype
 * @param dest
 * @param tag
 * @param comm
 * @return
 */
int MPI_Send(
    const void *buf,
    int count,
    MPI_Datatype datatype,
    int dest,
    int tag,
    MPI_Comm comm) {
  
  uint32_t length;
  uint8_t *data;
  
  debug_printf("[MPI-SEND] Start\n");

  // Check Lenght
  length = count * byteof(datatype);
  if (length == 0) {
    debug_printf("[MPI-SEND] Error! Message type not supported\n");
    rt_error(RTE_ABORT);
  }

  // Tag not supported
  if (tag != 0) {
    debug_printf("[MPI-SEND] Error! Tag not supported\n");
    rt_error(RTE_ABORT);
  }

  // Runtime communicators not supported
  if (comm != MPI_COMM_WORLD) {
    debug_printf("[MPI-SEND] Error! Communicators not supported\n");
  }
  
  data = (uint8_t *)buf;

  _send_ucast(dest, data, length, true);

  return MPI_SUCCESS;
}

#include "_mpi.h"

/**
 * @brief The SpiNNaker MPI_Scatter
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int MPI_Scatter(
    void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype,
    int root, MPI_Comm comm) {

  spin2_core_t destination, source;
  
  uint32_t i; 
  uint32_t mpi_rank;
  uint32_t mpi_size;
  uint8_t *data = (uint8_t *) sendbuf;

  uint16_t send_length = sendcount * byteof(sendtype);
  uint16_t recv_length = recvcount * byteof(recvtype);
  
  mpi_rank = get_rank();
  mpi_size = get_comm_size(comm);

  if (mpi_rank == root) {
    for (i = 0; i < mpi_size; i++) {
      if (i != root) {
        _send_ucast(i, data, send_length, true);
      }
      data += send_length;
    }
  } else {
    _recv_ucast(root, recvbuf, recv_length, true);
  }

  return MPI_SUCCESS;
}
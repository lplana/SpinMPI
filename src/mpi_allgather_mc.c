#include "_mpi.h"


/**
 * @brief The SpiNNaker MPI_Allgather
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int MPI_Allgather(
    void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype,
    MPI_Comm comm) {
  
  uint32_t i; 
  uint32_t mpi_rank;
  uint32_t mpi_size;
  
  uint8_t *data = (uint8_t *) recvbuf;

  uint32_t send_length = sendcount * byteof(sendtype);
  uint32_t recv_length = recvcount * byteof(recvtype);

  mpi_rank = get_rank();
  mpi_size = get_comm_size(comm);

  sark.vcpu->user3 = 0;

  for (i = 0; i < mpi_size; i++) {
    if (i != mpi_rank) {
      _recv_all(i, data, recv_length, true);
    } else {
      _send_all(mpi_rank, sendbuf, send_length, true);
      sark_mem_cpy(data, sendbuf, send_length);
    }
    data += recv_length;
    sark.vcpu->user3 += 1;
  }
  debug_printf("[MPI] Allgather: End\n");

  return MPI_SUCCESS;
}

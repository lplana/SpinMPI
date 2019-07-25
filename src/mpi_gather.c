#include "_mpi.h"

/**
 * @brief The SpiNNaker MPI_Gather
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int MPI_Gather(
    void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype,
    int root, MPI_Comm comm) {
  
  spin2_core_t destination, source;

  uint32_t i;
  uint32_t mpi_rank;
  uint32_t mpi_size;
  uint8_t *data = (uint8_t *) recvbuf;
  
  uint16_t send_length = sendcount * byteof(sendtype);
  uint16_t recv_length = recvcount * byteof(recvtype);
  
  mpi_rank = get_rank();
  mpi_size = get_comm_size(comm);

  if (mpi_rank == root) {
    for (i = 0; i < mpi_size; i++) {
      if (i != root) {
        _recv_ucast(i, data, recv_length, true);
        /*
        source = get_comm_destination(comm, i);
        acp_me_read(
            MPI_ACP_VARCODE_CONTEXT + i,
            data, 
            recv_length, 
            ACP_CHANNEL_CORE,
            &source,
            true);*/
      } else {
        sark_mem_cpy(data, sendbuf, send_length);
      }
      data += recv_length;
    }
  } else {
    _send_ucast(root, sendbuf, send_length, true);
    /*
    destination = get_comm_destination(comm, root);
    acp_me_update(
        MPI_ACP_VARCODE_CONTEXT + mpi_rank,
        sendbuf,
        send_length,
        ACP_CHANNEL_CORE,
        &destination,
        true);*/
  }

  return MPI_SUCCESS;
}
#include "_mpi.h"

#define FRAGMENT_TIMEOUT 50
#define COMM_BUFFER 60

inline void _inline_send_all(uint32_t mpi_rank_src, uint8_t *buffer, uint16_t buffer_length);
inline void _inline_recv(uint32_t mpi_rank_src, uint8_t *buffer, uint16_t buffer_length);


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

  sark.vcpu->user3 = 0;

  /// Wait for signal moved out of cycle
  spin2_mc_wfs();

  /// Principal communication loop
  for (int i = 0; i < mpi_size; i++) {

    if (i != mpi_rank) {
      _inline_recv(i, data, recv_length);
    } else {
      _inline_send_all(mpi_rank, sendbuf, send_length);
      sark_mem_cpy(data, sendbuf, send_length);
    }
    data += recv_length;
    sark.vcpu->user3 += 1;
  }
  return MPI_SUCCESS;
}

void _inline_send_all(uint32_t mpi_rank_src, uint8_t *buffer, uint16_t buffer_length) {
  uint8_t *data = buffer;
  uint16_t bytes = buffer_length;
  uint16_t to_send;

  do {
    to_send = (bytes < COMM_BUFFER) ? bytes : COMM_BUFFER;

    // Delay for communication fragment sync
    spin1_delay_us(FRAGMENT_TIMEOUT);

    cmd_net_variable_write_all(MPI_ACP_VARCODE_CONTEXT + mpi_rank_src, data, to_send);
    data += to_send;
    bytes -= to_send;

  } while (bytes > 0);

  return;
}

void _inline_recv(uint32_t mpi_rank_src, uint8_t *buffer, uint16_t buffer_length) {
  uint8_t *data = buffer;
  uint16_t bytes = buffer_length;
  uint16_t to_recv;

  do {
    to_recv = (bytes < COMM_BUFFER) ? bytes : COMM_BUFFER;

    acp_variable_read_after_write(MPI_ACP_VARCODE_CONTEXT + mpi_rank_src, data, &to_recv);
    data += to_recv;
    bytes -= to_recv;

  } while (bytes > 0);

  return;
}

#include "_mpi.h"

#define COMM_BUFFER 60

void _inline_send_all(uint32_t mpi_rank_src, uint8_t *buffer, uint16_t buffer_length);
void _inline_recv(uint32_t mpi_rank_src, uint8_t *buffer, uint16_t buffer_length);


/**
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {
  uint32_t i, mpi_rank, mpi_size;
  uint16_t length = count * byteof(datatype);

  mpi_rank = get_rank();
  mpi_size = get_comm_size(comm);

  spin2_mc_wfs();
  if (mpi_rank == root) {
      _inline_send_all(mpi_rank, buffer, length);
  } else {
      _inline_recv(root, buffer, length);
  }

  return MPI_SUCCESS;
}

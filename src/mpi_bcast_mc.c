#include "_mpi.h"


/**
 * @brief The SpiNNaker MPI_Bcast
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int MPI_Bcast(
        void *buffer, 
        int count, 
        MPI_Datatype datatype,
        int root, 
        MPI_Comm comm) {
  
  uint32_t mpi_rank;
  uint32_t mpi_size;

  uint32_t length = count * byteof(datatype);

  mpi_rank = get_rank();
  mpi_size = get_comm_size(comm);

  if (mpi_rank == root) {
      _send_all(mpi_rank, buffer, length, true);
  } else {
      _recv_all(root, buffer, length, true);
  }

  debug_printf("[MPI] Broadcast: End\n");

  return MPI_SUCCESS;
}

#include "_mpi.h"

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

  if (mpi_rank == root) {
    // TODO: implement with MC
    for (i = 0; i < mpi_size; i++) {
      if (i != root) {
        acp_net_variable_write(MPI_ACP_VARCODE_CONTEXT + mpi_rank,
                               (uint8_p) buffer, length,
                               get_comm_acp_address(MPI_COMM_WORLD, i),
                               get_comm_acp_port(MPI_COMM_WORLD, i));
      }
    }
  } else {
    acp_variable_read_after_write(MPI_ACP_VARCODE_CONTEXT + root,
                                  (uint8_t *) buffer,
                                  &length);
  }

  return MPI_SUCCESS;
}
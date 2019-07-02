#include "_mpi.h"


// --- Functions ---
/**
 * MPI_Spinn
 *
 * Initialise Spin1 library,
 * launch spin2_sdp_callback in queue 0 (Not queable)
 * launch mpi_main in queue 1
 *
 * @param handler
 * @return
 */
int MPI_Spinn(MPI_Main handler) {
  uint rc;
  uint8_t rank;

  debug_printf("[MPI] Init SPIN1 events\n");

  // Register queue
  spin1_callback_on(MCPL_PACKET_RECEIVED, spin2_mc_callback, -1);
  spin1_callback_on(SDP_PACKET_RX, spin2_sdp_callback, 0);

  spin2_mc_callback_register(acp_mc_callback);
  spin2_sdp_callback_on(ACP_PORT, acp_sdp_callback);

  // Register of entry point with lowest priority
  spin1_schedule_callback(handler, 0, 0, 1);

  // Disable Timer Event
  spin1_set_timer_tick(0);

  // Init routing tables
  spin2_mc_init();
  acp_mc_init();

  // Start application
  rc = spin1_start(SYNC_NOWAIT);

  // Exit
  if (rc == MPI_SUCCESS)
    io_printf(IO_BUF, "[MPI] SPIN1 terminated with SUCCESS\n");
  else
    io_printf(IO_BUF, "[MPI] SPIN1 terminated with ERRORS\n");

  return rc;
}

/**
 * MPI_Init
 * Inside Spin1 scheduler
 *
 * Register ACP variables
 * Wait SYNC0
 * Receive context configuration
 * Wait SYNC1
 *
 * @param argc
 * @param argv
 * @return
 */
int MPI_Init(int *argc, char ***argv) {
  debug_printf("[MPI] Init from SpiNNode %d@%d:%d\n",
               get_chip_p(), get_chip_x(), get_chip_y());

  // --- Register MPI ACP variables ---
  uint32_t rank;
  uint32_t rank_max;
  uint16_t length = sizeof(rank);
  mpi_processor_t processor;

  acp_variable_register(MPI_ACP_VARCODE_RANK, sizeof(rank),
                        NULL, NULL);
  acp_variable_register(MPI_ACP_VARCODE_RANK_MAX, sizeof(rank_max),
                        NULL, NULL);
  acp_variable_register(MPI_ACP_VARCODE_PROCESSOR, sizeof(mpi_processor_t),
                        context_add_node, NULL);

  uint32_t sync1;
  uint32_t sync2;
  uint32_t sync3;

  acp_variable_register(MPI_ACP_VARCODE_SYNC1, sizeof(sync1),
                        NULL, NULL);
  acp_variable_register(MPI_ACP_VARCODE_SYNC2, sizeof(sync2),
                        NULL, NULL);
  acp_variable_register(MPI_ACP_VARCODE_SYNC3, sizeof(sync3),
                        NULL, NULL);

  // --- Barrier ---
  event_wait();
  // ---------------

  // Receive rank and rank_max
  length = sizeof(rank);
  acp_variable_read_after_write(
      MPI_ACP_VARCODE_RANK, (uint8_p) &rank, &length);
  length = sizeof(rank_max);
  acp_variable_read_after_write(
      MPI_ACP_VARCODE_RANK_MAX, (uint8_p) &rank_max, &length);

  // Receive chip radius and processors
  length = sizeof(sync1);
  acp_variable_read_after_write(
      MPI_ACP_VARCODE_SYNC1, (uint8_p) &sync1, &length);
  length = sizeof(sync2);
  acp_variable_read_after_write(
      MPI_ACP_VARCODE_SYNC2, (uint8_p) &sync2, &length);
  length = sizeof(sync3);
  acp_variable_read_after_write(
      MPI_ACP_VARCODE_SYNC3, (uint8_p) &sync3, &length);

  // Set rank info and allocate the context structure
  context_init(rank, rank_max);

  // Set spin2 sync values
  spin2_mc_sync_max_set(1, sync1);
  spin2_mc_sync_max_set(2, sync2);
  spin2_mc_sync_max_set(3, sync3);

//  spin2_mc_sync1_max_set(sync1);
//  spin2_mc_sync2_max_set(sync2);

  // --- Barrier ---
  event_wait();
  // ---------------

  // --- Check context ---
  if (context_check()) {
    io_printf(IO_BUF, "[MPI] CONTEXT OK\n");
  } else {
    io_printf(IO_BUF, "[MPI] CONTEXT ERROR\n");
  }
  return MPI_SUCCESS;
}

/**
 * TODO
 * MPI_Finalize
 *
 * Wait SYNC0 (All SpiNNode have finish)
 * Free memory allocation
 * Spin1 Exit
 *
 * @return
 */
int MPI_Finalize() {

  if (get_comm_rank(MPI_COMM_WORLD) == 0) {
    io_printf(IO_BUF, "[MPI] Finalize, send ACP-STOP command\n");
    send_stop_to_host();
  }

  // Wait SYNC0
  debug_printf("[MPI] Finalize, wait SYNC\n");
  event_wait();

  // TODO: Free the memory

  // Stop sark event
  debug_printf("[MPI] Finalize, exit from SPIN1\n");
  spin1_exit(MPI_SUCCESS);
  return MPI_SUCCESS;
}

/**
 * MPI_Comm_rank
 *
 * @param comm
 * @param rank
 * @return
 */
int MPI_Comm_rank(MPI_Comm comm, int *rank) {
  debug_printf("[MPI] Communication rank\n");
  *rank = get_comm_rank(comm);
  return MPI_SUCCESS;
}

/**
 * MPI_Comm_size
 *
 * @param comm
 * @param size
 * @return
 */
int MPI_Comm_size(MPI_Comm comm, int *size) {
  debug_printf("[MPI] Communication size\n");
  *size = get_comm_size(comm);
  return MPI_SUCCESS;
}

/**
 * TODO
 * MPI_Get_processor_name
 *
 * @param name
 * @param resultlen
 * @return
 */
int MPI_Get_processor_name(char *name, int *resultlen) {
  debug_printf("[MPI] [Not Implemented] Get processor name\n");
  return MPI_SUCCESS;
}


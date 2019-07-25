#include "_mpi.h"


// --- Functions ---

/**
 * @brief The SpiNNAker MPI entry point
 *
 * Initialise Spin1 library,
 * launch spin2_sdp_callback in queue 0 and launch mpi_main in queue 1
 *
 * @param handler
 * @return
 */
int MPI_Spinn(MPI_Main handler) {
  uint rc;
  uint8_t rank;

  debug_printf("[MPI] Start MPI_Spinn\n");

  // Init Spin2 SDP
  spin2_sdp_init();
  debug_printf("[MPI] Init Spin2 SDP\n");

  // Init Spin2 MCM
  spin2_mc_init();
  debug_printf("[MPI] Init Spin2 MC\n");

  // Init ACP
  acp_init();
  debug_printf("[MPI] Init ACP\n");

  // Disable Timer Event
  spin1_set_timer_tick(0);

  // Register of entry point with lowest priority
  spin1_schedule_callback(handler, 0, 0, 1);

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
 * @brief The SpiNNaker MPI_Init
 *
 * Register the ACP variables
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
  mpi_processor_t processor;
  uint32_t rank, rank_max;
  uint32_t sync1, sync2, sync3;
  uint16_t length = sizeof(rank);
  
  // Memory Entitiy for receive my rank
  acp_me_create(
      MPI_ACP_VARCODE_RANK, 
      sizeof(rank),
      NULL, NULL);
  
  // Memory Entitiy for receive context size
  acp_me_create(
      MPI_ACP_VARCODE_RANK_MAX, 
      sizeof(rank_max),
      NULL, NULL);
  
  // Memory Entitiy for receive context info
  acp_me_create(
      MPI_ACP_VARCODE_PROCESSOR, 
      sizeof(mpi_processor_t),
      NULL, NULL);

  // Memory Entitiy for receive context info
  acp_me_create(
      MPI_ACP_VARCODE_PROCESSOR_BCAST, 
      sizeof(mpi_processor_t),
      NULL, NULL);

  // Memory Entitiy for receive MCM sync info
  acp_me_create(
      MPI_ACP_VARCODE_SYNC1, 
      sizeof(sync1),
      NULL, NULL);

  acp_me_create(
      MPI_ACP_VARCODE_SYNC2, 
      sizeof(sync2),
      NULL, NULL);

  acp_me_create(
      MPI_ACP_VARCODE_SYNC3, 
      sizeof(sync3),
      NULL, NULL);

  // --- Barrier ---
  event_wait();
  // ---------------

  // Receive rank and rank_max
  length = sizeof(rank);
  acp_me_read(
      MPI_ACP_VARCODE_RANK,
      (uint8_p) &rank, 
      length, ACP_CHANNEL_HOST, NULL, true);

  length = sizeof(rank_max);
  acp_me_read(
      MPI_ACP_VARCODE_RANK_MAX,
      (uint8_p) &rank_max, 
      length, ACP_CHANNEL_HOST, NULL, true);

  // Receive chip radius and processors
  length = sizeof(sync1);
  acp_me_read(
      MPI_ACP_VARCODE_SYNC1,
      (uint8_p) &sync1, 
      length, ACP_CHANNEL_HOST, NULL, true);
  
  length = sizeof(sync2);
  acp_me_read(
      MPI_ACP_VARCODE_SYNC2,
      (uint8_p) &sync2, 
      length, ACP_CHANNEL_HOST, NULL, true);
  
  length = sizeof(sync3);
  acp_me_read(
      MPI_ACP_VARCODE_SYNC3,
      (uint8_p) &sync3, 
      length, ACP_CHANNEL_HOST, NULL, true);

  // Set spin2 sync values
  spin2_mc_sync_max_set(1, sync1);
  spin2_mc_sync_max_set(2, sync2);
  spin2_mc_sync_max_set(3, sync3);

  // Set rank info and allocate the context structure
  context_init(rank, rank_max);

  // App barrier before host-board barrier
  //spin2_mc_wfs();

  // --- Barrier ---
  event_wait();
  // ---------------

  // --- Check context ---
  if (context_check()) {
    io_printf(IO_BUF, "[MPI] CONTEXT OK\n");
  } else {
    // TODO Manage Error
    io_printf(IO_BUF, "[MPI] CONTEXT ERROR\n");
  }

  return MPI_SUCCESS;
}


/**
 * @brief The SpiNNaker MPI_Finalize
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
 * @brief The SpiNNaker MPI_Comm_rank
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
 * @brief The SpiNNaker MPI_Comm_size
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
 * @brief The SpiNNaker MPI_Get_processor_name
 *
 * @param name
 * @param resultlen
 * @return
 */
int MPI_Get_processor_name(char *name, int *resultlen) {
  // TODO
  debug_printf("[MPI] [Not Implemented] Get processor name\n");
  return MPI_SUCCESS;
}


/**
 * @brief The SpiNNaker MPI_Barrier
 * 
 * @param comm
 * @return
 */
int MPI_Barrier(MPI_Comm comm) {
  debug_printf("[MPI] Barrier\n");
  spin2_mc_wfs();
  return MPI_SUCCESS;
}

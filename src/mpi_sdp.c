#include "_mpi.h"

/**
 * @brief TODO
 * @return
 */
bool send_stop_to_host() {
  // TODO: Implementing as User Command in ACP
  sdp_msg_t *msg = sark_msg_get();

  if (msg == NULL) {
    debug_printf("[MPI-SDP] Error in get SDP buffer");
    return false;
  }

  // --- SDP Header ---
  msg->flags = 0x07;
  msg->tag = 1;
  msg->dest_port = PORT_ETH;
  msg->dest_addr = sv->dbg_addr;
  msg->srce_port = spin1_get_core_id();
  msg->srce_addr = spin1_get_chip_id();
  msg->length = 16;

  // --- SCP/ACP Header ---
  msg->cmd_rc = MPI_ACP_STOP; // 0xFF;
  msg->seq = 0;
  msg->arg1 = 0;
  msg->arg2 = 0;
  msg->arg3 = 0;

  // --- Send ---
  if (!sark_msg_send(msg, 10)) {
    debug_printf("[MPI-SDP] Error, impossible send the SDP\n");
    return false;
  }

  debug_printf("[MPI-SDP] Send STOP to HOST\n");

  sark_msg_free(msg);
  return true;
}
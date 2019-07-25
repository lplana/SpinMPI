#include "_mpi.h"


// --- Global Variables ---

mpi_communicator_t comm_world;
mpi_processor_t *processor = NULL;


// --- Functions ---

void context_init(uint32_t rank, uint32_t rank_max) {
  uint32_t i;

  comm_world.rank_max = rank_max;
  comm_world.processors = sark_alloc(rank_max, sizeof(struct mpi_communicator));
  comm_world.processors_enabled = sark_alloc(rank_max, sizeof(bool));

  for (i = 0; i < rank_max; i++)
    comm_world.processors_enabled[i] = false;

  processor = &comm_world.processors[rank];
  processor->x = get_chip_x();
  processor->y = get_chip_y();
  processor->p = get_chip_p();
  processor->rank = rank;
  comm_world.processors_enabled[rank] = true;

  debug_printf("[MPI-CORE] Obtained rank %d/%d\n", rank, rank_max - 1);
  acp_me_create(MPI_ACP_VARCODE_CONTEXT, 0, NULL, NULL);

  mpi_processor_t p;
  uint16_t length;

  if (rank != 0) {
    for (i = 0; i < rank_max; i++) {
      debug_printf("[MPI-CORE] Wait ...\n");

      acp_me_read(
        MPI_ACP_VARCODE_PROCESSOR_BCAST,
        (uint8_t *)(&p), 
        sizeof(mpi_processor_t),
        ACP_CHANNEL_BROADCAST,
        NULL, 
        true);

      if(p.rank != processor->rank) {
        comm_world.processors[p.rank] = p;
        comm_world.processors_enabled[p.rank] = true;
      }

      debug_printf("[MPI-CORE] Obtained rank %d\n", p.rank);
    }
  } else {
    for (i = 0; i < rank_max; i++) {
        acp_me_read(
          MPI_ACP_VARCODE_PROCESSOR,
          (uint8_t *)&p, 
          sizeof(mpi_processor_t),
          ACP_CHANNEL_HOST, 
          NULL, 
          true);
        
        if(p.rank != processor->rank) {
          comm_world.processors[p.rank] = p;
          comm_world.processors_enabled[p.rank] = true;
        }
        acp_me_update(
          MPI_ACP_VARCODE_PROCESSOR_BCAST,
          (uint8_t *)&p, 
          sizeof(mpi_processor_t),
          ACP_CHANNEL_BROADCAST,
          NULL, 
          true);
    }
  }
  
  return;
}


void context_add_node(uint16_t varcode, uint8_t *data, uint16_t length) {
  mpi_processor_t *p = (mpi_processor_t *) data;

  if (processor != NULL && p->rank < comm_world.rank_max) {
    
    debug_printf("[MPI-CORE] Obtained rank %d\n", p->rank);
    if(p->rank != processor->rank) {
      comm_world.processors[p->rank] = *p;
      comm_world.processors_enabled[p->rank] = true;
    }
    acp_me_update(
      MPI_ACP_VARCODE_PROCESSOR_BCAST,
      data, 
      sizeof(mpi_processor_t),
      ACP_CHANNEL_BROADCAST,
      NULL, 
      true);
  }

  return;
}


bool context_check() {
  uint32_t i;
  bool r = true;
  if (comm_world.processors_enabled != NULL) {
    for (i = 0; i < comm_world.rank_max; i++) {
      if (!comm_world.processors_enabled[i])
        return false;
    }
    return true;
  }

  return false;
}


uint32_t get_rank() {
  return processor->rank;
}


uint8_t get_chip_x() {
  return (uint8_t)(((uint32_t) sv->p2p_addr) >> 8);
}


uint8_t get_chip_y() {
  return (uint8_t)(((uint32_t) sv->p2p_addr) & 0xFF);
}


uint8_t get_chip_p() {
  return processor->p = (uint8_t) sark.virt_cpu;
}


uint32_t get_comm_size(uint32_t comm) {
  if (comm == MPI_COMM_WORLD)
    return comm_world.rank_max;

  return 0;
}


spin2_core_t get_comm_destination(uint32_t comm, uint32_t rank){
  spin2_core_t destination = {0};
  if (comm == MPI_COMM_WORLD) {
    debug_printf("[MPI-CORE] Get address from rank:\n");
    debug_printf("  (rank) %04x -> (x:y:p) %02x : %02x : %02x\n",
                 rank,
                 comm_world.processors[rank].x,
                 comm_world.processors[rank].y,
                 comm_world.processors[rank].p);
    destination.x = comm_world.processors[rank].x;
    destination.y = comm_world.processors[rank].y;
    destination.p = comm_world.processors[rank].p;
  }
  else {
    debug_printf("[MPI-CORE] Communicators not supported");
    rt_error(RTE_ABORT);
  }

  return destination;
}


uint32_t get_comm_rank(uint32_t comm) {
  if (comm == MPI_COMM_WORLD) {
    return processor->rank;
  }

  return (uint32_t)0;
}


uint32_t byteof(MPI_Datatype d) {
  if (
      d == MPI_BYTE ||
      d == MPI_CHARACTER ||
      d == MPI_LOGICAL ||
      d == MPI_INT8_T ||
      d == MPI_UINT8_T) {
    return sizeof(uint8_t);
  }
  else if (
      d == MPI_INT16_T ||
      d == MPI_UINT16_T) {
    return sizeof(uint16_t);
  }
  else if (
      d == MPI_INTEGER ||
      d == MPI_REAL ||
      d == MPI_INT32_T ||
      d == MPI_UINT32_T) {
    return sizeof(uint32_t);
  }
  else if (
      d == MPI_DOUBLE_PRECISION ||
      d == MPI_INT64_T ||
      d == MPI_UINT64_T) {
    return sizeof(uint64_t);
  }

  return (uint32_t)0;
}


// --- Comm Primitives Unicast

void _send_ucast(
    uint32_t mpi_rank_to, 
    uint8_t *buffer, 
    uint32_t buffer_length,
    bool sync) {

  uint8_t *data;

  uint32_t bytes;
  uint16_t to_send;
  uint16_t send;

  spin2_core_t core;

  data = buffer;
  bytes = buffer_length;
  core = get_comm_destination(MPI_COMM_WORLD, mpi_rank_to);
  
  do {
    // Byte to send
    to_send = (bytes < COMM_BUFFER) ? bytes : COMM_BUFFER;

    // ACP send
    send = acp_me_update(
      MPI_ACP_VARCODE_CONTEXT,
      data, 
      to_send,
      ACP_CHANNEL_CORE,
      &core, 
      sync);

    // Byte count update
    data += send;
    bytes -= send;
  } while (bytes > 0);

  return;
}


void _recv_ucast(
    uint32_t mpi_rank_from, 
    uint8_t *buffer, 
    uint32_t buffer_length,
    bool sync) {

  uint8_t *data;

  uint32_t bytes;
  uint16_t to_recv;
  uint16_t recv;

  spin2_core_t core;

  data = buffer;
  bytes = buffer_length;
  core = get_comm_destination(MPI_COMM_WORLD, mpi_rank_from);

  do {
    // Byte to receive
    to_recv = (bytes < COMM_BUFFER) ? bytes : COMM_BUFFER;
    
    // ACP receive
    recv = acp_me_read(
      MPI_ACP_VARCODE_CONTEXT,
      data, 
      to_recv,
      ACP_CHANNEL_CORE,
      &core, 
      sync);

    // Byte count update
    data += recv;
    bytes -= recv;
  } while (bytes > 0);

  return;
}


// --- Comm Primitives Broadcast
void _send_all(
    uint32_t mpi_rank_src, 
    uint8_t *buffer, 
    uint32_t buffer_length,
    bool sync) {

  uint8_t *data;

  uint32_t bytes;
  uint16_t to_send;
  uint16_t send;

  data = buffer;
  bytes = buffer_length;
  
  do {
    // Byte to send
    to_send = (bytes < COMM_BUFFER) ? bytes : COMM_BUFFER;

    // ACP send
    send = acp_me_update(
      MPI_ACP_VARCODE_CONTEXT,
      data, 
      to_send,
      ACP_CHANNEL_BROADCAST,
      NULL, 
      sync);

    // Byte count update
    data += send;
    bytes -= send;
  } while (bytes > 0);

  return;
}


void _recv_all(
    uint32_t mpi_rank_src, 
    uint8_t *buffer, 
    uint32_t buffer_length,
    bool sync) {

  spin2_core_t source;
  uint8_t *data;

  uint32_t bytes;
  uint16_t to_recv;
  uint16_t recv;

  data = buffer;
  bytes = buffer_length;
  source = get_comm_destination(MPI_COMM_WORLD, mpi_rank_src);

  do {
    // Byte to receive
    to_recv = (bytes < COMM_BUFFER) ? bytes : COMM_BUFFER;
    
    // ACP receive
    recv = acp_me_read(
      MPI_ACP_VARCODE_CONTEXT,
      data, 
      to_recv,
      ACP_CHANNEL_BROADCAST,
      NULL, 
      sync);

    // Byte count update
    data += recv;
    bytes -= recv;
  } while (bytes > 0);

  return;
}
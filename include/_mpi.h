#ifndef __SPINN_MPI_PRIVATE_H__
#define __SPINN_MPI_PRIVATE_H__

#include "mpi.h"
#include "acp.h"

#ifdef MPI_DEBUG
#define debug_printf(...) \
  do { io_printf(IO_BUF, __VA_ARGS__); } while (0)
#else
#define debug_printf(...) \
  do { } while (0)
#endif

#define error_printf(...) \
  do { io_printf(IO_BUF, __VA_ARGS__); } while (0)


#define MPI_ACP_STOP 0x100

#define MPI_ACP_VARCODE_RANK 0x00
#define MPI_ACP_VARCODE_RANK_MAX 0x01
#define MPI_ACP_VARCODE_PROCESSOR 0x0A
#define MPI_ACP_VARCODE_PROCESSOR_BCAST 0x0B
#define MPI_ACP_VARCODE_CONTEXT 0x10

#define MPI_ACP_VARCODE_SYNC1 0x05
#define MPI_ACP_VARCODE_SYNC2 0x06
#define MPI_ACP_VARCODE_SYNC3 0x07

//#define MPI_ACP_BUFFER_SIZE_MAX 64
//#define FRAGMENT_TIMEOUT 50
//#define COMM_BUFFER 64

#define COMM_BUFFER 64  // Can be increased until 


// --- Struct ---

struct mpi_processor {
  uint8_t x;
  uint8_t y;
  uint8_t p;
  uint32_t rank;
};

struct mpi_communicator {
    struct mpi_processor *processors;
    bool *processors_enabled;
    uint rank_max;
};


// --- Typedefs ---

typedef uint8_t *uint8_p;

typedef struct mpi_processor mpi_processor_t;
typedef struct mpi_communicator mpi_communicator_t;


// --- Functions ---

void context_init(uint32_t rank, uint32_t rank_max);
void context_add_node(uint16_t varcode, uint8_t *data, uint16_t length);
bool context_check();

uint8_t get_chip_x();
uint8_t get_chip_y();
uint8_t get_chip_p();

uint32_t get_rank();
uint32_t get_comm_size(uint32_t comm);
uint32_t get_comm_rank(uint32_t comm);

spin2_core_t get_comm_destination(
  uint32_t comm, 
  uint32_t rank);

bool send_stop_to_host();
uint32_t byteof(MPI_Datatype d);


// --- Unicast primitive ---

void _send_ucast(
    uint32_t mpi_rank_to, 
    uint8_t *buffer, 
    uint32_t buffer_length,
    bool sync);


void _recv_ucast(
    uint32_t mpi_rank_from, 
    uint8_t *buffer, 
    uint32_t buffer_length,
    bool sync);


// --- Broadcast primitive ---

void _send_all(
    uint32_t mpi_rank_src, 
    uint8_t *buffer, 
    uint32_t buffer_length,
    bool sync);


void _recv_all(
    uint32_t mpi_rank_src, 
    uint8_t *buffer, 
    uint32_t buffer_length,
    bool sync);

#endif //__SPINN_MPI_PRIVATE_H__

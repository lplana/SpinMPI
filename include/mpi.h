/**
 * @file mpi.h
 * @author Barchi Francesco
 * @date 2017-2018
 * @brief The Message Passing Interface for SpiNNaker
 *
 */

#ifndef __SPINN_MPI_H__
#define __SPINN_MPI_H__

#include "spin2_api.h"

//#define MPI_DEBUG
#define MPI_VERSION "19w19"

#define MPI_COMM_WORLD 0
#define MPI_STATUS_NULL 0
#define MPI_STATUS_IGNORE NULL
#define MPI_SUCCESS 0
#define MPI_ERR_COMM 1
#define MPI_MAX_PROCESSOR_NAME 25

typedef enum {
  // 1 Byte
  MPI_BYTE,
  MPI_CHARACTER,
  MPI_LOGICAL,
  MPI_INT8_T,
  MPI_UINT8_T,

  // 2 Byte
  MPI_INT16_T,
  MPI_UINT16_T,

  // 4 Byte
  MPI_INTEGER,
  MPI_REAL,
  MPI_INT32_T,
  MPI_UINT32_T,

  // 8 Byte
  MPI_DOUBLE_PRECISION,
  MPI_INT64_T,
  MPI_UINT64_T,

  // Variable Byte
  MPI_COMPLEX,
  MPI_PACKED

} MPI_Datatype;

typedef unsigned int MPI_Comm;
typedef unsigned int MPI_Status;
typedef event_proc MPI_Main;


/**
 * MPI SPINN
 *
 * @param handler
 */
int MPI_Spinn(MPI_Main handler);


/**
 * MPI INIT
 *
 * @param argc
 * @param argv
 * @return
 */
int MPI_Init(int *argc, char ***argv);


/**
 * MPI FINALIZE
 *
 * @return
 */
int MPI_Finalize();


/**
 * MPI GET PROCESSOR NAME
 *
 * @param name
 * @param resultlen
 * @return
 */
int MPI_Get_processor_name(char *name, int *resultlen);


/**
 * MPI COMM RANK
 *
 * @param comm Communicator (handle)
 * @param rank The compute node's mpi rank
 */
int MPI_Comm_rank(MPI_Comm comm, int *rank);


/**
 * MPI COMM SIZE
 *
 * @param comm Communicator (handle)
 * @param rank The communicator size
 */
int MPI_Comm_size(MPI_Comm comm, int *size);


/**
 * MPI SEND
 *
 * @param buf: Initial address of send buffer
 * @param count: Number of elements in send buffer (non-negative integer)
 * @param datatype: The data-type of each send buffer element (handle)
 * @param dest: Rank of destination (integer)
 * @param tag: Message tag (integer)
 * @param comm: Communicator (handle)
 *
 * */
int MPI_Send(const void *buf,
             int count,
             MPI_Datatype datatype,
             int dest,
             int tag,
             MPI_Comm comm);


/**
 * MPI RECV [Blocking]
 *
 * @param buf: Initial address of receive buffer (choice)
 * @param count: Number of elements in receive buffer (non-negative integer)
 * @param datatype: The data-type of each receive buffer element (handle)
 * @param source: Rank of source or MPI_ANY_SOURCE (integer)
 * @param tag: Message tag or MPI_ANY_TAG (integer)
 * @param comm: Communicator (handle)
 * @param status: Status object (Status)
 */
int MPI_Recv(void *buf,
             int count,
             MPI_Datatype datatype,
             int source,
             int tag,
             MPI_Comm comm,
             MPI_Status *status);


/**
 *
 * @param buffer
 * @param count
 * @param datatype
 * @param root
 * @param comm
 * @return
 */
int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);


/**
 *
 * @param sendbuf
 * @param sendcount
 * @param sendtype
 * @param recvbuf
 * @param recvcount
 * @param recvtype
 * @param root
 * @param comm
 * @return
 */
int MPI_Scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                void *recvbuf, int recvcount, MPI_Datatype recvtype,
                int root, MPI_Comm comm);


/**
 *
 * @param sendbuf
 * @param sendcount
 * @param sendtype
 * @param recvbuf
 * @param recvcount
 * @param recvtype
 * @param root
 * @param comm
 * @return
 */
int MPI_Gather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
               void *recvbuf, int recvcount, MPI_Datatype recvtype,
               int root, MPI_Comm comm);


/**
 *
 * @param sendbuf
 * @param sendcount
 * @param sendtype
 * @param recvbuf
 * @param recvcount
 * @param recvtype
 * @param comm
 * @return
 */
int MPI_Allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                  void *recvbuf, int recvcount, MPI_Datatype recvtype,
                  MPI_Comm comm);


/**
 * MPI BARRIER [Blocking]
 *
 */
int MPI_Barrier(MPI_Comm comm);

#endif //__SPINN_MPI_H__

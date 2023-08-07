#ifndef NOMPI_H
#define NOMPI_H
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef int MPI_Comm;
typedef int MPI_Request;
typedef size_t MPI_Datatype;
typedef int MPI_Op;
typedef int MPI_Status;

const MPI_Comm MPI_COMM_WORLD = 0;

#define MPI_SUCCESS 0

#define MPI_INT sizeof(int)
#define MPI_FLOAT sizeof(float)
#define MPI_DOUBLE sizeof(double)
#define MPI_CHAR sizeof(char)
#define MPI_BYTE sizeof(byte)
#define MPI_LONG_LONG sizeof(long long)
#define MPI_UNSIGNED sizeof(unsigned)

#define MPI_SUM 1
#define MPI_MAX 2
#define MPI_MIN 3
#define MPI_MAXLOC 4
#define MPI_MINLOC 5
#define MPI_LOR 6

#define MPI_UNDEFINED -1
#define MPI_COMM_NULL -1
#define MPI_ANY_SOURCE -1
#define MPI_IN_PLACE NULL
#define MPI_MAX_PROCESSOR_NAME 128


inline int MPI_Init(int *argc, char ***argv) { return 0; }

inline void MPI_Get_processor_name(char *name, int *resultlen) {
  const char host[] = "localhost";
  int len = strlen(host);
  if (name) memcpy(name,host,len+1);
  if (resultlen) *resultlen = len;
}

inline int MPI_Comm_rank(MPI_Comm comm, int *rank) { *rank = 0; return 0; }
inline int MPI_Comm_size(MPI_Comm comm, int *size) { *size = 1; return 0; }
inline int MPI_Abort(MPI_Comm comm, int errorcode=-1) { exit(errorcode); return 0; }
inline int MPI_Finalize() { return 0; }
inline int MPI_Type_size(MPI_Datatype datatype, int *size) { *size = datatype; return 0; }
inline int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) {
  assert(false);
  return 0;
}
inline int MPI_Isend(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
  assert(false);
  return 0;
}
inline int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status) {
  assert(false);
  return 0;
}
inline int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request) {
  assert(false);
  return 0;
}
inline int MPI_Wait(MPI_Request *request, MPI_Status *status) {
  assert(false);
  return 0;
}
inline int MPI_Waitall(int n, MPI_Request *request, MPI_Status *status) {
  assert(false);
  return 0;
}
inline int MPI_Waitany(int count, MPI_Request *request, int *index, MPI_Status *status) {
  assert(false);
  return 0;
}
inline int MPI_Sendrecv(void *sbuf, int scount, MPI_Datatype sdatatype, int dest, int stag, void *rbuf, int rcount,
                 MPI_Datatype rdatatype, int source, int rtag, MPI_Comm comm, MPI_Status *status) {
  assert(false);
  return 0;
}

inline int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count) {
  assert(false);
  return 0;
}

inline int MPI_Get_version(int *version, int *subversion) {
  *version = 0;
  *subversion = 1;
  return 0;
}

inline int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *comm_out) {
  *comm_out = comm;
  return 0;
}

inline int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *comm_out) {
  *comm_out = comm;
  return 0;
}

inline int MPI_Comm_free(MPI_Comm *comm) { return 0; }


inline int MPI_Cart_create(MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart) {
  *comm_cart = comm_old;
  return 0;  
}

inline int MPI_Cart_get(MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords) {
  dims[0] = dims[1] = dims[2] = 1;
  periods[0] = periods[1] = periods[2] = 1;
  coords[0] = coords[1] = coords[2] = 0;
  return 0;
}

inline int MPI_Cart_shift(MPI_Comm comm, int direction, int displ, int *source, int *dest) {
  *source = 0;
  *dest = 0;
  return 0;
}

inline int MPI_Cart_rank(MPI_Comm comm, int *coords, int *rank) {
  *rank = 0;
  return 0;
}

inline int MPI_Barrier(MPI_Comm comm) { return 0; }

inline int MPI_Bcast(void *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm) { return 0; }

inline int MPI_Allreduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
  if (sendbuf == MPI_IN_PLACE || recvbuf == MPI_IN_PLACE) return 0;
  memcpy(recvbuf, sendbuf, count * datatype);
  return 0;
}

inline int MPI_Reduce(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
  if (sendbuf == MPI_IN_PLACE || recvbuf == MPI_IN_PLACE) return 0;
  memcpy(recvbuf, sendbuf, count * datatype);
  return 0;
}

inline int MPI_Scan(void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
  if (sendbuf == MPI_IN_PLACE || recvbuf == MPI_IN_PLACE) return 0;
  memcpy(recvbuf, sendbuf, count * datatype);
  return 0;
}

inline int MPI_Allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                  void *recvbuf, int recvcount, MPI_Datatype recvtype,
                  MPI_Comm comm) {
  assert(sendcount == recvcount);
  assert(sendtype == recvtype);
  if (sendbuf == MPI_IN_PLACE || recvbuf == MPI_IN_PLACE) return 0;
  memcpy(recvbuf, sendbuf, sendcount * sendtype);
  return 0;
}


inline int MPI_Allgatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                   void *recvbuf, int *recvcounts, int *displs,
                   MPI_Datatype recvtype, MPI_Comm comm) {
  assert(sendcount == recvcount);
  assert(sendtype == recvtype);
  if (sendbuf == MPI_IN_PLACE || recvbuf == MPI_IN_PLACE) return 0;
  memcpy(recvbuf+displs[0], sendbuf, sendcount * sendtype);
  return 0;
}

inline int MPI_Reduce_scatter(void *sendbuf, void *recvbuf, int *recvcounts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
  int count = recvcounts[0];
  if (sendbuf == MPI_IN_PLACE || recvbuf == MPI_IN_PLACE) return 0;
  memcpy(recvbuf, sendbuf, count * datatype);
  return 0;
}

inline int MPI_Gather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
               void *recvbuf, int recvcount, MPI_Datatype recvtype,
               int root, MPI_Comm comm) {
  assert(sendcount == recvcount);
  assert(sendtype == recvtype);
  if (sendbuf == MPI_IN_PLACE || recvbuf == MPI_IN_PLACE) return 0;
  memcpy(recvbuf, sendbuf, sendcount * sendtype);
  return 0;
}

inline int MPI_Gatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int *recvcounts, int *displs,
		MPI_Datatype recvtype, int root, MPI_Comm comm) {
  assert(sendcount == recvcount);
  assert(sendtype == recvtype);
  if (sendbuf == MPI_IN_PLACE || recvbuf == MPI_IN_PLACE) return 0;
  memcpy(recvbuf+displs[0], sendbuf, sendcount * sendtype);
}

inline int MPI_Scatterv(void *sendbuf, int *sendcounts, int *displs,
		 MPI_Datatype sendtype, void *recvbuf, int recvcount,
		 MPI_Datatype recvtype, int root, MPI_Comm comm) {
  assert(sendcount == recvcount);
  assert(sendtype == recvtype);
  if (sendbuf == MPI_IN_PLACE || recvbuf == MPI_IN_PLACE) return 0;
  memcpy(recvbuf, sendbuf+displs[0], sendcount * sendtype);
}

#ifdef __cplusplus
}
#endif
#endif

#ifndef NOMPI_H
#define NOMPI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

inline int mpi_error(char* a, char* b, char* c, int d) {
    printf("NoMPI: illegal call of %s(%s) in %s at line %d\n", a, b, c, d);
    exit(-1);
    return -1;
}

#define mpi_illegal(a__,b__) mpi_error(#a__,b__,__FILE__, __LINE__)

typedef int MPI_Comm;
typedef int MPI_Request;
// typedef size_t MPI_Datatype;
struct MPI_Datatype {
    size_t size;
    inline MPI_Datatype(void * ptr=NULL) : size(0) { assert( ptr == NULL ); }
    inline operator void* () { if (size) return this; else return NULL; }
};
inline MPI_Datatype mpi_datatype(size_t size_) {
  MPI_Datatype ret;
  ret.size = size_;
  return ret;
}
typedef size_t MPI_Aint;
typedef int MPI_Op;
typedef int MPI_Status;
typedef int MPI_Group;
typedef int MPI_Info;

const MPI_Comm MPI_COMM_WORLD = 1;

#define MPI_SUCCESS 0
#define MPI_INFO_NULL 0
#define MPI_STATUSES_IGNORE 0
#define MPI_INT mpi_datatype(sizeof(int))
#define MPI_FLOAT mpi_datatype(sizeof(float))
#define MPI_DOUBLE mpi_datatype(sizeof(double))
#define MPI_CHAR mpi_datatype(sizeof(char))
#define MPI_BYTE mpi_datatype(1)
#define MPI_LONG mpi_datatype(sizeof(long int))
#define MPI_LONG_LONG mpi_datatype(sizeof(long long))
#define MPI_UNSIGNED mpi_datatype(sizeof(unsigned))
#define MPI_UNSIGNED_LONG mpi_datatype(sizeof(unsigned long int))

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

#define MPI_UNIVERSE_SIZE 101

// int MPI_Comm_spawn(const char *command, char *argv[], int maxprocs, MPI_Info info,
//                                   int root, MPI_Comm comm, MPI_Comm *intercomm,
//                                   int array_of_errcodes[]);
#define MPI_Comm_spawn(X...) mpi_illegal(MPI_Comm_spawn, #X)

// int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader,
//                                         MPI_Comm bridge_comm, int remote_leader,
//                                         int tag, MPI_Comm *newintercomm);
#define MPI_Intercomm_create(X...) mpi_illegal(MPI_Intercomm_create, #X)
// int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
//                              int tag, MPI_Comm comm, MPI_Request *request);
#define MPI_Irecv(X...) mpi_illegal(MPI_Irecv, #X)
// int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest,
//                              int tag, MPI_Comm comm, MPI_Request *request);
#define MPI_Isend(X...) mpi_illegal(MPI_Isend, #X)
// int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source,
//                             int tag, MPI_Comm comm, MPI_Status *status);
#define MPI_Recv(X...) mpi_illegal(MPI_Recv, #X)
// int MPI_Type_indexed(int count, const int array_of_blocklengths[],
//                                     const int array_of_displacements[],
//                                     MPI_Datatype oldtype, MPI_Datatype *newtype);
#define MPI_Type_indexed(X...) mpi_illegal(MPI_Type_indexed, #X)

#define MPI_Group_excl(X...) mpi_illegal(MPI_Group_excl, #X)
#define MPI_Type_create_resized(X...) mpi_illegal(MPI_Type_create_resized, #X)


#define _mpi_world 1

inline int mpi_cp(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype) {
    memcpy(recvbuf, sendbuf, count*datatype.size);
    return 0;
}

inline int MPI_Abort(MPI_Comm comm, int errorcode){
    exit(-1);
    return -1;
}
inline int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm) {
    return mpi_cp(sendbuf, recvbuf, count, datatype);
}
inline int MPI_Barrier(MPI_Comm comm) { return 0; }
inline int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) { return 0; }
inline int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm) { *newcomm = comm; return 0; }
inline int MPI_Comm_free(MPI_Comm *comm) { return 0; }
inline int MPI_Comm_get_attr(MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag) {
    const static int _mpi_one = 1;
    switch( comm_keyval ) {
    case MPI_UNIVERSE_SIZE:
        *((const void**)attribute_val) = &_mpi_one;
        *flag = true;
        break;
    default:
        *flag = true;
        break;
    }
    return 0;
}
inline int MPI_Comm_get_parent(MPI_Comm *parent) { *parent = MPI_COMM_NULL; return 0; }
inline int MPI_Comm_group(MPI_Comm comm, MPI_Group *group) { *group = 1; return 0; };
inline int MPI_Comm_rank(MPI_Comm comm, int *rank) { *rank = 0; return 0; };
inline int MPI_Comm_remote_size(MPI_Comm comm, int *size) { *size = 1; return 0; };
inline int MPI_Comm_size(MPI_Comm comm, int *size) { *size = 1; return 0; };

inline int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm) { *newcomm = comm; return 0; }
inline int MPI_Finalize(void) { return 0; };
inline int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                              void *recvbuf, int recvcount, MPI_Datatype recvtype,
                              int root, MPI_Comm comm){
    assert(false);
}
inline int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                               void *recvbuf, const int recvcounts[], const int displs[],
                               MPI_Datatype recvtype, int root, MPI_Comm comm) {
  assert(false);
}
inline int MPI_Get_processor_name(char *name, int *resultlen){
  const char* name_ = "localhost";
  strcpy(name,name_);
  *resultlen = strlen(name_);
  return 0;
}
inline int MPI_Group_rank(MPI_Group group, int *rank) { *rank = 0; return 0; };
inline int MPI_Group_size(MPI_Group group, int *size) { *size = 1; return 0; };
inline int MPI_Init(int *argc, char ***argv) {
    printf("NoMPI: Init\n");
    return 0;
}
inline int MPI_Reduce(const void *sendbuf, void *recvbuf, int count,
                              MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm) {
    return mpi_cp(sendbuf, recvbuf, count, datatype);
}
inline int MPI_Scatterv(const void *sendbuf, const int sendcounts[], const int displs[],
                                MPI_Datatype sendtype, void *recvbuf, int recvcount,
                                MPI_Datatype recvtype, int root, MPI_Comm comm){
                                    assert(false);
                                }
inline int MPI_Type_commit(MPI_Datatype *type) { return 0; }
inline int MPI_Type_contiguous(int count, MPI_Datatype oldtype,
                                       MPI_Datatype *newtype) {
    *newtype = mpi_datatype(oldtype.size * count);
    return 0;
}
//inline int MPI_Type_create_resized(MPI_Datatype oldtype, MPI_Aint lb,
//                                           MPI_Aint extent, MPI_Datatype *newtype);
inline int MPI_Type_free(MPI_Datatype *type) { return 0; }
inline int MPI_Type_get_extent(MPI_Datatype type, MPI_Aint *lb,
                                       MPI_Aint *extent) {
    *lb = 0;
    *extent = type.size;
    return 0;
}
inline int MPI_Type_size(MPI_Datatype type, int *size) {
    *size = type.size;
    return 0;
}
inline int MPI_Wait(MPI_Request *request, MPI_Status *status) { return 0; }
inline int MPI_Waitall(int count, MPI_Request array_of_requests[],
                               MPI_Status *array_of_statuses) { return 0; }
inline int MPI_Waitany(int count, MPI_Request array_of_requests[],
                               int *index, MPI_Status *status) { return 0; }
inline double MPI_Wtime(void) { return 0.0; }

#endif

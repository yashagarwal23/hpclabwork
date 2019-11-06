#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);


    for(long long int i = 0; i < 1e10; i++);


    printf("Hello world from processor %s, rank %d out of %d processors , process : %d\n",
           processor_name, world_rank, world_size, (int)getpid());

    MPI_Finalize();
}

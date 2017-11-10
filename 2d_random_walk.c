#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct info {
  int l, a, b, n, N;
  float pl, pr, pu, pd;
  int numprocs, myid;
  int seed;
  int *particles_loc;
} info;

void printArray(void *d){
  info *data = d;
  for (int i = 0; i < data->N * data->a * data->b; ++i)
        printf("(%d %d)  ",  data->particles_loc[i*2], data->particles_loc[i*2 +1]);
  printf("\n\n");
}

int initialization(void *d){
  info *data = d;

  //data->particles_loc = (int*)malloc(2 * data->N * data->a * data->b * sizeof(int));
  memset(data->particles_loc, 0, sizeof(data->particles_loc));

  srand(time(NULL));

  int inner_x, inner_y;
  //fill array with random numbers
  int chest_num = -1;
  for (int i=0; i < data->N * data->a * data->b; ++i){
        if (i % data->N == 0)
                  ++chest_num;

                int chest_num_x = chest_num % data->a;
                int chest_num_y = (int)floor(chest_num / (float)data->b);

                inner_x = ((rand() % data->l) + chest_num_x * data->l);
            inner_y = ((rand() % data->l) + chest_num_y * data->l);
        data->particles_loc[i * 2] = inner_x;
                data->particles_loc[i * 2 + 1] = inner_y;
  }

  int *seeds = (int*) malloc(data->numprocs * sizeof(int));
  for (int i=0; i < data->numprocs; ++i){
        seeds[i] = rand();
  }
  //give seeds for all;
  MPI_Scatter(seeds, 1, MPI_INT, &data->seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

int simple_module_capture(int number, int module){
  if (number < 0)
        return module - number;
  return (number % module);
}

void go_random(void *d){
  info *data = d;
  int dest;
  int step[4][2];

  step[0][0] = -1;
  step[0][1] = 0;
  step[1][0] = 1;
  step[1][1] = 0;
  step[2][0] = 0;
  step[2][1] = 1;
  step[3][0] = 0;
  step[3][1] = -1;

  srand(data->seed);

  for (int walk_time = 0; walk_time <= data->n; ++walk_time)
  {
        for (int partic_num = data->myid; partic_num < data->N * data->a * data->b; partic_num += data->numprocs)
        {
          //if (data->myid == 1){
                //printf("now, in tread %d, particle num= %d\n", data->myid, partic_num);
                //printf("arr[i] = %d\n", data->particles_loc[partic_num]);
          //}
          float r = rand() / (float)RAND_MAX ;
          if (r >  data->pd)
                dest = 3;
          if (r > data->pu)
                dest = 2;
          if (r > data->pr)
                dest = 1;
          if (r > data->pl)
                 dest = 0;
         data->particles_loc[partic_num * 2] = simple_module_capture(data->particles_loc[partic_num * 2] + step[dest][0], data->a * data->l);
         data->particles_loc[partic_num * 2 + 1] = simple_module_capture(data->particles_loc[partic_num * 2 + 1] + step[dest][1], data->b * data->l);
        }
  }
}



int main(int argc, char *argv[])
{

  MPI_Init(&argc, &argv);
  int numprocs, myid;
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);

  info data = {
  .l = atoi(argv[1]),
  .a = atoi(argv[2]),
  .b = atoi(argv[3]),
  .n = atoi(argv[4]),
  .N = atoi(argv[5]),
  .pl = atof(argv[6]),
  .pr = atof(argv[7]),
  .pu = atof(argv[8]),
  .pd = atof(argv[9]),
  .numprocs = numprocs,
  .myid = myid
  };
  data.particles_loc = (int*)malloc(2 * data.N * data.a * data.b * sizeof(int));

  if (myid == 0)
        initialization(&data);

/*  int l, a, b, n, N;
  float pl, pr, pu, pd;
  int numprocs, myid;
  int *particles_loc;
*/
  printf("stsrting copy\n");
  MPI_Datatype datatype;
  MPI_Type_contiguous(2 * data.a * data.b * data.N, MPI_INT, &datatype);
  MPI_Type_commit(&datatype);
  printf("commit_type\n");
  if (myid == 0) {
    printf("create_type\n");
        MPI_Send(data.particles_loc, 1, datatype, 1, 0, MPI_COMM_WORLD);
        printArray(&data);
  } else {
        printf("in auxilary thread\n");
        MPI_Recv(data.particles_loc, 1, datatype, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("get_type\n");
        printArray(&data);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  go_random(&data);




  free(data.particles_loc);
  MPI_Finalize();
  //to-do: слить уже оходившие частицы с разных потоков
  printArray(&data);

  //printf("END\n\n");
  return 0;
}

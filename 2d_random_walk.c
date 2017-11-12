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
  memset(data->particles_loc, 0, sizeof(data->particles_loc));
  srand(time(NULL));

  int inner_x, inner_y;
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

  int particles_amount = data->a * data->b * data->N;
  int chank_size = (int) floor((particles_amount) / (float)data->numprocs);
  if (particles_amount % data->numprocs !=0)
        ++chank_size;
  for (int walk_time = 0; walk_time <= data->n; ++walk_time)
  {
        for (int partic_num = data->myid * chank_size; partic_num < particles_amount && partic_num < (data->myid + 1) * chank_size; ++partic_num)
        {
          float r = rand() / (float)RAND_MAX ;
          dest = 3;
          if (r < data->pu)
                dest = 2;
          if (r < data->pr)
                dest = 1;
          if (r < data->pl)
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
  data.pr += data.pl;
  data.pu += data.pr;
  data.pd += data.pu;
  data.particles_loc = (int*)malloc(2 * data.N * data.a * data.b * sizeof(int));

  if (myid == 0)
        initialization(&data);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Datatype datatype;
  MPI_Type_contiguous(2 * data.a * data.b * data.N, MPI_INT, &datatype);
  MPI_Type_commit(&datatype);
  //printf("commit_type\n");
  if (myid == 0) {
                for (int to_proc = 1; to_proc < data.numprocs; ++to_proc)
          MPI_Send(data.particles_loc, 1, datatype, to_proc, 0, MPI_COMM_WORLD);
        printArray(&data);
  } else {
        MPI_Recv(data.particles_loc, 1, datatype, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  go_random(&data);
  MPI_Barrier(MPI_COMM_WORLD);

  int particles_amount = data.a * data.b * data.N;
  int chank_size = (int) floor((particles_amount) / (float)data.numprocs);
  if (particles_amount % data.numprocs !=0)
    ++chank_size;


  MPI_Datatype return_datatype;
  MPI_Type_contiguous(chank_size * 2, MPI_INT, &return_datatype);
  MPI_Type_commit(&return_datatype);

  MPI_Datatype return_tail_datatype;
  MPI_Type_contiguous( 2 * (particles_amount % chank_size), MPI_INT, &return_tail_datatype);
  MPI_Type_commit(&return_tail_datatype);

  if (myid != 0){
        if (data.myid == data.numprocs - 1 && (particles_amount % chank_size) != 0){
                MPI_Send(&data.particles_loc[data.myid *2* chank_size], 1, return_tail_datatype, 0, 0, MPI_COMM_WORLD);
    }
        else{
                MPI_Send(&data.particles_loc[data.myid *2* chank_size], 1, return_datatype, 0, 0, MPI_COMM_WORLD);
        }
  } else {
        int *buf = (int*)malloc(chank_size *2* sizeof(int));
        for (int from_proc = 1; from_proc < data.numprocs - 1; ++from_proc){
          MPI_Recv(buf, 1, return_datatype, from_proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          memcpy(&data.particles_loc[from_proc *2* chank_size], buf, chank_size *2* sizeof(int));
        }

        if ((particles_amount % chank_size) != 0){
          MPI_Recv(buf, 1, return_tail_datatype, data.numprocs - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          memcpy(&data.particles_loc[(data.numprocs-1) *2* chank_size], buf, 2*(particles_amount % chank_size) * sizeof(int));
    } else {
          MPI_Recv(buf, 1, return_datatype, data.numprocs - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          memcpy(&data.particles_loc[(data.numprocs - 1) *2* chank_size], buf, chank_size *2* sizeof(int));
        }
  }

  MPI_Finalize();
  if (data.myid == 0){
        printf("finish\n");
        printArray(&data);
  }
  return 0;
}

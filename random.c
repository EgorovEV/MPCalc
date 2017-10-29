//This program implements 2D case of random walk
//using omp technology

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
	int a, b, x, N; //begin of line, end; start position, amount of particles;
	float p;   //probability to step right
	int P;    //amount of threads

	if (argc != 7) { printf("bad input\n"); return 1; }

	a = atoi(argv[1]);
	if (a < 0) { printf("a should be >= 0\n"); return 2; }
	b = atoi(argv[2]);
	if (b < 0 || b < a) { printf("a < b; b > 0\n"); return 3; }
	x = atoi(argv[3]);
	if (x > b || x < a) { printf("a < x < b\n"); return 4; }
	N = atoi(argv[4]);
	if (N < 0) { printf("N > 0\n"); return 5; }
	p = atof(argv[5]);
	if (0 > p || p > 1) { printf("0 < p < 1\n"); return 6; }
	P = atoi(argv[6]);
	if (P < 0) { printf("P >= 1!\n"); return 7; }

	long long steps = 0;        //int... 15 minutes left =(
	int get_to_b = 0;

	double whole_time = omp_get_wtime();
	omp_set_num_threads(P);
	srand(time(NULL));
#pragma omp parallel
	{
#pragma omp for reduction(+:steps, get_to_b)
		for (int i = 0; i < N; ++i) {
			unsigned int seed = rand();
			int curr_x = x;
			while ((a < curr_x) && (curr_x < b)) {
				if ((rand_r(&seed) / (float)RAND_MAX) <= p)
					++curr_x;
				else
					--curr_x;
				++steps;
			}
			if (curr_x == b)
				++get_to_b;
		}
	}

	whole_time = omp_get_wtime() - whole_time;

	float prob_to_get_b = get_to_b / (float)N;
	float avg_time = steps / (float)N;

	printf("#particles, get to the b = %f\n", get_to_b / (float)N);
	printf("Time = %f\n", whole_time);
	printf("Avg steps = %f\n", steps / (float)N);

	FILE *file;
	file = fopen("stats.txt", "a");
	if (file == NULL) {
		printf("Can't open file!\n");
		return 8;
	}
	fprintf(file, "%f %f %f %d %d %d %d %f %d\n", prob_to_get_b, avg_time, \
		whole_time, a, b, x, N, p, P);
	fclose(file);

	return 0;
}
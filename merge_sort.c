/* C program for Merge Sort */
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>

int compare (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

void printArray(int A[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}

void mysort(int *arr, int l, int r){
    qsort(&arr[l], r - l, sizeof(int), compare);
    printf("l=%d, r=%d", l , r);
    printArray(arr, r);
}

void merge(int arr[], int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;
    qsort(arr, r - l + 1 , sizeof(int), compare);
    /* create temp arrays
	//int L[n1], R[n2];
	int *L = NULL;
	int *R = NULL;
	L = (int*)malloc(n1 * sizeof(int));
	R = (int*)malloc(n2 * sizeof(int));


	
	for (i = 0; i < n1; i++)
		L[i] = arr[l + i];
	for (j = 0; j < n2; j++)
		R[j] = arr[m + 1 + j];

	i = 0; // Initial index of first subarray
	j = 0; // Initial index of second subarray
	k = l; // Initial index of merged subarray
	while (i < n1 && j < n2)
	{
		if (L[i] <= R[j])
		{
			arr[k] = L[i];
			i++;
		}
		else
		{
			arr[k] = R[j];
			j++;
		}
		k++;
	}

	while (i < n1)
	{
		arr[k] = L[i];
		i++;
		k++;
	}

	while (j < n2)
	{
		arr[k] = R[j];
		j++;
		k++;
	}
	free(L);
	free(R);*/
}


void parallelSort(int *arr, int arr_size, int chank_size, int threads_amount) {
    int chank_amount = floor(arr_size / (double)chank_size);
    printf("chank_am=%d\n", chank_amount);
    for (int i = 0; i < chank_amount; ++i){
        printf("%d\n", i * chank_size);
        mysort(arr, i * chank_size, (i + 1) * chank_size);
    }
		//merge(arr, l, m, r);
}





int main()
{
    int arr_size, chank_size, P;
    scanf("%d", &arr_size);
    scanf("%d", &chank_size);
    scanf("%d", &P);
	int *arr = (int*)malloc(sizeof(int) * arr_size);
    srand(time(NULL));
    for (int i = 0; i < arr_size; ++i){
        arr[i] = rand() % 100;
    }

	printf("Given array is \n");
	printArray(arr, arr_size);



        //double whole_time = omp_get_wtime();
    parallelSort(arr, arr_size, chank_size, P);
    //qsort (arr, arr_size, sizeof(int), compare);

    //whole_time = omp_get_wtime() - whole_time;
    //printf("%d\n", whole_time);
	printf("\nSorted array is \n");
	printArray(arr, arr_size);
	return 0;
}
  // CSC456: Name --
  // CSC456: Student ID # --
  // CSC456: I am implementing -- {What parallel sorting algorithm using pthread}
  // CSC456: Feel free to modify any of the following. You can only turnin this file.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <sys/time.h>
#include <pthread.h>
#include "mysort.h"


#define THREAD_COUNT 8

float *arr;
int n;
int part = 0;

struct tsk {
    int tsk_no;
    int tsk_low;
    int tsk_high;
};


void merge(int l, int m, int r)
{
  //printf("Inside Merge Range: %d %d %d\n", l, m, r);
  //printf("Inside Merge \n");

  int lsize = m - l + 1;
  int rsize = r - m;
  int i, j, k;

  float *left_subarray = (float *)malloc(lsize * sizeof(float));
  float *right_subarray = (float *)malloc(rsize * sizeof(float));

  //float *left_subarray = new float[n/2];
  //float *right_subarray = new float[n/2];

  for (i = 0; i < lsize; i++)
  {
    left_subarray[i] = arr[i + l];
  }

  for (i = 0; i < rsize; i++)
  {
    right_subarray[i] = arr[i + m + 1];
  }

  k = l;
  i = 0;
  j = 0;

  while (i < lsize && j < rsize) {
    if (left_subarray[i] <= right_subarray[j])
    {
        arr[k] = left_subarray[i];
        i++;
    }
    else
    {
        arr[k] = right_subarray[j];
        j++;
    }
    k++;
  }

  while (i < lsize) {
    arr[k] = left_subarray[i];
    i++;
    k++;
  }

  while (j < rsize) {
    arr[k] = right_subarray[j];
    j++;
    k++;
  }

  free(left_subarray);
  free(right_subarray);
}


void merge_sort(int l, int r)
{

  // printf("Inside merge sort \n");
  int m = l + ((r - l) / 2);
  if (l < r) {
    merge_sort(l, m);
    merge_sort(m + 1, r);
    merge(l, m, r);
  }
}

void* merge_sort_parallel(void* arg)
{
  printf("Inside Merge parallel \n");

  struct tsk *tsk = (struct tsk *)arg;
  int l;
  int r;

  // calculating low and high
  l = tsk->tsk_low;
  r = tsk->tsk_high;


  // unsigned long thread_part = (unsigned long) arg;

  // int thread_part = part;
  // part++;

  // int l = thread_part * (n / THREAD_COUNT);
  // int r = (thread_part + 1) * (n / THREAD_COUNT) - 1;

  int m = l + (r - l) / 2;
  if (l < r) {
    merge_sort(l, m);
    merge_sort(m + 1, r);
    merge(l, m, r);
  }
}


int pthread_sort(int num_of_elements, float *data)
{

  printf("Inside pthread \n");

  struct tsk *tsk;

  arr = data;
  // int mem_size = num_of_elements * sizeof(float);
  // arr = (float *)malloc(mem_size);

  // for(int i = 0; i < num_of_elements; i++){
  //   arr[i] = data[i];
  // }

  printf("Data copied \n");

  n = num_of_elements;


  pthread_t threads[THREAD_COUNT];
  struct tsk tsklist[THREAD_COUNT];

  int len = n / THREAD_COUNT;

  printf("THREADS:%d MAX:%d LEN:%d\n", THREAD_COUNT, n, len);

  int low = 0;

  for (int i = 0; i < THREAD_COUNT; i++, low += len) {
    tsk = &tsklist[i];
    tsk->tsk_no = i;


    tsk->tsk_low = low;
    tsk->tsk_high = low + len - 1;
    if (i == (THREAD_COUNT - 1))
      tsk->tsk_high = n - 1;


    printf("RANGE %d: %d %d\n", i, tsk->tsk_low, tsk->tsk_high);
  }

  // creating 2 threads
  for (int i = 0; i < THREAD_COUNT; i++) {
    tsk = &tsklist[i];
    pthread_create(&threads[i], NULL, merge_sort_parallel, (void *) tsk);
  }

  // joining all 2 threads
  for (int i = 0; i < THREAD_COUNT; i++)
    pthread_join(threads[i], NULL);

  // show the array values for each thread

  // for (int i = 0; i < THREAD_COUNT; i++) {
  //     tsk = &tsklist[i];
  //     printf("SUB %d:", tsk->tsk_no);
  //     for (int j = tsk->tsk_low; j <= tsk->tsk_high; ++j)
  //         printf(" %d", a[j]);
  //     printf("\n");
  // }

  // merging the final 4 parts

  struct tsk *tskm = &tsklist[0];
  for (int i = 1; i < THREAD_COUNT; i++) {
    struct tsk *tsk = &tsklist[i];
    printf("Final Merge RANGE %d: %d %d %d\n", i, tskm->tsk_low, tsk->tsk_low - 1,  tsk->tsk_high);
    merge(tskm->tsk_low, tsk->tsk_low - 1, tsk->tsk_high);
  }




  // pthread_t p1, p2;
  // int rc;
  // printf("main: begin\n");
  // rc = pthread_create(&p1, NULL, mythread, "A"); assert(rc == 0);
  // rc = pthread_create(&p2, NULL, mythread, "B"); assert(rc == 0);

  // pthread_t threads[THREAD_COUNT];

  // for (int i = 0; i < THREAD_COUNT; i++)
  // {
  //   pthread_create(&threads[i], NULL, merge_sort_parallel, (void*) i);
  // }

  // for (int i = 0; i < THREAD_COUNT; i++)
  // {
  //   pthread_join(threads[i], NULL);
  // }

  // merge(0, (n / 2 - 1) / 2, n / 2 - 1);
  // merge(n / 2, n/2 + (n-1-n/2)/2, n - 1);
  // merge(0, (n - 1)/2, n - 1);




  // for(int i = 0; i < n; i++){
  //   data[i] = arr[i];
  // }

  return 0;

}
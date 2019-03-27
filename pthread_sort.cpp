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
#include <assert.h>
#include "mysort.h"


#define THREAD_COUNT 2

float *arr;
int n;
int part = 0;

struct partition {
    int partition_l;
    int partition_r;
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

  struct partition *partition = (struct partition *)arg;
  int l;
  int r;

  l = partition->partition_l;
  r = partition->partition_r;


  int m = l + (r - l) / 2;
  if (l < r) {
    merge_sort(l, m);
    merge_sort(m + 1, r);
    merge(l, m, r);
  }
}


int pthread_sort(int num_of_elements, float *data)
{

  // printf("Inside pthread \n");

  struct partition *partition;

  arr = data;

  // printf("Data copied \n");

  n = num_of_elements;

  struct partition *partition1;
  struct partition *partition2;

  int parition_length = n / THREAD_COUNT;

  printf("THREADS:%d MAX:%d parition_length:%d\n", THREAD_COUNT, n, parition_length);

  
  int l = 0;

  // Generating partition values for partitions to be run on both the threads
  partition1->partition_l = l;
  partition1->partition_r = l + parition_length - 1;

  partition2->partition_l = l + parition_length;
  partition2->partition_r = n - 1;
   

  pthread_t p1, p2;
  int rc;
  printf("main: begin\n");
  rc = pthread_create(&p1, NULL, merge_sort_parallel, (void *) &partition1); assert(rc == 0);
  rc = pthread_create(&p2, NULL, merge_sort_parallel, (void *) &partition2); assert(rc == 0);

  
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);


  merge(partition1->partition_l, partition2->partition_l - 1, partition2->partition_r);


  return 0;

}
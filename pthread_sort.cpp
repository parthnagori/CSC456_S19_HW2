  // CSC456: Name -- Parth Nagori
  // CSC456: Student ID # -- 200208747
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

// Struct to pass range and memory information to each thread
struct partition {
    int l;
    int r;
    float *left;
    float *right;
};

// Merge operation
void merge(int l, int m, int r, float* left, float* right)
{
  int lsize = m - l + 1;
  int rsize = r - m;
  int i, j, k;

  float *left_subarray = left;
  float *right_subarray = right;

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

}

// Recursive Merge Sort
void merge_sort(int l, int r, float* left, float* right)
{
  int m = l + ((r - l) / 2);
  if (l < r) {
    merge_sort(l, m, left, right);
    merge_sort(m + 1, r, left, right);
    merge(l, m, r, left, right);
  }
}

// Call Merge Sort operation for each thread
void* merge_sort_parallel(void* arg)
{
  struct partition *partition = (struct partition *)arg;

  int m = partition->l + (partition->r - partition->l) / 2;
  if (partition->l < partition->r) {
    merge_sort(partition->l, m, partition->left, partition->right);
    merge_sort(m + 1, partition->r, partition->left, partition->right);
    merge(partition->l, m, partition->r, partition->left, partition->right);
  }
}

int pthread_sort(int num_of_elements, float *data)
{

  arr = data;
  n = num_of_elements;

  struct partition* partition1 = (struct partition*) malloc(sizeof(struct partition));
  struct partition* partition2 = (struct partition*) malloc(sizeof(struct partition));

  pthread_t p1, p2;

  int partition_length = n / THREAD_COUNT;

  int l = 0;

  // Generating partition values for partitions to be run on both the threads
  // Allocating a common memory location for each thread
  partition1->l = l;
  partition1->r = l + partition_length - 1;
  partition1->left = (float *)malloc(partition_length * sizeof(float));
  partition1->right = (float *)malloc(partition_length * sizeof(float)); 

  partition2->l = l + partition_length;
  partition2->r = n - 1;
  partition2->left = (float *)malloc(partition_length * sizeof(float));
  partition2->right = (float *)malloc(partition_length * sizeof(float));

  
  int rc;
  rc = pthread_create(&p1, NULL, merge_sort_parallel, (void *) partition1); assert(rc == 0);
  rc = pthread_create(&p2, NULL, merge_sort_parallel, (void *) partition2); assert(rc == 0);

  pthread_join(p1, NULL);
  pthread_join(p2, NULL);

  merge(partition1->l, partition2->l - 1, partition2->r, partition1->left, partition1->right);

  return 0;

}
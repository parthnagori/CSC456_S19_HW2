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


#define THREAD_COUNT 4

float *arr; 
  
void merge(int l, int m, int r, num_of_elements) 
{   
  int lsize = m - l + 1;
  int rsize = r - m;
  int i, j, k;

  float *left_subarray = new float[num_of_elements/2];
  float *right_subarray = new float[num_of_elements/2];

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
  
 
void merge_sort(int l, int r) 
{  
  int m = l + ((r - l) / 2); 
  if (l < r) { 
    merge_sort(l, m); 
    merge_sort(m + 1, r); 
    merge(l, m, r); 
  } 
} 
   
void* merge_sort_parallel(void* arg) 
{ 

  int thread_part = part;
  part++; 

  int l = thread_part * (num_of_elements / THREAD_COUNT); 
  int r = (thread_part + 1) * (num_of_elements / THREAD_COUNT) - 1; 

  int m = l + (r - l) / 2; 
  if (l < r) { 
    merge_sort(l, m); 
    merge_sort(m + 1, r); 
    merge(l, m, r); 
  } 
} 
  

int pthread_sort(int num_of_elements, float *data)
{ 

  arr = data;

  pthread_t threads[THREAD_COUNT]; 
 
  for (int i = 0; i < THREAD_COUNT; i++)
  { 
    pthread_create(&threads[i], NULL, merge_sort_parallel, (void*)NULL); 
  }

  for (int i = 0; i < THREAD_COUNT; i++)
  {
    pthread_join(threads[i], NULL);
  } 
   
  merge(0, (num_of_elements / 2 - 1) / 2, num_of_elements / 2 - 1); 
  merge(num_of_elements / 2, num_of_elements/2 + (num_of_elements-1-num_of_elements/2)/2, num_of_elements - 1); 
  merge(0, (num_of_elements - 1)/2, num_of_elements - 1); 

  return 0; 

}


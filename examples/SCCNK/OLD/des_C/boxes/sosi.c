#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "desBoxes.h"
#include <string.h>
#include <time.h>
#include "/shared/nil/snetInstall/include/scc.h"
#include "/shared/nil/snetInstall/include/scc_lpel.h"

//num_node 1 will be one branch of pipeline
/************ SOURCE SINK ***************/
void *snet_source(void *hnd, int mess, int s, int num_node, int sleep_micro, int change_mess, int change_percent,int window_size, int thresh_hold, int skip_update) {
  fprintf(stderr,"================================\n\tSOURCE start\n================================\n");
    
  char pt[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
  char key[7] = {'1', '2', '3', '4', '5', '6', '7'};
  int i, j;
  double r[5] = {0.6, 0.8, 1, 1.2, 1.4};
  int size[5];
  char *p[5];
  char *k[5];
  char *x, *y;
  for (i = 0; i < 5; i++) {
    size[i] = (int) (r[i] * s);
    x = (char *) SCCMallocPtr(size[i] * 8);
    y = (char *) SCCMallocPtr(size[i] * 7);
    for (j = 0; j < size[i]; j++) {
      memcpy(&x[j * 8], &pt[0], 8);
      memcpy(&y[j * 7], &key[0], 7);
    }
    p[i] = x;
    k[i] = y;
  }

  for (j = 0; j < mess; j++) {
    i = j % 5;
    C4SNetOut(hnd, 1, C4SNetCreate(CTYPE_char, 8 * size[i], p[i]), C4SNetCreate(CTYPE_char, 7 * size[i], k[i]), size[i], j % num_node);
    printf("Mess %d generated, written to pipe %d\n",j,(j % num_node));
  }

  for (i = 0; i < 5; i++) {
    SCCFreePtr(p[i]);
    SCCFreePtr(k[i]);
  }
  
  printf("Total mess generated %d\n",j);

  return hnd;
}


void *snet_sink(void *hnd, c4snet_data_t *ct, int size, int node) {
  // indicate the first output
  static int messCount=0;
  
  
  // update array of observe or
  if (messCount == 0) {
    printf("================================\n\tSINK start\n================================\n");
  }
  printf("Mess count %d\n\n",messCount);
  //snet box
  messCount++;
  C4SNetFree(ct);
  return hnd;
}


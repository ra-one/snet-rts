#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "desBoxes.h"
#include <string.h>
#include <time.h>
#include "/shared/nil/snetInstall/include/scc.h"
#include "/shared/nil/snetInstall/include/scc_lpel.h"


// global for source/sink
observer_t *obs;
FILE *fout;

//num_node 1 will be one branch of pipeline
/************ SOURCE SINK ***************/
void *snet_source(void *hnd, int mess, int s, int num_node, int sleep_micro, int change_mess, int change_percent, 
                    int window_size, int thresh_hold, int skip_update) {
  fprintf(stderr,"================================\n\tSOURCE start\n================================\n");
  fout = fopen("/shared/nil/mySnet/snet-rts/examples/SCCNK/des_C/out/source.txt", "w");
  if (fout == NULL)fprintf(stderr, "Can't open output file!\n");
  
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

  // init observer structure
  obs = (observer_t *) SCCMallocPtr(sizeof(observer_t));
  obs->window_size = window_size;
  obs->thresh_hold = thresh_hold;
  obs->skip_update = skip_update;
  obs->skip_count = 0;
  obs->output_index = 0;
  obs->output_rate = 0;
  obs->freq = 800; //default freq
  obs->output_interval = (double *) SCCMallocPtr(sizeof(double) * obs->window_size);
  
  int oi;
  for (oi = 0; oi < obs->window_size; oi++) obs->output_interval[oi] = 0.0;
  
  int counter = 0; // counter message with same sleeping interval

  int cur_sleep = sleep_micro;
  int new_sleep = sleep_micro;
  
  obs->input_rate = 1000.0 * 1000.0/cur_sleep;   // input rate in message/s
  printf("cur_input rate = %f, Time %f\n", obs->input_rate,SCCGetTime());  
  fprintf(fout,"inRate, %f, time, %f, sleep, %d\n", obs->input_rate,SCCGetTime(),new_sleep);  
  
  // copy observer address to MPB so sink can get it
  memcpy((void*)SOSIADDR, (const void*)&obs, sizeof(observer_t*));
  fprintf(stderr,"observer address source: %p\n",obs);
  for (j = 0; j < mess; j++) {
    if (counter == change_mess) {
      counter = 0;

      //TODO: change current interval
      //cur_sleep = cur_sleep* (1 - change_percent/100.0);
      //int val = (rand()%(max-min));
      //int val = rand()%60;
      //new_sleep = cur_sleep* (1 - val/100.0);
      
      obs->input_rate = 1000.0 * 1000.0/ new_sleep;
      //obs->input_rate = 1000.0 * 1000.0/ cur_sleep;
      printf("updated input rate = %f, Time %f\n", obs->input_rate,SCCGetTime());
      fprintf(fout,"inRate, %f, time, %f, sleep, %d\n", obs->input_rate,SCCGetTime(),new_sleep); 
      fflush(fout);
    }

    i = j % 5;
    C4SNetOut(hnd, 1, C4SNetCreate(CTYPE_char, 8 * size[i], p[i]), C4SNetCreate(CTYPE_char, 7 * size[i], k[i]), size[i], i % num_node);
    //usleep(cur_sleep);
    usleep(new_sleep);
    counter++;
  }

  for (i = 0; i < 5; i++) {
    SCCFreePtr(p[i]);
    SCCFreePtr(k[i]);
  }
  
  printf("Total mess generated %d\n",j);

  return hnd;
}


double time_diff(struct timeval x , struct timeval y)   // in micro second
{
  double x_ms , y_ms , diff;
  
  x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
  y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
  
  diff = (double)y_ms - (double)x_ms;
  
  return diff;
}


void compare_ir_or(FILE *fileHand) {
  obs->skip_count++;
  if (obs->skip_count < obs->skip_update){
    fprintf(fileHand,"InRate,%f,outRate,%f,",obs->input_rate,obs->output_rate);
    powerMeasurement(fileHand);
    fprintf(fileHand,"freq,%d,time,%f,",obs->freq,SCCGetTime());
    return;
  }
  double or = 0.0;
  int i;
  for (i = 0; i < obs->window_size; i++){
    or += obs->output_interval[i];
    //printf("i %d, interval %f\n",i,obs->output_interval[i]);
  }
  or = or/obs->window_size;
  or = 1000.0 * 1000.0/ or;   //output rate in message/second
  
  obs->output_rate = or;
  
  //fprintf(fileHand,"InRate,%f,outRate,%f,time,%f,",obs->input_rate,or,SCCGetTime());
  fprintf(fileHand,"InRate,%f,outRate,%f,",obs->input_rate,or);
  powerMeasurement(fileHand);
  fprintf(fileHand,"freq,%d,time,%f,",obs->freq,SCCGetTime());

  
  //DVFS is not enabled so return from here
  if(!DVFS) return;
  
  //TODO: check when to change the frequency
  if (obs->input_rate - or > obs->thresh_hold) {
    //printf("ah ha!! change cpu frequency please\n");
    increaseFrequency();
    //obs->skip_count = 0;
    //for (i = 0; i < obs->window_size; i++){
    //  obs->output_interval[i] = 0.0;
    //}
  }
  else  printf("ouput rate = %f, input_rate = %f, thresh_hold = %d\n", or, obs->input_rate, obs->thresh_hold);
}


void *snet_sink(void *hnd, c4snet_data_t *ct, int size, int node) {
  // indicate the first output
  static int start = 1;
  static int messCount=0;
  
  messCount++;
  // update array of observe or
  if (start == 1) {
    fprintf(stderr,"================================\n\tSINK start\n================================\n");
    fout = fopen("/shared/nil/mySnet/snet-rts/examples/SCCNK/des_C/out/sink.txt", "w");
    if (fout == NULL)fprintf(stderr, "Can't open output file!\n");
    
    while(*SOSIADDR == 0);
    memcpy((void*)&obs, (const void*)SOSIADDR, sizeof(observer_t*));
    fprintf(stderr,"observer address sink: %p\n",obs);
    start = 0;
    obs->last_output = SCCGetTime();
  } else {
    double tv = SCCGetTime();
    obs->output_interval[obs->output_index] = ((tv*1000000)-(obs->last_output*1000000));
    obs->last_output = tv;
    obs->output_index = (obs->output_index + 1) % obs->window_size;
    compare_ir_or(fout);
    fprintf(stderr,"Mess count %d\n\n",messCount);
    fprintf(fout,"Mess,%d\n",messCount);
    fflush(fout);fflush(stderr);
  }

  //snet box
  C4SNetFree(ct);
  return hnd;
}


# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include "sosi.h"
# include <unistd.h>
#include <time.h>
#include "/shared/nil/snetInstall/include/scc.h"
#include "/shared/nil/snetInstall/include/scc_lpel.h"

// allocate by source, access by sink
typedef struct {
  int window_size;    // window of observing ouput messages
  int thresh_hold;   //TODO: thresh_hold to change the freq
  int skip_update;    // skip update frequency by a number of ouput messages, should be >= window_size
  int skip_count;     //count number of output
  
  double *output_interval;  // window of output interval
  int output_index;     // index in the window of observed output rate
  struct timeval last_output; // timestamp of last output

  double input_rate;
} observer_t;

// global for snet_sink
observer_t *obs;

double time_diff(struct timeval x , struct timeval y)   // in micro second
{
  double x_ms , y_ms , diff;
  
  x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
  y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
  
  diff = (double)y_ms - (double)x_ms;
  
  return diff;
}


void compare_ir_or() {
  obs->skip_count++;
  if (obs->skip_count < obs->skip_update)
    return;
 
  double or = 0.0;
  int i;
  for (i = 0; i < obs->window_size; i++)
    or += obs->output_interval[i];
  or = or/obs->window_size;
  or = 1000.0 * 1000.0/ or;   //output rate in message/second
  
  //TODO: check when to change the frequency
  if (obs->input_rate - or > obs->thresh_hold) {
    //fprintf(stderr,"ah ha!! change cpu frequency please\n");
    increaseFrequency();
    obs->skip_count = 0;
  }
  else  fprintf(stderr,"ouput rate = %f, input_rate = %f, thresh_hold = %d\n", or, obs->input_rate, obs->thresh_hold);
}

void *snet_sink(void *hnd, c4snet_data_t *dx, c4snet_data_t *dy, c4snet_data_t *dw, int s, int ln, int size, int node) {
  
  // indicate the first output
	static int start = 1;
	// update array of observe or
  if (start == 1) {
    fprintf(stderr,"sink start %d\n",start); 
    while(*SOSIADDR == 0);
    memcpy((void*)&obs, (const void*)SOSIADDR, sizeof(observer_t*));
    fprintf(stderr,"observer address sink: %p\n",obs);
    start = 0;
    gettimeofday(&obs->last_output, NULL);
  } else {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    obs->output_interval[obs->output_index] = time_diff(obs->last_output, tv);
    obs->last_output = tv;
    obs->output_index = (obs->output_index + 1) % obs->window_size;
    compare_ir_or();
  }

  void **data = C4SNetGetData(dx);
  float *x = (float *) *data;
  SCCFreePtr(x);
  C4SNetFree(dx);
  
  data = C4SNetGetData(dw);
  float *w = (float *) *data;
  SCCFreePtr(w);
  C4SNetFree(dw);
  
  data = C4SNetGetData(dy);
  float *y = (float *) *data;
  SCCFreePtr(y);
  C4SNetFree(dy);
  return hnd;
}

void *snet_source(void *hnd, int mess, int ln, int size, int num_node,int sleep_micro, int change_mess, int change_percent, 
                    int window_size, int thresh_hold, int skip_update) {
  int n = 1 << ln;
  int i, j;
  float *x;
  x = (float *) SCCMallocPtr(sizeof(float) * n * 2);
  fprintf(stderr,"size: %d\n",(sizeof(float) * n * 2));
  for (i = 0; i < n; i++) {
    x[i] = i;
    x[i + n] = i + 1;
  }

	// init observer structure
  obs = (observer_t *) SCCMallocPtr(sizeof(observer_t));
  obs->window_size = window_size;
  obs->thresh_hold = thresh_hold;
  obs->skip_update = skip_update;
  obs->skip_count = 0;
  obs->output_index = 0;
  obs->output_interval = (double *) SCCMallocPtr(sizeof(double) * obs->window_size);

  int counter = 0; // counter message with same sleeping interval

  int cur_sleep = sleep_micro;
  
  obs->input_rate = 1000.0 * 1000.0/cur_sleep;   // input rate in message/s
  fprintf(stderr,"cur_input rate = %f\n", obs->input_rate);
  
  // copy observer address to MPB so sink can get it
  memcpy((void*)SOSIADDR, (const void*)&obs, sizeof(observer_t*));
  fprintf(stderr,"observer address source: %p\n",obs);
 
  for (j = 0; j < mess; j++) {
  	if (counter == change_mess) {
      counter = 0;

      //TODO: change current interval
      cur_sleep = cur_sleep* (1 - change_percent/100.0);
      
      obs->input_rate = 1000.0 * 1000.0/ cur_sleep;
      fprintf(stderr,"updated input rate = %f\n", obs->input_rate);  
    }
    float *s = SCCMallocPtr(sizeof(float) * n * 2);
    memcpy(s, x, sizeof(float) * n * 2);
    C4SNetOut(hnd, 1, C4SNetCreate(CTYPE_char, sizeof(void *), &s), ln, size, j % num_node);
    usleep(cur_sleep);
    counter++;
  }

  SCCFreePtr(x);
  return hnd;
}


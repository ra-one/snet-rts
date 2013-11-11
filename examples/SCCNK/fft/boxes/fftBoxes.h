#ifndef AUTH_FFT_H
#define AUTH_FFT_H
#include <C4SNet.h>

void *initP(void *hnd, c4snet_data_t *dx, int ln, int size, int node);
void *stepP(void *hnd, c4snet_data_t *dx, c4snet_data_t *dy, c4snet_data_t *dw, int s, int ln, int size, int node);
void *finalP(void *hnd, c4snet_data_t *dx, c4snet_data_t *dy, c4snet_data_t *dw, int s, int ln, int size, int node);
#endif 

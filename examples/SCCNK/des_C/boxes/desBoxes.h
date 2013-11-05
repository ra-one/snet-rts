#ifndef AUTH_DES_H
#define AUTH_DES_H
#include <C4SNet.h>

void *initP(void *hnd, c4snet_data_t *spt, c4snet_data_t *sk, int size);
void *subRound(void *hnd, c4snet_data_t *sd, c4snet_data_t *sk, int c, int size);
void *finalP(void *hnd, c4snet_data_t *sd, c4snet_data_t *sk, int c, int size);

#endif 

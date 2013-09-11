#ifndef AUTH_DES_H
#define AUTH_DES_H
#include <C4SNet.h>

void *source(void *hnd, int interval, int mess);
void *initP(void *hnd, c4snet_data_t *spt, c4snet_data_t *sk);
void *subRound(void *hnd, c4snet_data_t *sd, c4snet_data_t *sk, int c);
void *finalP(void *hnd, c4snet_data_t *sd, c4snet_data_t *sk, int c);
void *sink (void *hnd, c4snet_data_t *ct);

#endif 

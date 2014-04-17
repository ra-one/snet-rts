#ifndef AUTH_DES_H
#define AUTH_DES_H
#include <C4SNet.h>

void *snet_source(void *hnd, int interval, int mess);
void *initP(void *hnd, c4snet_data_t *spt, c4snet_data_t *sk);
void *subRound(void *hnd, c4snet_data_t *sd, c4snet_data_t *sk, int c);
void *finalP(void *hnd, c4snet_data_t *sd, c4snet_data_t *sk, int c);
void *snet_sink (void *hnd, c4snet_data_t *ct);

#endif 

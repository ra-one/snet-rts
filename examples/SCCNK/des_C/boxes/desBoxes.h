#ifndef AUTH_DES_H
#define AUTH_DES_H
#include <C4SNet.h>

void *snet_source(void *hnd, int mess, int size, int num_node, int sleep_micro, int change_mess, int change_percent, int window_size, int threshold, int skip_update);
void *initP(void *hnd, c4snet_data_t *spt, c4snet_data_t *sk, int size);
void *subRound(void *hnd, c4snet_data_t *sd, c4snet_data_t *sk, int c, int size);
void *finalP(void *hnd, c4snet_data_t *sd, c4snet_data_t *sk, int c, int size);
void *snet_sink (void *hnd, c4snet_data_t *ct, int size, int node);

#endif 

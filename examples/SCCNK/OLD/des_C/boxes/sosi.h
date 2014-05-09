#ifndef AUTH_SOSI_H
#define AUTH_SOSI_H
#include <C4SNet.h>

void *snet_source(void *hnd, int mess, int size, int num_node, int sleep_micro, int change_mess, int change_percent, int window_size, int thresh_hold, int skip_update);
void *snet_sink (void *hnd, c4snet_data_t *ct, int size, int node);

#endif 

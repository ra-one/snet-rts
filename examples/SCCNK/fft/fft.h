/**
 * @file fft.h
 *
 * Header definitions of compiled snet-file for runtime.
 *
 * THIS FILE HAS BEEN GENERATED.
 * DO NOT EDIT THIS FILE.
 * EDIT THE ORIGINAL SNET-SOURCE FILE fft.snet INSTEAD!
 *
 * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!
 *
*/

#ifndef _FFT_H_
#define _FFT_H_

#include "snetentities.h"
#include "distribution.h"

#define E__fft__NONE 0
#define F__fft__X 1
#define F__fft__Y 2
#define F__fft__W 3
#define T__fft__mess 4
#define T__fft__LN 5
#define T__fft__size 6
#define T__fft__num_node 7
#define T__fft__node 8
#define T__fft__S 9

#define SNET__fft__NUMBER_OF_LABELS 10


#define I__fft__C4SNet 0

#define SNET__fft__NUMBER_OF_INTERFACES 1


extern char *snet_fft_labels[];

extern char *snet_fft_interfaces[];


extern snet_stream_t *SNet__fft___fft(snet_stream_t *in_buf, snet_info_t *info, int location);

#endif


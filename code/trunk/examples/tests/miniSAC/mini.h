/**
 * @file mini.h
 *
 * Header definitions of compiled snet-file for runtime.
 *
 * THIS FILE HAS BEEN GENERATED.
 * DO NOT EDIT THIS FILE.
 * EDIT THE ORIGINAL SNET-SOURCE FILE mini.snet INSTEAD!
 *
 * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!
 *
*/

#ifndef _MINI_H_
#define _MINI_H_

#include "snetentities.h"
#include "distribution.h"

#define E__mini__NONE 0
#define F__mini__A 1

#define SNET__mini__NUMBER_OF_LABELS 2


#define I__mini__SAC4SNet 0

#define SNET__mini__NUMBER_OF_INTERFACES 1


extern char *snet_mini_labels[];

extern char *snet_mini_interfaces[];


extern snet_stream_t *SNet__mini___mini(snet_stream_t *in_buf, snet_info_t *info, int location);

#endif


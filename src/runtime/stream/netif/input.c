/*******************************************************************************
 *
 * $Id: input.c 2864 2010-09-17 11:28:30Z dlp $
 *
 * Author: Daniel Prokesch, Vienna University of Technology
 * -------
 *
 * Date:   27.10.2010
 * -----
 *
 * Description:
 * ------------
 *
 * Input thread for S-NET network interface.
 *
 *******************************************************************************/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "memfun.h"
#include "input.h"
#include "snetentities.h"
#include "label.h"
#include "interface.h"
#include "bool.h"
#include "debug.h"
#include "parserutils.h"

#include "threading.h"

#include "distribution.h"

#define PRT(x) fprintf(stderr,x);

typedef struct {
  FILE *file;
  snetin_label_t *labels;
  snetin_interface_t *interfaces;
  snet_stream_t *buffer;
} handle_t;


/**
 * This is the task doing the global input
 */
static void GlobInputTask(snet_entity_t *ent, void* data)
{
  handle_t *hnd = (handle_t *)data;
PRT("input.c: got handle\n")
  if(hnd->buffer != NULL) {
    int i;
    snet_stream_desc_t *outstream = SNetStreamOpen(hnd->buffer, 'w');
PRT("input.c: after out stream\n")
    FILE *fl = fopen("./input.xml", "r");
    SNetInParserInit( fl, hnd->labels, hnd->interfaces, outstream, ent);
    //SNetInParserInit( hnd->file, hnd->labels, hnd->interfaces, outstream, ent);
PRT("input.c: after pars init\n")
    i = SNET_PARSE_CONTINUE;
PRT("input.c: going in while\n")
    while(i != SNET_PARSE_TERMINATE){
      i = SNetInParserParse();
    }
PRT("input.c: after while\n")
    SNetInParserDestroy();
PRT("input.c: parse destroyed\n")
    SNetStreamClose( outstream, false);
PRT("input.c: stream closed\n")
  }
PRT("input.c: going to free mem\n")
  SNetMemFree(hnd);
}


void SNetInInputInit(FILE *file,
                     snetin_label_t *labels,
                     snetin_interface_t *interfaces,
                     snet_stream_t *in_buf
  )
{
  handle_t *hnd = SNetMemAlloc(sizeof(handle_t));

  hnd->file = file;
  hnd->labels = labels;
  hnd->interfaces = interfaces;
  hnd->buffer = in_buf;

  SNetThreadingSpawn(
      SNetEntityCreate( ENTITY_other, -1, NULL,
        "glob_input", GlobInputTask, (void*)hnd)
      );
}

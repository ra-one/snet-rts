/*----------------------------------------------------------------------------*/
/*
      -------------------------------------------------------------------

              * * * * ! SVP S-Net Graph Walker Runtime ! * * * *

                  Computer Systems Architecture (CSA) Group
                            Informatics Institute
                        University Of Amsterdam  2008
                         
      -------------------------------------------------------------------

    File Name      : record.int.utc.h

    File Type      : Header File

    ---------------------------------------

    File 
    Description    :

    Updates 
    Description    : N/A

*/
/*----------------------------------------------------------------------------*/

#ifndef __SVPSNETGWRT_RECORD_INT_H
#define __SVPSNETGWRT_RECORD_INT_H

#include "core/record.utc.h"

/*---*/

#include "common.int.utc.h"
#include "memmng.int.utc.h"

/*----------------------------------------------------------------------------*/
/* Items' copying operation flags */

#define DREC_ITEMS_COPY_MOVE       0x01
#define DREC_ITEMS_COPY_UNCONSUMED 0x02
#define DREC_ITEMS_COPY_CONSUMED   0x04
#define DREC_ITEMS_COPY_FIELDS     0x08
#define DREC_ITEMS_COPY_TAGS       0x10
#define DREC_ITEMS_COPY_BTAGS      0x20

/*----------------------------------------------------------------------------*/

typedef struct {
    const int *names;
    const int *new_names;

    unsigned int fields_cnt;
    unsigned int tags_cnt;
    unsigned int btags_cnt;

} snet_record_items_copy_specs_t;

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

extern void
SNetRecInit(
    snet_record_t *rec,
    snet_record_descr_t descr, va_list vargs);
   
/*---*/

extern void
SNetDataRecInit(
    snet_record_t *rec,
    snet_bli_id_t  blid,
    snet_variantencoding_t *venc, va_list vargs);

extern void 
SNetDataRecInitFromArrays(
    snet_record_t *rec,
    snet_bli_id_t  blid,
    snet_variantencoding_t *venc, void **fields, int *tags, int *btags);

/*---*/

extern void
SNetCtrlRecInit(
    snet_record_t *rec,
    snet_record_ctrl_mode_t mode,
    unsigned int opcode, const void *data, unsigned int data_sz);

/*---*/

extern void
SNetRecInitCopy(snet_record_t *rec, const snet_record_t *src);

/*---*/

extern void
SNetRecDestroyPartial(snet_record_t *rec);

/*----------------------------------------------------------------------------*/

extern bool 
SNetRecAddTag(snet_record_t *rec, int name);

extern bool 
SNetRecAddBTag(snet_record_t *rec, int name);

extern bool 
SNetRecAddField(snet_record_t *rec, int name);

/*---*/

extern bool 
SNetRecRemoveTag(snet_record_t *rec, int name);

extern bool
SNetRecRemoveBTag(snet_record_t *rec, int name);

extern bool
SNetRecRemoveField(snet_record_t *rec, int name);

/*---*/

extern bool
SNetRecCopyTag(
    snet_record_t *rec1,
    snet_record_t *rec2, int name, int new_name);

extern bool
SNetRecCopyBTag(
    snet_record_t *rec1,
    snet_record_t *rec2, int name, int new_name);

    
extern bool
SNetRecCopyField(
    snet_record_t *rec1,
    snet_record_t *rec2, int name, int new_name);

/*---*/

extern bool
SNetRecMoveTag(
    snet_record_t *rec1,
    snet_record_t *rec2, int name, int new_name);

extern bool
SNetRecMoveBTag(
    snet_record_t *rec1,
    snet_record_t *rec2, int name, int new_name);

    
extern bool
SNetRecMoveField(
    snet_record_t *rec1,
    snet_record_t *rec2, int name, int new_name);

/*----------------------------------------------------------------------------*/

extern unsigned int
SNetRecAddItems(
    snet_record_t *rec,
    int *names, 
    unsigned int fields_cnt, 
    unsigned int tags_cnt, unsigned int btags_cnt);

extern unsigned int
SNetRecRemoveItems(
    snet_record_t *rec, 
    int *names,
    unsigned int fields_cnt, 
    unsigned int tags_cnt, unsigned int btags_cnt);

extern unsigned int
SNetRecCopyItems(
    snet_record_t *rec1,
    snet_record_t *rec2,
    unsigned int   flags, snet_record_items_copy_specs_t *specs);

#endif // __SVPSNETGWRT_RECORD_INT_H

/*------------------------------- END OF FILE --------------------------------*/
/*----------------------------------------------------------------------------*/


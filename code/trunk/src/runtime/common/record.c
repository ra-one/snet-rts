/* 
 * record.c
 * Implementation of the record and its functions.
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <memfun.h>
#include <record.h>
#include <snetentities.h>
//#include <lbindings.h>

#define GETNUM( N, M)   int i;\
                         i = N( GetVEnc( rec));\
                        return( i - GetConsumedCount( M( GetVEnc( rec)), i));


#define REMOVE_FROM_REC( RECNUMS, RECNAMES, RECGET, TENCREMOVE, NAME, TYPE)\
  int i, *names, skip;\
  TYPE *new;\
  names = RECNAMES( rec);\
  new = SNetMemAlloc( ( RECNUMS( rec) - 1) * sizeof( TYPE));\
  skip = 0;\
  for( i=0; i<RECNUMS( rec); i++) {\
    if( names[i] != name) {\
      new[i - skip] = RECGET( rec, names[i]);\
    }\
    else {\
      skip += 1;\
    }\
  }\
  SNetMemFree( names);\
  TENCREMOVE( GetVEnc( rec), name);\
  SNetMemFree( rec->rec->data_rec->NAME);\
  rec->rec->data_rec->NAME = new;



/* macros for record datastructure */
#define REC_DESCR( name) name->rec_descr
#define RECPTR( name) name->rec
#define RECORD( name, type) RECPTR( name)->type

#define DATA_REC( name, component) RECORD( name, data_rec)->component
#define SYNC_REC( name, component) RECORD( name, sync_rec)->component
#define SORT_B_REC( name, component) RECORD( name, sort_begin_rec)->component
#define SORT_E_REC( name, component) RECORD( name, sort_end_rec)->component
#define TERMINATE_REC( name, component) RECORD( name, terminate_hnd)->component
#define STAR_REC( name, component) RECORD( name, star_rec)->component

/* DATA_REC(x, y) -> x->rec->data_rec->y */
struct iteration_counters {
  int counter;
  struct iteration_counters *next;
};

typedef struct {
  snet_variantencoding_t *v_enc;
  void **fields;
  int *tags;
  int *btags;
  int interface_id;
  struct iteration_counters **counters;
} data_rec_t;

typedef struct {
  snet_buffer_t *inbuf;
} sync_rec_t;

typedef struct {
  int num;
  int level;
} sort_begin_t;

typedef struct {
  int num;
  int level;  
} sort_end_t;


typedef struct {
  /* empty */
} terminate_rec_t;

typedef struct {
  snet_buffer_t *outbuf;
} star_rec_t;

union record_types {
  data_rec_t *data_rec;
  sync_rec_t *sync_rec;
  star_rec_t *star_rec;
  sort_begin_t *sort_begin_rec;
  sort_end_t *sort_end_rec;
  terminate_rec_t *terminate_rec;
};

struct record {
  snet_record_descr_t rec_descr;
  snet_record_types_t *rec;
};


/* *********************************************************** */

static int FindName( int *names, int count, int val) {

  int i=0;
 if( ( names == NULL)) {
  printf("\n\n ** Runtime Error ** : Record contains no name encodings. This"
         " is a \n"
         "                       runtime-system bug. [FindName()]\n\n");
  exit( 1);
  }
  while( (names[i] != val) && (i < count) ) {
    i += 1;
  }

  if( i == count) {
    i = NOT_FOUND;
  }
  
   return( i);
}

static snet_variantencoding_t *GetVEnc( snet_record_t *rec) {
  
  return( DATA_REC( rec, v_enc));
}


static int GetConsumedCount( int *names, int num) {
  
  int counter,i;

  counter=0;
  for( i=0; i<num; i++) {
    if( (names[i] == CONSUMED)) {
      counter = counter + 1;
    }
  }

  return( counter);
}

static void NotFoundError( int name, char *action, char *type) 
{
    printf("\n\n ** Runtime Error ** : Attempted '%s' on non-existent"
           " %s [%d].\n\n", action, type, name);
    exit( 1);
}
/* *********************************************************** */

extern snet_record_t *SNetRecCreate( snet_record_descr_t descr, ...) {

  snet_record_t *rec;
  va_list args;
  snet_variantencoding_t *v_enc;

  rec = SNetMemAlloc( sizeof( snet_record_t));
  REC_DESCR( rec) = descr;
  

  va_start( args, descr);

  switch( descr) {
    case REC_data:
      v_enc = va_arg( args, snet_variantencoding_t*);

      RECPTR( rec) = SNetMemAlloc( sizeof( snet_record_types_t));
      RECORD( rec, data_rec) = SNetMemAlloc( sizeof( data_rec_t));
      DATA_REC( rec, fields) = SNetMemAlloc( SNetTencGetNumFields( v_enc) * sizeof( void*));
      DATA_REC( rec, tags) = SNetMemAlloc( SNetTencGetNumTags( v_enc) * sizeof( int));
      DATA_REC( rec, btags) = SNetMemAlloc( SNetTencGetNumBTags( v_enc) * sizeof( int));
      DATA_REC( rec, v_enc) = v_enc;
      DATA_REC( rec, counters) = NULL;
      break;
    case REC_sync:
      RECPTR( rec) = SNetMemAlloc( sizeof( snet_record_types_t));
      RECORD( rec, sync_rec) = SNetMemAlloc( sizeof( sync_rec_t));
      SYNC_REC( rec, inbuf) = va_arg( args, snet_buffer_t*);
      break;
    case REC_collect:
      RECPTR( rec) = SNetMemAlloc( sizeof( snet_record_types_t));
      RECORD( rec, star_rec) = SNetMemAlloc( sizeof( star_rec_t));
      STAR_REC( rec, outbuf) = va_arg( args, snet_buffer_t*);
      break;
    case REC_terminate:
      RECPTR( rec) = SNetMemAlloc( sizeof( snet_record_types_t));
      break;
    case REC_sort_begin:
      RECPTR( rec) = SNetMemAlloc( sizeof( snet_record_types_t));
      RECORD( rec, sort_begin_rec) = SNetMemAlloc( sizeof( sort_begin_t));
      SORT_B_REC( rec, level) = va_arg( args, int);
      SORT_B_REC( rec, num) =   va_arg( args, int);    
      break;
    case REC_sort_end:
      RECPTR( rec) = SNetMemAlloc( sizeof( snet_record_types_t));
      RECORD( rec, sort_end_rec) = SNetMemAlloc( sizeof( sort_end_t));
      SORT_E_REC( rec, level) = va_arg( args, int);
      SORT_E_REC( rec, num) = va_arg( args, int);
      break;

    default:
      printf("\n\n ** Fatal Error ** : Unknown control record description. [%d]  \n\n", descr);
      exit( 1);
  }

  va_end( args); 

  return( rec);
}


extern void SNetRecDestroy( snet_record_t *rec) {

  int i;
  int num, *names;
  
  if( rec != NULL) {
  switch( REC_DESCR( rec)) {
    case REC_data: {
        void (*freefun)(void*);
        num = SNetRecGetNumFields( rec);
        names = SNetRecGetUnconsumedFieldNames( rec);
        freefun = SNetGetFreeFunFromRec( rec);
        for( i=0; i<num; i++) {
          freefun( SNetRecTakeField( rec, names[i]));
        }
        SNetMemFree( names);
        SNetTencDestroyVariantEncoding( DATA_REC( rec, v_enc));
        SNetMemFree( DATA_REC( rec, fields));
        SNetMemFree( DATA_REC( rec, tags));
        SNetMemFree( DATA_REC( rec, btags));
        while( DATA_REC( rec, counters)) {
          SNetRecRemoveIteration(rec);
        }
        SNetMemFree( RECORD( rec, data_rec));
      }
      break;

    case REC_sync:
      SNetMemFree( RECORD( rec, sync_rec));
      break;
    case REC_collect:
      SNetMemFree( RECORD( rec, star_rec));
      break;
      
    case REC_sort_begin:
        SNetMemFree( RECORD( rec, sort_begin_rec));
      break;
    
    case REC_sort_end:
        SNetMemFree( RECORD( rec, sort_end_rec));
      break;

    case REC_terminate:
      break;
      
    default:
      printf("\n\n ** Fatal Error ** : Unknown control record description in RECdestroy(). \n\n");
      exit( 1);
      break;
  }
  SNetMemFree( RECPTR( rec));
  SNetMemFree( rec);
 }
}

extern int SNetRecHasIteration(snet_record_t *rec) {
  switch(REC_DESCR(rec)) {
    case REC_data:
      return (DATA_REC(rec, counters) != NULL);
    break;
    default:
      printf(" \n\n ** Fatal Error **: Wrong type in SNetRecHAsITeration() (%d) \n\n", REC_DESCR(rec));
      exit(1);
    break;
  }
}

extern int SNetRecGetIteration(snet_record_t *rec) {
  int result;
  switch(REC_DESCR(rec)) {
    case REC_data:
      if(DATA_REC(rec, counters) != NULL) {
        result = (*(DATA_REC(rec, counters)))->counter;
      } else {
        printf("\n\n ** Fatal Error **: Iteration requested for data record with no initialized iterations!\n\n");
      }
      break;
    default:
      printf("\n\n ** Fatal Error **: Wrong type in RecGetIteration() (%d) \n\n", REC_DESCR(rec));
      exit(1);
    break;
  }
  return result;
}

extern void SNetRecIncIteration(snet_record_t *rec) {
  switch(REC_DESCR(rec)) {
    case REC_data:
      if(DATA_REC(rec, counters) != NULL) {
        (*DATA_REC(rec, counters))->counter++;
      } else {
        printf("\n\n ** Fatal Error **: IncIteration requested for data record with no inizialized iterations!\n\n");
      }
      break;
    default:
      printf("\n\n ** Fatal Error **: Wrong type in RecIncIteration() (%d) \n\n", REC_DESCR(rec));
      exit(1);
    break;
  }
}

extern void SNetRecAddIteration(snet_record_t *rec, int initial_value) {
  struct iteration_counters *new_iteration;
  switch(REC_DESCR(rec)) {
    case REC_data:
      new_iteration = SNetMemAlloc(sizeof(struct iteration_counters));
      new_iteration->counter = initial_value;
      new_iteration->next = *DATA_REC(rec, counters);
      DATA_REC(rec, counters) = &new_iteration;
    break;
    default:
      printf("\n\n ** Fatal Error **: Wrong type in RecAddIteration() (%d) \n\n", REC_DESCR(rec));
    break;
  }
}

extern void SNetRecRemoveIteration(snet_record_t *rec) {
  struct iteration_counters *to_delete;
  switch(REC_DESCR(rec)) {
    case REC_data:
      to_delete = *DATA_REC(rec, counters);
      DATA_REC(rec, counters) = &(to_delete->next);
      SNetMemFree(to_delete);
    break;
    default:
      printf("\n\n ** Fatal Error **: Wrong type in RecRemoveIteration() (%d) \n\n", REC_DESCR(rec));
    break;
  }
}


extern snet_record_descr_t SNetRecGetDescriptor( snet_record_t *rec) {
  return( REC_DESCR( rec));
}

extern snet_record_t *SNetRecSetInterfaceId( snet_record_t *rec, int id) 
{

  switch( REC_DESCR( rec)) {
    case REC_data:
      DATA_REC( rec, interface_id) = id;
      break;
    default:
      printf("\n\n ** Fatal Error ** : Wrong type in RECSetId() (%d) \n\n", REC_DESCR( rec));
      exit( 1);
      break;
  }

  return( rec);
}

extern int SNetRecGetInterfaceId( snet_record_t *rec) 
{

  int result;

  switch( REC_DESCR( rec)) {
    case REC_data:
      result = DATA_REC( rec, interface_id);
      break;
    default:
      printf("\n\n ** Fatal Error ** : Wrong type in RECGetId() (%d) \n\n", REC_DESCR( rec));
      exit( 1);
      break;
  }

  return( result);
}

extern snet_buffer_t *SNetRecGetBuffer( snet_record_t *rec) 
{
  snet_buffer_t *inbuf;

  switch( REC_DESCR( rec)) {
    case REC_sync:
      inbuf = SYNC_REC( rec, inbuf);
      break;
    case REC_collect:
      inbuf = STAR_REC( rec, outbuf);
      break;
    default:
      printf("\n\n ** Fatal Error ** : Wrong type in RECgetBuffer() (%d) \n\n", REC_DESCR( rec));
      exit( 1);
      break;
  }

  return( inbuf);
}


extern void SNetRecSetTag( snet_record_t *rec, int name, int val) {

  int offset=0;
  
  offset = FindName( SNetTencGetTagNames( GetVEnc( rec)), SNetTencGetNumTags( GetVEnc( rec)), name);
  DATA_REC( rec, tags[offset]) = val;
}

extern void SNetRecSetBTag( snet_record_t *rec, int name, int val) {
 
  int offset;
  
  offset = FindName( SNetTencGetBTagNames( GetVEnc( rec)), SNetTencGetNumBTags( GetVEnc( rec)), name);
  DATA_REC( rec, btags[offset]) = val;
}


extern void SNetRecSetField( snet_record_t *rec, int name, void *val) {
 
  int offset;
  
  offset = FindName( SNetTencGetFieldNames( GetVEnc( rec)), SNetTencGetNumFields( GetVEnc( rec)), name);
  DATA_REC( rec, fields[offset]) = val;
}



extern int SNetRecGetTag( snet_record_t *rec, int name) {
  
  int offset;
  offset = FindName( SNetTencGetTagNames( GetVEnc( rec)), SNetTencGetNumTags( GetVEnc( rec)), name);
  if( offset == NOT_FOUND) {
    NotFoundError( name, "get","tag");
  }
  return( DATA_REC( rec, tags[offset]));
}


extern int SNetRecGetBTag( snet_record_t *rec, int name) {
  
  int offset;
  
  offset = FindName( SNetTencGetBTagNames( GetVEnc( rec)), SNetTencGetNumBTags( GetVEnc( rec)), name);
  if( offset == NOT_FOUND) {
    NotFoundError( name, "get","binding tag");
  }
  return( DATA_REC( rec, btags[offset]));
}

extern void *SNetRecGetField( snet_record_t *rec, int name) {
  
  int offset;
  
  offset = FindName( SNetTencGetFieldNames( GetVEnc( rec)), SNetTencGetNumFields( GetVEnc( rec)), name);
  if( offset == NOT_FOUND) {
    NotFoundError( name, "get", "field");
  }
  return( DATA_REC( rec, fields[offset]));
}

extern int SNetRecTakeTag( snet_record_t *rec, int name) {
  
  int offset;
 
  offset = FindName( SNetTencGetTagNames( GetVEnc( rec)), SNetTencGetNumTags( GetVEnc( rec)), name);
  if( offset == NOT_FOUND) {
    NotFoundError( name, "take", "tag");
  }
  SNetTencRenameTag( GetVEnc( rec), name, CONSUMED);  
  return( DATA_REC( rec, tags[offset]));
}


extern int SNetRecTakeBTag( snet_record_t *rec, int name) {
  
  int offset;
 
  offset = FindName( SNetTencGetBTagNames( GetVEnc( rec)), SNetTencGetNumBTags( GetVEnc( rec)), name);
  if( offset == NOT_FOUND) {
    NotFoundError( name, "take","binding tag");
  }
  SNetTencRenameBTag( GetVEnc( rec), name, CONSUMED);  
  return( DATA_REC( rec, btags[offset]));
}


extern void *SNetRecTakeField( snet_record_t *rec, int name) {
  
  int offset;
 
  offset = FindName( SNetTencGetFieldNames( GetVEnc( rec)), SNetTencGetNumFields( GetVEnc( rec)), name);
  if( offset == NOT_FOUND) {
    NotFoundError( name, "take","field");
  }
  SNetTencRenameField( GetVEnc( rec), name, CONSUMED);  
  return( DATA_REC( rec, fields[offset]));
}


extern int SNetRecGetNumTags( snet_record_t *rec) {

  GETNUM( SNetTencGetNumTags, SNetTencGetTagNames)
}

extern int SNetRecGetNumBTags( snet_record_t *rec) {

  GETNUM( SNetTencGetNumBTags, SNetTencGetBTagNames)
}

extern int SNetRecGetNumFields( snet_record_t *rec) {
 
  GETNUM( SNetTencGetNumFields, SNetTencGetFieldNames)
}

extern int *SNetRecGetTagNames( snet_record_t *rec) {

  return( SNetTencGetTagNames( GetVEnc( rec)));
}


extern int *SNetRecGetBTagNames( snet_record_t *rec) {

  return( SNetTencGetBTagNames( GetVEnc( rec)));
}


extern int *SNetRecGetFieldNames( snet_record_t *rec) {

  return( SNetTencGetFieldNames( GetVEnc( rec)));
}



extern int SNetRecGetNum( snet_record_t *rec) {
  
  int counter;
  
  switch( REC_DESCR( rec)) {
    case REC_sort_begin:
      counter = SORT_B_REC( rec, num);  
      break;
    case REC_sort_end:
      counter = SORT_E_REC( rec, num);
      break;
    default:
      printf("\n\n ** Fatal Error ** : Wrong type in RECgetCounter() (%d) \n\n", REC_DESCR( rec));
      exit( 1);
      break;
  }  
  
  return( counter);
}

extern void SNetRecSetNum( snet_record_t *rec, int value) {
  
    switch( REC_DESCR( rec)) {
    case REC_sort_begin:
      SORT_B_REC( rec, num) = value;  
      break;
    case REC_sort_end:
      SORT_E_REC( rec, num) = value;
      break;
    default:
      printf("\n\n ** Fatal Error ** : Wrong type in RECgetCounter() (%d) \n\n", REC_DESCR( rec));
      exit( 1);
      break;
  }  
}


extern int SNetRecGetLevel( snet_record_t *rec) {
  
  int counter;
  
  switch( REC_DESCR( rec)) {
    case REC_sort_begin:
      counter = SORT_B_REC( rec, level);  
      break;
    case REC_sort_end:
      counter = SORT_E_REC( rec, level);
      break;
    default:
      printf("\n\n ** Fatal Error ** : Wrong type in RECgetCounter() (%d) \n\n", REC_DESCR( rec));
      exit( 1);
      break;
  }  
  
  return( counter);
}


extern void SNetRecSetLevel( snet_record_t *rec, int value) {
  
  switch( REC_DESCR( rec)) {
    case REC_sort_begin:
      SORT_B_REC( rec, level) = value;  
      break;
    case REC_sort_end:
      SORT_E_REC( rec, level) = value;
      break;
    default:
      printf("\n\n ** Fatal Error ** : Wrong type in RECgetCounter() (%d) \n\n", REC_DESCR( rec));
      exit( 1);
      break;
  }  
}
#define GET_UNCONSUMED_NAMES( RECNUM, RECNAMES)\
  int *names, i,j,k;\
  if( RECNUM( rec) == 0) {\
    names = NULL;\
  }\
  else {\
    names = SNetMemAlloc( RECNUM( rec) * sizeof( int));\
   j = 0; k = 0;\
   for( i=0; i<RECNUM( rec); i++) {\
       while( RECNAMES( rec)[k] == CONSUMED) {\
         k += 1;\
       }\
       names[j] = RECNAMES( rec)[k];\
       j += 1; k += 1;\
    }\
  }\
  return( names);


extern int *SNetRecGetUnconsumedFieldNames( snet_record_t *rec) {
  GET_UNCONSUMED_NAMES( SNetRecGetNumFields, SNetRecGetFieldNames);
}

extern int *SNetRecGetUnconsumedTagNames( snet_record_t *rec) {
  GET_UNCONSUMED_NAMES( SNetRecGetNumTags, SNetRecGetTagNames);
}

extern int *SNetRecGetUnconsumedBTagNames( snet_record_t *rec) {
  GET_UNCONSUMED_NAMES( SNetRecGetNumBTags, SNetRecGetBTagNames);
}


#define ADD_TO_RECORD( TENCADD, TENCNUM, FIELD, TYPE)\
  int i;\
  TYPE *data;\
  if( TENCADD( GetVEnc( rec), name)) {\
    data = SNetMemAlloc( TENCNUM( GetVEnc( rec)) * sizeof( TYPE));\
    for( i=0; i<( TENCNUM( GetVEnc( rec)) - 1); i++) {\
      data[i] = rec->rec->data_rec->FIELD[i];\
    }\
    SNetMemFree( rec->rec->data_rec->FIELD);\
    rec->rec->data_rec->FIELD = data;\
    return( true);\
  }\
  else {\
    return( false);\
  }

extern bool SNetRecAddTag( snet_record_t *rec, int name) {
  ADD_TO_RECORD( SNetTencAddTag, SNetTencGetNumTags, tags, int);
}
extern bool SNetRecAddBTag( snet_record_t *rec, int name) {
  ADD_TO_RECORD( SNetTencAddBTag, SNetTencGetNumBTags, btags, int);
}
extern bool SNetRecAddField( snet_record_t *rec, int name) {
  ADD_TO_RECORD( SNetTencAddField, SNetTencGetNumFields, fields, void*);
}




extern void SNetRecRenameTag( snet_record_t *rec, int name, int new_name) {
  SNetTencRenameTag( GetVEnc( rec), name, new_name);
}
extern void SNetRecRenameBTag( snet_record_t *rec, int name, int new_name) {
  SNetTencRenameBTag( GetVEnc( rec), name, new_name);
}
extern void SNetRecRenameField( snet_record_t *rec, int name, int new_name) {
  SNetTencRenameField( GetVEnc( rec), name, new_name);
}


extern void SNetRecRemoveTag( snet_record_t *rec, int name) {
  REMOVE_FROM_REC( SNetRecGetNumTags, SNetRecGetUnconsumedTagNames, SNetRecGetTag, SNetTencRemoveTag, tags, int);
}
extern void SNetRecRemoveBTag( snet_record_t *rec, int name) {
  REMOVE_FROM_REC( SNetRecGetNumBTags, SNetRecGetUnconsumedBTagNames, SNetRecGetBTag, SNetTencRemoveBTag, btags, int);
}
extern void SNetRecRemoveField( snet_record_t *rec, int name) {
  REMOVE_FROM_REC( SNetRecGetNumFields, SNetRecGetUnconsumedFieldNames, SNetRecGetField, SNetTencRemoveField, fields, void*);
}


extern snet_record_t *SNetRecCopy( snet_record_t *rec) {
  struct iteration_counters *current;
  struct iteration_counters *new_element;
  int i;
  snet_record_t *new_rec;
  switch( REC_DESCR( rec)) {
  
    case REC_data:
      new_rec = SNetRecCreate( REC_data, SNetTencCopyVariantEncoding( GetVEnc( rec)));
 
      for( i=0; i<SNetRecGetNumTags( rec); i++) {
        DATA_REC( new_rec, tags[i]) = DATA_REC( rec, tags[i]); 
      }
      for( i=0; i<SNetRecGetNumBTags( rec); i++) {
        DATA_REC( new_rec, btags[i]) = DATA_REC( rec, btags[i]); 
      }
      void* (*copyfun)(void*);      
      copyfun = SNetGetCopyFunFromRec( rec);
      for( i=0; i<SNetRecGetNumFields( rec); i++) {
        DATA_REC( new_rec, fields[i]) = copyfun( DATA_REC( rec, fields[i])); 
      }
      SNetRecSetInterfaceId( new_rec, SNetRecGetInterfaceId( rec));

      /*
       * We have to copy the stack of iteration counters into the new array;
       * assume, this counters are:
       * |-i1-i2-...-i(n-1)-in, whereas | is the bottom of the stack, i1 is the
       * element first added to the stack, i2 is added after i1, ...
       * then we create a temporary stack with the elements reversed by traversing
       * the stack in the given record and pushing a copy of each element onto the new
       * stack, resulting in: |-in-i(n-1)-i(n-2)-...-i2-i1.
       * After this, we have to traverse this stack top down and push every element 
       * on the result stack, resulting in:
       * |-i1-i2-...-i(n-1)-in 
       */
      struct iteration_counters *temp = NULL;
      current = *DATA_REC(rec, counters);
      while(current != NULL) {
        new_element = SNetMemAlloc(sizeof(struct iteration_counters));
        new_element->counter = current->counter;
        new_element->next = temp;
        temp = new_element;
        current = current->next;
      }
      while(temp != NULL) {
        temp->next = *DATA_REC(new_rec, counters);
        DATA_REC(new_rec, counters) = &temp;
        temp = temp->next;
      }
      break;
    case REC_sort_begin:
      new_rec = SNetRecCreate( REC_DESCR( rec),  SORT_B_REC( rec, level),
                               SORT_B_REC( rec, num));
      break;
    case REC_sort_end:
      new_rec = SNetRecCreate( REC_DESCR( rec),  SORT_E_REC( rec, level),  
                               SORT_E_REC( rec, num));    
      break;
    case REC_terminate:
      new_rec = SNetRecCreate( REC_terminate);
      break;
    default:
      printf("\n\n ** Fatal Error ** : Can't copy record of that type [%d]\n\n", (int)REC_DESCR( rec));
      exit( 1);
  }

  return( new_rec);
}
/*
extern snet_lang_descr_t SNetRecGetLanguage( snet_record_t *rec) {

  snet_lang_descr_t res;

  switch( REC_DESCR( rec)) {
    case REC_data:
      res = DATA_REC( rec, lang);
      break;
    default:
      printf("\n\n ** Fatal Error ** : Wrong type in SNetGetLanguage() (%d)"
             "\n\n", REC_DESCR( rec));
      exit( 1);
      break;
  }  
  return( res);
}

extern void SNetRecSetLanguage( snet_record_t *rec, snet_lang_descr_t lang) {

  switch( REC_DESCR( rec)) {
    case REC_data:
      DATA_REC( rec, lang) = lang;
      break;
    default:
      printf("\n\n ** Fatal Error ** : Wrong type in SNetSetLanguage() (%d)"
             "\n\n", REC_DESCR( rec));
      exit( 1);
      break;
  }  
}
*/

extern snet_variantencoding_t *SNetRecGetVariantEncoding( snet_record_t *rec) {
  return( GetVEnc( rec));
}



extern bool SNetRecHasTag( snet_record_t *rec, int name) {
  bool result;

  result = ( FindName( SNetRecGetTagNames( rec), 
             SNetRecGetNumTags( rec), name) == NOT_FOUND) ? false : true;

  return( result);
}

extern bool SNetRecHasBTag( snet_record_t *rec, int name) {
  bool result;

  result = ( FindName( SNetRecGetBTagNames( rec), 
             SNetRecGetNumBTags( rec), name) == NOT_FOUND) ? false : true;

  return( result);
}

extern bool SNetRecHasField( snet_record_t *rec, int name) {
  bool result;

  result = ( FindName( SNetRecGetFieldNames( rec), 
             SNetRecGetNumTags( rec), name) == NOT_FOUND) ? false : true;

  return( result);
}


#include "star.h"

#include "record.h"
#include "typeencode.h"
#include "threading.h"
#include "buffer.h"
#include "expression.h"
#include "handle.h"
#include "serial.h"
#include "debug.h"
#include "collectors.h"

#ifdef DISTRIBUTED_SNET
#include "routing.h"
#include "fun.h"
#endif /* DISTRIBUTED_SNET */

//#define STAR_DEBUG
/* ------------------------------------------------------------------------- */
/*  SNetStar                                                                 */
/* ------------------------------------------------------------------------- */

static bool 
MatchesExitPattern( snet_record_t *rec, 
                    snet_typeencoding_t *patterns, 
                    snet_expr_list_t *guards) 
{

  int i;
  bool is_match;
  snet_variantencoding_t *pat;

  for( i=0; i<SNetTencGetNumVariants( patterns); i++) {

    is_match = true;

    if( SNetEevaluateBool( SNetEgetExpr( guards, i), rec)) {
      pat = SNetTencGetVariant( patterns, i+1);
      is_match = SNetRecPatternMatches(pat, rec);
    }
    else {
      is_match = false;
    }

    if( is_match) {
      break;
    }
  }
  return( is_match);
}


static void *StarBoxThread( void *hndl)
{
#ifdef STAR_DEBUG
  char record_message[100];
#endif
  snet_handle_t *hnd = (snet_handle_t*)hndl;
  snet_startup_fun_t box;
  snet_startup_fun_t self;
  snet_tl_stream_t *real_outstream, *our_outstream, *starstream=NULL;
  bool terminate = false;
  snet_typeencoding_t *exit_tags;
  snet_record_t *rec, *current_sort_rec = NULL;
  snet_expr_list_t *guards;

#ifdef DISTRIBUTED_SNET
  int node_id;
  snet_fun_id_t fun_id;
  snet_info_t *info;
#endif /* DISTRIBUTED_SNET */

#ifdef STAR_DEBUG
  SNetUtilDebugNotice("(CREATION STAR)");
#endif

  real_outstream = SNetHndGetOutput( hnd);
  exit_tags = SNetHndGetType( hnd);
  box = SNetHndGetBoxfunA( hnd);
  self = SNetHndGetBoxfunB( hnd);
  guards = SNetHndGetGuardList( hnd);

  our_outstream = SNetTlCreateStream( BUFFER_SIZE);

#ifdef DISTRIBUTED_SNET
  node_id = SNetIDServiceGetNodeID();

  if(!SNetDistFunFun2ID(box, &fun_id)) {
    /* TODO: This is an error!*/
  }
#endif /* DISTRIBUTED_SNET */

#ifdef STAR_DEBUG
  SNetUtilDebugNotice("-");
  SNetUtilDebugNotice("| NONDET STAR CREATED");
  SNetUtilDebugNotice("| input: %p", SNetHndGetInput(hnd));
  SNetUtilDebugNotice("| output to next iteration: %p", 
                      our_outstream);
  SNetUtilDebugNotice("| output to collector: %p", 
                      real_outstream);
  SNetUtilDebugNotice("-");
#endif

  while( !( terminate)) {
#ifdef STAR_DEBUG
    SNetUtilDebugNotice("STAR %p: Reading from %p",
                        real_outstream,
                        SNetHndGetInput(hnd));
#endif
    rec = SNetTlRead( SNetHndGetInput( hnd));

    switch( SNetRecGetDescriptor( rec)) {
      case REC_data:
        if( MatchesExitPattern( rec, exit_tags, guards)) {
#ifdef STAR_DEBUG
          SNetUtilDebugDumpRecord(rec, record_message);
          SNetUtilDebugNotice("STAR %p stopping iterations of %s",
                              real_outstream,
                              record_message);
#endif
          SNetTlWrite(real_outstream, rec);
        }
        else {
          if( starstream == NULL) {
#ifdef STAR_DEBUG
            SNetUtilDebugNotice("STAR %p creating new instance",
                                real_outstream);
#endif
            // register new buffer with dispatcher,
            // starstream is returned by self, which is SNetStarIncarnate
#ifdef DISTRIBUTED_SNET
	    info = SNetInfoInit();

	    SNetInfoSetRoutingContext(info, SNetRoutingContextInit(SNetRoutingGetNewID(), true, node_id, &fun_id, node_id));

	    starstream = SNetSerial(our_outstream, info, node_id , box, self);
	      
	    starstream = SNetRoutingContextEnd(SNetInfoGetRoutingContext(info), starstream);

	    SNetInfoDestroy(info);
#else
            starstream = SNetSerial(our_outstream, box, self);

#ifdef STAR_DEBUG
            SNetUtilDebugNotice("STAR %p has created a new instance",
                                real_outstream);
#endif
#endif /* DISTRIBUTED_SNET */
	    SNetTlWrite(real_outstream, SNetRecCreate(REC_collect, starstream));

/*            if( current_sort_rec != NULL) {
              fprintf( stderr, "put\n");
              SNetBufPut( our_outbuf, SNetRecCreate( REC_sort_begin,
                              SNetRecGetLevel( current_sort_rec),
                              SNetRecGetNum( current_sort_rec)));
              }*/

          }
#ifdef STAR_DEBUG
          SNetUtilDebugDumpRecord(rec, record_message);
          SNetUtilDebugNotice("STAR %p: outputting %s to %p",
                              real_outstream,
                              record_message,
                              our_outstream);
#endif
         SNetTlWrite(our_outstream, rec);
       }
       break;

      case REC_sync:
#ifdef STAR_DEBUG
        SNetUtilDebugNotice("STAR %p: resetting input stream to %p",
                            real_outstream,
                            SNetRecGetStream(rec));
#endif
        SNetHndSetInput(hnd, SNetRecGetStream( rec));
        SNetRecDestroy( rec);
        break;

      case REC_collect:
        SNetUtilDebugNotice("[STAR] unhandled control record,"
                            " destroying it\n\n");
        SNetRecDestroy( rec);
        break;
      case REC_sort_begin:
        SNetRecDestroy( current_sort_rec);
        current_sort_rec = SNetRecCreate( REC_sort_begin, SNetRecGetLevel( rec),
                                          SNetRecGetNum( rec));
        if( starstream != NULL) {
          SNetTlWrite(our_outstream, SNetRecCreate( REC_sort_begin,
                                     SNetRecGetLevel( rec),
                                     SNetRecGetNum( rec)));
        }
        SNetTlWrite(SNetHndGetOutput(hnd), rec);
        break;
      case REC_sort_end:
        if( starstream != NULL) {
          SNetTlWrite(our_outstream, SNetRecCreate( REC_sort_end,
                                    SNetRecGetLevel( rec),
                                    SNetRecGetNum( rec)));
        }
        SNetTlWrite(SNetHndGetOutput( hnd), rec);
        break;
      case REC_terminate:
        terminate = true;
        SNetHndDestroy( hnd);
        if( starstream != NULL) {
          SNetTlWrite(our_outstream, SNetRecCopy( rec));
        }
        SNetTlWrite( real_outstream, rec);
        SNetTlMarkObsolete(our_outstream);
	SNetTlMarkObsolete(real_outstream);
        break;
      case REC_probe:
        if(starstream != NULL) {
          SNetTlWrite(our_outstream, SNetRecCopy(rec));
        }
        SNetTlWrite(our_outstream, rec);
      break;
    default:
      SNetUtilDebugNotice("[Star] Unknown control record destroyed (%d).\n", SNetRecGetDescriptor( rec));
      SNetRecDestroy( rec);
      break;
    }
  }

  return( NULL);
}

extern snet_tl_stream_t *SNetStar( snet_tl_stream_t *input,
#ifdef DISTRIBUTED_SNET
				   snet_info_t *info, 
				   int location,
#endif /* DISTRIBUTED_SNET */
                                   snet_typeencoding_t *type,
                                   snet_expr_list_t *guards,
				   snet_startup_fun_t box_a,
				   snet_startup_fun_t box_b)
{
  snet_tl_stream_t *star_output, *dispatch_output;
  snet_handle_t *hnd;

#ifdef DISTRIBUTED_SNET
  input = SNetRoutingContextUpdate(SNetInfoGetRoutingContext(info), input, location); 

  if(location == SNetIDServiceGetNodeID()) {

#ifdef DISTRIBUTED_DEBUG
    SNetUtilDebugNotice("Star created");
#endif /* DISTRIBUTED_DEBUG */

#endif /* DISTRIBUTED_SNET */

    star_output = SNetTlCreateStream(BUFFER_SIZE);
    
    hnd = SNetHndCreate(HND_star,
			input, star_output, box_a, box_b, type, guards, false);
    
    
    SNetTlCreateComponent(StarBoxThread, hnd, ENTITY_star_nondet);
    
    dispatch_output = CreateCollector( star_output);
    
#ifdef DISTRIBUTED_SNET
  } else {

    SNetEdestroyList( guards);

    SNetDestroyTypeEncoding( type);

    dispatch_output = input;
}
#endif /* DISTRIBUTED_SNET */

  return( dispatch_output);
}

extern snet_tl_stream_t *SNetStarIncarnate(snet_tl_stream_t *input,
#ifdef DISTRIBUTED_SNET
					   snet_info_t *info, 
					   int location,
#endif /* DISTRIBUTED_SNET */
					   snet_typeencoding_t *type,
					   snet_expr_list_t *guards,
					   snet_startup_fun_t box_a,
					   snet_startup_fun_t box_b)
{

  snet_tl_stream_t *output;
  snet_handle_t *hnd;

#ifdef DISTRIBUTED_SNET
  input = SNetRoutingContextUpdate(SNetInfoGetRoutingContext(info), input, location);

  if(location == SNetIDServiceGetNodeID()) {

#ifdef DISTRIBUTED_DEBUG
    SNetUtilDebugNotice("Star incarnate created");
#endif /* DISTRIBUTED_DEBUG */

#endif /* DISTRIBUTED_SNET */

    output = SNetTlCreateStream( BUFFER_SIZE);
    
    hnd = SNetHndCreate(HND_star,
			input, output, box_a, box_b, type, guards, true);
    
    SNetTlCreateComponent(StarBoxThread, hnd, ENTITY_star_nondet);
    
#ifdef DISTRIBUTED_SNET
  } else {
    SNetEdestroyList( guards);

    SNetDestroyTypeEncoding( type);

    output = input;
}
#endif /* DISTRIBUTED_SNET */
  
  return( output);
}





/* ------------------------------------------------------------------------- */
/*  SNetStarDet                                                              */
/* ------------------------------------------------------------------------- */

static void *DetStarBoxThread( void *hndl) {

  snet_handle_t *hnd = (snet_handle_t*)hndl;
  snet_startup_fun_t box;
  snet_startup_fun_t self;
  snet_tl_stream_t *real_output, *our_output, *starstream=NULL;
  bool terminate = false;
  snet_typeencoding_t *exit_tags;
  snet_record_t *rec;
  snet_expr_list_t *guards;
 
  snet_record_t *sort_begin = NULL, *sort_end = NULL;
  int counter = 0;

#ifdef DISTRIBUTED_SNET
  int node_id;
  snet_fun_id_t fun_id;
  snet_info_t *info;
#endif /* DISTRIBUTED_SNET */

  real_output = SNetHndGetOutput( hnd);
  exit_tags = SNetHndGetType( hnd);
  box = SNetHndGetBoxfunA( hnd);
  self = SNetHndGetBoxfunB( hnd);
  guards = SNetHndGetGuardList( hnd);
   
  our_output = SNetTlCreateStream( BUFFER_SIZE);

#ifdef DISTRIBUTED_SNET
  node_id = SNetIDServiceGetNodeID();

  if(!SNetDistFunFun2ID(box, &fun_id)) {
    /* TODO: This is an error!*/
  }
#endif /* DISTRIBUTED_SNET */

  while( !( terminate)) {

#ifdef STAR_DEBUG
    SNetUtilDebugNotice("STAR %p: reading from %p",
                        real_output,
                        SNetHndGetInput(hnd));
#endif
    rec = SNetTlRead( SNetHndGetInput( hnd));

    switch( SNetRecGetDescriptor( rec)) {
      case REC_data:
        if( !( SNetHndIsIncarnate( hnd))) { 
          SNetRecDestroy( sort_begin); SNetRecDestroy( sort_end);
          sort_begin = SNetRecCreate( REC_sort_begin, 0, counter);
          sort_end = SNetRecCreate( REC_sort_end, 0, counter);
        
          SNetTlWrite(real_output, SNetRecCopy( sort_begin));
          if( starstream != NULL) {
            SNetTlWrite(our_output, SNetRecCopy( sort_begin));
          }
          
          if(MatchesExitPattern( rec, exit_tags, guards)) {
            SNetTlWrite(real_output, rec);
          }
          else {
            if(starstream == NULL) {
#ifdef DISTRIBUTED_SNET
	      info = SNetInfoInit();

	      SNetInfoSetRoutingContext(info, SNetRoutingContextInit(SNetRoutingGetNewID(), true, node_id, &fun_id, node_id));

	      starstream = SNetSerial(our_output, info, node_id , box, self);
	      
	      starstream = SNetRoutingContextEnd(SNetInfoGetRoutingContext(info), starstream);

	      SNetInfoDestroy(info);
#else
	      starstream = SNetSerial(our_output, box, self);

#endif /* DISTRIBUTED_SNET */  	      
	      SNetTlWrite(real_output, SNetRecCreate(REC_collect,starstream));
	      
            }
#ifdef STAR_DEBUG
            SNetUtilDebugNotice("STAR %p: iteration: putting %p to %p",
                                real_output,
                                rec,
                                our_output);
#endif
            SNetTlWrite( our_output, rec);
          }

          SNetTlWrite( real_output, SNetRecCopy( sort_end));
          if( starstream != NULL) {
            SNetTlWrite(our_output, SNetRecCopy( sort_end));
          }
          counter += 1;
        }
        else { /* incarnation */
          if( MatchesExitPattern( rec, exit_tags, guards)) {
           SNetTlWrite(real_output, rec);
         }
         else {
           if( starstream == NULL) {
             // register new buffer with dispatcher,
             // starstream is returned by self, which is SNetStarIncarnate

#ifdef DISTRIBUTED_SNET
	     info = SNetInfoInit();

	     SNetInfoSetRoutingContext(info, SNetRoutingContextInit(SNetRoutingGetNewID(), true, node_id, &fun_id, node_id));
	     
	     starstream = SNetSerial(our_output, info, node_id , box, self);
	     
	     starstream = SNetRoutingContextEnd(SNetInfoGetRoutingContext(info), starstream);
	     
	     SNetInfoDestroy(info);
#else
	     starstream = SNetSerial(our_output, box, self);
#endif /* DISTRIBUTED_SNET */      
     
             SNetTlWrite(real_output, SNetRecCreate(REC_collect, starstream));
//           SNetTlWrite( our_output, sort_begin); /* sort_begin is set in "case REC_sort_xxx" */
          }
#ifdef STAR_DEBUG
           SNetUtilDebugNotice("STAR %p: outputting %p to %p",
                               real_output,
                               rec,
                               our_output);
#endif
          SNetTlWrite( our_output, rec);
         }
        }
       break;

      case REC_sync:
        SNetHndSetInput(hnd, SNetRecGetStream( rec));
        SNetRecDestroy( rec);
        break;

      case REC_collect:
        SNetUtilDebugNotice("[STAR] Unhandled control record,"
                      " destroying it.\n\n");
        SNetRecDestroy( rec);
        break;
      case REC_sort_end:
      case REC_sort_begin:
        if( !( SNetHndIsIncarnate( hnd))) {
          SNetRecSetLevel( rec, SNetRecGetLevel( rec) + 1);
        }
        SNetTlWrite( SNetHndGetOutput(hnd), SNetRecCopy( rec));
        if( starstream != NULL) {
          SNetTlWrite(our_output, SNetRecCopy( rec));
        }
        else {
          if( SNetRecGetDescriptor( rec) == REC_sort_begin) {
            SNetRecDestroy( sort_begin);
            sort_begin = SNetRecCopy( rec);
          }
          else {
            SNetRecDestroy( sort_end);
            sort_end = SNetRecCopy( rec);
          }
        }
        SNetRecDestroy( rec);
        break;
      case REC_terminate:

        terminate = true;
        
        SNetHndDestroy( hnd);
        if( starstream != NULL) {
          SNetTlWrite( our_output, SNetRecCreate( REC_sort_begin, 0, counter));
          SNetTlWrite( our_output, SNetRecCopy( rec));
        }
        SNetTlWrite( real_output, SNetRecCreate( REC_sort_begin, 0, counter));
        SNetTlWrite( real_output, rec);
        SNetTlMarkObsolete(our_output);
	SNetTlMarkObsolete(real_output);
        break;
      case REC_probe:
        if(starstream != NULL) {
          SNetTlWrite(our_output, SNetRecCopy(rec));
        }
        SNetTlWrite(our_output, rec);
      break;
    default:
      SNetUtilDebugNotice("[Star] Unknown control record destroyed (%d).\n", SNetRecGetDescriptor( rec));
      SNetRecDestroy( rec);
      break;
    }
  }

  return( NULL);
}

extern snet_tl_stream_t *SNetStarDet(snet_tl_stream_t *input,
#ifdef DISTRIBUTED_SNET
				     snet_info_t *info, 
				     int location,
#endif /* DISTRIBUTED_SNET */
				     snet_typeencoding_t *type,
				     snet_expr_list_t *guards,
				     snet_startup_fun_t box_a,
				     snet_startup_fun_t box_b)
{

    
  snet_tl_stream_t *star_output, *dispatch_output;
  snet_handle_t *hnd;

#ifdef DISTRIBUTED_SNET
  input = SNetRoutingContextUpdate(SNetInfoGetRoutingContext(info), input, location);

  if(location == SNetIDServiceGetNodeID()) {

#ifdef DISTRIBUTED_DEBUG
    SNetUtilDebugNotice("DetStar created");
#endif /* DISTRIBUTED_DEBUG */

#endif /* DISTRIBUTED_SNET */

    star_output = SNetTlCreateStream( BUFFER_SIZE);
    
    hnd = SNetHndCreate( HND_star, 
			 input, star_output, 
			 box_a, box_b, 
			 type, guards, false);
#ifdef STAR_DEBUG
    SNetUtilDebugNotice("-");
    SNetUtilDebugNotice("| DET STAR CREATED");
    SNetUtilDebugNotice("| input: %p", input);
    SNetUtilDebugNotice("| output: %p", star_output);
    SNetUtilDebugNotice("-");
#endif
    
    SNetTlCreateComponent(DetStarBoxThread, hnd, ENTITY_star_det);
    
    dispatch_output = CreateDetCollector( star_output);
    
#ifdef DISTRIBUTED_SNET
  } else {
    SNetEdestroyList( guards);

    SNetDestroyTypeEncoding( type);

    dispatch_output = input;
}
#endif /* DISTRIBUTED_SNET */

  return(dispatch_output);
}

extern snet_tl_stream_t *SNetStarDetIncarnate(snet_tl_stream_t *input,
#ifdef DISTRIBUTED_SNET
				       snet_info_t *info, 
				       int location,
#endif /* DISTRIBUTED_SNET */
				       snet_typeencoding_t *type,
				       snet_expr_list_t *guards,
				       snet_startup_fun_t box_a,
				       snet_startup_fun_t box_b)
{
  snet_tl_stream_t *output;
  snet_handle_t *hnd;

#ifdef DISTRIBUTED_SNET
  input = SNetRoutingContextUpdate(SNetInfoGetRoutingContext(info), input, location);

  if(location == SNetIDServiceGetNodeID()) {

#ifdef DISTRIBUTED_DEBUG
    SNetUtilDebugNotice("DetStar incarnate created");
#endif /* DISTRIBUTED_DEBUG */

#endif /* DISTRIBUTED_SNET */

    output = SNetTlCreateStream(BUFFER_SIZE);
    
    hnd = SNetHndCreate( HND_star, \
			 input, output, 
			 box_a, box_b, 
			 type, guards, true);
    
#ifdef STAR_DEBUG
    SNetUtilDebugNotice("-");
    SNetUtilDebugNotice("| DET STAR INCARNATE CREATED");
    SNetUtilDebugNotice("| input: %p", input);
    SNetUtilDebugNotice("| output: %p", output);
    SNetUtilDebugNotice("-");
#endif
    
    SNetTlCreateComponent(DetStarBoxThread, hnd, ENTITY_star_det);
    
#ifdef DISTRIBUTED_SNET
  } else {
    SNetEdestroyList( guards);

    SNetDestroyTypeEncoding( type);

    output = input;
}
#endif /* DISTRIBUTED_SNET */
  
  return( output);
}







/**
 * @file ts02.snet.c
 *
 * Source code of compiled snet-file for runtime.
 *
 * THIS FILE HAS BEEN GENERATED.
 * DO NOT EDIT THIS FILE.
 * EDIT THE ORIGINAL SNET-SOURCE FILE ts02.snet INSTEAD!
 *
 * ALL CHANGES MADE TO THIS FILE WILL BE OVERWRITTEN!
 *
*/

#include "ts02.snet.h"
#include "networkinterface.h"

char *snet_ts02_labels[] = {
	"E__ts02__None",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"T"};

char *snet_ts02_interfaces[] = {};


static void SNet__ts02__A(snet_handle_t *hnd)
{
  snet_record_t *rec = NULL;
  void *field_a = NULL;

  rec = SNetHndGetRecord(hnd);

  field_a = SNetRecTakeField(rec, F__ts02__a);

  SNetCall__A(hnd, field_a);
}


static void SNet__ts02__B(snet_handle_t *hnd)
{
  snet_record_t *rec = NULL;
  void *field_b = NULL;

  rec = SNetHndGetRecord(hnd);

  field_b = SNetRecTakeField(rec, F__ts02__b);

  SNetCall__B(hnd, field_b);
}


static void SNet__ts02__C(snet_handle_t *hnd)
{
  snet_record_t *rec = NULL;
  void *field_c = NULL;

  rec = SNetHndGetRecord(hnd);

  field_c = SNetRecTakeField(rec, F__ts02__c);

  SNetCall__C(hnd, field_c);
}


static void SNet__ts02__D(snet_handle_t *hnd)
{
  snet_record_t *rec = NULL;
  void *field_d = NULL;

  rec = SNetHndGetRecord(hnd);

  field_d = SNetRecTakeField(rec, F__ts02__d);

  SNetCall__D(hnd, field_d);
}


static void SNet__ts02__E(snet_handle_t *hnd)
{
  snet_record_t *rec = NULL;
  void *field_e = NULL;

  rec = SNetHndGetRecord(hnd);

  field_e = SNetRecTakeField(rec, F__ts02__e);

  SNetCall__E(hnd, field_e);
}

static snet_tl_stream_t *SNet__ts02___SL___SL___SL(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;
  snet_typeencoding_t *out_type = NULL;
  snet_box_sign_t *out_sign = NULL;

  out_type = SNetTencTypeEncode(2, 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__b), 
                SNetTencCreateVector(0), 
                SNetTencCreateVector(0)), 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__b), 
                SNetTencCreateVector(1, T__ts02__T), 
                SNetTencCreateVector(0)));


  out_sign = SNetTencBoxSignEncode( out_type, 
              SNetTencCreateVector(1, field), 
              SNetTencCreateVector(2, field, tag));


  out_buf = SNetBox(in_buf, 
              &SNet__ts02__A, 
              out_sign);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL___SL___SR___ST___SL(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;
  snet_typeencoding_t *out_type = NULL;
  snet_box_sign_t *out_sign = NULL;

  out_type = SNetTencTypeEncode(2, 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__c), 
                SNetTencCreateVector(0), 
                SNetTencCreateVector(0)), 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__d), 
                SNetTencCreateVector(0), 
                SNetTencCreateVector(0)));


  out_sign = SNetTencBoxSignEncode( out_type, 
              SNetTencCreateVector(1, field), 
              SNetTencCreateVector(1, field));


  out_buf = SNetBox(in_buf, 
              &SNet__ts02__B, 
              out_sign);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL___SL___SR___ST___SR___PL(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;
  snet_typeencoding_t *out_type = NULL;
  snet_box_sign_t *out_sign = NULL;

  out_type = SNetTencTypeEncode(3, 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__b), 
                SNetTencCreateVector(0), 
                SNetTencCreateVector(0)), 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__b), 
                SNetTencCreateVector(1, T__ts02__T), 
                SNetTencCreateVector(0)), 
              SNetTencVariantEncode(
                SNetTencCreateVector(2, F__ts02__b, F__ts02__e), 
                SNetTencCreateVector(1, T__ts02__T), 
                SNetTencCreateVector(0)));


  out_sign = SNetTencBoxSignEncode( out_type, 
              SNetTencCreateVector(1, field), 
              SNetTencCreateVector(2, field, tag), 
              SNetTencCreateVector(3, field, field, tag));


  out_buf = SNetBox(in_buf, 
              &SNet__ts02__C, 
              out_sign);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL___SL___SR___ST___SR___PR(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;
  snet_typeencoding_t *out_type = NULL;
  snet_box_sign_t *out_sign = NULL;

  out_type = SNetTencTypeEncode(3, 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__b), 
                SNetTencCreateVector(0), 
                SNetTencCreateVector(0)), 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__b), 
                SNetTencCreateVector(1, T__ts02__T), 
                SNetTencCreateVector(0)), 
              SNetTencVariantEncode(
                SNetTencCreateVector(2, F__ts02__b, F__ts02__e), 
                SNetTencCreateVector(1, T__ts02__T), 
                SNetTencCreateVector(0)));


  out_sign = SNetTencBoxSignEncode( out_type, 
              SNetTencCreateVector(1, field), 
              SNetTencCreateVector(2, field, tag), 
              SNetTencCreateVector(3, field, field, tag));


  out_buf = SNetBox(in_buf, 
              &SNet__ts02__D, 
              out_sign);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL___SL___SR___ST___SR(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;

  out_buf = SNetParallelDet(in_buf, 
              SNetTencCreateTypeEncodingList(2, 
                SNetTencTypeEncode(1, 
                  SNetTencVariantEncode(
                    SNetTencCreateVector(1, F__ts02__c), 
                    SNetTencCreateVector(0), 
                    SNetTencCreateVector(0))), 
                SNetTencTypeEncode(1, 
                  SNetTencVariantEncode(
                    SNetTencCreateVector(1, F__ts02__d), 
                    SNetTencCreateVector(0), 
                    SNetTencCreateVector(0)))), 
              &SNet__ts02___SL___SL___SR___ST___SR___PL, 
              &SNet__ts02___SL___SL___SR___ST___SR___PR);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL___SL___SR___ST(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;

  out_buf = SNetSerial(in_buf, 
              &SNet__ts02___SL___SL___SR___ST___SL, 
              &SNet__ts02___SL___SL___SR___ST___SR);

  return (out_buf);
}

static snet_tl_stream_t *SNet____STAR_INCARNATE_ts02___SL___SL___SR(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;

  out_buf = SNetStarIncarnate(in_buf, 
              SNetTencTypeEncode(1, 
                SNetTencVariantEncode(
                  SNetTencCreateVector(1, F__ts02__e), 
                  SNetTencCreateVector(1, T__ts02__T), 
                  SNetTencCreateVector(0))), 
              SNetEcreateList(1, 
                SNetElt( 
                  SNetEtag( T__ts02__T), 
                  SNetEconsti( 5))), 
              &SNet__ts02___SL___SL___SR___ST, 
              &SNet____STAR_INCARNATE_ts02___SL___SL___SR);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL___SL___SR(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;

  out_buf = SNetStar(in_buf, 
              SNetTencTypeEncode(1, 
                SNetTencVariantEncode(
                  SNetTencCreateVector(1, F__ts02__e), 
                  SNetTencCreateVector(1, T__ts02__T), 
                  SNetTencCreateVector(0))), 
              SNetEcreateList(1, 
                SNetElt( 
                  SNetEtag( T__ts02__T), 
                  SNetEconsti( 5))), 
              &SNet__ts02___SL___SL___SR___ST, 
              &SNet____STAR_INCARNATE_ts02___SL___SL___SR);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL___SL(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;

  out_buf = SNetSerial(in_buf, 
              &SNet__ts02___SL___SL___SL, 
              &SNet__ts02___SL___SL___SR);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL___SR___IS(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;
  snet_typeencoding_t *out_type = NULL;
  snet_box_sign_t *out_sign = NULL;

  out_type = SNetTencTypeEncode(2, 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__e), 
                SNetTencCreateVector(0), 
                SNetTencCreateVector(0)), 
              SNetTencVariantEncode(
                SNetTencCreateVector(1, F__ts02__f), 
                SNetTencCreateVector(0), 
                SNetTencCreateVector(0)));


  out_sign = SNetTencBoxSignEncode( out_type, 
              SNetTencCreateVector(1, field), 
              SNetTencCreateVector(1, field));


  out_buf = SNetBox(in_buf, 
              &SNet__ts02__E, 
              out_sign);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL___SR(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;

  out_buf = SNetSplit(in_buf, 
              &SNet__ts02___SL___SR___IS, T__ts02__T, T__ts02__T);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SL(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;

  out_buf = SNetSerial(in_buf, 
              &SNet__ts02___SL___SL, 
              &SNet__ts02___SL___SR);

  return (out_buf);
}

static snet_tl_stream_t *SNet__ts02___SR(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;

  out_buf = SNetSync(in_buf, 
              SNetTencTypeEncode(1, 
                SNetTencVariantEncode(
                  SNetTencCreateVector(2, F__ts02__e, F__ts02__f), 
                  SNetTencCreateVector(0), 
                  SNetTencCreateVector(0))), 
              SNetTencTypeEncode(2, 
                SNetTencVariantEncode(
                  SNetTencCreateVector(1, F__ts02__e), 
                  SNetTencCreateVector(0), 
                  SNetTencCreateVector(0)), 
                SNetTencVariantEncode(
                  SNetTencCreateVector(1, F__ts02__f), 
                  SNetTencCreateVector(0), 
                  SNetTencCreateVector(0))), 
              NULL);

  return (out_buf);
}

snet_tl_stream_t *SNet__ts02___ts02(snet_tl_stream_t *in_buf)
{
  snet_tl_stream_t *out_buf = NULL;

  out_buf = SNetSerial(in_buf, 
              &SNet__ts02___SL, 
              &SNet__ts02___SR);

  return (out_buf);
}

int SNetMain__ts02(int argc, char* argv[])
{
  int ret = 0;

  SNetGlobalInitialise();

  ret = SNetInRun(argc, argv,
              snet_ts02_labels,
              SNET__ts02__NUMBER_OF_LABELS,
              snet_ts02_interfaces,
              SNET__ts02__NUMBER_OF_INTERFACES,
              SNet__ts02___ts02);

  SNetGlobalDestroy();

  return( ret);
}


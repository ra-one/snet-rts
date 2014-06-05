# include <stdlib.h>
# include <stdio.h>
# include <math.h>
#include <string.h>
# include "fftBoxes.h"
# include <unistd.h>
#include "/shared/nil/snetInstall/include/scc.h"

/*
FILE *srcFile, *snkFile;
timespecSCC srcT,snkT;


static inline void PrintTimingN( const timespecSCC *t, FILE *file)
{
	if (t->tv_sec == 0) {
		(void) fprintf( file, "%lu\n", t->tv_nsec);
	} else {
		(void) fprintf( file, "%lu%09lu\n",
				(unsigned long) t->tv_sec, (t->tv_nsec)
		);
	}
}
*/

void step ( int n, int mj, float a[], float b[], float c[], float d[], 
  float w[], float sgn );

void printval(int n, float x[]){
    int i;
    for (i = 0; i < n * 2; i=i+2)
      printf("%f  %f\n", x[i], x[i+1]);

  printf("------\n");
}

void *stepP(void *hnd, c4snet_data_t *dx, c4snet_data_t *dy, c4snet_data_t *dw, int s, int ln, int size, int node) {
  void **data = (void **) C4SNetGetData(dx);
  float *x = (float *) *data;
  data = (void **) C4SNetGetData(dy);
  float *y = (float *) *data;
  data = (void **) C4SNetGetData(dw);
  float *w = (float *) *data;

  int n = 1 << ln;
  int mj;
  int i;
  for (i = s; i < size + s; i++) {
    mj = 1 << i;
    if (i % 2 == 1)
        step ( n, mj, &y[0*2+0], &y[(n/2)*2+0], &x[0*2+0], &x[mj*2+0], w, 1.0 );
    else
       step ( n, mj, &x[0*2+0], &x[(n/2)*2+0], &y[0*2+0], &y[mj*2+0], w, 1.0 );
  }
  /*printval(n, x);
  printval(n, y);*/
 if ((i == ln) && (i % 2 == 0)) 
    C4SNetOut(hnd, 1, dy, dx, dw, i, ln, size, node); // last pass
 else
    C4SNetOut(hnd, 1, dx, dy, dw, i, ln, size, node); 
 //usleep(240000); //240 millisecond, 4 time box function
 return hnd;
}


void cffti ( int n, float w[] )
{
  float arg;
  float aw;
  int i;
  int n2;
  const float pi = 3.141592653589793;

  n2 = n / 2;
  aw = 2.0 * pi / ( ( float ) n );

  for ( i = 0; i < n2; i++ )
  {
    arg = aw * ( ( float ) i );
    w[i*2+0] = cos ( arg );
    w[i*2+1] = sin ( arg );
  }
  return;
}


void step ( int n, int mj, float a[], float b[], float c[],
  float d[], float w[], float sgn )
{
  float ambr;
  float ambu;
  int j;
  int ja;
  int jb;
  int jc;
  int jd;
  int jw;
  int k;
  int lj;
  int mj2;
  float wjw[2];

  mj2 = 2 * mj;
  lj  = n / mj2;

  for ( j = 0; j < lj; j++ )
  {
    jw = j * mj;
    ja  = jw;
    jb  = ja;
    jc  = j * mj2;
    jd  = jc;

    wjw[0] = w[jw*2+0]; 
    wjw[1] = w[jw*2+1];

    if ( sgn < 0.0 ) 
    {
      wjw[1] = - wjw[1];
    }

    for ( k = 0; k < mj; k++ )
    {
      c[(jc+k)*2+0] = a[(ja+k)*2+0] + b[(jb+k)*2+0];
      c[(jc+k)*2+1] = a[(ja+k)*2+1] + b[(jb+k)*2+1];

      ambr = a[(ja+k)*2+0] - b[(jb+k)*2+0];
      ambu = a[(ja+k)*2+1] - b[(jb+k)*2+1];

      d[(jd+k)*2+0] = wjw[0] * ambr - wjw[1] * ambu;
      d[(jd+k)*2+1] = wjw[1] * ambr + wjw[0] * ambu;
    }
  }
  return;
}

void *initP(void *hnd, c4snet_data_t *dx, int ln, int size, int node) {
  int n = 1 << ln;
  float *y = (float *) SCCMallocPtr(sizeof(float) * n * 2);
  float *w = (float *) SCCMallocPtr(sizeof(float) * n);
  cffti(n, w);
  
  C4SNetOut(hnd, 1, dx, C4SNetCreate(CTYPE_char, sizeof(void *), &y), C4SNetCreate(CTYPE_char, sizeof(void *), &w), 0, ln, size, node);
  //usleep(240000); //240 millisecond, 4 time box function
  return hnd;
}

void *snet_sink(void *hnd, c4snet_data_t *dx, c4snet_data_t *dy, c4snet_data_t *dw, int s, int ln, int size, int node) {
  static int msg = 0;
  if(!msg)printf("================================\n\tSINK start\n================================\n"); 
  printf("Mess %d\n",++msg);
  void **data = C4SNetGetData(dx);
  float *x = (float *) *data;
  SCCFreePtr(x);
  C4SNetFree(dx);
  
  data = C4SNetGetData(dw);
  float *w = (float *) *data;
  SCCFreePtr(w);
  C4SNetFree(dw);
  
  data = C4SNetGetData(dy);
  float *y = (float *) *data;
  SCCFreePtr(y);
  C4SNetFree(dy);
  return hnd;
}

void *snet_source(void *hnd, int mess, int ln, int size, int num_node) {
  int n = 1 << ln;
  int i, j;
  float *x;
  x = (float *) SCCMallocPtr(sizeof(float) * n * 2);
  for (i = 0; i < n; i++) {
    x[i] = i;
    x[i + n] = i + 1;
  }
  printf("================================\n\tSOURCE start\n================================\n"); 
  for (j = 0; j < mess; j++) {
    float *s = SCCMallocPtr(sizeof(float) * n * 2);
    memcpy(s, x, sizeof(float) * n * 2);
    C4SNetOut(hnd, 1, C4SNetCreate(CTYPE_char, sizeof(void *), &s), ln, size, j % num_node);
    printf("Mess %d\n",j);
  }
  
  SCCFreePtr(x);
  return hnd;
}


#include <unistd.h>
#include <complex.h>

typedef struct {
    char f0[20];   // original func
    char f1[20];   // new func 
    char fg[20];   // function group
    void *fptr;    // ptr to the orignal func
    double time_in;   // inclusive time
    double time_ex;   // exclusive time
    size_t count;
} libprof_freplace ;

extern libprof_freplace libprof_farray[] ;
extern int libprof_fsize;

enum findex{
#include "BLAS/index1.h"
#include "LAPACK/index1.h"
#include "PBLAS/index1.h"
#include "ScaLAPACK/index1.h"
};



#define X(f0_val, f1_val, fgroup) { f0_val, f1_val, fgroup, NULL, 0.0, 0.0, 0 }


#define libprof_int_t int
#define libprof_fcomplex_t complex
#define libprof_dcomplex_t double complex


#include "BLAS/index2.h"
#include "LAPACK/index2.h"
#include "PBLAS/index2.h"
#include "ScaLAPACK/index2.h"

#define INIT_FARRAY \
  BLAS,\
  LAPACK,\
  PBLAS,\
  ScaLAPACK

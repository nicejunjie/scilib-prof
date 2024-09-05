#include <unistd.h>

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
// level-3 BLAS
  sgemm,
  dgemm,
  cgemm,
  cgemm3m,
  zgemm,
  zgemm3m,
  ssymm,
  dsymm,
  csymm,
  zsymm,
  ssyrk,
  dsyrk,
  csyrk,
  zsyrk,
  ssyr2k,
  dsyr2k,
  csyr2k,
  zsyr2k,
  strmm,
  dtrmm,
  ctrmm,
  ztrmm,
  strsm,
  dtrsm,
  ctrsm,
  ztrsm,
  chemm,
  zhemm,
  cherk,
  zherk,
  cher2k,
  zher2k,
// level-2 BLAS
  sgbmv, 
  dgbmv, 
  cgbmv, 
  zgbmv, 
  sgemv, 
  dgemv, 
  cgemv, 
  zgemv, 
  sger, 
  dger,
  cgerc, 
  zgerc, 
  cgeru, 
  zgeru, 
  chbmv, 
  zhbmv, 
  chemv, 
  zhemv, 
  cher,
  zher,
  cher2, 
  zher2, 
  chpmv, 
  zhpmv, 
  chpr,
  zhpr,
  chpr2, 
  zhpr2, 
  ssbmv, 
  dsbmv, 
  sspmv, 
  dspmv, 
  sspr,
  dspr,
  sspr2, 
  dspr2, 
  ssymv, 
  dsymv, 
  ssyr,
  dsyr,
  ssyr2, 
  dsyr2, 
  stbmv, 
  dtbmv, 
  ctbmv, 
  ztbmv, 
  stbsv, 
  dtbsv, 
  ctbsv, 
  ztbsv, 
  stpmv, 
  dtpmv, 
  ctpmv, 
  ztpmv, 
  stpsv, 
  dtpsv, 
  ctpsv, 
  ztpsv, 
  strmv, 
  dtrmv, 
  ctrmv, 
  ztrmv, 
  strsv, 
  dtrsv, 
  ctrsv, 
  ztrsv, 
// level-1 BLAS
   sasum,
   dasum,
   scasum,
   dzasum,
   sdot,
   ddot,
   sdsdot,
   dsdot,
   snrm2,
   dnrm2,
   scnrm2,
   dznrm2,
   isamax,
   idamax,
   icamax,
   izamax,
   isamin,
   idamin,
   icamin,
   izamin,
   scabs1,
   dcabs1,
   saxpy,
   daxpy,
   caxpy,
   zaxpy,
   scopy,
   dcopy,
   ccopy,
   zcopy,
   sscal,
   dscal,
   cscal,
   zscal,
   sswap,
   dswap,
   cswap,
   zswap,
   srotg,
   drotg,
   crotg,
   zrotg,
   srot,
   drot,
   crot,
   zrot,
   srotm,
   drotm,
   srotmg,
   drotmg,
   saxpby,
   daxpby,
   caxpby,
   zaxpby,
};

#define X(f0_val, f1_val) { f0_val, f1_val, "BLAS", NULL, 0.0, 0.0, 0 }
#define INIT_BLAS_L3 \
    X("sgemm_", "my_sgemm"), \
    X("dgemm_", "my_dgemm"), \
    X("cgemm_", "my_cgemm"), \
    X("cgemm_", "my_cgemm3m"), \
    X("zgemm_", "my_zgemm"), \
    X("zgemm_", "my_zgemm3m"), \
    X("ssymm_", "my_ssymm"), \
    X("dsymm_", "my_dsymm"), \
    X("csymm_", "my_csymm"), \
    X("zsymm_", "my_zsymm"), \
    X("ssyrk_", "my_ssyrk"), \
    X("dsyrk_", "my_dsyrk"), \
    X("csyrk_", "my_csyrk"), \
    X("zsyrk_", "my_zsyrk"), \
    X("ssyr2k_", "my_ssyr2k"), \
    X("dsyr2k_", "my_dsyr2k"), \
    X("csyr2k_", "my_csyr2k"), \
    X("zsyr2k_", "my_zsyr2k"), \
    X("strmm_", "my_strmm"), \
    X("dtrmm_", "my_dtrmm"), \
    X("ctrmm_", "my_ctrmm"), \
    X("ztrmm_", "my_ztrmm"), \
    X("strsm_", "my_strsm"), \
    X("dtrsm_", "my_dtrsm"), \
    X("ctrsm_", "my_ctrsm"), \
    X("ztrsm_", "my_ztrsm"), \
    X("chemm_", "my_chemm"), \
    X("zhemm_", "my_zhemm"), \
    X("cherk_", "my_cherk"), \
    X("zherk_", "my_zherk"), \
    X("cher2k_", "my_cher2k"), \
    X("zher2k_", "my_zher2k")

#define INIT_BLAS_L2 \
    X("sgbmv_", "my_sgbmv"), \
    X("dgbmv_", "my_dgbmv"), \
    X("cgbmv_", "my_cgbmv"), \
    X("zgbmv_", "my_zgbmv"), \
    X("sgemv_", "my_sgemv"), \
    X("dgemv_", "my_dgemv"), \
    X("cgemv_", "my_cgemv"), \
    X("zgemv_", "my_zgemv"), \
    X("sger_", "my_sger"), \
    X("dger_", "my_dger"), \
    X("cgerc_", "my_cgerc"), \
    X("zgerc_", "my_zgerc"), \
    X("cgeru_", "my_cgeru"), \
    X("zgeru_", "my_zgeru"), \
    X("chbmv_", "my_chbmv"), \
    X("zhbmv_", "my_zhbmv"), \
    X("chemv_", "my_chemv"), \
    X("zhemv_", "my_zhemv"), \
    X("cher_", "my_cher"), \
    X("zher_", "my_zher"), \
    X("cher2_", "my_cher2"), \
    X("zher2_", "my_zher2"), \
    X("chpmv_", "my_chpmv"), \
    X("zhpmv_", "my_zhpmv"), \
    X("chpr_", "my_chpr"), \
    X("zhpr_", "my_zhpr"), \
    X("chpr2_", "my_chpr2"), \
    X("zhpr2_", "my_zhpr2"), \
    X("ssbmv_", "my_ssbmv"), \
    X("dsbmv_", "my_dsbmv"), \
    X("sspmv_", "my_sspmv"), \
    X("dspmv_", "my_dspmv"), \
    X("sspr_", "my_sspr"), \
    X("dspr_", "my_dspr"), \
    X("sspr2_", "my_sspr2"), \
    X("dspr2_", "my_dspr2"), \
    X("ssymv_", "my_ssymv"), \
    X("dsymv_", "my_dsymv"), \
    X("ssyr_", "my_ssyr"), \
    X("dsyr_", "my_dsyr"), \
    X("ssyr2_", "my_ssyr2"), \
    X("dsyr2_", "my_dsyr2"), \
    X("stbmv_", "my_stbmv"), \
    X("dtbmv_", "my_dtbmv"), \
    X("ctbmv_", "my_ctbmv"), \
    X("ztbmv_", "my_ztbmv"), \
    X("stbsv_", "my_stbsv"), \
    X("dtbsv_", "my_dtbsv"), \
    X("ctbsv_", "my_ctbsv"), \
    X("ztbsv_", "my_ztbsv"), \
    X("stpmv_", "my_stpmv"), \
    X("dtpmv_", "my_dtpmv"), \
    X("ctpmv_", "my_ctpmv"), \
    X("ztpmv_", "my_ztpmv"), \
    X("stpsv_", "my_stpsv"), \
    X("dtpsv_", "my_dtpsv"), \
    X("ctpsv_", "my_ctpsv"), \
    X("ztpsv_", "my_ztpsv"), \
    X("strmv_", "my_strmv"), \
    X("dtrmv_", "my_dtrmv"), \
    X("ctrmv_", "my_ctrmv"), \
    X("ztrmv_", "my_ztrmv"), \
    X("strsv_", "my_strsv"), \
    X("dtrsv_", "my_dtrsv"), \
    X("ctrsv_", "my_ctrsv"), \
    X("ztrsv_", "my_ztrsv")

#define INIT_BLAS_L1 \
    X("sasum_", "my_sasum"), \
    X("dasum_", "my_dasum"), \
    X("scasum_", "my_scasum"), \
    X("dzasum_", "my_dzasum"), \
    X("sdot_", "my_sdot"), \
    X("ddot_", "my_ddot"), \
    X("sdsdot_", "my_sdsdot"), \
    X("dsdot_", "my_dsdot"), \
    X("snrm2_", "my_snrm2"), \
    X("dnrm2_", "my_dnrm2"), \
    X("scnrm2_", "my_scnrm2"), \
    X("dznrm2_", "my_dznrm2"), \
    X("isamax_", "my_isamax"), \
    X("idamax_", "my_idamax"), \
    X("icamax_", "my_icamax"), \
    X("izamax_", "my_izamax"), \
    X("isamin_", "my_isamin"), \
    X("idamin_", "my_idamin"), \
    X("icamin_", "my_icamin"), \
    X("izamin_", "my_izamin"), \
    X("scabs1_", "my_scabs1"), \
    X("dcabs1_", "my_dcabs1"), \
    X("saxpy_", "my_saxpy"), \
    X("daxpy_", "my_daxpy"), \
    X("caxpy_", "my_caxpy"), \
    X("zaxpy_", "my_zaxpy"), \
    X("scopy_", "my_scopy"), \
    X("dcopy_", "my_dcopy"), \
    X("ccopy_", "my_ccopy"), \
    X("zcopy_", "my_zcopy"), \
    X("sscal_", "my_sscal"), \
    X("dscal_", "my_dscal"), \
    X("cscal_", "my_cscal"), \
    X("zscal_", "my_zscal"), \
    X("sswap_", "my_sswap"), \
    X("dswap_", "my_dswap"), \
    X("cswap_", "my_cswap"), \
    X("zswap_", "my_zswap"), \
    X("srotg_", "my_srotg"), \
    X("drotg_", "my_drotg"), \
    X("crotg_", "my_crotg"), \
    X("zrotg_", "my_zrotg"), \
    X("srot_", "my_srot"), \
    X("drot_", "my_drot"), \
    X("crot_", "my_crot"), \
    X("zrot_", "my_zrot"), \
    X("srotm_", "my_srotm"), \
    X("drotm_", "my_drotm"), \
    X("srotmg_", "my_srotmg"), \
    X("drotmg_", "my_drotmg"), \
    X("saxpby_", "my_saxpby"), \
    X("daxpby_", "my_daxpby"), \
    X("caxpby_", "my_caxpby"), \
    X("zaxpby_", "my_zaxpby") 




#define INIT_FARRAY \ 
   INIT_BLAS_L3, \
   INIT_BLAS_L2, \
   INIT_BLAS_L1





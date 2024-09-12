# Scientific Libraries Profiler

Profiling is ready for BLAS PBLAS, LAPACK, and ScaLAPACK. 


To do: CBLAS, LAPACKe, FFTW, MPI.

## Compile
simply `make`, the following will be created:
`libprof-dbi.so`: profiling with Dynamic Binary Instrumentation, works for both dynamically linked and statically linked libs. 
`libprof-dl.so` : profiling based on runtime symbol resolution, only works for dynamically linked libs but is friendly when working with other tools, such as scilib-accel. 


## Usage
`LD_PRELOAD=$PATH_TO_SCILIB_PROF/libprof-dbi.so`
or 
`LD_PRELOAD=$PATH_TO_SCILIB_PROF/libprof-dl.so`

## Sample output

```
-------------------------------------------------------------------------------
                      Scientific Library Profiler    
-------------------------------------------------------------------------------
 for application: /scratch/07893/junjieli/parsec/use-nvpl/nvhpc24.3/parsec_dev-omp/src/parsec-nvfortran.mpi.omp
 total runtime (s):    113.246, library time (s):     79.993, lib percentage: 70.6%.

                            Inclusive Times                                   
-------------------------------------------------------------------------------
           group     function           count [imb%]           time(s) [imb%]  
-------------------------------------------------------------------------------
  1        PBLAS      pdgemm_            1505 [  0.0%]         60.937 [  0.5%] 
  2         BLAS       dgemm_           32048 [ 46.3%]         59.367 [ 10.0%] 
  3    ScaLAPACK     pdsyevd_              10 [  0.0%]         11.402 [  0.0%] 
  4        PBLAS      pdtrsm_             750 [  0.0%]          8.395 [ 33.7%] 
  5    ScaLAPACK     pdsytrd_               5 [  0.0%]          7.782 [  0.0%] 
  6    ScaLAPACK     pdlatrd_             745 [  0.0%]          7.014 [  2.8%] 
  7        PBLAS      pdsymv_           23840 [  0.0%]          5.502 [ 20.2%] 
  8    ScaLAPACK     pdstedc_               5 [  0.0%]          2.811 [  0.6%] 
  9    ScaLAPACK     pdlaed0_               5 [  0.0%]          2.791 [  0.6%] 
 10    ScaLAPACK     pdlaed1_             745 [  0.0%]          2.786 [  0.6%] 
 11    ScaLAPACK     pdlaed3_             745 [  0.0%]          1.150 [  3.7%] 
 12        PBLAS      pdgemv_          143040 [  0.0%]          1.120 [ 82.9%] 
 13    ScaLAPACK     pdormtr_               5 [  0.0%]          0.783 [  2.1%] 
 14    ScaLAPACK     pdormqr_               5 [  0.0%]          0.774 [  0.0%] 
 15        PBLAS     pdsyr2k_             745 [  0.0%]          0.766 [ 25.6%] 
 16       LAPACK      dlaed4_           85058 [  1.4%]          0.739 [  4.0%] 
 17    ScaLAPACK     pdlarfb_             745 [  0.0%]          0.691 [  6.9%] 
 18         BLAS       dgemv_          860236 [ 54.7%]          0.647 [ 88.9%] 
 19    ScaLAPACK     pdpotrf_               5 [  0.0%]          0.400 [  2.3%] 
 20        PBLAS      pdsyrk_             745 [  0.0%]          0.372 [  3.0%] 
 21         BLAS       dcopy_         1549273 [> 999%]          0.324 [533.9%] 
 22         BLAS       dnrm2_           82759 [  0.0%]          0.231 [  5.0%] 
 23    ScaLAPACK     pdlarfg_           23840 [  0.0%]          0.214 [ 59.4%] 
 24        PBLAS       pddot_           47840 [  0.0%]          0.202 [ 38.5%] 
 25         BLAS       dtrsm_             397 [  1.3%]          0.179 [ 40.7%] 
 26        PBLAS      pdnrm2_           11920 [  1.3%]          0.159 [ 66.1%] 
 27         BLAS        ddot_           73229 [  5.0%]          0.149 [ 23.6%] 
 28    ScaLAPACK     pdlaed2_             745 [  0.0%]          0.113 [ 47.3%] 
 29    ScaLAPACK     pdlamch_             755 [  0.0%]          0.077 [ 67.2%] 
 30    ScaLAPACK     pdlaedz_             745 [  0.0%]          0.043 [115.2%] 
...
...
-------------------------------------------------------------------------------
  * inclusive time: time including all children functions
  * average count and timing reported.
  * imblance metric: imb% = (max-min)/avg*100%. 


                            Exclusive Times                                   
-------------------------------------------------------------------------------
           group     function           count [imb%]           time(s) [imb%]  
-------------------------------------------------------------------------------
  1         BLAS       dgemm_           32048 [ 46.3%]         59.367 [ 10.0%] 
  2        PBLAS      pdgemm_            1505 [  0.0%]          8.045 [ 28.7%] 
  3        PBLAS      pdsymv_           23840 [  0.0%]          4.822 [ 31.3%] 
  4        PBLAS      pdtrsm_             750 [  0.0%]          2.066 [133.8%] 
  5        PBLAS      pdgemv_          143040 [  0.0%]          1.095 [ 85.2%] 
  6       LAPACK      dlaed4_           85058 [  1.4%]          0.735 [  4.0%] 
  7         BLAS       dgemv_          860236 [ 54.7%]          0.647 [ 88.9%] 
  8        PBLAS     pdsyr2k_             745 [  0.0%]          0.575 [ 12.2%] 
  9    ScaLAPACK     pdlarfb_             745 [  0.0%]          0.427 [ 22.8%] 
 10         BLAS       dcopy_         1549273 [> 999%]          0.324 [533.9%] 
 11        PBLAS      pdsyrk_             745 [  0.0%]          0.314 [  8.4%] 
 12         BLAS       dnrm2_           82759 [  0.0%]          0.231 [  5.0%] 
 13        PBLAS       pddot_           47840 [  0.0%]          0.199 [ 39.4%] 
 14    ScaLAPACK     pdlaed3_             745 [  0.0%]          0.180 [ 10.4%] 
 15         BLAS       dtrsm_             397 [  1.3%]          0.179 [ 40.7%] 
 16        PBLAS      pdnrm2_           11920 [  1.3%]          0.159 [ 66.1%] 
 17         BLAS        ddot_           73229 [  5.0%]          0.149 [ 23.6%] 
 18    ScaLAPACK     pdlamch_             755 [  0.0%]          0.077 [ 67.2%] 
 19    ScaLAPACK     pdlarfg_           23840 [  0.0%]          0.047 [113.6%] 
 20    ScaLAPACK     pdlaedz_             745 [  0.0%]          0.043 [115.3%] 
 21    ScaLAPACK     pdlarft_             745 [  0.0%]          0.035 [136.9%] 
 22    ScaLAPACK     pdlatrd_             745 [  0.0%]          0.024 [ 31.6%] 
 23         BLAS        dger_             160 [  0.0%]          0.023 [ 24.9%] 
 24    ScaLAPACK     pdpotf2_             750 [  0.0%]          0.017 [ 48.9%] 
 25         BLAS      dsyr2k_             775 [247.7%]          0.016 [249.6%] 
 26         BLAS       dsymv_           25337 [248.6%]          0.015 [247.1%] 
 27    ScaLAPACK     pdlaed2_             745 [  0.0%]          0.013 [  6.7%] 
 28       LAPACK      dlapy2_          177469 [  1.0%]          0.012 [ 31.8%] 
 29    ScaLAPACK      pdlarf_             160 [  0.0%]          0.012 [ 55.6%] 
 30    ScaLAPACK     pdormtr_               5 [  0.0%]          0.009 [175.8%] 
...
...
-------------------------------------------------------------------------------
                                     total library time (s):   79.993
-------------------------------------------------------------------------------
  * exclusive time: time excluding all children functions
  * average count and timing reported.
  * imblance metric: imb% = (max-min)/avg*100%. 
```

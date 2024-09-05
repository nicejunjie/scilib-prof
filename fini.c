#define _GNU_SOURCE
#include <dlfcn.h> //for dlsym


#include "freplace.h"
#include "utils.h"
#include "global.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>


int compare_time_in(const void *a, const void *b) {
    const libprof_freplace *fa = (const libprof_freplace *)a;
    const libprof_freplace *fb = (const libprof_freplace *)b;
    if (fa->time_in > fb->time_in) return -1;
    if (fa->time_in < fb->time_in) return 1;
    return 0;
}
int compare_time_ex(const void *a, const void *b) {
    const libprof_freplace *fa = (const libprof_freplace *)a;
    const libprof_freplace *fb = (const libprof_freplace *)b;
    if (fa->time_ex > fb->time_ex) return -1;
    if (fa->time_ex < fb->time_ex) return 1;
    return 0;
}

void serial_stats() {
    // Check if any count is non-zero
    int has_non_zero_count = 0;
    double libtime=0.0;
    for (int i = 0; i < libprof_fsize; i++) {
        if (libprof_farray[i].count > 0) {
            has_non_zero_count = 1;
            break;
        }
    }

    if (has_non_zero_count) {

        for (int i = 0; i < libprof_fsize; i++) libtime += libprof_farray[i].time_ex;

        fprintf(stderr,"\n\n-------------------------------------------------------------------------------\n");
        fprintf(stderr,"                      Scientific Library Profiler    \n");
        fprintf(stderr,"-------------------------------------------------------------------------------\n");

        char *exe_path;
        get_exe_path(&exe_path);
        fprintf(stderr," for application: %s\n", exe_path);
        fprintf(stderr," total runtime (s): %10.3f, library time (s): %10.3f, lib percentage: %4.1f%%.\n", \
                apptime, libtime, libtime/apptime*100);

        qsort(libprof_farray, libprof_fsize, sizeof(libprof_freplace), compare_time_in);
        fprintf(stderr,"\n                            Inclusive Times                                   \n");
        fprintf(stderr,"-------------------------------------------------------------------------------\n");
        fprintf(stderr, "           group     function        count           time(s)   \n");
        fprintf(stderr,"-------------------------------------------------------------------------------\n");
 
        for (int i = 0; i < libprof_fsize; i++) {
            if (libprof_farray[i].count > 0) {
                fprintf(stderr, "%2d    %10s   %10s   %10d   %15.3f\n", i,
                        libprof_farray[i].fg, libprof_farray[i].f0,
                        libprof_farray[i].count, libprof_farray[i].time_in);
            }
        }
        fprintf(stderr,"-------------------------------------------------------------------------------\n");
        fprintf(stderr,"  * inclusive time: time including all children functions\n");
        fprintf(stderr, "\n");

        fprintf(stderr, "\n");

        // Sort and print based on exclusive time
        qsort(libprof_farray, libprof_fsize, sizeof(libprof_freplace), compare_time_ex);
        fprintf(stderr,"\n                            Exclusive Times                                   \n");
        fprintf(stderr,"-------------------------------------------------------------------------------\n");
        fprintf(stderr, "           group     function        count           time(s)   \n");
        fprintf(stderr,"-------------------------------------------------------------------------------\n");
        for (int i = 0; i < libprof_fsize; i++) {
            if (libprof_farray[i].count > 0) {
                fprintf(stderr, "%2d    %10s   %10s   %10d   %15.3f\n", i,
                        libprof_farray[i].fg, libprof_farray[i].f0,
                        libprof_farray[i].count, libprof_farray[i].time_ex);
            }
        }
        fprintf(stderr,"-------------------------------------------------------------------------------\n");
        fprintf(stderr,"                            total library time (s): %8.3f\n", libtime);
        fprintf(stderr,"-------------------------------------------------------------------------------\n");
        fprintf(stderr,"  * exclusive time: time excluding all children functions\n");
        fprintf(stderr, "\n");


    }
}

 

// Structure to store statistics for sorting
typedef struct {
    char f0[20];
    char fg[20];
    double avg_time;
    double min_time;
    double max_time;
    size_t avg_count;
    size_t min_count;
    size_t max_count;
} libprof_stats;

// Comparison function for sorting based on avg time
int compare_avg_time(const void *a, const void *b) {
    libprof_stats *statsA = (libprof_stats *)a;
    libprof_stats *statsB = (libprof_stats *)b;
    if (statsA->avg_time > statsB->avg_time) return -1;
    if (statsA->avg_time < statsB->avg_time) return 1;
    return 0;
}


int MPI_Finalize(void) {
    return 0;
}
int mpi_finalize_(void){
    return 0;
}

int libprof_mpi_finalize(void){
    int (*fptr)(void)=dlsym(RTLD_NEXT, "PMPI_Finalize");
    return fptr();
}

void gather_and_calculate_stats() {

    size_t check_count=0;

    int mpi_initialized;
    MPI_Initialized(&mpi_initialized);

    if (!mpi_initialized) {
        MPI_Init(NULL, NULL);
    }

    int mpi_rank, mpi_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);


    // Create a datatype for libprof_freplace structure
    MPI_Datatype MPI_LIBPROF_FREPLACE;
    int blocklengths[7] = {20, 20, 20, 1, 1, 1, 1};
    MPI_Aint displacements[7];
    MPI_Datatype types[7] = {MPI_CHAR, MPI_CHAR, MPI_CHAR, MPI_BYTE, MPI_DOUBLE, MPI_DOUBLE, MPI_UNSIGNED_LONG};

    displacements[0] = offsetof(libprof_freplace, f0);
    displacements[1] = offsetof(libprof_freplace, f1);
    displacements[2] = offsetof(libprof_freplace, fg);
    displacements[3] = offsetof(libprof_freplace, fptr);
    displacements[4] = offsetof(libprof_freplace, time_in);
    displacements[5] = offsetof(libprof_freplace, time_ex);
    displacements[6] = offsetof(libprof_freplace, count);

    MPI_Type_create_struct(7, blocklengths, displacements, types, &MPI_LIBPROF_FREPLACE);
    MPI_Type_commit(&MPI_LIBPROF_FREPLACE);

    // Gather the arrays from all ranks
    libprof_freplace *all_data = NULL;
    if (mpi_rank == 0) {
        all_data = (libprof_freplace *)malloc(libprof_fsize * mpi_size * sizeof(libprof_freplace));
    }

    MPI_Gather(libprof_farray, libprof_fsize, MPI_LIBPROF_FREPLACE,
               all_data, libprof_fsize, MPI_LIBPROF_FREPLACE,
               0, MPI_COMM_WORLD);

    // Calculate min, avg, and max for each field if you are the root rank
    if (mpi_rank == 0) {
        libprof_stats *stats_inclusive = (libprof_stats *)malloc(libprof_fsize * sizeof(libprof_stats));
        libprof_stats *stats_exclusive = (libprof_stats *)malloc(libprof_fsize * sizeof(libprof_stats));
        double libtime_avg = 0.0;

        for (int i = 0; i < libprof_fsize; i++) {
            double min_time_in = all_data[i].time_in;
            double max_time_in = all_data[i].time_in;
            double sum_time_in = all_data[i].time_in;
            
            double min_time_ex = all_data[i].time_ex;
            double max_time_ex = all_data[i].time_ex;
            double sum_time_ex = all_data[i].time_ex;
            
            size_t min_count = all_data[i].count;
            size_t max_count = all_data[i].count;
            size_t sum_count = all_data[i].count;

            for (int j = 1; j < mpi_size; j++) {
                int idx = j * libprof_fsize + i;
                
                if (all_data[idx].time_in < min_time_in) min_time_in = all_data[idx].time_in;
                if (all_data[idx].time_in > max_time_in) max_time_in = all_data[idx].time_in;
                sum_time_in += all_data[idx].time_in;
                
                if (all_data[idx].time_ex < min_time_ex) min_time_ex = all_data[idx].time_ex;
                if (all_data[idx].time_ex > max_time_ex) max_time_ex = all_data[idx].time_ex;
                sum_time_ex += all_data[idx].time_ex;
                
                if (all_data[idx].count < min_count) min_count = all_data[idx].count;
                if (all_data[idx].count > max_count) max_count = all_data[idx].count;
                sum_count += all_data[idx].count;
            }

            double avg_time_in = sum_time_in / mpi_size;
            double avg_time_ex = sum_time_ex / mpi_size;
            size_t avg_count = sum_count / mpi_size;

            // Store calculated stats for sorting inclusive times
            strncpy(stats_inclusive[i].f0, all_data[i].f0, 20);
            strncpy(stats_inclusive[i].fg, all_data[i].fg, 20);
            stats_inclusive[i].avg_time = avg_time_in;
            stats_inclusive[i].min_time = min_time_in;
            stats_inclusive[i].max_time = max_time_in;
            stats_inclusive[i].avg_count = avg_count;
            stats_inclusive[i].min_count = min_count;
            stats_inclusive[i].max_count = max_count;

            // Store calculated stats for sorting exclusive times
            strncpy(stats_exclusive[i].f0, all_data[i].f0, 20);
            strncpy(stats_exclusive[i].fg, all_data[i].fg, 20);
            stats_exclusive[i].avg_time = avg_time_ex;
            stats_exclusive[i].min_time = min_time_ex;
            stats_exclusive[i].max_time = max_time_ex;
            stats_exclusive[i].avg_count = avg_count;
            stats_exclusive[i].min_count = min_count;
            stats_exclusive[i].max_count = max_count;

            if(max_count > check_count) check_count = max_count;
            libtime_avg += avg_time_ex;
        }

        // Sort the statistics arrays based on avg_time_in and avg_time_ex
        qsort(stats_inclusive, libprof_fsize, sizeof(libprof_stats), compare_avg_time);
        qsort(stats_exclusive, libprof_fsize, sizeof(libprof_stats), compare_avg_time);

        if (check_count > 0) {
          fprintf(stderr,"\n\n-------------------------------------------------------------------------------\n");
          fprintf(stderr,"                      Scientific Library Profiler    \n");
          fprintf(stderr,"-------------------------------------------------------------------------------\n");

          char *exe_path;
          get_exe_path(&exe_path);
          fprintf(stderr," for application: %s\n", exe_path);
          fprintf(stderr," total runtime (s): %10.3f, library time (s): %10.3f, lib percentage: %4.1f%%.\n", \
                apptime, libtime_avg, libtime_avg/apptime*100);

          fprintf(stderr,"\n                            Inclusive Times                                   \n");
          fprintf(stderr,"-------------------------------------------------------------------------------\n");
          fprintf(stderr, "           group     function           count [imb%]           time(s) [imb%]  \n");
          fprintf(stderr,"-------------------------------------------------------------------------------\n");
          for (int i = 0; i < libprof_fsize; i++) {
              if(stats_inclusive[i].max_count >0) {
/*
                 fprintf(stderr," %2d  %10s       %10zu,%10zu,%10zu        %8.3f,%8.3f,%8.3f \n", i, stats_inclusive[i].f0, \
                      stats_inclusive[i].min_count, stats_inclusive[i].avg_count, stats_inclusive[i].max_count, \
                      stats_inclusive[i].min_time, stats_inclusive[i].avg_time, stats_inclusive[i].max_time);
*/
                 fprintf(stderr," %2d   %10s   %10s    %12zu [%5.1f%]       %8.3f [%5.1f%] \n", i,\
                      stats_inclusive[i].fg, stats_inclusive[i].f0, \
                      stats_inclusive[i].avg_count, \
                      ((double)stats_inclusive[i].max_count-(double)stats_inclusive[i].min_count)/(double)stats_inclusive[i].avg_count*100.0, \
                      stats_inclusive[i].avg_time, \
                      ((double)stats_inclusive[i].max_time-(double)stats_inclusive[i].min_time)/(double)stats_inclusive[i].avg_time*100.0);
              }
          }
          fprintf(stderr,"-------------------------------------------------------------------------------\n");
          fprintf(stderr,"  * inclusive time: time including all children functions\n");
          fprintf(stderr,"  * average count and timing reported.\n");
          fprintf(stderr,"  * imblance metric: imb% = (max-min)/avg*100%%. \n");
          fprintf(stderr, "\n");

          fprintf(stderr,"\n                            Exclusive Times                                   \n");
          fprintf(stderr,"-------------------------------------------------------------------------------\n");
          fprintf(stderr, "           group     function           count [imb%]           time(s) [imb%]  \n");
          fprintf(stderr,"-------------------------------------------------------------------------------\n");

          for (int i = 0; i < libprof_fsize; i++) {
              if(stats_exclusive[i].max_count >0) {
/*
                 fprintf(stderr," %2d  %10s        %10zu,%10zu,%10zu        %8.3f,%8.3f,%8.3f \n", i, stats_exclusive[i].f0, \
                      stats_exclusive[i].min_count, stats_exclusive[i].avg_count, stats_exclusive[i].max_count, \
                      stats_exclusive[i].min_time, stats_exclusive[i].avg_time, stats_exclusive[i].max_time);
*/
                 fprintf(stderr," %2d   %10s   %10s    %12zu [%5.1f%]       %8.3f [%5.1f%] \n", i, \
                      stats_exclusive[i].fg, stats_exclusive[i].f0, \
                      stats_exclusive[i].avg_count, \
                      ((double)stats_exclusive[i].max_count-(double)stats_exclusive[i].min_count)/(double)stats_exclusive[i].avg_count*100.0, \
                      stats_exclusive[i].avg_time, \
                      ((double)stats_exclusive[i].max_time-(double)stats_exclusive[i].min_time)/(double)stats_exclusive[i].avg_time*100.0);

              }
          }
          fprintf(stderr,"-------------------------------------------------------------------------------\n");
          fprintf(stderr,"                                     total library time (s): %8.3f\n", libtime_avg);
          fprintf(stderr,"-------------------------------------------------------------------------------\n");
          fprintf(stderr,"  * exclusive time: time excluding all children functions\n");
          fprintf(stderr,"  * average count and timing reported.\n");
          fprintf(stderr,"  * imblance metric: imb% = (max-min)/avg*100%%. \n");
          fprintf(stderr, "\n");
        }

        free(all_data);
        free(stats_inclusive);
        free(stats_exclusive);
    }

    // Free the MPI datatype
    MPI_Type_free(&MPI_LIBPROF_FREPLACE);

    // Finalize MPI if it was initialized by this function
//    if (!mpi_initialized) {  // && !mpi_finalized) {
//        PMPI_Finalize();
    libprof_mpi_finalize();
 //   }
}






void libprof_fini() {
    apptime += libprof_second();

    if (!check_MPI()) {
        serial_stats();
    } else {
        gather_and_calculate_stats();
    }

    fflush(stderr);
    fflush(stdout);
}


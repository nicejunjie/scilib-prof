
#define _GNU_SOURCE
//#include "utils.h"
#include "global.h"

#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>


double libprof_second()
{
/* struct timeval { long        tv_sec;
            long        tv_usec;        };

struct timezone { int   tz_minuteswest;
             int        tz_dsttime;      };     */

        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

double libprof_second_high_precision()
{
    struct timespec measure;

    // Get the current time as the start time
    clock_gettime(CLOCK_MONOTONIC, &measure);

    // Return the elapsed time in seconds
    return (double)measure.tv_sec + (double)measure.tv_nsec * 1e-9;
}

double libprof_second_() {return libprof_second();}
double libprof_second_high_precision_() {return libprof_second_high_precision();}


void get_exe_path(char **path){
    char *exe_path=malloc(PATH_MAX);
//    char exe_path[PATH_MAX];  //destroyed after call
    ssize_t len = readlink("/proc/self/exe", exe_path, PATH_MAX-1);
    if (len != -1) {
        exe_path[len] = '\0';
        //printf("Executable path: %s,  len: %zd\n", exe_path, len);
       *path = exe_path;
    } else {
        perror("Failed to get executable path in get_exe_path");
        free(exe_path);
        *path = NULL;
        return;
    }
}

void get_argv0(char **argv0) {
    char* buffer = (char *)malloc(sizeof(char) * (1024));
    strcpy(buffer, "null\0");
    FILE *fp = fopen("/proc/self/cmdline", "r");
    if (!fp) {
        perror("fopen");
        *argv0 = buffer;
        return;
    }

    int n = fread(buffer, 1, 1024, fp);
    if (n == 0) {
        perror("fread");
        *argv0 = buffer;
        return;
    }
    buffer[n-1] = '\0';
    *argv0 = buffer;
}


#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
/* Check if string ends with commands to be ignored*/
int check_string(const char *str) {
    const char *exe_list[] = {
        "ibrun",
        "mpirun",
        "orterun",
        "orted",
        "mpirun_rsh",
        "mpiexec",
        "mpiexec.hydra",
        "srun",
        "hydra_bstrap_proxy",
        "hydra_pmi_proxy",
        "numactl",
        "pip",
        "pip3",
        "virtualenv",
// profilers
        "map",
        "scorep",
        "nvprof",
        "nsys",
        "ncu",
    };

    const char *dir_list[] = {
        "/bin",
        "/usr",
        "/sbin",
    };

    // Check if the program path starts with any in the dir_list
    //fprintf(stderr, "str=%s\n", str);

    for (int i = 0; i < ARRAY_SIZE(dir_list); i++) {
        size_t len = strlen(dir_list[i]);
        //fprintf(stderr, "dir_list=%s\n", dir_list[i]);
        if (strlen(str) >= len && strncmp(str, dir_list[i], len) == 0) {
         //     fprintf(stderr, "--skip due to dir\n");
            return 1;
        }
    }
    // Check if the executable name matches any in the list
    for (int i = 0; i < ARRAY_SIZE(exe_list); i++) {
        size_t len = strlen(exe_list[i]);
        //fprintf(stderr, "exe_list=%s\n", exe_list[i]);
        if (strlen(str) >= len && strcmp(str + strlen(str) - len, exe_list[i]) == 0) {
        //      fprintf(stderr, "--skip due to exe\n");
            return 1;
        }
    }

    return 0;
}



#include <assert.h>
#include <string.h>
char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

int in_str(const char* s1, char** s2) {
    if (s2 == NULL || s1 == NULL) return 0;  
    
    for (int i = 0; s2[i] != NULL; i++) {
        if (strcmp(s2[i], s1) == 0) {
            return 1;  // Found a match
        }
    }
    
    return 0;  // No match found
}



/*  Experimental  */

#include <errno.h>
#include <unistd.h>  // For sysconf
#include <dlfcn.h> 
#include <fcntl.h>



void *ymalloc(size_t size) {
    static void* (*real_malloc)() = NULL; 
    void *ptr = NULL;
    if (!real_malloc)  real_malloc = dlsym(RTLD_NEXT, "malloc") ;
    ptr = real_malloc(size);

    // Apply madvise to the allocated memory region
    if (ptr != NULL && size >= 512*1024*1024) { 
        if (madvise(ptr, size, MADV_HUGEPAGE) != 0) {
            perror("madvise");
        }
    }
    return ptr;
}


//size_t pagesize = sysconf(_SC_PAGESIZE);
#define S_1G 1024*1024*1024*2
#define S_1M 1024*1024
#define S_64K 65536
#define S_4K 1024
#define NALIGN S_64K 
#define BAR  1 //S_1M*10
size_t nalign=NALIGN;
size_t bar=BAR;
void* xmalloc(size_t size) {
    static void* (*real_malloc)() = NULL; 
    void *ptr = NULL;
    if (!real_malloc)  real_malloc = dlsym(RTLD_NEXT, "malloc") ;

    if ( 0 ||  size < bar) return real_malloc(size);

    size = ( size/nalign + 1 ) * nalign;
    int result = posix_memalign(&ptr, nalign, size);
    return ptr;
}


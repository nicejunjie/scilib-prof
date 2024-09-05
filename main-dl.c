#define _GNU_SOURCE

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


#include "utils.h"
#include "init.h"
#include "global.h"

 
libprof_freplace libprof_farray[] = {
  INIT_FARRAY
};
int libprof_fsize = sizeof(libprof_farray) / sizeof(libprof_farray[0]);


char *exe_path;
int skip_flag; 

void libprof_elf_init(){

  libprof_init();

// register functions
  for( int i=0; i< libprof_fsize; i++) {
     libprof_farray[i].fptr= dlsym(RTLD_NEXT, libprof_farray[i].f0);
  }

  return;
}

void libprof_elf_fini(){

  libprof_fini();
 
  return;
}


__attribute__((section(".init_array"))) void *__init = libprof_elf_init;
__attribute__((section(".fini_array"))) void *__fini = libprof_elf_fini;

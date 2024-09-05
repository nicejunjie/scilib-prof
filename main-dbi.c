#include "frida-gum.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


#include "utils.h"
#include "init.h"
#include "fini.h"
#include "global.h"

GumInterceptor * interceptor;
gpointer *hook_address;

libprof_freplace libprof_farray[] = {
  INIT_FARRAY
};
int libprof_fsize = sizeof(libprof_farray) / sizeof(libprof_farray[0]);


char *exe_path;
int skip_flag; 

void libprof_elf_init(){

  get_exe_path(&exe_path);
  skip_flag = check_string(exe_path);
  if(skip_flag) return;

  libprof_init();

  hook_address = malloc(libprof_fsize * sizeof(gpointer));

  gum_init_embedded ();
  interceptor = gum_interceptor_obtain ();
  
  gum_interceptor_begin_transaction (interceptor);
  for( int i=0; i< libprof_fsize; i++) {
//     if( !scilib_offload_func || in_str(libprof_farray[i].f0, scilib_offload_func)) {
//         fprintf(stderr,"replacing function %10s with %10s\n", libprof_farray[i].f0, libprof_farray[i].f1);
         hook_address[i] = gum_find_function(libprof_farray[i].f0);
         gpointer newf = gum_find_function(libprof_farray[i].f1);
         if (hook_address[i] && newf) {
             gum_interceptor_replace_fast(interceptor, hook_address[i], newf, 
                   (gpointer*)(&(libprof_farray[i].fptr)));
 //       }
     }
     else 
        hook_address[i] = NULL;
  }
  gum_interceptor_end_transaction (interceptor);

  return;
}


void libprof_elf_fini(){

  if(skip_flag) return;

  for( int i=0; i< libprof_fsize; i++) {
    if (hook_address[i]) gum_interceptor_revert(interceptor, hook_address[i]);
  }

  g_object_unref (interceptor);
  gum_deinit_embedded ();

  libprof_fini();
 
  return;
}

__attribute__((section(".init_array"))) void *__init = libprof_elf_init;
__attribute__((section(".fini_array"))) void *__fini = libprof_elf_fini;

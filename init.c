
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"


double apptime = 0.0;
//double libtime = 0.0;
#define MAX_LAYER 20   //max tracked nested calls
int layer_count = 0 ;
double layer_time[MAX_LAYER];
char layer_caller[MAX_LAYER][40];


void libprof_init(){
//  libprof_parse_env_var();
//
  apptime = -libprof_second();
  
  return;
}

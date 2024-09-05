#!/usr/bin/env python

import re
import sys

header = """
#define func_group "BLAS"
#include "global.h"
#include "complex.h"
#include <math.h>
#include <stdlib.h>
#include <init.h>
#include "utils.h"

"""

fcontent1 = """    double local_time=0.0;

    if (!orig_f) orig_f = libprof_farray[fi].fptr;

//  memset(layer_time, 0, (MAX_LAYER*sizeof(layer_time[0]));
    for (int i=layer_count+1;i<MAX_LAYER;i++) layer_time[i]=0.0;
//  if (layer_count==0) layer_time[0]=0.0;
    layer_count++;

    local_time=libprof_second();
"""

fcontent2 = """    local_time=libprof_second()-local_time;

    libprof_farray[fi].time_in += local_time;
    libprof_farray[fi].time_ex += local_time - layer_time[layer_count];
    layer_time[layer_count-1] += local_time;
    libprof_farray[fi].count ++;

//  printf("layer time %.3f\\n",layer_time[0]);
//  printf("layer count %d\\n", layer_count);
//  printf("local time %.3f, layer time %.3f\\n",local_time, layer_time[layer_count]);
    layer_count--;
"""


def generate_wrapper(signature, ptype):
    # Extract return type, function name, and arguments
    match = re.match(r'(\w+)\s+(\w+)\((.*)\)', signature)
    if not match:
        return "Invalid function signature"
    
    return_type, func_name, args = match.groups()
    func_name = func_name.rstrip('_')
 
    if ptype == 'dl':
          my_func_name = func_name + '_'
    else: 
          my_func_name = 'my_' + func_name
    
    # Generate argument names
    arg_names = [arg.split()[-1].strip('*') for arg in args.split(', ')]
    
    # Generate wrapper code
    wrapper = f"""{return_type} {my_func_name}({args})
{{
    {return_type} (*orig_f)() = NULL;
    """
    wrapper += f"enum findex fi = {func_name};\n" 
    #wrapper += func_name
    wrapper += fcontent1

    if return_type == "void":
        wrapper += f"    orig_f({', '.join(arg_names)});\n"
    else:
        wrapper += f"    {return_type} result = orig_f({', '.join(arg_names)});\n"

    wrapper += fcontent2

    if return_type != "void":
        wrapper += "    return result;\n"
    else: 
        wrapper += "    return;\n"

    wrapper += "}"

    return wrapper

if __name__ == '__main__':

  ptype = 'dbi'
  if len(sys.argv) >1: 
     arg = sys.argv[1].lower()
     if arg == 'dl': 
           ptype = 'dl'

  fname = "PROTOTYPES"
  inputfile = open(fname, 'r')

  print(header)
  for line in  inputfile:
    line = line.strip()
    if line and not line.startswith('#'):
       print(generate_wrapper(line, ptype))
       print()





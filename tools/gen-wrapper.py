#!/usr/bin/env python

import re
import sys
import argparse

header = """
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "freplace.h"
#include "global.h"
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

    if ptype == 'dl':
          my_func_name = func_name
    else:
          my_func_name = 'dbi_' + func_name #.rstrip('_')
    
    # Generate argument names
    arg_names = [arg.split()[-1].strip('*') for arg in args.split(', ')]
    
    # Generate wrapper code
    wrapper = f"""{return_type} {my_func_name}({args})
{{
    {return_type} (*orig_f)() = NULL;
    """
    wrapper += f"enum findex fi = {func_name.rstrip('_')}_index;\n"
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
    parser = argparse.ArgumentParser(description='Generate wrapper functions.')
    parser.add_argument('fname', help='Input file name containing function prototypes')
    group = parser.add_mutually_exclusive_group()
    #group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-dl', action='store_true', help='Use dl mode')
    group.add_argument('-dbi', action='store_true', help='Use dbi mode (default)')
    
    args = parser.parse_args()

    # Default to 'dbi' if neither -dl nor -dbi is specified
    ptype = 'dl' if args.dl else 'dbi'

    try:
        with open(args.fname, 'r') as inputfile:
            print(header)
            for line in inputfile:
                line = line.strip()
                if line and not line.startswith('#'):
                    print(generate_wrapper(line, ptype))
                    print()
    except FileNotFoundError:
        print(f"Error: File '{args.fname}' not found.")
        sys.exit(1)

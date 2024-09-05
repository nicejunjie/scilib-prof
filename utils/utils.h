
/* Timer Tools */

// timer at us accuracy
double libprof_second();
double libprof_second_();

// timer at ns accuracy
double libprof_second_high_precision();
double libprof_second2_high_precision_();


int check_MPI();
int get_MPI_local_rank();

void get_exe_path(char **path);
void get_argv0(char **argv0);

char** str_split(char* a_str, const char a_delim);
int in_str(const char* s1, char** s2);

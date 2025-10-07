#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#include "pdsl.cpp"

const int DOT_INTERVAL = 10;

static inline double cpuTime(void) {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000; }

static inline int memReadStat(int field) {
    char    name[256];
    pid_t pid = getpid();
    sprintf(name, "/proc/%d/statm", pid);
    FILE*   in = fopen(name, "rb");
    if (in == NULL) return 0;
    int     value;
    for (; field >= 0; field--)
        fscanf(in, "%d", &value);
    fclose(in);
    return value;
}

static inline uint64_t memUsed() { 
    return (uint64_t)memReadStat(0) * (uint64_t)getpagesize();
}

void help(char *exec_name) {
    fprintf(stderr,"Program Usage:\n",exec_name);
    fprintf(stderr,"%s [input_file] [-r <report_file>]\n",exec_name);
    fprintf(stderr,"%s [-h] | [-?]\n\n",exec_name);
}

static void SIGALRM_handler(int signum) {
  fprintf(stderr,".");
  alarm(DOT_INTERVAL);
}
            
int main(int argc, char** argv) {
#ifdef REDIRECT
	// Silence stderr
	int id = open("/dev/null",O_WRONLY | O_APPEND);
	close(2);
	dup(id);
	close(id);
#endif

   if ((argc == 2) && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"-?"))) {
      help(argv[0]);
      return 0;}

    int rep_switch_pos;
    for (rep_switch_pos = 1; rep_switch_pos < argc; rep_switch_pos++)
      if (!strcmp(argv[rep_switch_pos],"-r")) break;
    if (rep_switch_pos == argc - 1) {
      fprintf(stderr,"Error: missing report file.\n");
      return(1);}

    bool rep_file = (rep_switch_pos < argc);
    if (!rep_file && argc > 2) {
      fprintf(stderr,"Error: invalid argument.\n");
      return(1);}
            
    fprintf(stderr,"PDSL - Project Definition Scripting Language.\n");
    char *input_file_name = NULL,*report_file_name = NULL;
    if ((argc == 1) || (rep_switch_pos == 1))
      fprintf(stderr,"Reading from standard input.\n");
    else
      input_file_name = strdup(argv[1]);
    if (rep_file)
      report_file_name = strdup(argv[rep_switch_pos + 1]);
      
    signal(SIGALRM,SIGALRM_handler);
    alarm(DOT_INTERVAL);
    double cpu_time = cpuTime();
    Pdsl pdsl;

    fprintf(stderr,"Solving...\n");
    pdsl.run(input_file_name,report_file_name);
    fprintf(stderr,"\nCPU Time: %0.3f second\t\tMemory Usage: %0.3f Mbyte\n\n",cpuTime() - cpu_time,memUsed() / 1048576.0);
#ifdef REDIRECT
    printf("%0.3f\n",cpuTime() - cpu_time);
#endif
}

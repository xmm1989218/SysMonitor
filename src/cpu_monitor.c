#include <stdio.h>
#include <unistd.h>

#include "sys_monitor.h"

void print_cpu_precent(CPUPrecent* precent) {
  fprintf(stdout,
      "====================================\n"
      "intr         = %lld\n"
      "ctxt         = %lld\n"
      "btime        = %lld\n"
      "processes    = %lld\n"
      "proc_running = %lld\n"
      "proc_blocked = %lld\n"
      "user         = %.2lf\n"
      "nice         = %.2lf\n"
      "system       = %.2lf\n"
      "idle         = %.2lf\n"
      "iowait       = %.2lf\n"
      "irq          = %.2lf\n"
      "softirq      = %.2lf\n"
      "====================================\n",
      precent->intr_, precent->ctxt_,
      precent->btime_, precent->processes_,
      precent->proc_running_, precent->proc_blocked_, precent->user_,
      precent->nice_, precent->system_, precent->idle_,
      precent->iowait_, precent->irq_, precent->softirq_);
}


int main(int argc, char** argv) {

  CPUMonitor* monitor = create_cpu_monitor();

  while(1) {
    sleep(1);
    CPUPrecent precent = get_cpu_precent(monitor);
    print_cpu_precent(&precent);
  }

  free_cpu_monitor(monitor);

  return 0;
}

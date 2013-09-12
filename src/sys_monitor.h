#ifndef __SYS_MONITOR_H__
#define __SYS_MONITOR_H__

typedef struct {
  long long intr_;      /* the number of interrupt from system boot */
  long long ctxt_;      /* the number of context switch from system boot */
  long long btime_;     /* time interval from system boot */
  long long processes_; /* the number of task created from system boot */
  long long proc_running_;  /* the number of task which is in running queue now */
  long long proc_blocked_;  /* the number of task which is blocked now */

  double user_;
  double nice_;
  double system_;
  double idle_;
  double iowait_;
  double irq_;
  double softirq_;
} CPUPrecent;

struct _CPU;

typedef struct CPUMonitor {
  struct _CPU* last_;
  CPUPrecent last_precent_;
} CPUMonitor;

CPUMonitor* create_cpu_monitor();
void free_cpu_monitor(CPUMonitor* monitor);
CPUPrecent get_cpu_precent(CPUMonitor* monitor);

#endif

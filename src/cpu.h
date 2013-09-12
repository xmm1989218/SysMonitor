#ifndef __SYS_MONITOR_CPU_H__
#define __SYS_MONITOR_CPU_H__

typedef union {
  struct {
    long long user_;      /* time cost in user space from system boot */
    long long nice_;      /* time about nice is negative, high priority level from system boot*/
    long long system_;    /* time cost in kernel space from system boot */
    long long idle_;      /* time wait from system boot, expect io wait */
    long long iowait_;    /* time about io wait from system boot */
    long long irq_;       /* time about irq(hardware interrupt) from system boot */
    long long softirq_;   /* time about softirq(software interrupt) from system boot */
  };
  long long argv_[7];
} _CPUCore;

typedef struct _CPU {
  long long intr_;      /* the number of interrupt from system boot */
  long long ctxt_;      /* the number of context switch from system boot */
  long long btime_;     /* time interval from system boot */
  long long processes_; /* the number of task created from system boot */
  long long proc_running_;  /* the number of task which is in running queue now */
  long long proc_blocked_;  /* the number of task which is blocked now */

  unsigned int core_num_;
  _CPUCore cores_[];
} _CPU;

_CPU* get_current_cpu();
void  destory_cpu(_CPU* cpu);

#endif

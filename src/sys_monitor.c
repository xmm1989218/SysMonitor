#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "cpu.h"
#include "sys_monitor.h"

/* ============================CPU ============================= */

const static CPUPrecent NullCPUPrecent = {
  .intr_  = 0,
  .ctxt_  = 0,
  .btime_ = 0,
  .processes_    = 0,
  .proc_running_ = 0,
  .proc_blocked_ = 0,
  .user_   = 0,
  .nice_   = 0,
  .system_ = 0,
  .idle_   = 0,
  .iowait_ = 0,
  .irq_    = 0,
  .softirq_ = 0
};

const static CPUPrecent KeepCPUPrecent = {
  .intr_  = -1,
  .ctxt_  = -1,
  .btime_ = -1,
  .processes_    = -1,
  .proc_running_ = -1,
  .proc_blocked_ = -1,
  .user_   = -1,
  .nice_   = -1,
  .system_ = -1,
  .idle_   = -1,
  .iowait_ = -1,
  .irq_    = -1,
  .softirq_ = -1
};

static inline int is_keep_cpu_precent(CPUPrecent *precent) {
  assert(precent != NULL);
  return precent->intr_ == -1 && precent->ctxt_ == -1;
}

static CPUPrecent _calc_current_cpu_precent(const _CPU* last, const _CPU* current) {
  assert(last != NULL);
  assert(current != NULL);
  assert(last->cores_ != NULL);
  assert(current->cores_ != NULL);
  assert(last->core_num_ == current->core_num_);

  CPUPrecent precent;
  memset(&precent, 0, sizeof(CPUPrecent));

#define sum_cpu_diff(idx, p) \
  (precent.p += current->cores_[idx].p - last->cores_[idx].p)

  int i;
  for (i = 0; i < last->core_num_; i++) {
    sum_cpu_diff(i, user_);
    sum_cpu_diff(i, system_);
    sum_cpu_diff(i, nice_);
    sum_cpu_diff(i, idle_);
    sum_cpu_diff(i, iowait_);
    sum_cpu_diff(i, irq_);
    sum_cpu_diff(i, softirq_);
  }

#undef sum_cpu_diff

  double total = precent.user_ +
    precent.system_ +
    precent.nice_ +
    precent.idle_ +
    precent.iowait_ +
    precent.irq_ +
    precent.softirq_;

  if (total == 0) {
    return KeepCPUPrecent;
  }

#define cpu_property_precent(p) (precent.p = 100.0 * precent.p / total)

  cpu_property_precent(user_);
  cpu_property_precent(system_);
  cpu_property_precent(nice_);
  cpu_property_precent(idle_);
  cpu_property_precent(iowait_);
  cpu_property_precent(irq_);
  cpu_property_precent(softirq_);

#undef cpu_property_precent

  precent.intr_  = current->intr_ - last->intr_;
  precent.ctxt_  = current->ctxt_ - last->ctxt_;
  precent.btime_ = current->btime_ - last->btime_;
  precent.proc_running_ = current->proc_running_;
  precent.proc_blocked_ = current->proc_blocked_;

  return precent;
}


/* ================================= public ============================= */

CPUMonitor* create_cpu_monitor() {
  _CPU* cpu           = get_current_cpu();
  CPUMonitor* monitor = malloc(sizeof(CPUMonitor));
  if (monitor == NULL) {
    destory_cpu(cpu);
    return NULL;
  }
  monitor->last_         = cpu;
  monitor->last_precent_ = NullCPUPrecent;

  return monitor;
}

void free_cpu_monitor(CPUMonitor* monitor) {
  if (monitor != NULL) {
    if (monitor->last_ != NULL) {
      destory_cpu(monitor->last_);
      monitor->last_ = NULL;
    }
    free(monitor);
  }
}

CPUPrecent get_cpu_precent(CPUMonitor* monitor) {
  _CPU* current = get_current_cpu();
  if (current == NULL) {
    return NullCPUPrecent;
  }

  CPUPrecent precent = _calc_current_cpu_precent(monitor->last_, current);
  if (is_keep_cpu_precent(&precent)) {
    return monitor->last_precent_;
  }

  if (monitor->last_ != NULL) {
    destory_cpu(monitor->last_);
  }
  monitor->last_ = current;
  monitor->last_precent_ = precent;

  return precent;
}


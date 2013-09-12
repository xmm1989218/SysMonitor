#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "file_op.h"
#include "parser.h"
#include "cpu.h"

static _CPU* _create_cpu() {
  unsigned int core_num = sysconf(_SC_NPROCESSORS_ONLN);
  _CPU* cpu = malloc(sizeof(_CPU) + sizeof(_CPUCore) * core_num);
  if (cpu == NULL) {
    return NULL;
  }

  memset(cpu, 0, sizeof(_CPU) + sizeof(_CPUCore) * core_num);
  cpu->core_num_ = core_num;

  return cpu;
}

void destory_cpu(_CPU* cpu) {
  if (cpu != NULL) {
    free(cpu);
  }
}

_CPU* get_current_cpu() {
  _CPU* cpu = NULL;
  char* buffer = NULL;
  int length = read_file_to_end("/proc/stat", &buffer);
  if (length < 0) {
    goto CLEAN;
  }

  cpu = _create_cpu();
  if (cpu == NULL) {
    goto CLEAN;
  }

  if (parse_cpu(buffer, length, cpu)
      != SYS_MONITOR_PARSE_ALL_SUCCESS) {
    goto CLEAN;
  }

  free(buffer);
  return cpu;
CLEAN:
  if (buffer != NULL) {
    free(buffer);
  }
  if (cpu != NULL) {
    destory_cpu(cpu);
  }
  return NULL;
}


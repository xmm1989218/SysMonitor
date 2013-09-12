#ifndef __SYS_MONITOR_PARSER_H__
#define __SYS_MONITOR_PARSER_H__

#define SYS_MONITOR_PARSE_ALL_SUCCESS       0
#define SYS_MONITOR_PARSE_ERROR             1
#define SYS_MONITOR_PARSE_NOT_ENOUGH_ARGS   2
#define SYS_MONITOR_PARSE_START_WITH_NUMBER 3
#define SYS_MONITOR_PARSE_READ_FILE_FAIL    4

struct _CPU;

int parse_cpu(const char* buffer, const int length,
    struct _CPU* cpu);

#endif

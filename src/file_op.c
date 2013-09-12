#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#include "file_op.h"

#define READ_BLOCK_SIZE 4096

static int read_file(int fd, char** buffer) {
  *buffer = NULL;
  char* p = calloc(sizeof(char), READ_BLOCK_SIZE);
  if (p == NULL) {
    return -1;
  }
  int remain = READ_BLOCK_SIZE;

  int len = 0;
  while(1) {
    int tlen = read(fd, p + len, remain);
    if (tlen == 0) {
      break;
    } else if (tlen < 0) {
      if (errno == EINTR) {
        continue;
      }
      free(p);
      return -1;
    } else {
      remain -= tlen;
      len += tlen;
      if (remain == 0) {
        p = realloc(p, sizeof(char) * len * 2);
        if (p == NULL) {
          break;
        }
        remain = len;
      }
    }
  }

  *buffer = p;
  return len;
}

int read_file_to_end(const char* name, char** buffer) {
  *buffer = NULL;
  int fd = open(name, O_RDONLY);
  if (fd < 0) return -1;
  int length = read_file(fd, buffer);
  if (length < 0) return -1;
  close(fd);
  return length;
}

#undef READ_BLOCK_SIZE


#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"
#include "cpu.h"
#include "parser.h"

enum {
  CPU             = 0,
  CPU_CORE        = 1,
  INTR            = 2,
  CTXT            = 3,
  BTIME           = 4,
  PROCESSES       = 5,
  PROCS_RUNNING   = 6,
  PROCS_BLOCKED   = 7,
  ERROR           = 8,
  NUMBER          = 9,
  SEPARATOR       = 10,
  START           = 11,
  END             = 12
};

typedef struct {
  const int id_;
  const char* name_;
  const int length_;
  const int argc_;
} _Tag;

const static _Tag tags_[] = {
  {CPU, "cpu", 3, 7},
  {CPU_CORE, "cpu", 3, 7},
  {INTR, "intr", 4, 1},
  {CTXT, "ctxt", 4, 1},
  {BTIME, "btime", 5, 1},
  {PROCESSES, "processes", 9, 1},
  {PROCS_RUNNING, "procs_running", 13, 1},
  {PROCS_BLOCKED, "procs_blocked", 13, 1},
};

const static _Tag BeginTag_     = {START, "start", 5, 0};
const static _Tag NumberTag_    = {NUMBER, "number", 6, 0};
const static _Tag SeparatorTag_ = {SEPARATOR, "separator", 9, 0};
const static _Tag ErrorTag_     = {ERROR, "error", 5, 0};
const static _Tag EndTag_       = {END, "end", 3, 0};

typedef struct {
  const _Tag* tag_;
  const char* sptr_;
  const char* eptr_;
  long long data_;
} _Token;

const static _Token BeginToken_     = {&BeginTag_, NULL, NULL, 0};
const static _Token EndToken_       = {&EndTag_, NULL, NULL, 0};

#define _make_token(t, tg, b, off) do { \
  token.sptr_ = b; \
  token.eptr_ = b + off; \
  token.tag_  = &(tg); \
}while(0)

#define _make_error_token(t, b, off) do { \
  _make_token(t, ErrorTag_, b, off); \
  token.data_ = 0; \
}while(0)

#define _make_sep_token(t, b, off) do { \
  _make_token(t, SeparatorTag_, b, off); \
  token.data_ = 0; \
}while(0)

static int _skip_bad_tag(const char* buffer,
    const int length) {
  int offset;
  for (offset = 0; offset < length; offset++) {
    if (_is_space_or_newline(buffer[offset])) {
      break;
    }
  }
  return offset;
}

static int _read_integer(const char* buffer,
    const int length, long long* number) {
  int i = 0, flag = 1;
  long long n = 0;
  if (length <= 0) {
    return 0;
  }
  if (buffer[i] == '+') {
    i++;
  } else if (buffer[i] == '-') {
    flag = -1;
    i++;
  }
  for (;i < length; i++) {
    if (_is_number(buffer[i])) {
      n = n * 10 + buffer[i] - '0';
    } else if (_is_space_or_newline(buffer[i])) {
      break;
    } else {
      return 0;
    }
  }

  *number = flag * n;
  return i;
}

static _Token _get_token(const char* buffer, const int length) {
  if (length == 0) {
    return EndToken_;
  }

  _Token token;
  //memset(&token, 0, sizeof(token));

  int i;
  for (i = 0; i < length; i++) {
    if (!_is_space_or_newline(buffer[i])) {
      break;
    }
  }

  if (i != 0) {
    _make_sep_token(token, buffer, i);
    return token;
  }

  // not number;
  if (_is_number(*buffer) || _is_add_or_sub_flag(*buffer)) {
    int offset = _read_integer(buffer, length, &(token.data_));
    if (offset == 0) {
      offset = _skip_bad_tag(buffer, length);
      _make_error_token(token, buffer, offset);
      return token;
    }
    _make_token(token, NumberTag_, buffer, offset);
    return token;
  } else {
    int j;
    for (j = 0; j < ERROR; j++) {
      if (length < tags_[j].length_) {
        continue;
      } else if (strncmp(buffer, tags_[j].name_,
            tags_[j].length_) == 0) {
        break;
      }
    }

    if (j == ERROR) {
      int offset = _skip_bad_tag(buffer, length);
      _make_error_token(token, buffer, offset);
      return token;
    }

    token.data_ = 0;
    _make_token(token, tags_[j], buffer, tags_[j].length_);

    if (j == CPU) {
      if (!_is_space_or_newline(*(buffer + tags_[j].length_))) {
        // CPU_CORE
        int offset = _read_integer((const char*)(buffer + tags_[j].length_),
            length - tags_[j].length_, &(token.data_));
        if (offset == 0) {
          offset = _skip_bad_tag(buffer + tags_[j].length_,
            length - tags_[j].length_);
          token.tag_ = &ErrorTag_;
          token.eptr_ += offset;
          return token;
        }
        token.eptr_ += offset;
        token.tag_ = tags_ + CPU_CORE;
      }
    }
  }

  return token;
}

static int _set_cpu_value(const _Token* name, const int index,
    const _Token* value, _CPU* cpu) {
  assert(name != NULL);
  assert(value != NULL);
  switch(name->tag_->id_) {
    case CPU:
      // nothing to do
      break;
    case CPU_CORE:
      if (name->data_ < name->tag_->argc_) {
        cpu->cores_[name->data_].argv_[index] = value->data_;
      }
      break;
    case INTR:
      cpu->intr_ = value->data_;
      break;
    case CTXT:
      cpu->ctxt_ = value->data_;
      break;
    case BTIME:
      cpu->btime_ = value->data_;
      break;
    case PROCESSES:
      cpu->processes_ = value->data_;
      break;
    case PROCS_RUNNING:
      cpu->proc_running_ = value->data_;
      break;
    case PROCS_BLOCKED:
      cpu->proc_blocked_ = value->data_;
      break;
    default: return 1;
  }
  return 0;
}

int parse_cpu(const char* buffer, const int length,
    struct _CPU* cpu) {
  int index = 0;
  _Token name = BeginToken_;
  int got_argc = 0;
  while(index < length) {
    _Token token = _get_token(buffer + index, length);
    if (token.tag_ == &SeparatorTag_) {
      index += token.eptr_ - token.sptr_;
      continue;
    } else if (token.tag_ == &ErrorTag_) {
      // we allow exists tag we don't recongnize
      // return SYS_MONITOR_PARSE_ERROR;
    } else if (token.tag_ == &EndTag_) {
      break;
    }
    index += token.eptr_ - token.sptr_;
    if (token.tag_ == &NumberTag_) {
      if (name.tag_->id_ <= ERROR) {
        if (got_argc < name.tag_->argc_) {
          _set_cpu_value(&name, got_argc, &token, cpu);
          got_argc++;
        } // else {
        // throw it
        // }
      } else {
        // start with number #invalid
        return SYS_MONITOR_PARSE_START_WITH_NUMBER;
      }
    } else if (got_argc < name.tag_->argc_) {
      // got not enough arguments
      return SYS_MONITOR_PARSE_NOT_ENOUGH_ARGS;
    } else {
      // new tag
      name = token;
      got_argc = 0;
    }
  }

  return SYS_MONITOR_PARSE_ALL_SUCCESS;
}

#undef _make_token
#undef _make_error_token
#undef _make_sep_token

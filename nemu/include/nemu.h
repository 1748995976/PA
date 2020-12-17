#ifndef __NEMU_H__
#define __NEMU_H__

#include "common.h"
#include "memory/memory.h"
#include "isa/reg.h"

extern CPU_state cpu;

uint32_t expr(char *e, bool *success);

#endif

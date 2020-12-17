#include "nemu.h"

const char *regsl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  printf("\n------riscv32 registers------\n");
  for (size_t i = 0; i < 32; i++)
  {
    if(i%4 == 0){
      printf("\n");
    }
    printf("%s:0x%x",regsl[i],cpu.gpr[i]._32);
  }
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  const char *reg = s + 1;
  if(reg[0]>='0' && reg[0]<='9'){
    int order = 0;
    sscanf(reg,"%d",&order);
    if(order>=0 && order <= 31){
      return cpu.gpr[order]._32;
    }else{
      *success = false;
      return 0;
    }
  }else
  {
    for (int i = 0; i < 32; i++)
    {
      if(strcmp(regsl[i],reg) == 0){
        return cpu.gpr[i]._32;
      }
    }
    *success = false;
    return 0;
  }
}

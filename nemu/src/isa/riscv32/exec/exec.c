#include "cpu/exec.h"
#include "all-instr.h"

//load
static OpcodeEntry load_table [8] = {
  EXW(lb,1), EXW(lb,2), EXW(ld, 4), EMPTY, EXW(ld,1), EXW(ld,2), EMPTY, EMPTY
};

static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

//store
static OpcodeEntry store_table [8] = {
  EXW(st,1), EXW(st,2), EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

//imm_series
static OpcodeEntry imm_series_srli_table[2] = {
	EXW(srli, 4), EXW(srai, 4)
};

static make_EHelper(imm_series_srli){
	
};

static OpcodeEntry imm_series_table [8] = {
  EXW(addi, 4), EXW(slli, 4), EXW(slti, 4), EXW(sltiu, 4), EXW(xori, 4), EXW(imm_series_sr, 4), EXW(ori, 4), EXW(andi, 4)
};

static make_EHelper(imm_series){

};

//R_series
static OpcodeEntry R_series_add_table [2] = {
	EXW(add, 4), EXW(sub, 4)
};

static make_EHelper(R_series_add){
	
};

static OpcodeEntry R_series_srl_table [2] = {
	EXW(srl, 4), EXW(sra, 4)
};

static make_EHelper(R_series_srl){
	
};

static OpcodeEntry R_series_table [8] = {
	EXW(R_series_add, 4), EXW(sll, 4), EXW(slt, 4), EXW(sltu, 4), EXW(xor, 4), EXW(R_series_srl, 4), EXW(or, 4), EXW(and, 4)
};

static make_EHelper(reg_serial){
	decinfo.width = R_series_table[decinfo.isa.instr.funct3].width;
	idex(pc, &R_series_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry M_series_table [8] = {
	EXW(mul, 4), EXW(mulh, 4), EXW(mulhsu, 4), EXW(mulhu, 4), EXW(div, 4), EXW(divu, 4), EXW(rem, 4), EXW(remu, 4)
};

static make_EHelper(M_series){
	decinfo.width = M_series_table[decinfo.isa.instr.funct3].width;
	idex(pc, &M_series_table[decinfo.isa.instr.funct3]);
};

static OpcodeEntry MR_series_table [2] = {
	EXW(R_series, 4), EXW(M_series, 4)
};

static make_EHelper(MR_series){
	
};

//B_series
static OpcodeEntry B_series_table [8] = {
	EXW(beq, 4), EXW(bne, 4), EMPTY, EMPTY, EXW(blt, 4), EXW(bge, 4), EXW(bltu, 4), EXW(bgeu, 4)
};

static make_EHelper(B_series){
	decinfo.width = B_series_table[decinfo.isa.instr.funct3].width;
	idex(pc, &B_series_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEX(I,imm_series), IDEX(U,auipc), EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R,MR_series), IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ IDEX(B,B_series), IDEX(I,jalr), EX(nemu_trap), IDEX(J,jal), EMPTY, EMPTY, EMPTY, EMPTY,
};

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}

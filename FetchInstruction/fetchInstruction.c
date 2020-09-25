#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#include "instruction.h"

/* Reads one byte from memory, at the specified address. Stores the
     read value into *value. Returns 1 in case of success, or 0 in case
     of failure (e.g., if the address is beyond the limit of the memory
     size). */
int memReadByte(machine_state_t *state, uint64_t address, uint8_t *value)
{    
     if(address < 0 || address > state->programSize){
          return 0;
     }
     *value = state->programMap[address];
     return 1;
}

/* Reads one quad-word (64-bit number) from memory in little-endian
     format, at the specified starting address. Stores the read value
     into *value. Returns 1 in case of success, or 0 in case of failure
     (e.g., if the address is beyond the limit of the memory size). */
int memReadQuadLE(machine_state_t *state, uint64_t address, uint64_t *value)
{
     if(address < 0 || address > state->programSize || address + 8 > state->programSize) {
          return 0;
     }
     *value = 0;
     for (int i =0; i <8; i++){
          int8_t currbyte = state->programMap[address+i];
          *value |= ((uint64_t)currbyte) << (8*i);  
     }
     return 1;
}

/* Fetches one instruction from memory, at the address specified by
     the program counter. Does not modify the machine's state. The
     resulting instruction is stored in *instr. Returns 1 if the
     instruction is a valid non-halt instruction, or 0 (zero)
     otherwise. */
int fetchInstruction(machine_state_t *state, y86_instruction_t *instr)
{
     uint8_t OPFUN; 
     uint8_t REG;
     uint64_t DATA;

     if (!memReadByte(state, state->programCounter, &OPFUN)) {
          return 0;
     }
     
     uint8_t OP = (OPFUN & 0xF0) >> 4;
     uint8_t FUN = OPFUN & 0x0F;

     switch(OP) {
          case I_HALT: 
               instr->icode = I_HALT;
               instr->ifun = FUN;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 1;
               return 0;

          case I_NOP:
               instr->icode = I_NOP;
               instr->ifun = FUN;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 1;
               return 1;

          case I_RRMVXX: 
               instr->icode = I_RRMVXX;
               instr->ifun = FUN;
               if(!memReadByte(state,state->programCounter + 1, &REG)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->rA = (REG >> 4);
               instr->rB = (REG & 0x0F);
               if(!memReadQuadLE(state, state->programCounter + 2, &DATA)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->valC = DATA;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 10;
               return 1;
      
     
          case I_IRMOVQ: // irmovq
               instr->icode = I_IRMOVQ;
               instr->ifun = FUN;       // 0x0F
               if(!memReadByte(state,state->programCounter + 1, &REG)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->rA = R_NONE;    
               instr->rB = (REG & R_NONE);   
               if(!memReadQuadLE(state, state->programCounter + 2, &DATA)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->valC = DATA;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 10;
               return 1;

          case I_RMMOVQ:
               instr->icode = I_RMMOVQ;
               instr->ifun = FUN;
               if(!memReadByte(state,state->programCounter + 1, &REG)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->rA = (REG >> 4);
               instr->rB = (REG & 0x0F);
               if(!memReadQuadLE(state, state->programCounter + 2, &DATA)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->valC = DATA;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 10;
               return 1;

     
          case I_MRMOVQ:
               instr->icode = I_MRMOVQ;
               instr->ifun = FUN;
               if(!memReadByte(state,state->programCounter + 1, &REG)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->rA = (REG >> 4);
               instr->rB = (REG & 0x0F);
               if(!memReadQuadLE(state, state->programCounter + 2, &DATA)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->valC = DATA;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 10;
               return 1;


          case I_OPQ:
               if (FUN >= 0x0 && FUN <= 0x6) { // OPq
                    instr->icode = I_OPQ;
                    instr->ifun = FUN;
               } else {
                    instr->icode = I_INVALID;
                    return 0; 
               }
               if(!memReadByte(state,state->programCounter + 1, &REG)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->rA = (REG >> 4);
               instr->rB = (REG & 0x0F);
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 2;
               return 1;


          case I_JXX:
               if (FUN >= 0x0 && FUN <= 0x6) { // jXX
                    instr->icode = I_JXX;
                    instr->ifun = FUN;
               }else {
                    instr->icode = I_INVALID;
                    return 0;
               }
               if(!memReadQuadLE(state, state->programCounter + 1, &DATA)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->valC = DATA;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 9;
               return 1;
              
          case I_CALL:
               instr->icode = I_CALL;
               instr->ifun = FUN;
               if(!memReadQuadLE(state, state->programCounter + 1, &DATA)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->valC = DATA;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 9;
               return 1;
              
          case I_RET:
               instr->icode = I_RET;
               instr->ifun = FUN;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 1;
               return 1;

          case I_PUSHQ:
               instr->icode = I_PUSHQ;
               instr->ifun = FUN;
               if(!memReadByte(state, state->programCounter + 1, &REG)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->rA = (REG >> 4);
               instr->rB = 0x0F;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 2;
               return 1;

          case I_POPQ:
               instr->icode = I_POPQ;
               instr->ifun = FUN;
               if(!memReadByte(state, state->programCounter+1, &REG)) {
                    instr->icode = I_TOO_SHORT;
                    return 0;
               }
               instr->rA = (REG >> 4);
               instr->rB = 0x0F;
               instr->location = state->programCounter;
               instr->valP = state->programCounter + 2;
               return 1;
              
     }
     return 0;
}



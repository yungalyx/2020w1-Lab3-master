#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#include "instruction.h"

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

/* Stores the specified quad-word (64-bit) value into memory, at the
   specified start address, using little-endian format. Returns 1 in
   case of success, or 0 in case of failure (e.g., if the address is
   beyond the limit of the memory size). */
int memWriteQuadLE(machine_state_t *state, uint64_t address, uint64_t value)
{
    if(address < 0 || address > state->programSize || address + 8 > state->programSize) {
        return 0;
    }

    for (int i =0; i <8; i++){
        state->programMap[address+i] = 0xFF & (value>>(8*i));
    }
    return 1;
}

/* Executes the instruction specified by *instr, modifying the
   machine's state (memory, registers, condition codes, program
   counter) in the process. Returns 1 if the instruction was executed
   successfully, or 0 if there was an error. Typical errors include an
   invalid instruction or a memory access to an invalid address. */
int executeStackOperation(machine_state_t *state, y86_instruction_t *instr)
{
    int success = 1;

    uint64_t S_ADDRESS;
    uint64_t DATA;

    switch (instr->icode)
    {
    case I_CALL:
        state->programCounter = instr->valP; // next instruction
        S_ADDRESS = state->registerFile[R_RSP];   // get stack pointer address
        S_ADDRESS -= 8; // decrement stack pointer
        state->registerFile[R_RSP] = S_ADDRESS; // store new address into stack pointer.
        if(!memWriteQuadLE(state, S_ADDRESS, state->programCounter)){
            return 0;
        }
        state->programCounter = instr->valC; // called instruction
        break;
    
    case I_RET:
        S_ADDRESS = state->registerFile[R_RSP]; 
        if(!memReadQuadLE(state, S_ADDRESS, &DATA)) {
            return 0;
        }
        S_ADDRESS += 8; // increment stack pointer and put it back to rsp
        state->registerFile[R_RSP] = S_ADDRESS;
        state->programCounter = DATA;
        break;

    case I_PUSHQ:
        // ADJUSTING STACK POINTER
        S_ADDRESS = state->registerFile[R_RSP]; 
        S_ADDRESS -= 8; 
        state->registerFile[R_RSP] = S_ADDRESS; 

        // WRITING CONTENTS ONTO STACK
        DATA = state->registerFile[instr->rA];
        if(!memWriteQuadLE(state, S_ADDRESS, DATA)){
            return 0;
        }
        //NEXT
        state->programCounter = instr->valP; 
        break;
    
    case I_POPQ:
        S_ADDRESS = state->registerFile[R_RSP]; 
        if(!memReadQuadLE(state, S_ADDRESS, &DATA)) {
            return 0;
        }
        // POPPING TO REG
        state->registerFile[instr->rA] = DATA;
        
        // INC STACK
        S_ADDRESS += 8; 
        state->registerFile[R_RSP] = S_ADDRESS;

        //NEXT
        state->programCounter = instr->valP;
        break;
     
    default:
        success = 0;
    }
    return success;
}
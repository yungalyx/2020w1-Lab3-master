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
int executeSimpleOperation(machine_state_t *state, y86_instruction_t *instr)
{
    
    if (instr->icode == I_INVALID || instr->icode == I_TOO_SHORT) {
        return 0;
    }
    

    uint64_t DATA;
    uint64_t OFFSET;
    uint64_t M_ADDRESS;

    int success = 1;

    switch (instr->icode) {
        case I_HALT:
            break;

        case I_NOP:
            state->programCounter = instr->valP;
            return 1;
            break;

        case I_IRMOVQ:
            state->programCounter = instr->valP;
            state->registerFile[instr->rB] = instr->valC;
            break;

        case I_RMMOVQ:
            OFFSET = instr->valC;
            M_ADDRESS = state->registerFile[instr->rB];
            DATA = state->registerFile[instr->rA];
            if (!memWriteQuadLE(state, M_ADDRESS+OFFSET, DATA)){
                return 0;
            }
            state->programCounter = instr->valP;
            break;


        case I_MRMOVQ:
            OFFSET = instr->valC;
            M_ADDRESS = state->registerFile[instr->rB];
            if (!memReadQuadLE(state, M_ADDRESS+OFFSET, &DATA)){
                return 0;
            }
            state->registerFile[instr->rA] = DATA;
            state->programCounter = instr->valP;
            break;
        
        default:
            success = 0;
    }

    return success;
}
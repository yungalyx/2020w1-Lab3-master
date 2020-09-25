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
int executeAluOperation(machine_state_t *state, y86_instruction_t *instr)
{
    int success = 1;
    uint64_t vala = state->registerFile[instr->rA];
    uint64_t valb = state->registerFile[instr->rB];
    uint64_t result;
    state->conditionCodes = 0x0;

    switch (instr->ifun) {
        case 0x0: // ADD
            result = valb + vala;
            if (result == 0) {
                state->conditionCodes = CC_ZERO_MASK;
            } else if ((result >> 63) == 1) {
                state->conditionCodes = CC_SIGN_MASK;
            }
            state->registerFile[instr->rB] = result;
            state->programCounter = instr->valP;
            break;

        case 0x1: // SUB
            result = valb - vala;
            if (result == 0) {
                state->conditionCodes = CC_ZERO_MASK;
            } else if ((result >> 63) == 1) {
                state->conditionCodes = CC_SIGN_MASK;
            }
            state->registerFile[instr->rB] = result;
            state->programCounter = instr->valP;
            break;

        case 0x2: // AND
            result = valb & vala;
            if (result == 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else if (result < 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else {
            state->conditionCodes = 0 | 0;
            }
            state->registerFile[instr->rB] = result;
            state->programCounter = instr->valP;
            break;

        case 0x3: // XOR
            result = valb ^ vala;
            if (result == 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else if (result < 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else {
            state->conditionCodes = 0 | 0;
            }
            state->registerFile[instr->rB] = result;
            state->programCounter = instr->valP;
            break;

        case 0x4: // MUL
            result = valb * vala;
            if (result == 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else if (result < 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else {
            state->conditionCodes = 0 | 0;
            }
            state->registerFile[instr->rB] = result;
            state->programCounter = instr->valP;
            break;

        case 0x5: // DIV
            result = valb / vala;
            if (result == 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else if (result < 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else {
            state->conditionCodes = 0 | 0;
            }
            state->registerFile[instr->rB] = result;
            state->programCounter = instr->valP;
            break;

        case 0x6: // MOD
            result = valb % vala;
            if (result == 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else if (result < 0) {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            } else {
            state->conditionCodes = state->conditionCodes | CC_ZERO_MASK;
            }
            state->registerFile[instr->rB] = result;
            state->programCounter = instr->valP;
            break;

        default:
            success = 0;
            break;
        
    }
    return success;
}
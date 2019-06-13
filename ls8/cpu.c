#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_LEN 6

// Reading the CPU struct's information at this specific address or index in memory
unsigned char cpu_ram_read(struct cpu *cpu, unsigned char address)
{
  return cpu->ram[address];
}

// Writing or Assigning a value to this specific address or index in memory
unsigned char cpu_ram_write(struct cpu *cpu, unsigned char address, unsigned char value)
{
  return cpu->ram[address] = value;
}

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *filename)
{
  // TODO: Replace this with something less hard-coded
  FILE *fp;
  char line[1024];
  int address = 0;

  fp = fopen(filename, "r");

  if (fp == NULL)
  {
    fprintf(stderr, "file not found\n");
    exit(1);
  }

  while (fgets(line, 1024, fp) != NULL)
  { // not EOF
    char *endptr;

    unsigned char v = strtoul(line, &endptr, 2);

    if (endptr == line)
    {
      continue;
    }
    //printf("%s", line);
    //printf("%u\n", v);
    cpu->ram[address] = v;
    address++;
  }
  fclose(fp);
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    // TODO
    cpu->registers[regA] *= cpu->registers[regB];
    break;

  // TODO: implement more ALU ops
  case ALU_ADD:
    cpu->registers[regA] += cpu->registers[regB];
    break;
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  unsigned char IR, operandA, operandB;
  int retaddr;

  while (running)
  {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    IR = cpu_ram_read(cpu, cpu->PC);
    // 2. Figure out how many operands this next instruction requires
    // 3. Get the appropriate value(s) of the operands following this instruction
    operandA = cpu_ram_read(cpu, cpu->PC + 1);
    operandB = cpu_ram_read(cpu, cpu->PC + 2);

    //printf("TRACE: %02X | IR: %02X operandA: %02X operandB: %02X |\n", cpu->PC, IR, operandA, operandB);

    // 4. switch() over it to decide on a course of action.
    switch (IR)
    {
      // 5. Do whatever the instruction should do according to the spec.
      // 6. Move the PC to the next instruction.
    case LDI:
      cpu->registers[operandA] = operandB;
      cpu->PC += 3;
      break;

    case MUL:
      alu(cpu, ALU_MUL, operandA, operandB);
      cpu->PC += 3;
      break;

    case ADD:
      alu(cpu, ALU_ADD, operandA, operandB);
      cpu->PC += 3;
      break;

    case PRN:
      printf("%d\n", cpu->registers[operandA]);
      cpu->PC += 2;
      break;

    case PUSH:
      cpu->registers[cpu->SP]--;                                    // decrement SP
      cpu->ram[cpu->registers[cpu->SP]] = cpu->registers[operandA]; //Copy the value in given register to the address pointed to by `SP`.
      cpu->PC += 2;
      break;

    case POP:
      cpu->registers[operandA] = cpu->ram[cpu->registers[cpu->SP]]; //Copy the value from the address pointed to by `SP` to the given register.
      cpu->registers[cpu->SP]++;                                    // increament SP
      cpu->PC += 2;
      break;

    case CALL:
      // push return addr on stack
      retaddr = cpu->PC + 2;
      cpu->registers[cpu->SP]--; // decrement SP
      cpu->ram[cpu->registers[cpu->SP]] = retaddr;
      // set the PC to the subroutine address
      cpu->PC = cpu->registers[operandA];
      break;

    case RET:
      // pop return addr from stack
      retaddr = cpu->ram[cpu->registers[cpu->SP]];
      cpu->registers[cpu->SP]++;
      // Set pc to return addr
      cpu->PC = retaddr;
      break;

    case HLT:
      running = 0;
      cpu->PC += 1;
      break;

    default:
      printf("Unknown instruction %02x at address %02x\n", IR, cpu->PC);
      exit(1);
    }
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->PC = 0;
  cpu->SP = 7;                    //The SP points at the value at the top of the stack (most recently pushed)
  cpu->registers[cpu->SP] = 0xF4; // The SP points at address `F4` if the stack is empty.

  //memset() is used to fill a block of memory with a particular value.
  memset(cpu->registers, 0, 8 * sizeof(unsigned char));
  memset(cpu->ram, 0, 256 * sizeof(unsigned char));
}

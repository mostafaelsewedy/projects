#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MEM_SIZE 2048
#define REG_SIZE 32

int Mem[MEM_SIZE];
int Regs[REG_SIZE];
int PC = 0;
int instrucCount = 0;
int dataCount = 0;
int pipelineCount = 0;

// Structure to hold intermediate values
typedef struct
{
    int opcode;
    int r1, r2, r3;
    int shamt, imm, address;
    int result;
    int memData;
    int writeToReg;
    int memWrite;
    int memRead;
    int regWrite;
} PipelineRegisters;

PipelineRegisters pipelineRegisters[5]; // Array to hold pipeline stages (0-4 for fetch to write-back)

// Function prototypes
int parseOperation(char *instruction);
int parseReg(char *instruction);
int parseSHAMT(char *instruction);
int parseAddress(char *instruction);
int parseImmediate(char *instruction);
void parse(char *instruction);
int fetch();
void decode(int instruction);
void execute();
void MEM();
void WB();
void pipeline();
void printPipelineState();
void printRegistersAndMemory();
void flush();

void scanFile()
{
    FILE *file = fopen("instructions.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    char line[32];
    while (fgets(line, sizeof(line), file))
    {
        char line_copy[32];
        strcpy(line_copy, line);
        parse(line_copy);
    }

    fclose(file);
}

int parseOperation(char *instruction)
{
    if (strcmp(instruction, "ADD") == 0)
        return 0;
    if (strcmp(instruction, "SUB") == 0)
        return 1;
    if (strcmp(instruction, "MUL") == 0)
        return 2;
    if (strcmp(instruction, "MOVI") == 0)
        return 3;
    if (strcmp(instruction, "JEQ") == 0)
        return 4;
    if (strcmp(instruction, "AND") == 0)
        return 5;
    if (strcmp(instruction, "XORI") == 0)
        return 6;
    if (strcmp(instruction, "JMP") == 0)
        return 7;
    if (strcmp(instruction, "LSL") == 0)
        return 8;
    if (strcmp(instruction, "LSR") == 0)
        return 9;
    if (strcmp(instruction, "MOVR") == 0)
        return 10;
    if (strcmp(instruction, "MOVM") == 0)
        return 11;
    return -1;
}

int parseReg(char *instruction)
{
    if (instruction[0] == 'R' && strlen(instruction) > 1)
    {
        int regNum = atoi(&instruction[1]);
        if (regNum >= 0 && regNum < REG_SIZE)
            return regNum;
    }
    return -1;
}

int parseSHAMT(char *instruction)
{
    return atoi(instruction);
}

int parseAddress(char *instruction)
{
    return atoi(instruction);
}

int parseImmediate(char *instruction)
{
    return atoi(instruction);
}

void parse(char *instruction)
{
    int instruc = 0;
    int opcode = 0;
    int count = 0;

    char *token = strtok(instruction, " "); // splits line at " "
    while (token != NULL)
    {
        if (count == 0)
        { // First token is the operation
            opcode = parseOperation(token);
            if (opcode == -1)
            {
                printf("Invalid operation: %s\n", token);
                return;
            }
            instruc += (opcode << 28);
        }
        else
        {
            if (opcode == 7)
            { // J Format
                instruc += parseAddress(token);
            }
            else if (opcode == 3)
            { // I Format One Reg
                if (count == 1)
                { // First register
                    int tmp = parseReg(token);
                    if (tmp == -1)
                    {
                        printf("Invalid register: %s\n", token);
                        return;
                    }
                    instruc += (tmp << 23);
                }
                else if (count == 3)
                { // Immediate value
                    int tmp = parseImmediate(token);
                    if (tmp > 0x3FFFF)
                    {
                        printf("Immediate value out of range: %s\n", token);
                        return;
                    }
                    instruc += tmp;
                }
            }
            else if (opcode == 4 || opcode == 6 || opcode == 10 || opcode == 11)
            { // I Format 2 Regs
                if (count == 1)
                { // First register
                    int tmp = parseReg(token);
                    if (tmp == -1)
                    {
                        printf("Invalid register: %s\n", token);
                        return;
                    }
                    instruc += (tmp << 23);
                }
                else if (count == 2)
                { // Second register
                    int tmp = parseReg(token);
                    if (tmp == -1)
                    {
                        printf("Invalid register: %s\n", token);
                        return;
                    }
                    instruc += (tmp << 18);
                }
                else if (count == 3)
                { // Immediate value
                    int tmp = parseImmediate(token);
                    if (tmp > 0x3FFFF)
                    {
                        printf("Immediate value out of range: %s\n", token);
                        return;
                    }
                    instruc += tmp;
                }
            }
            else if (opcode == 8 || opcode == 9)
            { // R Format with SHAMT
                if (count == 4)
                { // SHAMT value
                    instruc += parseSHAMT(token);
                }
                else if (count < 3)
                { // Registers
                    int tmp = parseReg(token);
                    if (tmp == -1)
                    {
                        printf("Invalid register: %s\n", token);
                        return;
                    }
                    instruc += (tmp << (23 - 5 * (count - 1)));
                }
            }
            else
            { // Default R Format
                int tmp = parseReg(token);
                if (tmp == -1)
                {
                    printf("Invalid register: %s\n", token);
                    return;
                }
                instruc += (tmp << (23 - 5 * (count - 1)));
            }
        }
        token = strtok(NULL, " ");
        count++;
    }

    if (instrucCount < 1024)
    {
        Mem[instrucCount] = instruc;
        instrucCount++;
    }
    else
    {
        printf("Instruction Memory Full!\n");
    }
}

int fetch()
{
    if (PC >= instrucCount)
        return -1; // PC out of range
    int currentInstruction = Mem[PC];
    PC++;
    return currentInstruction;
}

void decode(int instruction)
{
    int opcode = 0;  // bits 31:28
    int r1 = 0;      // bits 27:23
    int r2 = 0;      // bits 22:18
    int r3 = 0;      // bits 17:13
    int shamt = 0;   // bits 12:0
    int imm = 0;     // bits 17:0
    int address = 0; // bits 27:0

    opcode = (instruction >> 28) & 0xF;
    r1 = (instruction >> 23) & 0x1F;
    r2 = (instruction >> 18) & 0x1F;
    r3 = (instruction >> 13) & 0x1F;
    shamt = instruction & 0x1FFF;
    imm = instruction & 0x3FFFF;
    address = instruction & 0xFFFFFFF;

    pipelineRegisters[1].opcode = opcode;
    pipelineRegisters[1].r1 = r1;
    pipelineRegisters[1].r2 = r2;
    pipelineRegisters[1].r3 = r3;
    pipelineRegisters[1].shamt = shamt;
    pipelineRegisters[1].imm = imm;
    pipelineRegisters[1].address = address;

    printf("Instruction %i\n", instruction);
    printf("opcode = %i\n", opcode);
    printf("r1 = %i\n", r1);
    printf("r2 = %i\n", r2);
    printf("r3 = %i\n", r3);
    printf("shift amount = %i\n", shamt);
    printf("immediate = %i\n", imm);
    printf("address = %i\n", address);
}

void execute()
{
    int opcode = pipelineRegisters[1].opcode;
    int r1 = pipelineRegisters[1].r1;
    int r2 = pipelineRegisters[1].r2;
    int r3 = pipelineRegisters[1].r3;
    int shamt = pipelineRegisters[1].shamt;
    int imm = pipelineRegisters[1].imm;
    int address = pipelineRegisters[1].address;

    int result = 0;
    int memRead = 0;
    int memWrite = 0;
    int writeToReg = 0;
    int regWrite = 0;

    switch (opcode)
    {
    case 0: // ADD
        result = Regs[r2] + Regs[r3];
        writeToReg = r1;
        regWrite = 1;
        break;
    case 1: // SUB
        result = Regs[r2] - Regs[r3];
        writeToReg = r1;
        regWrite = 1;
        break;
    case 2: // MUL
        result = Regs[r2] * Regs[r3];
        writeToReg = r1;
        regWrite = 1;
        break;
    case 3: // MOVI
        result = imm;
        writeToReg = r1;
        regWrite = 1;
        break;
    case 4: // JEQ
        if (Regs[r1] == Regs[r2])
        {
            PC = PC + imm;
            flush();
        }
        break;
    case 5: // AND
        result = Regs[r2] & Regs[r3];
        writeToReg = r1;
        regWrite = 1;
        break;
    case 6: // XORI
        result = Regs[r2] ^ imm;
        writeToReg = r1;
        regWrite = 1;
        break;
    case 7: // JMP
        PC = address;
        flush();
        break;
    case 8: // LSL
        result = Regs[r2] << shamt;
        writeToReg = r1;
        regWrite = 1;
        break;
    case 9: // LSR
        result = Regs[2] >> shamt;
        writeToReg = r1;
        regWrite = 1;
        break;
    case 10: // MOVR
        result = Regs[r2] + imm;
        writeToReg = r1;
        regWrite = 1;
        memRead = 1;
        break;
    case 11: // MOVM
        result = Regs[r2] + imm;
        memWrite = 1;
        break;
    default:
        printf("Invalid operation code: %d\n", opcode);
    }

    pipelineRegisters[2].result = result;
    pipelineRegisters[2].memRead = memRead;
    pipelineRegisters[2].memWrite = memWrite;
    pipelineRegisters[2].writeToReg = writeToReg;
    pipelineRegisters[2].regWrite = regWrite;
}

void MEM()
{
    int memRead = pipelineRegisters[2].memRead;
    int memWrite = pipelineRegisters[2].memWrite;
    int result = pipelineRegisters[2].result;

    if (memRead)
    {
        if ((result >= 2048) || (result < 0))
        {
            printf("Memory out of bounds");
            return;
        }
        pipelineRegisters[3].memData = Mem[result];
    }
    else if (memWrite)
    {
        if ((result >= 2048) || (result < 0))
        {
            printf("Memory out of bounds");
            return;
        }
        Mem[result] = Regs[pipelineRegisters[2].r1];
    }
    else
    {
        pipelineRegisters[3].memData = result;
    }

    pipelineRegisters[3].writeToReg = pipelineRegisters[2].writeToReg;
    pipelineRegisters[3].regWrite = pipelineRegisters[2].regWrite;
}

void WB()
{
    int writeToReg = pipelineRegisters[3].writeToReg;
    int memData = pipelineRegisters[3].memData;
    int regWrite = pipelineRegisters[3].regWrite;

    if (regWrite)
    {
        Regs[writeToReg] = memData;
    }
}

void pipeline()
{
    if (pipelineCount >= (instrucCount + 2))
    {
        return;
    }
    pipelineRegisters[4] = pipelineRegisters[3];
    WB();

    pipelineRegisters[3] = pipelineRegisters[2];
    MEM();

    pipelineRegisters[2] = pipelineRegisters[1];
    execute();

    int instr = fetch();
    if (instr != -1)
    {
        decode(instr);
    }
    pipelineRegisters[0].opcode = -1;
    printPipelineState();
    pipelineCount++;
    pipeline();
}

void printPipelineState()
{
    printf("Pipeline state:\n");
    for (int i = 0; i < 5; ++i)
    {
        printf("Stage %d: opcode=%d, r1=%d, r2=%d, r3=%d, result=%d, memData=%d, writeToReg=%d\n",
               i,
               pipelineRegisters[i].opcode,
               pipelineRegisters[i].r1,
               pipelineRegisters[i].r2,
               pipelineRegisters[i].r3,
               pipelineRegisters[i].result,
               pipelineRegisters[i].memData,
               pipelineRegisters[i].writeToReg);
    }
}

void printRegistersAndMemory()
{
    printf("Registers:\n");
    for (int i = 0; i < REG_SIZE; i++)
    {
        printf("R%d = %d\n", i, Regs[i]);
    }
    printf("\nMemory:\n");
    for (int i = 0; i < instrucCount; i++)
    {
        printf("Mem[%d] = %d\n", i, Mem[i]);
    }
}

void flush()
{
    for (int i = 0; i < 2; i++)
    {
        pipelineRegisters[i].opcode = 0;
        pipelineRegisters[i].r1 = 0;
        pipelineRegisters[i].r2 = 0;
        pipelineRegisters[i].r3 = 0;
        pipelineRegisters[i].shamt = 0;
        pipelineRegisters[i].imm = 0;
        pipelineRegisters[i].address = 0;
        pipelineRegisters[i].result = 0;
        pipelineRegisters[i].memData = 0;
        pipelineRegisters[i].writeToReg = 0;
        pipelineRegisters[i].memWrite = 0;
        pipelineRegisters[i].memRead = 0;
        pipelineRegisters[i].regWrite = 0;
    }
}

int main()
{
    memset(Regs, 0, sizeof(Regs));
    memset(Mem, 0, sizeof(Mem));
    memset(pipelineRegisters, 0, sizeof(pipelineRegisters));

    scanFile();

    pipeline();

    printRegistersAndMemory();
    return 0;
}

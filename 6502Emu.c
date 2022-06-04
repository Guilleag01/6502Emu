#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define TEST printf("TEST\n")

void allocateMemory(char ** memory, int size);
void executeInstruction(void (* instructionSet[])(), unsigned char instruction  );
void haltCPU();
void cycle(unsigned int cycle);
void initializeInstructionSet(void (* instructionSet[])());
void loadFileToMemory(char * fileName, char * memory);
void run();
char readFromMemory();
void printStatus();
void loadAcumulatorWithMemory();
void storeAccumulatorInMemory();
char addWithCarry(char a, char b);

// Memoria
char * memory;
int cpuClock = 0;
float frecuency = 1000000; //1 MHz

// Registros
short PC = 0;
char accumulator = 0;
char X = 0;
char Y = 0;
//             Neg  Overflow  -    Break Decimal  Int   Zero   Carry
bool SR[8] = {false, false, false, false, false, false, false, false};
/*
N	Negative
V	Overflow
-	ignored
B	Break
D	Decimal (use BCD for arithmetics)
I	Interrupt (IRQ disable)
Z	Zero
C	Carry
*/
char SP = 0;

void (* instructionSet[256])();

bool stopSignal = false;

bool useFrecuency = true;

int main(){
    initializeInstructionSet(instructionSet);
    allocateMemory(&memory, 65535);
    loadFileToMemory("programFile.bin", memory);
    run();
    printStatus();
    //printf("%x\n", memory[1024]);
}

void run(){
    while(!stopSignal){
        unsigned char instruction = (unsigned char)readFromMemory();
        executeInstruction(instructionSet, instruction);
    }
}

void loadFileToMemory(char * fileName, char * memory){
    FILE * file;
    file = fopen(fileName, "r");
    if(file != NULL){
        int pos = 0;
        char c;
        while((c = fgetc(file)) != EOF){
            memory[pos++] = (char) c;
        }
    }else{
        printf("ERROR\n");
    }
    fclose(file);
}

void allocateMemory(char ** memory, int size){
    *memory = (char *)malloc(size);
}

char readFromMemory(){
    return memory[PC++];
}

void cycle(unsigned int cycle){
    cpuClock += cycle;
    if(useFrecuency){
        sleep(cycle/frecuency);
    }
}

void printStatus(){
    printf("\n======== GENERAL STATUS ========\n");
    printf("Clock: %d Frecuency: %f\n", cpuClock, frecuency);
    printf("=========== REGISTERS ==========\n");
    printf("PC: %d Acc: %x X: %x Y: %x SP: %x\n", PC, accumulator, X, Y, SP);
    printf("Flags: N: %d V: %d B: %d D: %d I: %d Z: %d C: %d\n\n", SR[0], SR[1], SR[3], SR[4], SR[5], SR[6], SR[7]);
}

char addWithCarry(char a, char b){
    bool c = SR[7];
    if(a + b + c > 127){
        SR[7] = true;
    }
    return a + b + c;
}

void initializeInstructionSet(void (* instructionSet[])()){

    // BRK
    instructionSet[0x00] = haltCPU;

    //LDA
    instructionSet[0xA9] = loadAcumulatorWithMemory;
    instructionSet[0xA5] = loadAcumulatorWithMemory;
    instructionSet[0xB5] = loadAcumulatorWithMemory;
    instructionSet[0xAD] = loadAcumulatorWithMemory;
    instructionSet[0xBD] = loadAcumulatorWithMemory;
    instructionSet[0xB9] = loadAcumulatorWithMemory;
    instructionSet[0xA1] = loadAcumulatorWithMemory;
    instructionSet[0xB1] = loadAcumulatorWithMemory;

    //STA
    instructionSet[0x85] = storeAccumulatorInMemory;
    instructionSet[0x95] = storeAccumulatorInMemory;
    instructionSet[0x8D] = storeAccumulatorInMemory;
    instructionSet[0x9D] = storeAccumulatorInMemory;
    instructionSet[0x99] = storeAccumulatorInMemory;
    instructionSet[0x81] = storeAccumulatorInMemory;
    instructionSet[0x91] = storeAccumulatorInMemory;
}

void executeInstruction(void (* instructionSet[])(), unsigned char instruction){
    instructionSet[instruction]();
}

void haltCPU(){
    printf("System halted\n");
    cycle(7);
    stopSignal = true;
}

// LDA
void loadAcumulatorWithMemory(){
    unsigned char opcode = memory[PC - 1];
    printf("LDA\n");
    char ll, hh;
    switch (opcode){
    // Inmediate
    case 0xA9:
        accumulator = readFromMemory();
        break;
        cycle(2);
    // Zeropage
    case 0xA5:
        accumulator = memory[readFromMemory()];
        break;
    // Zeropage, X 
    case 0xB5:
        accumulator = memory[readFromMemory() + X];
        break;
    // Absolute
    case 0xAD:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = memory[hh * 0x100 + ll];
        break;
    // Absolute, X
    case 0xBD:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = memory[addWithCarry(hh * 0x100 + ll, X)];
        break;
    // Absolute, Y
    case 0xB9:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = memory[addWithCarry(hh * 0x100 + ll, Y)];
        break;
    // (Indirect, X)
    case 0xA1:
        ll = readFromMemory();
        accumulator = memory[memory[ll + X] * 0x100 + memory[ll + X + 1]];
        break;
    // (Indirect), Y
    case 0xB1:
        ll = readFromMemory();
        accumulator = memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)];
        break;
    default:
        break;
    }
}

// STA
void storeAccumulatorInMemory(){
    unsigned char opcode = memory[PC - 1];
    printf("STA\n");
    char ll, hh;
    switch (opcode){
    // Zeropage
    case 0x85:
        memory[readFromMemory()] = accumulator;
        break;
    // Zeropage, X
    case 0x95:
        memory[readFromMemory() + X] = accumulator;
        break;
    // Absolute
    case 0x8D:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[hh * 0x100 + ll] = accumulator;
        break;
    // Absolute, X
    case 0x9D:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[addWithCarry(hh * 0x100 + ll, X)] = accumulator;
        break;
    // Absolute, Y
    case 0x99:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[addWithCarry(hh * 0x100 + ll, Y)] = accumulator;
        break;
    // (Indirect, X)
    case 0x81:
        ll = readFromMemory();
        memory[memory[ll + X] * 0x100 + memory[ll + X + 1]] = accumulator;
        break;
    // (Indirect), Y
    case 0x91:
        ll = readFromMemory();
        memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)] = accumulator;
        break;
    default:
        break;
    }
}


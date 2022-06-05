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
void loadFileToMemory(char * fileName, int offset);
void run();
char readFromMemory();
void printStatus();
void loadAcumulatorWithMemory();
void storeAccumulatorInMemory();
char addWithCarry(char a, char b);
void addWithCarryIns();
void andMemoryWithAccumulator();
void arithmeticShiftLeft();
void pushStack(char data);
char popStack();
void loadSystemVectors();
void startup();
void branchOnCarrySet();
void branchOnResultZero();
void branchOnResultMinus();
void testBitsInMemoryWithAccumulator();
void branchOnResultNotZero();
void branchOnResultPlus();
void branchOnCarryClear();
void branchOnOverflowClear();
void branchOnOverflowSet();
void clearCarryFlag();
void clearDecimalMode();
void clearInterruptDisableBit();
void clearOverflowFlag();

char * memory;
int cpuClock = 0;
float frecuency = 1000000; //1 MHz

// Registers
unsigned short PC = 0;
char accumulator = 0;
char X = 0;
char Y = 0;
//             Neg  Overflow  -    Break Decimal  Int   Zero   Carry
bool SR[8] = {false, false, false, false, false, false, false, false};

// Stack from 0x0100 to 0x01FF 
unsigned char SP = 0xFF;

void (* instructionSet[256])();

bool stopSignal = false;

bool useFrecuency = true;

int main(){
    initializeInstructionSet(instructionSet);
    allocateMemory(&memory, 65535);
    loadFileToMemory("programFile.bin", 200);
    startup();
    run();
    printStatus();

    // char a = 0x07;
    // bool b = true;
    // a = a & 0xfe;
    // a = a | b;
    // printf("%x\n", a);

    // for(int i = 999; i <= 1001; i++){
    //     printf(" %.2x", memory[i]);
    // }
    printf("\n");
}

void run(){
    while(!stopSignal){
        unsigned char instruction = (unsigned char)readFromMemory();
        executeInstruction(instructionSet, instruction);
    }
}

void loadFileToMemory(char * fileName, int offset){
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

void loadSystemVectors(){
    // NMI (Non-Maskable Interrupt)
    // RES (Reset)
    memory[0xFFFC] = 00;
    memory[0xFFFD] = 02;
    // IRQ (Interrupt Request)
}

void startup(){
    printf("System startup\n");
    cycle(7);
    PC = (memory[0xFFFD] * 0x100 + memory[0xFFFC]);
    cycle(1);
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
    if(a > 0 && b > 0 && a + b + c < 0){
        SR[1] = true;
    }else if(a < 0 && b < 0 && a + b + c > 0){
        SR[1] = true;
    }else{
        SR[1] = false;
    }

    unsigned char ua = (unsigned char)a;
    unsigned char ub = (unsigned char)b;
    if (ua + ub < a && ua + ub < ub){
        SR[7] = true;
    }else{
        SR[7] = false;
    }

    return a + b + c;
}

void pushStack(char data){
    SP--;
    memory[SP + 0x100] = data;
}

char popStack(){
    SP++;
    return memory[SP + 0x100 - 1];
}

void initializeInstructionSet(void (* instructionSet[])()){

    // BRK
    instructionSet[0x00] = haltCPU;

    // LDA
    instructionSet[0xA9] = loadAcumulatorWithMemory;
    instructionSet[0xA5] = loadAcumulatorWithMemory;
    instructionSet[0xB5] = loadAcumulatorWithMemory;
    instructionSet[0xAD] = loadAcumulatorWithMemory;
    instructionSet[0xBD] = loadAcumulatorWithMemory;
    instructionSet[0xB9] = loadAcumulatorWithMemory;
    instructionSet[0xA1] = loadAcumulatorWithMemory;
    instructionSet[0xB1] = loadAcumulatorWithMemory;

    // STA
    instructionSet[0x85] = storeAccumulatorInMemory;
    instructionSet[0x95] = storeAccumulatorInMemory;
    instructionSet[0x8D] = storeAccumulatorInMemory;
    instructionSet[0x9D] = storeAccumulatorInMemory;
    instructionSet[0x99] = storeAccumulatorInMemory;
    instructionSet[0x81] = storeAccumulatorInMemory;
    instructionSet[0x91] = storeAccumulatorInMemory;

    // ADC
    instructionSet[0x69] = addWithCarryIns;
    instructionSet[0x65] = addWithCarryIns;
    instructionSet[0x75] = addWithCarryIns;
    instructionSet[0x6D] = addWithCarryIns;
    instructionSet[0x7D] = addWithCarryIns;
    instructionSet[0x79] = addWithCarryIns;
    instructionSet[0x61] = addWithCarryIns;
    instructionSet[0x71] = addWithCarryIns;

    // AND
    instructionSet[0x29] = andMemoryWithAccumulator;
    instructionSet[0x25] = andMemoryWithAccumulator;
    instructionSet[0x35] = andMemoryWithAccumulator;
    instructionSet[0x2D] = andMemoryWithAccumulator;
    instructionSet[0x3D] = andMemoryWithAccumulator;
    instructionSet[0x39] = andMemoryWithAccumulator;
    instructionSet[0x21] = andMemoryWithAccumulator;
    instructionSet[0x31] = andMemoryWithAccumulator;

    // AND
    instructionSet[0x0A] = arithmeticShiftLeft;
    instructionSet[0x06] = arithmeticShiftLeft;
    instructionSet[0x16] = arithmeticShiftLeft;
    instructionSet[0x0E] = arithmeticShiftLeft;
    instructionSet[0x1E] = arithmeticShiftLeft;

    // BCC
    instructionSet[90] = branchOnCarryClear;

    // BCS
    instructionSet[0xB0] = branchOnCarrySet;
    
    // BEQ
    instructionSet[0xF0] = branchOnResultZero;

    // BIT
    instructionSet[0x24] = testBitsInMemoryWithAccumulator;
    instructionSet[0x2C] = testBitsInMemoryWithAccumulator;

    // BMI
    instructionSet[0x30] = branchOnResultMinus;

    // BNE
    instructionSet[0xD0] = branchOnResultNotZero;

    // BPL
    instructionSet[0x10] = branchOnResultPlus;

    // BVC
    instructionSet[0x50] = branchOnOverflowClear;

    // BVS
    instructionSet[0x70] = branchOnOverflowSet;

    // CLC
    instructionSet[0x18] = clearCarryFlag;

    // CLD
    instructionSet[0xD8] = clearDecimalMode;

    // CLI
    instructionSet[0x58] = clearInterruptDisableBit;

    // CLV
    instructionSet[0xB8] = clearOverflowFlag;
}

void executeInstruction(void (* instructionSet[])(), unsigned char instruction){
    instructionSet[instruction]();
}

// BRK
void haltCPU(){
    printf("System halted\n");
    cycle(7);
    stopSignal = true;
    SR[5] = true;
}

// LDA
void loadAcumulatorWithMemory(){
    unsigned char opcode = memory[PC - 1];
    printf("LDA\n");
    unsigned char ll, hh;
    switch (opcode){
    // Inmediate
    case 0xA9:
        accumulator = readFromMemory();
        cycle(2);
        break;
    // Zeropage
    case 0xA5:
        accumulator = memory[readFromMemory()];
        cycle(3);
        break;
    // Zeropage, X 
    case 0xB5:
        accumulator = memory[readFromMemory() + X];
        cycle(4);
        break;
    // Absolute
    case 0xAD:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = memory[hh * 0x100 + ll];
        cycle(4);
        break;
    // Absolute, X
    case 0xBD:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = memory[addWithCarry(hh * 0x100 + ll, X)];
        cycle(4);
        break;
    // Absolute, Y
    case 0xB9:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = memory[addWithCarry(hh * 0x100 + ll, Y)];
        cycle(4);
        break;
    // (Indirect, X)
    case 0xA1:
        ll = readFromMemory();
        accumulator = memory[memory[ll + X] * 0x100 + memory[ll + X + 1]];
        cycle(6);
        break;
    // (Indirect), Y
    case 0xB1:
        ll = readFromMemory();
        accumulator = memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)];
        cycle(5);
        break;
    default:
        break;
    }    

    // Flags
    SR[0] = accumulator < 0 ? true : false;
    SR[6] = accumulator == 0 ? true : false;
}

// STA
void storeAccumulatorInMemory(){
    unsigned char opcode = memory[PC - 1];
    printf("STA\n");
    unsigned char ll, hh;
    switch (opcode){
    // Zeropage
    case 0x85:
        memory[readFromMemory()] = accumulator;
        cycle(3);
        break;
    // Zeropage, X
    case 0x95:
        memory[readFromMemory() + X] = accumulator;
        cycle(4);
        break;
    // Absolute
    case 0x8D:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[hh * 0x100 + ll] = accumulator;
        cycle(4);
        break;
    // Absolute, X
    case 0x9D:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[addWithCarry(hh * 0x100 + ll, X)] = accumulator;
        cycle(5);
        break;
    // Absolute, Y
    case 0x99:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[addWithCarry(hh * 0x100 + ll, Y)] = accumulator;
        cycle(5);
        break;
    // (Indirect, X)
    case 0x81:
        ll = readFromMemory();
        memory[memory[ll + X] * 0x100 + memory[ll + X + 1]] = accumulator;
        cycle(6);
        break;
    // (Indirect), Y
    case 0x91:
        ll = readFromMemory();
        memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)] = accumulator;
        cycle(6);
        break;
    default:
        break;
    }
}

// ADC
void addWithCarryIns(){
    unsigned char opcode = memory[PC - 1];
    printf("ADC\n");
    unsigned char ll, hh;
    switch (opcode){
    case 0x69:
        accumulator = addWithCarry(accumulator, readFromMemory());
        cycle(2);
        break;
    case 0x65:
        accumulator = addWithCarry(accumulator, memory[readFromMemory()]); 
        cycle(3);
        break;
    case 0x75:
        accumulator = addWithCarry(accumulator, memory[readFromMemory() + X]);
        cycle(4);
        break;
    case 0x6D:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = addWithCarry(accumulator, memory[hh * 0x100 + ll]); 
        cycle(4);
        break;
    case 0x7D:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = addWithCarry(accumulator, memory[addWithCarry(hh * 0x100 + ll, X)]); 
        cycle(4);
        break;
    case 0x79:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = addWithCarry(accumulator, memory[addWithCarry(hh * 0x100 + ll, Y)]); 
        cycle(4);
        break;
    case 0x61:
        ll = readFromMemory();
        accumulator = addWithCarry(accumulator, memory[memory[ll + X] * 0x100 + memory[ll + X + 1]]);
        cycle(6);
        break;
    case 0x71:
        accumulator = addWithCarry(accumulator, memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)]);
        cycle(5);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = accumulator < 0 ? true : false;
    SR[6] = accumulator == 0 ? true : false;
}

// AND
void andMemoryWithAccumulator(){
    unsigned char opcode = memory[PC - 1];
    printf("AND\n");
    char ll, hh;
    switch (opcode){
    case 0x29:
        accumulator = accumulator & readFromMemory();
        cycle(2);
        break;
    case 0x25:
        accumulator = accumulator & memory[readFromMemory()]; 
        cycle(3);
        break;
    case 0x35:
        accumulator = accumulator & memory[readFromMemory() + X];
        cycle(4);
        break;
    case 0x2D:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = accumulator & memory[hh * 0x100 + ll]; 
        cycle(4);
        break;
    case 0x3D:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = accumulator & memory[addWithCarry(hh * 0x100 + ll, X)]; 
        cycle(4);
        break;
    case 0x39:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = accumulator & memory[addWithCarry(hh * 0x100 + ll, Y)]; 
        cycle(4);
        break;
    case 0x21:
        ll = readFromMemory();
        accumulator = accumulator & memory[memory[ll + X] * 0x100 + memory[ll + X + 1]];
        cycle(6);
        break;
    case 0x31:
        accumulator = accumulator & memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)];
        cycle(5);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = accumulator < 0 ? true : false;
    SR[6] = accumulator == 0 ? true : false;
}

// ASL
void arithmeticShiftLeft(){
    unsigned char opcode = memory[PC - 1];
    printf("ASL\n");
    char ll, hh;
    bool c = SR[7];
    bool c2 = false;
    switch (opcode){
    case 0x0A:
        accumulator <<= 1;
        c2 = accumulator & 0x01;
        accumulator = accumulator & 0xfe;
        accumulator = accumulator | c;
        cycle(2);
        break;
    case 0x06:
        memory[readFromMemory()] <<= 1;
        c2 = memory[readFromMemory()] & 0x01;
        memory[readFromMemory()] = memory[readFromMemory()] & 0xfe;
        memory[readFromMemory()] = memory[readFromMemory()] | c;
        cycle(5);
        break;
    case 0x16:
        memory[readFromMemory() + X] <<= 1;
        c2 = memory[readFromMemory() + X] & 0x01;
        memory[readFromMemory() + X] = memory[readFromMemory() + X] & 0xfe;
        memory[readFromMemory() + X] = memory[readFromMemory() + X] | c;
        cycle(6);
        break;
    case 0x0E:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[hh * 0x100 + ll] <<= 1;
        c2 = memory[hh * 0x100 + ll] & 0x01;
        memory[hh * 0x100 + ll] = memory[hh * 0x100 + ll] & 0xfe;
        memory[hh * 0x100 + ll] = memory[hh * 0x100 + ll] | c;
        cycle(6);
        break;
    case 0x1E:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[addWithCarry(hh * 0x100 + ll, X)] <<= 1;
        c2 = memory[addWithCarry(hh * 0x100 + ll, X)] & 0x01;
        memory[addWithCarry(hh * 0x100 + ll, X)] = memory[addWithCarry(hh * 0x100 + ll, X)] & 0xfe;
        memory[addWithCarry(hh * 0x100 + ll, X)] = memory[addWithCarry(hh * 0x100 + ll, X)] | c;
        cycle(7);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = accumulator < 0 ? true : false;
    SR[6] = accumulator == 0 ? true : false;
    SR[7] = accumulator & c2;
}

// BCC
void branchOnCarryClear(){
    printf("BCC\n");
    char offset = readFromMemory();
    if(!SR[7]){
        char ll = (char)(PC & 0x00FF);
        char hh = (char)(PC & 0xFF00);
        pushStack(ll);
        pushStack(hh);
        PC = hh * 0x100 + ll + offset;
        cycle(1);
    }
    cycle(2);
}

// BCS
void branchOnCarrySet(){
    printf("BCS\n");
    char offset = readFromMemory();
    if(SR[7]){
        char ll = (char)(PC & 0x00FF);
        char hh = (char)(PC & 0xFF00);
        pushStack(ll);
        pushStack(hh);
        PC = hh * 0x100 + ll + offset;
        cycle(1);
    }
    cycle(2);
}

// BEQ
void branchOnResultZero(){
    printf("BEQ\n");
    char offset = readFromMemory();
    if(SR[6]){
        char ll = (char)(PC & 0x00FF);
        char hh = (char)(PC & 0xFF00);
        pushStack(ll);
        pushStack(hh);
        PC = hh * 0x100 + ll + offset;
        cycle(1);
    }
    cycle(2);
}

// BIT
void testBitsInMemoryWithAccumulator(){
    printf("BIT\n");
    unsigned char opcode = memory[PC - 1];
    printf("BIT\n");
    char ll, hh, data;
    switch (opcode){
    case 0x24:
        data = memory[readFromMemory() + X];
        SR[6] = accumulator & data;
        SR[1] = data & 0x02;
        SR[0] = data & 0x01;
        cycle(3);
        break;
    case 0x2C:
        ll = readFromMemory();
        hh = readFromMemory();
        data = memory[hh * 0x100 + ll];
        SR[6] = accumulator & data;
        SR[1] = data & 0x02;
        SR[0] = data & 0x01;
        cycle(4);
        break;

    default:
        break;
    }
}

// BMI
void branchOnResultMinus(){
    printf("BMI\n");
    char offset = readFromMemory();
    if(SR[0]){
        char ll = (char)(PC & 0x00FF);
        char hh = (char)(PC & 0xFF00);
        pushStack(ll);
        pushStack(hh);
        PC = hh * 0x100 + ll + offset;
        cycle(1);
    }
    cycle(2);
}

// BNE
void branchOnResultNotZero(){
    printf("BNE\n");
    char offset = readFromMemory();
    if(!SR[6]){
        char ll = (char)(PC & 0x00FF);
        char hh = (char)(PC & 0xFF00);
        pushStack(ll);
        pushStack(hh);
        PC = hh * 0x100 + ll + offset;
        cycle(1);
    }
    cycle(2);
}

// BPL
void branchOnResultPlus(){
    printf("BPL\n");
    char offset = readFromMemory();
    if(!SR[0]){
        char ll = (char)(PC & 0x00FF);
        char hh = (char)(PC & 0xFF00);
        pushStack(ll);
        pushStack(hh);
        PC = hh * 0x100 + ll + offset;
        cycle(1);
    }
    cycle(2);
}

// BVC
void branchOnOverflowClear(){
    printf("BVC\n");
    char offset = readFromMemory();
    char ll, hh;
    if(!SR[1]){
        ll = (char)(PC & 0x00FF);
        hh = (char)(PC & 0xFF00);
        pushStack(ll);
        pushStack(hh);
        PC = hh * 0x100 + ll + offset;
        cycle(1);
    }
    cycle(2);
}

// BVS
void branchOnOverflowSet(){
    printf("BVS\n");
    char offset = readFromMemory();
    char ll, hh;
    if(SR[1]){
        ll = (char)(PC & 0x00FF);
        hh = (char)(PC & 0xFF00);
        pushStack(ll);
        pushStack(hh);
        PC = hh * 0x100 + ll + offset;
        cycle(1);
    }
    cycle(2);
}

// CLC
void clearCarryFlag(){
    printf("CLC\n");
    SR[7] = false;
    cycle(2);
}

// CLD
void clearDecimalMode(){
    printf("CLD\n");
    SR[4] = false;
    cycle(2);
}

// CLI
void clearInterruptDisableBit(){
    printf("CLI\n");
    SR[5] = false;
    cycle(2);
}

// CLV
void clearOverflowFlag(){
    printf("CLV\n");
    SR[2] = false;
    cycle(2);
}



// THE BIBLE: https://masswerk.at/6502/6502_instruction_set.html#ASL
// GOD: https://www.youtube.com/c/BenEater

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#define TEST printf("TEST\n")

void allocateMemory(unsigned char ** memory, int size);
void executeInstruction(void (* instructionSet[])(), unsigned char instruction  );
void haltCPU();
void cycle(unsigned int cycle);
void initializeInstructionSet(void (* instructionSet[])());
void loadFileToMemory(char * fileName, unsigned short offset);
void run();
char readFromMemory();
void printStatus();
void loadAcumulatorWithMemory();
void storeAccumulatorInMemory();
char addWithCarry(char a, char b);
void addWithCarryIns();
void andMemoryWithAccumulator();
void rotateOneBitLeft();
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
void shiftLeftOneBit();
void compareMemoryWithAccumulator();
void compareMemoryAndIndexX();
void compareMemoryAndIndexY();
void decrementMemoryByOne();
void decrementIndexXByOne();
void decrementIndexYByOne();
void exclusiveOrWithAccumulator();
void incrementMemoryByOne();
void incrementIndexXByOne();
void incrementIndexYByOne();
void jumpToNewLocation();
void jumpToNewLocationSavingReturnAdress();
void loadIndexXWithMemory();
void loadIndexYWithMemory();
void shiftOneBitRight();
void noOperation();
void orMemoryWithAccumulator();
void pushAccumulatorOnStack();
void pushProcessorStatusOnStack();
unsigned char statusToChar();
void charToStatus(unsigned char c);
void pullAccumulatorOffStack();
void pullProcessorStatusOffStack();
void rotateOneBitRight();
void returnFromIterrupt();
void returnFromSubroutine();
void substractMemoryFromAccumulatorWithBorrow();
char substractWithBorrow(char a, char b);
void setCarryFlag();
void setDecimalFlag();
void setInterruptDisableStatus();
void storeIndexXInMemory();
void storeIndexYInMemory();
void transferAccumulatorToIndexX();
void transferAccumulatorToIndexY();
void transferStackPointerToIndexX();
void transferIndexXToAccumulator();
void transferIndexXToStackRegister();
void transferIndexYToAccumulator();

unsigned char * memory;
int cpuClock = 0;
float frecuency = 100; //1 MHz

// Registers
unsigned short PC = 0;
char accumulator = 0;
unsigned char X = 0;
unsigned char Y = 0;
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
    loadSystemVectors();
    loadFileToMemory("APPLE2.ROM", 0xB000);
    startup();
    run();
    printStatus();
}

void run(){
    printStatus();
    while(!stopSignal){
        unsigned char instruction = (unsigned char)readFromMemory();
        executeInstruction(instructionSet, instruction);
        
        printStatus();
    }
}

void loadFileToMemory(char * fileName, unsigned short offset){
    FILE * file;
    file = fopen(fileName, "r");
    if(file != NULL){
        unsigned short pos = offset;
        unsigned char c;
        fseek(file, 0, SEEK_END);
        int fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
        for(int i = 0; i < fileSize; i++){
            c = fgetc(file);
            //printf("%x: %.2x\n", pos, c);
            memory[pos++] = (char) c;
            // printf("%x\n", pos);
        }
        fclose(file);
    }else{
        printf("ERROR\n");
    }
}

void allocateMemory(unsigned char ** memory, int size){
    *memory = (char *)malloc(size);
}

void loadSystemVectors(){
    // NMI (Non-Maskable Interrupt)
    // RES (Reset)
    // memory[0xFFFC] = 0x00;
    // memory[0xFFFD] = 0x08;
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
        usleep((int)((cycle/frecuency)*1000000));
    }
}

void printStatus(){
    printf("\n======== GENERAL STATUS ========\n");
    printf("Clock: %d Frecuency: %f\n", cpuClock, frecuency);
    printf("=========== REGISTERS ==========\n");
    printf("PC: %x Acc: %02hhx X: %02hhx Y: %02hhx SP: %02hhx\n", PC, accumulator, X, Y, SP);
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
    if ((unsigned char)(ua + ub) < ua || (unsigned char)(ua + ub) < ub){
        SR[7] = true;
    }else{
        SR[7] = false;
    }

    return a + b + c;
}

char substractWithBorrow(char a, char b){
    bool c = SR[7];
    if(a > 0 && b > 0 && a - b - c < 0){PC = 
        SR[1] = true;
    }else if(a < 0 && b < 0 && a - b - c > 0){
        SR[1] = true;
    }else{
        SR[1] = false;
    }

    unsigned char ua = (unsigned char)a;
    unsigned char ub = (unsigned char)b;
    printf("%x - %x = %x\n", ua, ub, ua-ub);
    if ((unsigned char)(ua - ub) > ua || (unsigned char)(ua - ub) > ub){
        SR[7] = true;
    }else{
        SR[7] = false;
    }

    return a - b - c;
}

void pushStack(char data){
    SP--;
    memory[SP + 0x100] = data;
}

char popStack(){
    SP++;
    return memory[SP + 0x100 - 1];
}

unsigned char statusToChar(){
    unsigned char c = 0;
    unsigned char num = 0x80;
    for(int i = 0; i < 8; i++){
        c += SR[i] * num;
        num /= 2;
    }
    return c;
}

void charToStatus(unsigned char c){
    unsigned char num = 0x80;
    for(int i = 0; i < 8; i++){
        SR[i] = c & num;
        num /= 2;
    }
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

    // ASL
    instructionSet[0x0A] = shiftLeftOneBit;
    instructionSet[0x06] = shiftLeftOneBit;
    instructionSet[0x16] = shiftLeftOneBit;
    instructionSet[0x0E] = shiftLeftOneBit;
    instructionSet[0x1E] = shiftLeftOneBit;

    // ROL
    instructionSet[0x2A] = rotateOneBitLeft;
    instructionSet[0x26] = rotateOneBitLeft;
    instructionSet[0x36] = rotateOneBitLeft;
    instructionSet[0x2E] = rotateOneBitLeft;
    instructionSet[0x3E] = rotateOneBitLeft;

    // BCC
    instructionSet[0x90] = branchOnCarryClear;

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

    // CMP
    instructionSet[0xC9] = compareMemoryWithAccumulator;
    instructionSet[0xC5] = compareMemoryWithAccumulator;
    instructionSet[0xD5] = compareMemoryWithAccumulator;
    instructionSet[0xCD] = compareMemoryWithAccumulator;
    instructionSet[0xDD] = compareMemoryWithAccumulator;
    instructionSet[0xD9] = compareMemoryWithAccumulator;
    instructionSet[0xC1] = compareMemoryWithAccumulator;
    instructionSet[0xD1] = compareMemoryWithAccumulator;

    // CPX
    instructionSet[0xE0] = compareMemoryAndIndexX;
    instructionSet[0xE4] = compareMemoryAndIndexX;
    instructionSet[0xEC] = compareMemoryAndIndexX;

    // CPY
    instructionSet[0xC0] = compareMemoryAndIndexY;
    instructionSet[0xC4] = compareMemoryAndIndexY;
    instructionSet[0xCC] = compareMemoryAndIndexY;

    // DEC
    instructionSet[0xC6] = decrementMemoryByOne;
    instructionSet[0xD6] = decrementMemoryByOne;
    instructionSet[0xCE] = decrementMemoryByOne;
    instructionSet[0xDE] = decrementMemoryByOne;

    // DEX
    instructionSet[0xCA] = decrementIndexXByOne;

    // DEY
    instructionSet[0x88] = decrementIndexYByOne;

    // EOR
    instructionSet[0x49] = exclusiveOrWithAccumulator;
    instructionSet[0x45] = exclusiveOrWithAccumulator;
    instructionSet[0x55] = exclusiveOrWithAccumulator;
    instructionSet[0x4D] = exclusiveOrWithAccumulator;
    instructionSet[0x5D] = exclusiveOrWithAccumulator;
    instructionSet[0x59] = exclusiveOrWithAccumulator;
    instructionSet[0x41] = exclusiveOrWithAccumulator;
    instructionSet[0x51] = exclusiveOrWithAccumulator;
    
    // INC
    instructionSet[0xE6] = incrementMemoryByOne;
    instructionSet[0xF6] = incrementMemoryByOne;
    instructionSet[0xEE] = incrementMemoryByOne;
    instructionSet[0xFE] = incrementMemoryByOne;

    // INX
    instructionSet[0xE8] = incrementIndexXByOne;

    // INY
    instructionSet[0xC8] = incrementIndexYByOne;

    // JMP
    instructionSet[0x4C] = jumpToNewLocation;
    instructionSet[0x6C] = jumpToNewLocation;

    // JSR
    instructionSet[0x20] = jumpToNewLocationSavingReturnAdress;

    // LDX
    instructionSet[0xA2] = loadIndexXWithMemory;
    instructionSet[0xA6] = loadIndexXWithMemory;
    instructionSet[0xB6] = loadIndexXWithMemory;
    instructionSet[0xAE] = loadIndexXWithMemory;
    instructionSet[0xBE] = loadIndexXWithMemory;

    // LDY
    instructionSet[0xA0] = loadIndexYWithMemory;
    instructionSet[0xA4] = loadIndexYWithMemory;
    instructionSet[0xB4] = loadIndexYWithMemory;
    instructionSet[0xAC] = loadIndexYWithMemory;
    instructionSet[0xBC] = loadIndexYWithMemory;

    // LSR
    instructionSet[0x4A] = shiftOneBitRight;
    instructionSet[0x46] = shiftOneBitRight;
    instructionSet[0x56] = shiftOneBitRight;
    instructionSet[0x4E] = shiftOneBitRight;
    instructionSet[0x5E] = shiftOneBitRight;

    // NOP
    instructionSet[0xEA] = noOperation;

    // ORA
    instructionSet[0x09] = orMemoryWithAccumulator;
    instructionSet[0x05] = orMemoryWithAccumulator;
    instructionSet[0x15] = orMemoryWithAccumulator;
    instructionSet[0x0D] = orMemoryWithAccumulator;
    instructionSet[0x1D] = orMemoryWithAccumulator;
    instructionSet[0x19] = orMemoryWithAccumulator;
    instructionSet[0x01] = orMemoryWithAccumulator;
    instructionSet[0x11] = orMemoryWithAccumulator;

    // PHA
    instructionSet[0x48] = pushAccumulatorOnStack;

    // PHP
    instructionSet[0x08] = pushProcessorStatusOnStack;

    // PLA
    instructionSet[0x68] = pullAccumulatorOffStack;

    // PLP
    instructionSet[0x28] = pullProcessorStatusOffStack;

    // ROR
    instructionSet[0x6A] = rotateOneBitRight;
    instructionSet[0x66] = rotateOneBitRight;
    instructionSet[0x76] = rotateOneBitRight;
    instructionSet[0x6E] = rotateOneBitRight;
    instructionSet[0x7E] = rotateOneBitRight;

    // RTI
    instructionSet[0x40] = returnFromIterrupt;

    // RTS
    instructionSet[0x60] = returnFromSubroutine;

    // SBC
    instructionSet[0xE9] = substractMemoryFromAccumulatorWithBorrow;
    instructionSet[0xE5] = substractMemoryFromAccumulatorWithBorrow;
    instructionSet[0xF5] = substractMemoryFromAccumulatorWithBorrow;
    instructionSet[0xED] = substractMemoryFromAccumulatorWithBorrow;
    instructionSet[0xFD] = substractMemoryFromAccumulatorWithBorrow;
    instructionSet[0xF9] = substractMemoryFromAccumulatorWithBorrow;
    instructionSet[0xE1] = substractMemoryFromAccumulatorWithBorrow;
    instructionSet[0xF1] = substractMemoryFromAccumulatorWithBorrow;

    // SEC
    instructionSet[0x38] = setCarryFlag;

    // SED
    instructionSet[0xF8] = setDecimalFlag;

    // SEI
    instructionSet[0x78] = setInterruptDisableStatus;

    // STX
    instructionSet[0x86] = storeIndexXInMemory;
    instructionSet[0x96] = storeIndexXInMemory;
    instructionSet[0x8E] = storeIndexXInMemory;

    // STY
    instructionSet[0x84] = storeIndexYInMemory;
    instructionSet[0x94] = storeIndexYInMemory;
    instructionSet[0x8C] = storeIndexYInMemory;

    // TAX
    instructionSet[0xAA] = transferAccumulatorToIndexX;

    // TAY
    instructionSet[0xA8] = transferAccumulatorToIndexY;

    // TSX
    instructionSet[0xBA] = transferStackPointerToIndexX;

    // TXA
    instructionSet[0x8A] = transferIndexXToAccumulator;

    // TXS
    instructionSet[0x9A] = transferIndexXToStackRegister;

    // TYA
    instructionSet[0x98] = transferIndexYToAccumulator;
}

void executeInstruction(void (* instructionSet[])(), unsigned char instruction){
    //printf("%x\n", instruction);
    instructionSet[instruction](); 
}

// BRK
void haltCPU(){
    printf("BRK\nSystem halted\n");
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
    // Immediate
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
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
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
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
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
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
}

// ROL
void rotateOneBitLeft(){
    unsigned char opcode = memory[PC - 1];
    printf("ROL\n");
    unsigned char ll, hh;
    bool c = SR[7];
    bool c2 = false;
    switch (opcode){
    case 0x2A:
        c2 = accumulator & 0x80;
        accumulator <<= 1;
        // accumulator = accumulator & 0xfe;
        // accumulator = accumulator | c;
        cycle(2);
        break;
    case 0x26:
        c2 = memory[readFromMemory()] & 0x80;
        memory[readFromMemory()] <<= 1;
        // memory[readFromMemory()] = memory[readFromMemory()] & 0xfe;
        memory[readFromMemory()] = memory[readFromMemory()] | c;
        cycle(5);
        break;
    case 0x36:
        c2 = memory[readFromMemory() + X] & 0x80;
        memory[readFromMemory() + X] <<= 1;
        // memory[readFromMemory() + X] = memory[readFromMemory() + X] & 0xfe;
        memory[readFromMemory() + X] = memory[readFromMemory() + X] | c;
        cycle(6);
        break;
    case 0x2E:
        ll = readFromMemory();
        hh = readFromMemory();
        c2 = memory[hh * 0x100 + ll] & 0x80;
        memory[hh * 0x100 + ll] <<= 1;
        // memory[hh * 0x100 + ll] = memory[hh * 0x100 + ll] & 0xfe;
        memory[hh * 0x100 + ll] = memory[hh * 0x100 + ll] | c;
        cycle(6);
        break;
    case 0x3E:
        ll = readFromMemory();
        hh = readFromMemory();
        c2 = memory[addWithCarry(hh * 0x100 + ll, X)] & 0x80;
        memory[addWithCarry(hh * 0x100 + ll, X)] <<= 1;
        // memory[addWithCarry(hh * 0x100 + ll, X)] = memory[addWithCarry(hh * 0x100 + ll, X)] & 0xfe;
        memory[addWithCarry(hh * 0x100 + ll, X)] = memory[addWithCarry(hh * 0x100 + ll, X)] | c;
        cycle(7);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
    SR[7] = c2; //accumulator & c2;
}

// ASL
void shiftLeftOneBit(){
    unsigned char opcode = memory[PC - 1];
    printf("ROL\n");
    char ll, hh;
    bool c = SR[7];
    bool c2 = false;
    switch (opcode){
    case 0x2A:
        c2 = accumulator & 0x80;
        accumulator <<= 1;
        //accumulator = accumulator & 0xfe;
        cycle(2);
        break;
    case 0x26:
        c2 = memory[readFromMemory()] & 0x80;
        memory[readFromMemory()] <<= 1;
        //memory[readFromMemory()] = memory[readFromMemory()] & 0xfe;
        cycle(5);
        break;
    case 0x36:
        memory[readFromMemory() + X] <<= 1;
        c2 = memory[readFromMemory() + X] & 0x80;
        //memory[readFromMemory() + X] = memory[readFromMemory() + X] & 0xfe;
        cycle(6);
        break;
    case 0x2E:
        ll = readFromMemory();
        hh = readFromMemory();
        c2 = memory[hh * 0x100 + ll] & 0x80;
        memory[hh * 0x100 + ll] <<= 1;
        //memory[hh * 0x100 + ll] = memory[hh * 0x100 + ll] & 0xfe;
        cycle(6);
        break;
    case 0x3E:
        ll = readFromMemory();
        hh = readFromMemory();
        c2 = memory[addWithCarry(hh * 0x100 + ll, X)] & 0x80;
        memory[addWithCarry(hh * 0x100 + ll, X)] <<= 1;
        //memory[addWithCarry(hh * 0x100 + ll, X)] = memory[addWithCarry(hh * 0x100 + ll, X)] & 0xfe;
        cycle(7);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
    SR[7] = c2;
}

// BCC
void branchOnCarryClear(){
    printf("BCC\n");
    char offset = readFromMemory();
    if(!SR[7]){
        PC += offset;
        cycle(1);
    }
    cycle(2);
}

// BCS
void branchOnCarrySet(){
    printf("BCS\n");
    char offset = readFromMemory();
    if(SR[7]){
        PC += offset;
        cycle(1);
    }
    cycle(2);
}

// BEQ
void branchOnResultZero(){
    printf("BEQ\n");
    char offset = readFromMemory();
    if(SR[6]){
        PC += offset;
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
        PC += offset;
        cycle(1);
    }
    cycle(2);
}

// BNE
void branchOnResultNotZero(){
    printf("BNE\n");
    char offset = readFromMemory();
    if(!SR[6]){
        PC += offset;
        cycle(1);
    }
    cycle(2);
}

// BPL
void branchOnResultPlus(){
    printf("BPL\n");
    char offset = readFromMemory();
    if(!SR[0]){
        PC += offset;
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
        PC += offset;
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
        PC += offset;
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

// CMP
void compareMemoryWithAccumulator(){
    unsigned char opcode = memory[PC - 1];
    printf("CMP\n");
    unsigned char ll, hh;
    char result;
    switch (opcode){
    // Immediate
    case 0xC9:
        result = accumulator - readFromMemory();
        cycle(2);
        break;
    // Zeropage
    case 0xC5:
        result = accumulator - memory[readFromMemory()];
        cycle(3);
        break;
    // Zeropage, X
    case 0xD5:
        result = accumulator - memory[readFromMemory() + X];
        cycle(4);
        break;
    // Absolute
    case 0xCD:
        ll = readFromMemory();
        hh = readFromMemory();
        result = accumulator - memory[hh * 0x100 + ll];
        cycle(4);
        break;
    // Absolute, X
    case 0xDD:
        ll = readFromMemory();
        hh = readFromMemory();
        result = accumulator - memory[addWithCarry(hh * 0x100 + ll, X)];
        cycle(4);
        break;
    // Absolute, Y
    case 0xD9:
        ll = readFromMemory();
        hh = readFromMemory();
        result = accumulator - memory[addWithCarry(hh * 0x100 + ll, Y)];
        cycle(4);
        break;
    // (Indirect, X)
    case 0xC1:
        ll = readFromMemory();
        result = accumulator - memory[memory[ll + X] * 0x100 + memory[ll + X + 1]];
        cycle(6);
        break;
    // (Indirect), Y
    case 0xD1:
        ll = readFromMemory();
        result = accumulator - memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)];
        cycle(5);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = result < 0;
    SR[6] = result == 0;
}

// CPX
void compareMemoryAndIndexX(){
    unsigned char opcode = memory[PC - 1];
    printf("CPX\n");
    unsigned char ll, hh;
    char result;
    switch (opcode){
    case 0xE0:
        result = X - readFromMemory();
        cycle(2);
        break;
    case 0xE4:
        result = X - memory[readFromMemory()];
        cycle(3);
        break;
    case 0xEC:
        ll = readFromMemory();
        hh = readFromMemory();
        result = X - memory[hh * 0x100 + ll];
        cycle(4);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = result < 0;
    SR[6] = result == 0;    
}

// CPY
void compareMemoryAndIndexY(){
    unsigned char opcode = memory[PC - 1];
    printf("CPY\n");
    unsigned char ll, hh;
    char result;
    switch (opcode){
    case 0xE0:
        result = Y - readFromMemory();
        cycle(2);
        break;
    case 0xE4:
        result = Y - memory[readFromMemory()];
        cycle(3);
        break;
    case 0xEC:
        ll = readFromMemory();
        hh = readFromMemory();
        result = Y - memory[hh * 0x100 + ll];
        cycle(4);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = result < 0;
    SR[6] = result == 0;    
}

// DEC
void decrementMemoryByOne(){
    unsigned char opcode = memory[PC - 1];
    printf("DEC\n");
    unsigned char ll, hh;
    char result;
    switch (opcode){
    case 0xC6:
        result = --memory[readFromMemory()];
        cycle(0);
        break;
    case 0xD6:
        result = --memory[readFromMemory() + X];
        cycle(0);
        break;
    case 0xCE:
        ll = readFromMemory();
        hh = readFromMemory();
        result = --memory[hh * 0x100 + ll];
        cycle(0);
        break;
    case 0xDE:
        ll = readFromMemory();
        hh = readFromMemory();
        result = --memory[addWithCarry(hh * 0x100 + ll, X)];
        cycle(0);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = result < 0;
    SR[6] = result == 0;
}

// DEX
void decrementIndexXByOne(){
    printf("DEX\n");
    X--;

    // Flags
    SR[0] = X < 0;
    SR[6] = X == 0;
}

// DEY
void decrementIndexYByOne(){
    printf("DEY\n");
    Y--;

    // Flags
    SR[0] = Y < 0;
    SR[6] = Y == 0;
}

// EOR
void exclusiveOrWithAccumulator(){
    unsigned char opcode = memory[PC - 1];
    printf("EOR\n");
    unsigned char ll, hh;
    switch (opcode){
    case 0x49:
        accumulator = accumulator ^ readFromMemory();
        cycle(2);
        break;
    case 0x45:
        accumulator = accumulator ^ memory[readFromMemory()]; 
        cycle(3);
        break;
    case 0x55:
        accumulator = accumulator ^ memory[readFromMemory() + X];
        cycle(4);
        break;
    case 0x4D:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = accumulator ^ memory[hh * 0x100 + ll]; 
        cycle(4);
        break;
    case 0x5D:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = accumulator ^ memory[addWithCarry(hh * 0x100 + ll, X)]; 
        cycle(4);
        break;
    case 0x59:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = accumulator ^ memory[addWithCarry(hh * 0x100 + ll, Y)]; 
        cycle(4);
        break;
    case 0x41:
        ll = readFromMemory();
        accumulator = accumulator ^ memory[memory[ll + X] * 0x100 + memory[ll + X + 1]];
        cycle(6);
        break;
    case 0x51:
        accumulator = accumulator ^ memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)];
        cycle(5);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
}

// INC
void incrementMemoryByOne(){
    unsigned char opcode = memory[PC - 1];
    printf("INC\n");
    unsigned char ll, hh;
    char result;
    switch (opcode){
    case 0xC6:
        result = ++memory[readFromMemory()];
        cycle(0);
        break;
    case 0xD6:
        result = ++memory[readFromMemory() + X];
        cycle(0);
        break;
    case 0xCE:
        ll = readFromMemory();
        hh = readFromMemory();
        result = ++memory[hh * 0x100 + ll];
        cycle(0);
        break;
    case 0xDE:
        ll = readFromMemory();
        hh = readFromMemory();
        result = ++memory[addWithCarry(hh * 0x100 + ll, X)];
        cycle(0);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = result < 0;
    SR[6] = result == 0;
}

// INX
void incrementIndexXByOne(){
    printf("INX\n");
    X++;

    // Flags
    SR[0] = X < 0;
    SR[6] = X == 0;
}

// INY
void incrementIndexYByOne(){
    printf("INY\n");
    Y++;

    // Flags
    SR[0] = Y < 0;
    SR[6] = Y == 0;
}

// JMP
void jumpToNewLocation(){
    unsigned char opcode = memory[PC - 1];
    printf("INC\n");
    unsigned char ll, hh;
    switch (opcode){
    case 0x4C:
        ll = readFromMemory();
        hh = readFromMemory();
        PC = hh * 0x100 + ll;
        cycle(3);
        break;
    case 0x6C:
        ll = readFromMemory();
        hh = readFromMemory();
        PC = memory[hh * 0x100 + ll] * 0x100 + memory[hh * 0x100 + ll + 1];
        cycle(5);
        break;
    
    default:
        break;
    }
}

// JSR
void jumpToNewLocationSavingReturnAdress(){
    printf("JSR\n");
    unsigned char ll, hh;
    ll = (unsigned char)(PC & 0x00FF);
    hh = (unsigned char)(PC & 0xFF00);
    pushStack(hh);
    pushStack(ll + 2);
    ll = readFromMemory();
    hh = readFromMemory();
    PC = (unsigned short)(hh * 0x100 + ll);
    cycle(6);
}

// LDX
void loadIndexXWithMemory(){
    unsigned char opcode = memory[PC - 1];
    printf("LDX\n");
    unsigned char ll, hh;
    switch (opcode){
    case 0xA2:
        X = readFromMemory();
        cycle(2);
        break;
    case 0xA6:
        X = memory[readFromMemory()];
        cycle(3);
        break;
    case 0xB6:
        X = memory[readFromMemory() + Y];
        cycle(4);
        break;
    case 0xAE:
        ll = readFromMemory();
        hh = readFromMemory();
        X = memory[hh * 0x100 + ll];
        cycle(4);
        break;
    case 0xBE:
        ll = readFromMemory();
        X = memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)];
        cycle(4);
        break;

    default:
        break;
    }

    SR[0] = X < 0;
    SR[6] = X == 0;
}

// LDY
void loadIndexYWithMemory(){
    unsigned char opcode = memory[PC - 1];
    printf("LDY\n");
    unsigned char ll, hh;
    switch (opcode){
    case 0xA0:
        Y = readFromMemory();
        cycle(2);
        break;
    case 0xA4:
        Y = memory[readFromMemory()];
        cycle(3);
        break;
    case 0xB4:
        Y = memory[readFromMemory() + X];
        cycle(4);
        break;
    case 0xAC:
        ll = readFromMemory();
        hh = readFromMemory();
        Y = memory[hh * 0x100 + ll];
        cycle(4);
        break;
    case 0xBC:
        ll = readFromMemory();
        Y = memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], X)];
        cycle(4);
        break;

    default:
        break;
    }

    SR[0] = Y < 0;
    SR[6] = Y == 0;
}

// LSR
void shiftOneBitRight(){
    unsigned char opcode = memory[PC - 1];
    printf("LSR\n");
    char ll, hh;
    bool c = SR[7];
    bool c2 = false;
    switch (opcode){
    case 0x4A:
        c2 = accumulator & 0x01;
        accumulator >>= 1;
        //accumulator = accumulator & 0x7F;
        cycle(2);
        break;
    case 0x46:
        c2 = memory[readFromMemory()] & 0x01;
        memory[readFromMemory()] >>= 1;
        //memory[readFromMemory()] = memory[readFromMemory()] & 0x7F;
        cycle(5);
        break;
    case 0x56:
        c2 = memory[readFromMemory() + X] & 0x01;
        memory[readFromMemory() + X] >>= 1;
        //memory[readFromMemory() + X] = memory[readFromMemory() + X] & 0x7F;
        cycle(6);
        break;
    case 0x4E:
        ll = readFromMemory();
        hh = readFromMemory();
        c2 = memory[hh * 0x100 + ll] & 0x01;
        memory[hh * 0x100 + ll] >>= 1;
        memory[hh * 0x100 + ll] = memory[hh * 0x100 + ll] & 0x7F;
        printf("%x\n", memory[hh * 0x100 + ll]);
        cycle(6);
        break;
    case 0x5E:
        ll = readFromMemory();
        hh = readFromMemory();
        c2 = memory[addWithCarry(hh * 0x100 + ll, X)] & 0x01;
        memory[addWithCarry(hh * 0x100 + ll, X)] >>= 1;
        memory[addWithCarry(hh * 0x100 + ll, X)] = memory[addWithCarry(hh * 0x100 + ll, X)] & 0x7F;
        cycle(7);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = false;
    SR[6] = accumulator == 0;
    SR[7] = c2;
}

// NOP
void noOperation(){
    printf("NOP\n");
    cycle(2);
}

// ORA
void orMemoryWithAccumulator(){
    unsigned char opcode = memory[PC - 1];
    printf("ORA\n");
    unsigned char ll, hh;
    switch (opcode){
    case 0x09:
        accumulator = accumulator ^ readFromMemory();
        cycle(2);
        break;
    case 0x05:
        accumulator = accumulator ^ memory[readFromMemory()]; 
        cycle(3);
        break;
    case 0x15:
        accumulator = accumulator ^ memory[readFromMemory() + X];
        cycle(4);
        break;
    case 0x0D:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = accumulator ^ memory[hh * 0x100 + ll]; 
        cycle(4);
        break;
    case 0x1D:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = accumulator ^ memory[addWithCarry(hh * 0x100 + ll, X)]; 
        cycle(4);
        break;
    case 0x19:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = accumulator ^ memory[addWithCarry(hh * 0x100 + ll, Y)]; 
        cycle(4);
        break;
    case 0x01:
        ll = readFromMemory();
        accumulator = accumulator ^ memory[memory[ll + X] * 0x100 + memory[ll + X + 1]];
        cycle(6);
        break;
    case 0x11:
        accumulator = accumulator ^ memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)];
        cycle(5);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
}

// PHA
void pushAccumulatorOnStack(){
    printf("PHA\n");
    pushStack(accumulator);
    cycle(3);
}

// PHP
void pushProcessorStatusOnStack(){
    printf("PHP\n");
    pushStack(statusToChar());
    cycle(3);
}

// PLA
void pullAccumulatorOffStack(){
    printf("PLA\n");
    accumulator = popStack();
    cycle(3);

    // Flags
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
}

// PLP
void pullProcessorStatusOffStack(){
    printf("PLP\n");
    charToStatus(popStack());
    cycle(3);
}

// ROR
void rotateOneBitRight(){
    unsigned char opcode = memory[PC - 1];
    printf("ROR\n");
    unsigned char ll, hh;
    bool c = SR[7];
    bool c2 = false;
    switch (opcode){
    case 0x6A:
        c2 = accumulator & 0x01;
        accumulator >>= 1;
        //printf("%x", accumulator);
        //accumulator = accumulator & 0x7F;
        accumulator = accumulator | c * 0x80;
        cycle(2);
        break;
    case 0x66:
        c2 = memory[readFromMemory()] & 0x01;
        memory[readFromMemory()] >>= 1;
        // memory[readFromMemory()] = memory[readFromMemory()] & 0x7F;
        memory[readFromMemory()] = memory[readFromMemory()] | c * 0x80;
        cycle(5);
        break;
    case 0x76:
        c2 = memory[readFromMemory() + X] & 0x01;
        memory[readFromMemory() + X] >>= 1;
        // memory[readFromMemory() + X] = memory[readFromMemory() + X] & 0x7F;
        memory[readFromMemory() + X] = memory[readFromMemory() + X] | c * 0x80;
        cycle(6);
        break;
    case 0x6E:
        ll = readFromMemory();
        hh = readFromMemory();
        c2 = memory[hh * 0x100 + ll] & 0x01;
        memory[hh * 0x100 + ll] >>= 1;
        // memory[hh * 0x100 + ll] = memory[hh * 0x100 + ll] & 0x7F;
        memory[hh * 0x100 + ll] = memory[hh * 0x100 + ll] | c * 0x80;
        cycle(6);
        break;
    case 0x7E:
        ll = readFromMemory();
        hh = readFromMemory();
        c2 = memory[addWithCarry(hh * 0x100 + ll, X)] & 0x01;
        memory[addWithCarry(hh * 0x100 + ll, X)] >>= 1;
        // memory[addWithCarry(hh * 0x100 + ll, X)] = memory[addWithCarry(hh * 0x100 + ll, X)] & 0x7F;
        memory[addWithCarry(hh * 0x100 + ll, X)] = memory[addWithCarry(hh * 0x100 + ll, X)] | c * 0x80;
        cycle(7);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
    SR[7] = c2; //accumulator & c2 * 0x80;
}

// RTI
void returnFromIterrupt(){
    charToStatus(popStack());
    char ll, hh;
    hh = popStack();
    ll = popStack();
    PC = hh * 0x100 + ll;
    SR[3] = true;
}

// RTS
void returnFromSubroutine(){
    char ll, hh;
    hh = popStack();
    ll = popStack();
    PC = hh * 0x100 + ll + 1;
}

// SBC
void substractMemoryFromAccumulatorWithBorrow(){
    unsigned char opcode = memory[PC - 1];
    printf("SBC\n");
    unsigned char ll, hh;
    switch (opcode){
    case 0xE9:
        accumulator = substractWithBorrow(accumulator, readFromMemory());
        cycle(2);
        break;
    case 0xE5:
        accumulator = substractWithBorrow(accumulator, memory[readFromMemory()]); 
        cycle(3);
        break;
    case 0xF5:
        accumulator = substractWithBorrow(accumulator, memory[readFromMemory() + X]);
        cycle(4);
        break;
    case 0xED:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = substractWithBorrow(accumulator, memory[hh * 0x100 + ll]); 
        cycle(4);
        break;
    case 0xFD:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = substractWithBorrow(accumulator, memory[addWithCarry(hh * 0x100 + ll, X)]); 
        cycle(4);
        break;
    case 0xF9:
        ll = readFromMemory();
        hh = readFromMemory();
        accumulator = substractWithBorrow(accumulator, memory[addWithCarry(hh * 0x100 + ll, Y)]); 
        cycle(4);
        break;
    case 0xE1:
        ll = readFromMemory();
        accumulator = substractWithBorrow(accumulator, memory[memory[ll + X] * 0x100 + memory[ll + X + 1]]);
        cycle(6);
        break;
    case 0xF1:
        accumulator = substractWithBorrow(accumulator, memory[addWithCarry(memory[ll] * 0x100 + memory[ll + 1], Y)]);
        cycle(5);
        break;
    
    default:
        break;
    }

    // Flags
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
}

// SEC
void setCarryFlag(){
    printf("SEC\n");
    SR[7] = true;
    cycle(2);
}

// SED
void setDecimalFlag(){
    printf("SED\n");
    SR[4] = true;
    cycle(2);
}

// SEI
void setInterruptDisableStatus(){
    printf("SED\n");
    SR[5] = true;
    cycle(2);
}

// STX
void storeIndexXInMemory(){
    unsigned char opcode = memory[PC - 1];
    printf("STX\n");
    unsigned char ll, hh;
    switch (opcode){
    case 0x86:
        memory[readFromMemory()] = X; 
        cycle(3);
        break;
    case 0x96:
        memory[readFromMemory() + Y] = X; 
        cycle(4);
        break;
    case 0x8E:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[hh * 0x100 + ll] = X;
        cycle(4);
        break;
    
    default:
        break;
    }
}

// STY
void storeIndexYInMemory(){
    unsigned char opcode = memory[PC - 1];
    //printf("STY %x\n", opcode);
    unsigned char ll, hh;
    switch (opcode){
    case 0x84:
        memory[readFromMemory()] = Y; 
        cycle(3);
        break;
    case 0x94:
        memory[readFromMemory() + X] = Y; 
        cycle(4);
        break;
    case 0x8C:
        ll = readFromMemory();
        hh = readFromMemory();
        memory[hh * 0x100 + ll] = Y;
        cycle(4);
        break;
    
    default:
        break;
    }
}

// TAX
void transferAccumulatorToIndexX(){
    printf("TAX\n");
    X = accumulator;
    cycle(2);

    // Flags
    SR[0] = X < 0;
    SR[6] = X == 0;
}

// TAY
void transferAccumulatorToIndexY(){
    printf("TAY\n");
    Y = accumulator;
    cycle(2);

    // Flags
    SR[0] = Y < 0;
    SR[6] = Y == 0;
}

// TSX
void transferStackPointerToIndexX(){
    printf("TSX\n");
    X = SP;
    cycle(2);

    // Flags
    SR[0] = X < 0;
    SR[6] = X == 0;
}

// TXA
void transferIndexXToAccumulator(){
    printf("TXA\n");
    accumulator = X;
    cycle(2);

    // Flags
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
}

// TXS
void transferIndexXToStackRegister(){
    printf("TXS\n");
    X = SP;
    cycle(2);
}

// TYA
void transferIndexYToAccumulator(){
    printf("TXA\n");
    accumulator = Y;
    cycle(2);

    // Flags
    SR[0] = accumulator < 0;
    SR[6] = accumulator == 0;
}


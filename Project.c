#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#define SIZE 10

int skip = 0;
int cyclesNeeded = 0;
struct queueElement
{
    int firstOp;
    int secondOp;
    int opcode;
};

struct instr
{
    char instruction[17];
};

char fetchQueueOutput[17];
struct queueElement decodeQueueOutput;

struct queueElement decodeQueue[SIZE];
int frontDecode = -1;
int rearDecode = -1;

// Function to check if the decodeQueue is full
int isFullDecode()
{
    return rearDecode == SIZE - 1;
}

// Function to check if the decodeQueue is empty
int isEmptyDecode()
{
    return frontDecode == -1 || frontDecode > rearDecode;
}

// Function to add an element to the decodeQueue
void enqueueDecode(struct queueElement element)
{
    if (isFullDecode())
    {
        printf("Queue is full!\n");
    }
    else
    {
        if (frontDecode == -1)
        {
            frontDecode = 0;
        }
        rearDecode++;
        decodeQueue[rearDecode] = element;
    }
}

// Function to remove an element from the decodeQueue
void dequeueDecode()
{
    if (isEmptyDecode())
    {
        printf("Queue is empty!\n");
    }
    else
    {
        decodeQueueOutput = decodeQueue[frontDecode];
        frontDecode++;
        if (frontDecode > rearDecode)
        {
            frontDecode = rearDecode = -1;
        }
    }
}

struct instr fetchQueue[SIZE];
int frontFetch = -1;
int rearFetch = -1;

// Function to check if the decodeQueue is full
int isFullFetch()
{
    return rearFetch == SIZE - 1;
}

// Function to check if the decodeQueue is empty
int isEmptyFetch()
{
    return frontFetch == -1 || frontFetch > rearFetch;
}

// Function to add an element to the decodeQueue
void enqueueFetch(char instr[17])
{
    struct instr element;
    strcpy(element.instruction, instr);
    if (isFullFetch())
    {
        printf("Queue is full!\n");
    }
    else
    {
        if (frontFetch == -1)
        {
            frontFetch = 0;
        }
        rearFetch++;
        fetchQueue[rearFetch] = element;
    }
}

// Function to remove an element from the decodeQueue
char *dequeueFetch()
{
    if (isEmptyFetch())
    {
        printf("Queue is empty!\n");
    }
    else
    {
        strcpy(fetchQueueOutput, fetchQueue[frontFetch].instruction);
        frontFetch++;
        if (frontFetch > rearFetch)
        {
            frontFetch = rearFetch = -1;
        }
    }
}

int clock = 0;
char currentInstr[17];

struct instr IM[1024];
int DM[2048];
int8_t REG[64];
int opcodeVal;
int number_of_instr = 0;

int firstOP;
int secondOP;

int8_t SREG;
unsigned short PC;

void updateOverflow(uint8_t op1, uint8_t op2, uint8_t val)
{
    uint8_t firstMSB = (op1 & 0b10000000) >> 7;
    uint8_t secondMSB = (op1 & 0b10000000) >> 7;
    uint8_t valMSB = (val & 0b10000000) >> 7;

    if (firstMSB == secondMSB)
    {
        if (firstMSB != val)
        {
            // mask other SREG bits
            SREG = SREG & 0b11110111;

            // update needed bit (2nd bit)
            SREG = SREG | 0b00001000;
        }
    }
    // no need to mask then set as we want it to be zero
    SREG = SREG & 0b11110111;
}

void updateSignFlag()
{
    uint8_t nBit = (SREG & 0b00000100) >> 2;
    uint8_t vBit = (SREG & 0b00000010) >> 1;

    if (nBit ^ vBit)
    {
        // mask other SREG bits
        SREG = SREG & 0b11111101;

        // update needed bit (2nd bit)
        SREG = SREG | 0b00000010;
    }
    else
    {
        // no need to mask then set as we want it to be zero
        SREG = SREG & 0b11111101;
    }
}

void updateZeroFlag(int val)
{
    // check if operation value=0
    if (val == 0)
    {

        // mask other SREG bits
        SREG = SREG & 0b11111110;

        // update needed bit (4th bit)
        SREG = SREG | 0b00000001;
    }
    else
    {
        // no need to mask then set as we want it to be zero
        SREG = SREG & 0b11111110;
    }
}

void updateNegativeFlag(int val)
{
    int comp = val & 0b0000000000000000000000010000000;
    uint8_t newVal = comp >> 5;

    // mask other SREG bits
    SREG = SREG & 0b11111011;

    // update needed bit (2nd bit)
    SREG = SREG | newVal;
}

void updateCarryflag(int val)
{
    // gets unsigned value
    if (val < 0)
    {
        val = val / -1;
    }

    int comp = val & 0b00000000000000000000000100000000;
    uint8_t newVal = comp >> 4;

    // mask other SREG bits
    SREG = SREG & 0b11101111;

    // update needed bit (4th bit)
    SREG = SREG | newVal;
}

void getBinary(int value, int i)
{
    char binary[6];
    int index = 5;
    int numBits = 0;
    int flag = 0;
    // Calculate the number of bits needed
    int temp = value;

    while (temp != 0)
    {

        if (temp % 2 == 0)
        {
            binary[index] = '0';
            index = index - 1;
        }
        else
        {
            binary[index] = '1';
            index = index - 1;
        }
        temp /= 2;
    }
    while (index > -1)
    {
        binary[index] = '0';
        index = index - 1;
    }

    if (value < 0)
    {

        index = 5;

        while (index > -1)
        {
            if (flag)
            {
                if (binary[index] == '0')
                {
                    binary[index] = '1';
                }
                else
                {
                    binary[index] = '0';
                }
            }
            else
            {
                if (binary[index] == '1')
                {
                    flag = 1;
                }
            }
            index = index - 1;
        }
    }

    strcat(IM[i].instruction, binary);
}

void getBinaryMod(int value, char binary[7])
{
    int index = 5;
    int numBits = 0;
    int flag = 0;
    int temp = value;

    while (temp != 0)
    {

        if (temp % 2 == 0)
        {
            binary[index] = '0';
            index = index - 1;
        }
        else
        {
            binary[index] = '1';
            index = index - 1;
        }
        temp /= 2;
    }
    while (index > -1)
    {
        binary[index] = '0';
        index = index - 1;
    }

    if (value < 0)
    {

        index = 5;

        while (index > -1)
        {
            if (flag)
            {
                if (binary[index] == '0')
                {
                    binary[index] = '1';
                }
                else
                {
                    binary[index] = '0';
                }
            }
            else
            {
                if (binary[index] == '1')
                {
                    flag = 1;
                }
            }
            index = index - 1;
        }
    }
}

void parseInstruction(char *instr, int i)
{
    char opcode[5];
    char *opcodePtr = instr;
    int opcodeLen = 0;
    char regValue[2];

    // counts number of characters of opcode
    while ((*opcodePtr) != ' ')
    {
        opcodePtr++;
        opcodeLen++;
    }
    memset(opcode, 0, 4);
    memset(regValue, 0, 2);

    strncpy(opcode, instr, opcodeLen);

    opcodeLen = 0;

    // check what the type of opcode
    if (strcmp("ADD", opcode) == 0)
    {
        strcpy(IM[i].instruction, "0000");
    }
    if (strcmp("SUB", opcode) == 0)
    {
        strcpy(IM[i].instruction, "0001");
    }
    if (strcmp("MUL", opcode) == 0)
    {
        strcpy(IM[i].instruction, "0010");
    }
    if (strcmp("LDI", opcode) == 0)
    {
        strcpy(IM[i].instruction, "0011");
    }
    if (strcmp("BEQZ", opcode) == 0)
    {
        strcpy(IM[i].instruction, "0100");
    }
    if (strcmp("AND", opcode) == 0)
    {
        strcpy(IM[i].instruction, "0101");
    }
    if (strcmp("OR", opcode) == 0)
    {
        strcpy(IM[i].instruction, "0110");
    }
    if (strcmp("JR", opcode) == 0)
    {
        strcpy(IM[i].instruction, "0111");
    }
    if (strcmp("SLC", opcode) == 0)
    {
        strcpy(IM[i].instruction, "1000");
    }
    if (strcmp("SRC", opcode) == 0)
    {
        strcpy(IM[i].instruction, "1001");
    }
    if (strcmp("LB", opcode) == 0)
    {
        strcpy(IM[i].instruction, "1010");
    }
    if (strcmp("SB", opcode) == 0)
    {
        strcpy(IM[i].instruction, "1011");
    }

    opcodePtr++;
    opcodePtr++;

    regValue[0] = (*opcodePtr);
    opcodePtr++;

    if ((*opcodePtr) != ' ')
    {
        regValue[1] = (*opcodePtr);
        opcodePtr++;
    }
    opcodePtr++;
    getBinary(atoi(regValue), i);

    memset(regValue, 0, 2);

    if ((*opcodePtr) == 'R')
    {
        opcodePtr++;
        while ((*opcodePtr) != ' ' && (*opcodePtr) != '\n' && (*opcodePtr) != '\0')
        {
            strncat(regValue, (opcodePtr), 1);
            opcodePtr++;
        }
        getBinary(atoi(regValue), i);
    }
    else
    {

        while ((*opcodePtr) != ' ' && (*opcodePtr) != '\n' && (*opcodePtr) != '\0')
        {
            strncat(regValue, (opcodePtr), 1);
            opcodePtr++;
        }
        getBinary(atoi(regValue), i);
    }
}

void instructionFetch()
{
    strncpy(currentInstr, IM[PC].instruction, 16);
    printf("Instruction being Fetched: %s\n", IM[PC].instruction);

    enqueueFetch(IM[PC].instruction);
    PC++;
}

void instructionDecode()
{
    char opcode[5];
    char firstOperand[7];
    char secondOperand[7];
    char temp[32];
    struct queueElement elementToInsert;

    printf("Instruction being Decoded: %s\n", fetchQueueOutput);

    strncpy(opcode, fetchQueueOutput, 4);
    strncpy(firstOperand, &fetchQueueOutput[4], 6);
    strncpy(secondOperand, &fetchQueueOutput[10], 6);

    opcode[4] = '\0';
    firstOperand[6] = '\0';
    secondOperand[6] = '\0';
    temp[31] = '\0';

    if (firstOperand[0] == '1')
    {
        strcpy(temp, "11111111111111111111111111");
        strncat(temp, firstOperand, 6);
        firstOP = (int)strtol(temp, NULL, 2);
    }
    else
    {
        strcpy(temp, "00000000000000000000000000");
        strncat(temp, firstOperand, 6);
        firstOP = (int)strtol(temp, NULL, 2);
    }

    if (secondOperand[0] == '1')
    {
        strcpy(temp, "11111111111111111111111111");
        strcat(temp, secondOperand);
        secondOP = (int)strtol(temp, NULL, 2);
    }
    else
    {
        strcpy(temp, "00000000000000000000000000");
        strcat(temp, secondOperand);
        secondOP = (int)strtol(temp, NULL, 2);
    }
    opcodeVal = (int)strtol(opcode, NULL, 2);

    elementToInsert.firstOp = firstOP;
    elementToInsert.secondOp = secondOP;
    elementToInsert.opcode = opcodeVal;
    enqueueDecode(elementToInsert);
    if (opcodeVal == 4)
    {
        dequeueFetch();
    }
}

void instructionExecute()
{
    printf("Instruction Being Executed %s\n", IM[PC - 3].instruction);
    int tempVal = 0;
    switch (opcodeVal)
    {
    // ADD
    case 0:
        tempVal = REG[firstOP - 1] + REG[secondOP - 1];

        updateCarryflag(tempVal);
        updateNegativeFlag(tempVal);
        updateOverflow(REG[firstOP - 1], REG[secondOP - 1], tempVal);
        updateZeroFlag(tempVal);
        updateSignFlag(tempVal);
        REG[firstOP - 1] = REG[firstOP - 1] + REG[secondOP - 1];
        break;

    // SUB
    case 1:
        tempVal = REG[firstOP - 1] - REG[secondOP - 1];
        updateOverflow(REG[firstOP - 1], REG[secondOP - 1], tempVal);
        updateNegativeFlag(tempVal);
        updateZeroFlag(tempVal);
        updateSignFlag(tempVal);

        REG[firstOP - 1] = REG[firstOP - 1] - REG[secondOP - 1];

        break;

    // MUL
    case 2:
        tempVal = REG[firstOP - 1] * REG[secondOP - 1];

        updateNegativeFlag(tempVal);
        updateZeroFlag(tempVal);

        break;

    // LDI
    case 3:
        REG[firstOP - 1] = secondOP;
        break;

    // BEQZ
    case 4:
        if (REG[firstOP - 1] == 0)
        {
            PC += secondOP;
        }

        // flush any instruction in decode and fetch pipeline(queue)
        while (!isEmptyDecode())
        {
            dequeueDecode();
        }

        while (!isEmptyFetch())
        {
            dequeueFetch();
        }
        cyclesNeeded = (number_of_instr - secondOP - 1) + 3;
        break;

    // AND
    case 5:
        tempVal = REG[firstOP - 1] & REG[secondOP - 1];
        REG[firstOP - 1] = REG[firstOP - 1] & REG[secondOP - 1];

        // Updating zero flag
        updateZeroFlag(tempVal);

        // Updating negative flag
        updateNegativeFlag(tempVal);

        break;
    // OR
    case 6:
        REG[firstOP - 1] = REG[firstOP - 1] | REG[secondOP - 1];

        tempVal = REG[firstOP - 1] | REG[secondOP - 1];
        // Updating zero flag
        updateZeroFlag(tempVal);

        // Updating negative flag
        updateNegativeFlag(tempVal);

        break;

    // JR
    case 7:
        char temp[27];

        char tempBin[7];
        getBinaryMod(REG[firstOP - 1], tempBin);
        strcpy(temp, tempBin);
        getBinaryMod(REG[secondOP - 1], tempBin);
        strcat(temp, tempBin);
        if (REG[firstOP - 1] >= 0)
        {
            PC = (int)strtol(temp, NULL, 2);
        }
        else
        {
            strcat(temp, "11111111111111111111");
            PC = (int)strtol(temp, NULL, 2);
        }

        break;

    // SLC
    case 8:
        // Get the modulo 8 of the immediate value to determine shift amount
        int shift = secondOP % 8;

        // Perform the left circular shift
        REG[firstOP - 1] = (REG[firstOP - 1] << shift) | (REG[firstOP - 1] >> (8 - shift));

        tempVal = (REG[firstOP - 1] >> shift) | (REG[firstOP - 1] << (8 - shift));
        // Updating zero flag
        updateZeroFlag(tempVal);

        // Updating negative flag
        updateNegativeFlag(tempVal);

        break;
    // SRC
    case 9:
        // Get the modulo 8 of the immediate value to determine shift amount
        shift = secondOP % 8;

        // Perform the right circular shift
        REG[firstOP - 1] = (REG[firstOP - 1] >> shift) | (REG[firstOP - 1] << (8 - shift));

        tempVal = (REG[firstOP - 1] >> shift) | (REG[firstOP - 1] << (8 - shift));
        // Updating zero flag
        updateZeroFlag(tempVal);

        // Updating negative flag
        updateNegativeFlag(tempVal);

        break;
    // LB
    case 10:
        REG[firstOP - 1] = DM[secondOP - 1];
        break;
    // SB
    case 11:
        DM[secondOP - 1] = REG[firstOP - 1];
        break;

    default:
        break;
    }
    printf("___________________________________________\n");
    printf("Registers Values:\n");
    printf("_________________\n");
    printf("PC: %d\n", PC);
    printf("General Purpose Registers:\n");
    printf("    R%d:%d\n", firstOP, REG[firstOP - 1]);
    printf("SREG:\n");
    printf("    -Zero Flag:%d\n",(SREG&0b0000000));
    printf("    -Sign Flag:%d\n",(SREG&0b0000010)>>1);
    printf("    -Negative Flag:%d\n",(SREG&0b0000100)>>2);
    printf("    -Twos Comp Flag:%d\n",(SREG&0b0001000)>>3);
    printf("    -Carry Flag:%d\n",(SREG&0b00010000)>>4);
}

int main(int argc, char const *argv[])
{
    FILE *fptr;
    fptr = fopen("assembly.text", "r");
    char instruction[100];
    int i = 0;
    PC = 0;

    if (fptr != NULL)
    {
        // read file line by line and store in instruction memory
        while (fgets(instruction, 100, fptr))
        {
            parseInstruction(instruction, i);
            i++;
            number_of_instr++;
        }
        fclose(fptr);

        // calculate cycles needed
        cyclesNeeded = 3 + (number_of_instr - 1);
        while (clock != cyclesNeeded)
        {
            printf("Cycle %d:\n", clock + 1);
            printf("Pipeline Stages:\n");
            printf("_________________\n");
            instructionFetch();
            if (!isEmptyFetch() && clock != 0)
            {
                dequeueFetch();
                instructionDecode();
            }
            if (!isEmptyDecode() && clock != 1)
            {
                dequeueDecode();
                firstOP = decodeQueueOutput.firstOp;
                secondOP = decodeQueueOutput.secondOp;
                opcodeVal = decodeQueueOutput.opcode;
                instructionExecute();
            }
            clock++;
            printf("________________________________\n");
        }
    }

    return 0;
}
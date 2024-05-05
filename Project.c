#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int clock = 0;
char currentInstr[17];

struct instr
{
    char instruction[17];
};

// TODO:hayb2o of type ehh
// TODO: el REG hatb2a int wala a3ml type yeb2a 1 byte
struct instr IM[1024];
int DM[2048];
int8_t REG[64];
int opcodeVal;

int firstOP;
int secondOP;

int8_t SREG;
unsigned short PC;

void sub()
{
    REG[firstOP - 1] -= REG[secondOP - 1];
}

void mul()
{
    REG[firstOP - 1] *= REG[secondOP - 1];
}

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

    printf("SREG after neg:%d\n", SREG);
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

    printf("SREG after carry:%d\n", SREG);
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
    printf("%s\n", IM[i].instruction);
}

void instructionFetch()
{
    strncpy(currentInstr, IM[PC].instruction, 16);
    PC++;

    // copy opcode from instruction
}

void instructionDecode()
{
    char opcode[5];
    char firstOperand[7];
    char secondOperand[7];
    char temp[32];

    strncpy(opcode, currentInstr, 4);
    strncpy(firstOperand, &currentInstr[4], 6);
    strncpy(secondOperand, &currentInstr[10], 6);

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
}

void instructionExecute()
{
    switch (opcodeVal)
    {
    // ADD
    case 0:
        int tempVal = REG[firstOP - 1] + REG[secondOP - 1];
        updateCarryflag(tempVal);
        updateNegativeFlag(tempVal);
        // Updating zero flag
        updateZeroFlag(tempVal);
        break;

    // SUB
    case 1:
        sub();

        // Updating zero flag

        // Updating negative flag
        break;

    // MUL
    case 2:
        mul();

        // Updating zero flag

        // Updating negative flag
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

        break;

    // AND
    case 5:
        REG[firstOP - 1] = REG[firstOP - 1] & REG[secondOP - 1];

        // Updating zero flag

        // Updating negative flag

        break;
    // OR
    case 6:
        REG[firstOP - 1] = REG[firstOP - 1] | REG[secondOP - 1];

        // Updating zero flag

        // Updating negative flag

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
        REG[firstOP] = (REG[firstOP] << shift) | (REG[firstOP] >> (8 - shift));

        // Updating zero flag

        // Updating negative flag

        break;
    // SRC
    case 9:
        // Get the modulo 8 of the immediate value to determine shift amount
        shift = secondOP % 8;

        // Perform the right circular shift
        REG[firstOP] = (REG[firstOP] >> shift) | (REG[firstOP] << (8 - shift));

        // Updating zero flag

        // Updating negative flag
        break;
    // LB
    case 10:
        REG[firstOP] = DM[secondOP];
        printf("%d \n", REG[firstOP]);
        break;
    // SB
    case 11:
        DM[secondOP] = REG[firstOP];
        printf("%d \n", DM[secondOP]);
        // Code for opcode 11
        break;

    default:
        break;
    }
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
        }

        REG[0] = -3;
        REG[1] = 3;
        printf("____________________________________________\n");
        instructionFetch();
        instructionDecode();
        instructionExecute();

        // ay 7aga
        // printf("Result: %d\n", REG[firstOP - 1]);
        // printf("Zero flag: %d\n", SREG[0] & zeroMask);
        printf("Negative flag: %d\n", SREG);

        // printf("%d\n", PC);
        fclose(fptr);
    }

    return 0;
}
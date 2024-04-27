#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
int REG[64];
int opcodeVal;

int firstOP;
int secondOP;

int SREG[1];
unsigned short PC;

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
    if (strcmp("LDI ", opcode) == 0)
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
    }
    opcodePtr++;

    opcodePtr++;
    getBinary(atoi(regValue), i);

    memset(regValue, 0, 2);

    if ((*opcodePtr) == 'R')
    {
        opcodePtr++;
        while ((*opcodePtr) != ' ' && (*opcodePtr) != '\n')
        {
            strncat(regValue, (opcodePtr), 1);
            opcodePtr++;
        }
        getBinary(atoi(regValue), i);
    }
    else
    {

        while ((*opcodePtr) != ' ' && (*opcodePtr) != '\n')
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

    strncpy(opcode, currentInstr, 4);
    strncpy(firstOperand, &currentInstr[4], 6);
    strncpy(secondOperand, &currentInstr[10], 6);
    opcode[4] = '\0';
    firstOperand[6] = '\0';
    secondOperand[6] = '\0';

    opcodeVal = (int)strtol(opcode, NULL, 2);
    firstOP = (int)strtol(firstOperand, NULL, 2);
    secondOP = (int)strtol(secondOperand, NULL, 2);
}

void instructionExecute()
{
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
        printf("_\n");
        instructionFetch();
        instructionDecode();
        fclose(fptr);
    }

    return 0;

    // instruction  memory teb2a string w method takes (el int to rep el opcode w wa7da tanya te3ml  )
}
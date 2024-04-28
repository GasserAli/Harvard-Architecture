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
        break;
    // SUB
    case 1:
        // Code for opcode 1
        break;
    // MUL
    case 2:
        // Code for opcode 2
        break;
    // LDI
    case 3:
        REG[firstOP] = secondOP;
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
        REG[firstOP-1] = REG[firstOP-1] & REG[secondOP-1];
        
        //Updating zero flag
        updateZeroFlag();

        //Updating negative flag
        updateNegativeFlag();

        break;
    // OR
    case 6:
        REG[firstOP-1] = REG[firstOP-1] | REG[secondOP-1];

        //Updating zero flag
        updateZeroFlag();

        //Updating negative flag
        updateNegativeFlag();
        
        break;

    //JR
    case 7:
        printf("first OP: %d\n", REG[firstOP]);
        printf("first OP address: %d\n", firstOP);
        printf("REG[1]: %d\n", REG[1]);

        char concatenatedIndex[3];
        sprintf(concatenatedIndex, "%d%d", REG[firstOP-1], REG[secondOP-1]);
        printf("concatenated Index: %s\n", concatenatedIndex);
        int index = atoi(concatenatedIndex);

        // Update the Program Counter (PC)

        PC = index;
        break;

    // SLC
    case 8:
        // Code for opcode 8
        break;
    // SRC
    case 9:
        // Code for opcode 9
        break;
    // LB
    case 10:
        // Code for opcode 10
        break;
    // SB
    case 11:
        // Code for opcode 11
        break;

    default:
        break;
    }
}

void updateZeroFlag()
{
    if (REG[firstOP-1] == 0)
    {
        SREG[0] = SREG[0] | 1;
    }
    else
    {
        SREG[0] = SREG[0] & (~1);
    }
}

void updateNegativeFlag()
{
    if (REG[firstOP-1] < 0)
    {
        SREG[0] = SREG[0] | 4;
    }
    else
    {
        SREG[0] = SREG[0] & (~4);
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
        printf("_\n");
        instructionFetch();
        instructionDecode();
        REG[1] = 0b00000100;
        REG[0] = 0b00000100;
        instructionExecute();
        printf("%d\n", PC);
        fclose(fptr);
    }

    return 0;



    // instruction  memory teb2a string w method takes (el int to rep el opcode w wa7da tanya te3ml  )
}
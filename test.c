#include <stdio.h>

int registerFile [] = {22,16,87,54,102,434,33,67,98,49,55,9,18,1,32,45};
int instructionMemory[] = {67093461,-1879052286,-1342193613,335527172};
int NumberofInstructions = sizeof(instructionMemory)/sizeof(int);
int pc = 0;

// Binary int format in c is 0b00000000000000000000000000000000 (32 bits)

void decode(int instruction) {
        
        // Part of the decode stage....
        
        int opcode = (instruction&0b11111100000000000000000000000000)>>26;    // bits31:26
        int immediate = (instruction&0b00000000000000001111111111111111); 
        // bits15:0
        if(((immediate&0b00000000000000001000000000000000)>>15)==1){
            immediate=(instruction&0b00000000000000001111111111111111)|0b11111111111111110000000000000000;
        }
        
        int ALUOp = 0;
        int RegDst = 0;
        int ALUSrc = 0;
        int RegWrite = 0;
        int MemRead = 0;
        int MemWrite = 0;
        int Branch = 0;
        int MemToReg = 0;
        
        // Implement the Sign Extend and Control Unit here:
        
        // Printings
            switch(opcode){
            case 0b100011:{
                ALUOp=0b00;
                RegDst=0;
                ALUSrc=1;
                RegWrite=1;
                MemRead=1;
                MemWrite=0;
                break;
            }

            case 0b101011:{
                ALUOp=0b00;
                RegDst=0;
                ALUSrc=1;
                RegWrite=0;
                MemRead=0;
                MemWrite=1;
                break;
            }

            case 0b000100:{
                ALUOp=0b01;
                RegDst=0;
                ALUSrc=0;
                RegWrite=0;
                MemRead=0;
                MemWrite=0;
                break;
            }

            case 0b000000:{
                ALUOp=0b10;
                RegDst=1;
                ALUSrc=0;
                RegWrite=1;
                MemRead=0;
                MemWrite=0;
                break;
            }
        }
        printf("Instruction %i\n",pc);
		printf("opcode = %i\n",opcode);
		printf("ALUOp = %i\n",ALUOp);
		printf("RegDst = %i\n",RegDst);
		printf("ALUSrc = %i\n",ALUSrc);
		printf("RegWrite = %i\n",RegWrite);
		printf("MemRead = %i\n",MemRead);
		printf("MemWrite = %i\n",MemWrite);
		printf("Branch = %i\n",Branch);
		printf("MemToReg = %i\n",MemToReg);
		printf("Sign-extended Immediate = %i\n",immediate);
		printf("---------- \n");

    
             
}

void fetch() {
        
        int instruction = 0;
        for(int i =0;i<NumberofInstructions;i++){
            instruction = instructionMemory[pc];
            decode(instruction);
            pc++;
        }
        
}

void main() {
	   
    fetch();
    
   // Expected output
    
    /* 
    
    Instruction 0
    opcode = 0
    ALUOp = 2
    RegDst = 1
    ALUSrc = 0
    RegWrite = 1
    MemRead = 0
    MemWrite = 0
    Branch = 0
    MemToReg = 0
    Sign-extended Immediate = -15403
    
    ----------
    
    Instruction 1
    opcode = 35
    ALUOp = 0
    RegDst = 0
    ALUSrc = 1
    RegWrite = 1
    MemRead = 1
    MemWrite = 0
    Branch = 0
    MemToReg = 1
    Sign-extended Immediate = -4094
    
    ----------
    
    Instruction 2
    opcode = 43
    ALUOp = 0
    RegDst = 0
    ALUSrc = 1
    RegWrite = 0
    MemRead = 0
    MemWrite = 1
    Branch = 0
    MemToReg = 0
    Sign-extended Immediate = -16333
    
    ----------
    
    Instruction 3
    opcode = 4
    ALUOp = 1
    RegDst = 0
    ALUSrc = 0
    RegWrite = 0
    MemRead = 0
    MemWrite = 0
    Branch = 1
    MemToReg = 0
    Sign-extended Immediate = -17148
    
    ----------


    */ 
		
}
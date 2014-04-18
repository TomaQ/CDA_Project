//Thomas Tavarez
//4/11/2014

#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{

    switch((int)ALUControl) //performs operations based on the ALUControl
    {
        case 000:
            *ALUresult = A + B;
            break;
		case 001:
			*ALUresult = A - B;
			break;
		case 010:
			if((int)A < (int)B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;
		case 011:
			if(A < B)
                *ALUresult = 1;
            else
                *ALUresult = 0;
			break;
		case 100:
			*ALUresult = A & B;
			break;
		case 101:
			*ALUresult = A | B;
			break;
		case 110:
			*ALUresult = B << 16;
			break;
		case 111:
			*ALUresult = ~A;
    }

    if(ALUresult == 0) //if the result was zero then set Zero to 1
        *Zero = 1;
    else
        *Zero = 0;

}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

    if(PC%4 == 0) //checks if the memory address is word aligned
    {
        *instruction = Mem[PC >> 2];

        if(*instruction < 8) //chekcs if the instruction is legal then return 0
            return 0;
        else
            return 1; //the instruction is illegal and the program must halt
    }

    return 1;//the address was not memory aligned

}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

    *op = (instruction & 0xfc000000) >> 26; //partitions the instruction bits and sets them to each variable
	*r1 = (instruction & 0x03e00000) >> 21;
	*r2 = (instruction & 0x001f0000) >> 16;
	*r3 = (instruction & 0x0000f800) >> 11;
	*funct = instruction & 0x0000003f;
	*offset = instruction & 0x0000ffff;
	*jsec = instruction & 0x03ffffff;

}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

    switch(op) //decodes the op code and sets the controls accordingly
    {
        case 0:
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->RegWrite = 1;
            controls->MemWrite = 0;
            controls->RegDst = 1;
            controls->ALUSrc = 0;
            controls->Branch = 0;
            controls->ALUOp = 7;
            controls->Jump = 0;
            break;
        case 8:
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->RegWrite = 1;
            controls->MemWrite = 0;
            controls->RegDst = 0;
            controls->ALUSrc = 1;
            controls->Branch = 0;
            controls->ALUOp = 0;
            controls->Jump = 0;
            break;
        case 15:
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->RegWrite = 1;
            controls->MemWrite = 0;
            controls->RegDst = 0;
            controls->ALUSrc = 1;
            controls->Branch = 0;
            controls->ALUOp = 6;
            controls->Jump = 0;
            break;
        case 35:
            controls->MemtoReg = 1;
            controls->MemRead = 1;
            controls->RegWrite = 1;
            controls->MemWrite = 0;
            controls->RegDst = 0;
            controls->ALUSrc = 1;
            controls->Branch = 0;
            controls->ALUOp = 0;
            controls->Jump = 0;
            break;
        case 43:
            controls->MemtoReg = 2;
            controls->MemRead = 0;
            controls->RegWrite = 0;
            controls->MemWrite = 1;
            controls->RegDst = 2;
            controls->ALUSrc = 1;
            controls->Branch = 0;
            controls->ALUOp = 0;
            controls->Jump = 0;
            break;
        case 4:
            controls->MemtoReg = 2;
            controls->MemRead = 0;
            controls->RegWrite = 0;
            controls->MemWrite = 0;
            controls->RegDst = 2;
            controls->ALUSrc = 0;
            controls->Branch = 1;
            controls->ALUOp = 1;
            controls->Jump = 0;
            break;
        case 10:
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->RegWrite = 1;
            controls->MemWrite = 0;
            controls->RegDst = 1;
            controls->ALUSrc = 0;
            controls->Branch = 0;
            controls->ALUOp = 2;
            controls->Jump = 0;
            break;
        case 11:
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->RegWrite = 1;
            controls->MemWrite = 0;
            controls->RegDst = 1;
            controls->ALUSrc = 0;
            controls->Branch = 0;
            controls->ALUOp = 3;
            controls->Jump = 0;
            break;
        case 2:
            controls->MemtoReg = 0;
            controls->MemRead = 0;
            controls->RegWrite = 0;
            controls->MemWrite = 0;
            controls->RegDst = 0;
            controls->ALUSrc = 0;
            controls->Branch = 0;
            controls->ALUOp = 0;
            controls->Jump = 1;
            break;
        default: //not a correct op code
            return 1;
    }

    return 0;

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{

    *data1 = Reg[r1]; //sets the data to the registers
    *data2 = Reg[r2];

}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

    if((offset >> 15) == 1) //if it is a negative number
        *extended_value = offset | 0xffff0000;

    else //should be a positive number
        *extended_value = offset & 0x0000ffff;

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

    if(ALUSrc == 1) //sees which data is being used
        data2 = extended_value;

    if(ALUOp == 7)
    {
        switch(funct) //sets the ALUOp based on the function
        {
            case 32:
                ALUOp = 0;
                break;
            case 34:
                ALUOp = 1;
                break;
            case 42:
                ALUOp = 2;
                break;
            case 43:
                ALUOp = 3;
                break;
            case 36:
                ALUOp = 4;
                break;
            case 37:
                ALUOp = 5;
                break;
            case 4:
                ALUOp = 6;
                break;
            case 39:
                ALUOp = 7;
                break;
            default: //not a correct function
                return 1;
        }

        ALU(data1, data2, ALUOp, ALUresult, Zero); //calls the ALU to perform the operations

    }

    else
        ALU(data1, data2, ALUOp, ALUresult, Zero); //doesnt change the ALUOp since the function doesnt matter

    return 0;

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

    if(MemRead == 1)//sees if we're reading from memory
    {
        if(ALUresult % 4 == 0) //checks to see if it's word aligned
            *memdata = Mem[ALUresult >> 2]; //sets the memory to the result
        else
            return 1; //not word aligned
    }

    if(MemWrite == 1) //sees if we're writing to memory
    {
        if(ALUresult % 4 == 0) //checks for word alignment
            Mem[ALUresult >> 2] = data2;
        else
            return 1; //not word aligned
    }

    return 0;

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

    if(RegWrite == 1) //checks to see if we're writing
    {
        if(MemtoReg == 1) //sees if we're putting what's in memory in the register
            Reg[r2] = memdata;
        else //sees which register to use
        {
            if(RegDst == 1)
                Reg[r3] = ALUresult;

            else
                Reg[r2] = ALUresult;
        }
    }

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

    *PC += 4; //updates the address

    if(Zero == 1 && Branch == 1) //if there's a branch
        *PC += extended_value << 2; //jumps to the branch

    if(Jump == 1) //if there's a jump
        *PC = (jsec << 2) | (*PC & 0xf0000000); //set the address to the jump

}


// Task - 1 Implement Instruction Memory and then pass it to the cpu
#include <stdio.h>
#include <inttypes.h>

// Definig Byte and Word
typedef uint8_t Byte;
typedef uint32_t Word;

// Memory Components
Byte Instruction_Memory[512];
Word Data_Memory[512];

// Defining Opcodes
#define I_type ((Byte)(0x13))
#define R_type ((Byte)(0x33))
#define B_type ((Byte)(0x63))
#define L_type ((Byte)(0x03))
#define S_type ((Byte)(0x23))
#define J_type ((Byte)(0x6F))
#define Lui ((Byte)(0x37))
#define Auipc ((Byte)(0x17))
#define Jalr ((Byte)(0x67))
// Getting Sign Extension - Sign Extension for U,J,B,I,S,L Instructions
Word Sign_Extend(Word Instruction, Byte Opcode)
{
    Word Immediate_Value;
    switch (Opcode)
    {
    case (Lui): // Instruction[31:12]
        Immediate_Value = Instruction >> 12;
        printf("U Type Instruction\n");
        break;
    case (Auipc):
        // Instruction[31:12]
        Immediate_Value = Instruction >> 12;
        printf("U Type Instruction\n");
        break;
    case J_type:
        // {Instruction[31],Instruction[19:12],Instruction[30:21],Instruction[20],1'b0} - JAL Instruction
        // Sign Extension is required from here
        Immediate_Value = (Instruction & 0x80000000) ? (0xFFE00000) | (Instruction & 0x80000000) >> 11 | (Instruction & 0x7FE00000) >> 20 | (Instruction & 0x00100000) >> 9 | (Instruction & 0x000FF000) : (Instruction & 0x80000000) >> 11 | (Instruction & 0x7FE00000) >> 20 | (Instruction & 0x00100000) >> 9 | (Instruction & 0x000FF000);
        printf("J Type Instruction\n");
        break;
    case B_type:
        // {Instruction[31],Instruction[7],Instruction[30:25],Instruction[11:8]} - BEQ BNE BLT BGE BLTU BGEU instruction
        Immediate_Value = (Instruction & 0x80000000) ? (0xFFE00000) | (Instruction & 0x80000000) >> 19 | (Instruction & 0x7E000000) >> 20 | (Instruction & 0x00000F00) >> 7 | (Instruction & 0x00000080) << 4 : (Instruction & 0x80000000) >> 19 | (Instruction & 0x7E000000) >> 20 | (Instruction & 0x00000F00) >> 7 | (Instruction & 0x00000080) << 4;
        printf("B Type Instruction\n");
        break;
    case I_type:
        // {Instruction[31:20]}
        Immediate_Value = (Instruction & 0x80000000) ? (0xFFE00000) | (Instruction) >> 20 : (Instruction) >> 20;
        printf("I Type Instruction\n");
        break;
    case Jalr :
        // {Instruction[31:20]}
        Immediate_Value = (Instruction & 0x80000000) ? (0xFFE00000) | (Instruction) >> 20 : (Instruction) >> 20;
        printf("J Type Instruction\n");
        break;
    case S_type:
        Immediate_Value = (Instruction & 0x80000000) ? (0xFFE00000) | (Instruction & 0xFE000000) >> 20 | (Instruction & 0xF80) >> 7 : (Instruction & 0xFE000000) >> 20 | (Instruction & 0xF80) >> 7;
        printf("S Type Instruction\n");
        break;
    case L_type:
        Immediate_Value = (Instruction & 0x80000000) ? (0xFFE00000) | (Instruction) >> 20 : (Instruction) >> 20;
        printf("L Type Instruction\n");
        break;
    default:
        printf("R Type Instruction\n");
        Immediate_Value = 0x0;
        break;
    }
    return Immediate_Value;
}

// CPU
Word CPU(Word Instruction, Word PC)
{
    // Instead of considering bitfields for Opcode funct3 and other Considering every variable as a byte
    Byte Opcode, funct3, funct7, rd, rs1, rs2, shamt;
    Word Immediate_Value,Temp;
    Word RegFile[32];
    RegFile[0] = 0;
    Opcode = ((Instruction & 0x0000007F) >> 0);  // Opcode = Instruction[6:0]
    rd = ((Instruction & 0x00000F80) >> 7);      // rd = Instruction[11:7]
    funct3 = ((Instruction & 0x00007000) >> 12); // funct3 = Instruction[14:12]
    rs1 = ((Instruction & 0x000F8000) >> 15);    // rs1 = Instruction[19:15]
    rs2 = ((Instruction & 0x01F00000) >> 20);    // rs2 = Instruction[24:20]
    shamt = rs2;                                 // Shifting in I Type Instructions
    funct7 = ((Instruction & 0xFE000000) >> 25); // funct7 = Instruction[31:25]
    printf("funct7 : %x rs2 : %x rs1 : %x funct3 : %x rd : %x Opcode : %x\n", funct7, rs2, rs1, funct3, rd, Opcode);
    Immediate_Value = Sign_Extend(Instruction, Opcode);

  
    switch (Opcode)
    {
    case R_type:
        switch(funct3)
        {
        case 0b000 :
                    if(funct7 == 0b0100000 )
                    {
                     RegFile[rd] = RegFile[rs2]+RegFile[rs1];
                     printf("ADD\n");
                    }
                    else if(funct7 == 0b0000000)
                    {
                     RegFile[rd] = RegFile[rs1]-RegFile[rs2];
                     printf("SUB\n");
                    }
                    break;
        case 0b001 : RegFile[rd] = RegFile[rs1] << (RegFile[rs2] & 0x1F);
                     printf("SLL\n");
                     break;
        case 0b010 : RegFile[rd] = ( (signed long)RegFile[rs1] < (signed long)RegFile[rs2] ) ? 1 : 0;
                     printf("SLT\n");
                     break;
        case 0b011 : RegFile[rd] = (RegFile[rs1]<RegFile[rs2]) ? 1 : 0;
                     printf("SLTU\n");
                     break;
        case 0b100 : RegFile[rd] = RegFile[rs1] ^ RegFile[rs2];
                     printf("XOR\n");
                     break;
        case 0b101 : 
                    if(funct7 == 0b0100000 )
                    {
                     RegFile[rd]=RegFile[rs1]; 
                     shamt=(RegFile[rs2] & 0x1F); 
                     Temp=RegFile[rs1]&0x80000000;
                     while (shamt>0)
                      { RegFile[rd]=(RegFile[rd]>>1)|Temp; 
                        shamt--; // This is for sign shifting
                      }
                      printf("SRAI\n");
                    }
                    else if(funct7 == 0b0000000)
                    {
                     RegFile[rd] = RegFile[rs1] >> (RegFile[rs2] & 0x1F);
                     printf("SRL\n");
                    }
                    else
                      printf("Invalid Instruction\n");
                    break; 
        case 0b110 : RegFile[rd] = RegFile[rs1] | RegFile[rs2];
                     printf("OR\n");
                     break;
        case 0b111 : RegFile[rd] = RegFile[rs1] & RegFile[rs2];
                     printf("AND\n"); 
                     break;
        default: printf("Invalid Instruction\n");
                 break;
        }
        break;
    case I_type :
        switch(funct3)
        {
        case 0b000 : RegFile[rd] = RegFile[rs1] + Immediate_Value;
                     printf("ADDI\n");
                     break;
        case 0b001 : if(funct7 == 0b0000000)
                     {
                     RegFile[rd] = RegFile[rs1] << (shamt);
                     printf("SLLI\n");
                     }
                     else
                     {
                     printf("Invalid instruction\n");
                     }
                     break;
        case 0b010 : RegFile[rd] = ( (signed long)RegFile[rs1] < (signed long)Immediate_Value ) ? 1 : 0;
                     printf("SLTI\n");
                     break;
        case 0b011 : RegFile[rd] = (RegFile[rs1]<Immediate_Value) ? 1 : 0;
                     printf("SLTIU\n");
                     break;
        case 0b100 : RegFile[rd] = RegFile[rs1] ^ Immediate_Value;
                     printf("XORI\n");
                     break;
        case 0b101 : 
                    if(funct7 == 0b0100000 )
                    {
                     RegFile[rd]=RegFile[rs1];  
                     Temp=RegFile[rs1]&0x80000000;
                     while (shamt>0)
                      { RegFile[rd]=(RegFile[rd]>>1)|Temp; 
                        shamt--; // This is for sign shifting
                      }
                      printf("SRAI\n");
                    }
                    else if(funct7 == 0b0000000)
                    {
                     RegFile[rd] = RegFile[rs1] >> (shamt);
                     printf("SRLI\n");
                    }
                    else
                      printf("Invalid Instruction\n");
                     break;
        case 0b110 : RegFile[rd] = RegFile[rs1] | Immediate_Value;
                     printf("ORI\n");
                     break;
        case 0b111 : RegFile[rd] = RegFile[rs1] & Immediate_Value;
                     printf("ANDI\n"); 
                     break;
        default: printf("Invalid Instruction\n");
                 break;
        }
        break;
    case Lui :
        RegFile[rd] = Immediate_Value << 12;
        printf("LUI\n");
        break;
    case Auipc :
        RegFile[rd] = (Immediate_Value << 12) + PC ;
        printf("AUIPC\n");
        break;
    case J_type :
        RegFile[rd] = PC + 0x4;
        PC = Immediate_Value + PC - 0x4;
        printf("JAL\n");
        break;
    case Jalr :
        RegFile[rd] = PC + 0x4;
        PC = ((Immediate_Value + RegFile[rs1]) & 0xFFFFFFFE ) - 0x4 ; // LSB needs to be cleared in JALR
        printf("JALR\n");
        break;
    case B_type :
        switch (funct3)
        {
        case 0b000 : //BEQ
                printf("BEQ\n");
                if(RegFile[rs1] == RegFile[rs2])
                {
                PC = Immediate_Value + PC - 0x4;
                printf("Executed\n");
                }
                else 
                  printf("Not Executed\n");      
            break;
        case 0b001 : //BNE
                printf("BNE\n");
                if(RegFile[rs1] != RegFile[rs2])
                {
                PC = Immediate_Value + PC - 0x4;
                printf("Executed\n");
                }
                else 
                  printf("Not Executed\n"); 
            break;
        case 0b100 : //BLT
                printf(" BLT \n");
                if ((signed long)RegFile[rs1]< (signed long)RegFile[rs2]) 
                {
                PC = (Immediate_Value + PC) - 0x4; 
                printf("Executed\n");
                }
                else
                    printf("Not Executed\n");
            break;
        case 0b101 : //BGE
                printf(" BGE \n");
                if ((signed long)RegFile[rs1] >= (signed long)RegFile[rs2]) 
                {
                PC = (Immediate_Value + PC) - 0x4; 
                printf("Executed\n");
                }
                else
                printf("Not Executed\n");
            
            break;
        case 0b110 : //BLTU
                printf(" BLTU \n");
                if (RegFile[rs1]< RegFile[rs2]) 
                {
                PC = (Immediate_Value + PC) - 0x4; 
                printf("Executed\n");
                }
                else
                    printf("Not Executed\n"); 
            break;
        case 0b111 : //BGTU
                printf(" BGTU \n");
                if (RegFile[rs1] >= RegFile[rs2]) 
                {
                PC = (Immediate_Value + PC) - 0x4; 
                printf("Executed\n");
                }
                else
                printf("Not Executed\n");
            break;
        default:
            printf("Invalid Insttruction\n");
            break;
        }
        break;
    case S_type :
        switch (funct3)
        {
        case 0b000 :
            printf("SB\n");
            Data_Memory[(Immediate_Value + RegFile[rs1])] = ( Data_Memory[(Immediate_Value + RegFile[rs1])] & 0xFFFFFF00 ) | (RegFile[rs2] & 0xFF) ;
            // Makes it store value in the last 8 Bits of Memory location without altering other values
            break;
        case 0b001 :
            printf("SH\n");
            Data_Memory[(Immediate_Value + RegFile[rs1])] = ( Data_Memory[(Immediate_Value + RegFile[rs1])] & 0xFFFF0000 ) | (RegFile[rs2] & 0xFFFF) ;
            // Makes it store value in the last 16 Bits of Memory location
            break;
        case 0b010 :
            printf("SW\n");
            Data_Memory[(Immediate_Value + RegFile[rs1])] = RegFile[rs2] ;
            // Makes it store value in the 32 Bits of Memory location
            break;
        default:
            printf("Invalid Instruction\n");
            break;
        }
        break;
    case L_type :
        switch (funct3)
        {
        case 0b000 :
            printf("LB\n");
            RegFile[rd] = ( Data_Memory[(Immediate_Value + RegFile[rs1])] & 0x80000000 ) ?// Checking Sign Bit
            (0xFFFFFF00 | ( Data_Memory[(Immediate_Value + RegFile[rs1])] & 0x000000FF )) // Performing Sign Extension 
            : ( Data_Memory[(Immediate_Value + RegFile[rs1])] & 0x000000FF )  ;
            // Makes it store value in the last 8 Bits of Memory location without altering other values
            break;
        case 0b001 :
            printf("LH\n");
            RegFile[rd] = ( Data_Memory[(Immediate_Value + RegFile[rs1])] & 0x80000000 ) ?
            (0xFFFF0000 | ( Data_Memory[(Immediate_Value + RegFile[rs1])] & 0x0000FFFF )) // Performing Sign Extension 
            : ( Data_Memory[(Immediate_Value + RegFile[rs1])] & 0x0000FFFF )  ;
              ;
            // Makes it store value in the last 16 Bits of Memory location
            break;
        case 0b010 :
            printf("LW\n");
            RegFile[rd] = Data_Memory[Immediate_Value + RegFile[rs1]] ;
            // Makes it store value in the last 32 Bits of Memory location
            break;
        case 0b100 :
            printf("LBU\n");
             RegFile[rd] = Data_Memory[(Immediate_Value)+RegFile[rs1]] & 0x000000FF; 
            // Makes it store value in the last 8 Bits of Memory location but unsigned 
            break;
        case 0b101 :
            printf("LHU\n");
            RegFile[rd] = Data_Memory[(Immediate_Value)+RegFile[rs1]] & 0x0000FFFF; 
            // Makes it store value in the last 16 Bits of Memory location but unsigned
            break;
        default:
            printf("Invalid Instruction\n");
            break;
        }
        break;
    default: printf("Invalid Instruction\n");
        break;
    }

    RegFile[0] = 0; // This is Hardwired Can't assign a constant to array element so makinf it zero after every operation
    return PC + 0x4;
}

int main()
{
    // Each cell of instruction Memory is 8 Bit

    Instruction_Memory[0] = 0x33;
    Instruction_Memory[1] = 0xA2;
    Instruction_Memory[2] = 0x41;
    Instruction_Memory[3] = 0x00;

    Instruction_Memory[4] = 0x93;
    Instruction_Memory[5] = 0x01;
    Instruction_Memory[6] = 0xC0;
    Instruction_Memory[7] = 0x00;

    Instruction_Memory[8] = 0x93;
    Instruction_Memory[9] = 0x83;
    Instruction_Memory[10] = 0x71;
    Instruction_Memory[11] = 0xFF;

    Instruction_Memory[12] = 0x33;
    Instruction_Memory[13] = 0xE2;
    Instruction_Memory[14] = 0x23;
    Instruction_Memory[15] = 0x00;

    Instruction_Memory[16] = 0xB3;
    Instruction_Memory[17] = 0x83;
    Instruction_Memory[18] = 0x42;
    Instruction_Memory[19] = 0x00;

    Instruction_Memory[20] = 0x63;
    Instruction_Memory[21] = 0x88;
    Instruction_Memory[22] = 0x72;
    Instruction_Memory[23] = 0x02;

    Instruction_Memory[24] = 0x33;
    Instruction_Memory[25] = 0xA2;
    Instruction_Memory[26] = 0x41;
    Instruction_Memory[27] = 0x00;

    Instruction_Memory[28] = 0x63;
    Instruction_Memory[29] = 0x04;
    Instruction_Memory[30] = 0x02;
    Instruction_Memory[31] = 0x00;

    Instruction_Memory[32] = 0x93;
    Instruction_Memory[33] = 0x02;
    Instruction_Memory[34] = 0x00;
    Instruction_Memory[35] = 0x00;

    Instruction_Memory[36] = 0x33;
    Instruction_Memory[37] = 0xA2;
    Instruction_Memory[38] = 0x23;
    Instruction_Memory[39] = 0x00;

    Instruction_Memory[40] = 0xB3;
    Instruction_Memory[41] = 0x03;
    Instruction_Memory[42] = 0x52;
    Instruction_Memory[43] = 0x00;

    Instruction_Memory[44] = 0xB3;
    Instruction_Memory[45] = 0x83;
    Instruction_Memory[46] = 0x23;
    Instruction_Memory[47] = 0x40;

    Instruction_Memory[48] = 0x23;
    Instruction_Memory[49] = 0xAA;
    Instruction_Memory[50] = 0x71;
    Instruction_Memory[51] = 0x04;

    Instruction_Memory[52] = 0x03;
    Instruction_Memory[53] = 0x21;
    Instruction_Memory[54] = 0x00;
    Instruction_Memory[55] = 0x06;

    Instruction_Memory[56] = 0xB3;
    Instruction_Memory[57] = 0x04;
    Instruction_Memory[58] = 0x51;
    Instruction_Memory[59] = 0x00;

    Instruction_Memory[60] = 0xEF;
    Instruction_Memory[61] = 0x01;
    Instruction_Memory[62] = 0x80;
    Instruction_Memory[63] = 0x00;

    Instruction_Memory[64] = 0x13;
    Instruction_Memory[65] = 0x01;
    Instruction_Memory[66] = 0x10;
    Instruction_Memory[67] = 0x00;

    Instruction_Memory[68] = 0x33;
    Instruction_Memory[69] = 0x01;
    Instruction_Memory[70] = 0x91;
    Instruction_Memory[71] = 0x00;

    Instruction_Memory[72] = 0x13;
    Instruction_Memory[73] = 0x02;
    Instruction_Memory[74] = 0x10;
    Instruction_Memory[75] = 0x00;

    Instruction_Memory[76] = 0xB7;
    Instruction_Memory[77] = 0x02;
    Instruction_Memory[78] = 0x00;
    Instruction_Memory[79] = 0x80;

    Instruction_Memory[80] = 0x33;
    Instruction_Memory[81] = 0xA3;
    Instruction_Memory[82] = 0x42;
    Instruction_Memory[83] = 0x00;

    Instruction_Memory[84] = 0x33;
    Instruction_Memory[85] = 0x06;
    Instruction_Memory[86] = 0x03;
    Instruction_Memory[87] = 0x00;

    Instruction_Memory[88] = 0xB7;
    Instruction_Memory[89] = 0xE4;
    Instruction_Memory[90] = 0xCD;
    Instruction_Memory[91] = 0xAB;

    Instruction_Memory[92] = 0xB7;
    Instruction_Memory[93] = 0xE4;
    Instruction_Memory[94] = 0xCD;
    Instruction_Memory[95] = 0xAB;

    Instruction_Memory[96] = 0x33;
    Instruction_Memory[97] = 0x01;
    Instruction_Memory[98] = 0x91;
    Instruction_Memory[99] = 0x00;

    Instruction_Memory[100] = 0x23;
    Instruction_Memory[101] = 0xA0;
    Instruction_Memory[102] = 0x21;
    Instruction_Memory[103] = 0x04;

    Instruction_Memory[104] = 0x63;
    Instruction_Memory[105] = 0x00;
    Instruction_Memory[106] = 0x21;
    Instruction_Memory[107] = 0x00;

    Word PC = 0x00000000;
    int Count = 30;
    // int Count = sizeof(Instruction_Memory)/4;
    // Just 2 instructions at present R[2]<-R[0] + 5 ; R[3]<-R[0] + C
    while (Count)
    {   
        printf("----------------------------------------------------------\n");
        printf("Execution Starts \n");
        printf("Program Counter : %x\n",PC);
        Word Instruction = (Instruction_Memory[PC + 3] << 24) | (Instruction_Memory[PC + 2]) << 16 | (Instruction_Memory[PC + 1]) << 8 | (Instruction_Memory[PC]);
        // Instruction is 32 bit long {Ins_Mem[3],Ins_Mem[2],Ins_Mem[1],Ins_Mem[0]} little endian
        printf("Instruction : %x\n", Instruction);
        PC = CPU(Instruction, PC);
        Count--;
        printf("Execution Ends \n");
    }
}

#ifndef DECODER_HPP
#define DECODER_HPP

#include<iostream>

enum opcode {
STALL,
ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,
JALR, LB, LH, LW, LBU, LHU, ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, 
SB, SH, SW,
LUI, AUIPC,
BEQ, BNE, BLT, BGE, BLTU, BGEU,
JAL
};

enum ins_types {
BUBBLE, R_TYPE, I_TYPE, S_TYPE, U_TYPE, B_TYPE, J_TYPE
};



class Decoder {
public:
    ins_types get_type[38] = {
    BUBBLE,
    R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, //10
    I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE,//15
    S_TYPE, S_TYPE, S_TYPE, //3
    U_TYPE, U_TYPE, //2
    B_TYPE, B_TYPE, B_TYPE, B_TYPE, B_TYPE, B_TYPE,//6 
    J_TYPE//1
    };
    opcode op = ADD;
    ins_types ins_type = R_TYPE;
    unsigned imm = 0, rd = 0, rs1 = 0, rs2 = 0, shamt = 0, func3 = 0, func7 = 0, opcode = 0;
    
    Decoder() : op(STALL), ins_type(BUBBLE) {}

    void decode(unsigned instruction) {
        func3 = (instruction & (1 << (14 + 1)) - 1) >> 12;
        func7 = (instruction & (1 << (31 + 1)) - 1) >> 25;
        opcode = (instruction & (1 << (6 + 1)) - 1) >> 0;

        switch (opcode)
        {
            case 0b0110111: op = LUI;
                break;
            case 0b0010111: op = AUIPC;
                break;
            case 0b1101111: op = JAL;
                break;
            case 0b1100111: op = JALR;
                break;
            case 0b1100011: {
                switch (func3)
                {
                    case 0b000: op = BEQ;
                        break;
                    case 0b001: op = BNE;
                        break;
                    case 0b100: op = BLT;
                        break;
                    case 0b101: op = BGE;
                        break;
                    case 0b110: op = BLTU;
                        break;
                    case 0b111: op = BGEU;
                        break;
                }
                break;
            }
            case 0b0000011: {
                switch (func3)
                {
                    case 0b000: op = LB;
                        break;
                    case 0b001: op = LH;
                        break;
                    case 0b010: op = LW;
                        break;
                    case 0b100: op = LBU;
                        break;
                    case 0b101: op = LHU;
                        break;
                }
                break;
            }
            case 0b0100011: {
                switch (func3)
                {
                    case 0b000: op = SB;
                        break;
                    case 0b001: op = SH;
                        break;
                    case 0b010: op = SW;
                        break;
                }
                break;
            }
            case 0b0010011: {
                switch (func3)
                {
                    case 0b000: op = ADDI;
                        break;
                    case 0b001: op = SLLI;
                        break;
                    case 0b100: op = XORI;
                        break;
                    case 0b111: op = ANDI;
                        break;
                    case 0b010: op = SLTI;
                        break;
                    case 0b011: op = SLTIU;
                        break;
                    case 0b110: op = ORI;
                        break;
                    case 0b101: {
                        if(func7 == 0)
                            op = SRLI;
                        else
                            op = SRAI;
                        break;
                    }
                }
                break;
            }
            case 0b0110011: {
                switch (func3)
                {
                    case 0b000: {
                        if(func7 == 0)
                            op = ADD;
                        else
                            op = SUB;
                        break;
                    }
                    case 0b001: op = SLL;
                        break;
                    case 0b100: op = XOR;
                        break;
                    case 0b101: {
                        if(func7 == 0)
                            op = SRL;
                        else
                            op = SRA;
                        break;
                    }
                    case 0b110: op = OR;
                        break;
                    case 0b111: op = AND;
                        break;
                    case 0b010: op = SLT;
                        break;
                    case 0b011: op = SLTU;
                        break;
                }
                break;
            }
            default:
                op = STALL;
                break; 
        }
        
        ins_type = get_type[op];
        if(ins_type != J_TYPE && ins_type != U_TYPE)
            rs1 = (instruction & (1 << (19 + 1)) - 1) >> 15;  
        if(ins_type == R_TYPE || ins_type == B_TYPE || ins_type == S_TYPE)
            rs2 = (instruction & (1 << (24 + 1)) - 1) >> 20;
        if(ins_type != S_TYPE && ins_type != B_TYPE)
            rd = (instruction & (1 << (11 + 1)) - 1) >> 7;  
        
        switch(ins_type) 
        {
            case R_TYPE: imm = 0;
                break;
            case I_TYPE: {imm = (instruction & (1 << (31 + 1)) - 1) >> 20; if (imm & (1 << 11)) imm = imm | 0xfffff000;break;}
            case S_TYPE: {imm = ((instruction & (1 << (31 + 1)) - 1) >> 25) << 5 | ((instruction & (1 << (11 + 1)) - 1) >> 7); if (imm & (1 << 11)) imm = imm | 0xfffff000;break;}
            case B_TYPE: {imm = (instruction >> 31) << 12 | ((instruction & (1 << (30 + 1)) - 1) >> 25) << 5 | ((instruction & (1 << (11 + 1)) - 1) >> 8) << 1 | ((instruction & (1 << (7 + 1)) - 1) >> 7) << 11; if (imm & (1 << 12)) imm = imm | 0xfffff000;break;}
            case U_TYPE: imm = (instruction & (1 << (31 + 1)) - 1) >> 12 << 12;
                break;
            case J_TYPE: {imm = (instruction >> 31) << 20 | ((instruction & (1 << (30 + 1)) - 1) >> 21) << 1 | ((instruction & (1 << (20 + 1)) - 1) >> 20) << 11 | ((instruction & (1 << (19 + 1)) - 1) >> 12) << 12; if (imm & (1 << 20)) imm = imm | 0xfff00000;break;}
            default:
                imm = 0;
                break;
        }

        if(op == SLLI || SRLI || SRAI)
            shamt = (instruction & (1 << (24 + 1)) - 1) >> 20;

    }

};

#endif



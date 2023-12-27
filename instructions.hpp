#ifndef INSTRUCTIONS_HPP
#define INSTRUCTIONS_HPP

#include <cstdint>

enum opcode {
SLLI, SRLI, SRAI, ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND,
JALR, LB, LH, LW, LBU, LHU, ADDI, SLTI, SLTIU, XORI, ORI, ANDI,
SB, SH, SW,
LUI, AUIPC,
BEQ, BNE, BLT, BGE, BLTU, BGEU,
JAL
};

enum ins_types {
R_TYPE, I_TYPE, S_TYPE, U_TYPE, B_TYPE, J_TYPE
};

ins_types get_type[37] = {
R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, R_TYPE, 
I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, I_TYPE, 
S_TYPE, S_TYPE, S_TYPE, 
U_TYPE, U_TYPE, 
B_TYPE, B_TYPE, B_TYPE, B_TYPE, B_TYPE, B_TYPE, 
J_TYPE
};

class Instruction {
public:
    opcode op;
    ins_types ins_type;
    uint32_t imm = 0, rd = 0, rs1 = 0, rs2 = 0, shamt = 0, func3 = 0, func7 = 0, opcode = 0;

    void init() {
        imm = 0, rd = 0, rs1 = 0, rs2 = 0, shamt = 0, func3 = 0, func7 = 0, opcode = 0;
        op = SLLI;
        ins_type = R_TYPE;
    }

    void init(uint32_t instruction) {
        func3 = (instruction & (1 << (14 + 1)) - 1) >> 12;
        func7 = (instruction & (1 << (31 + 1)) - 1) >> 25;
        opcode = (instruction & (1 << (6 + 1)) - 1) >> 0;

        switch (opcode)
        {
            case 0b0110111: op = LUI;
            case 0b0010111: op = AUIPC;
            case 0b1101111: op = JAL;
            case 0b1100111: op = JALR;
            case 0b1100011: {
                switch (func3)
                {
                    case 000: op = BEQ;
                    case 001: op = BNE;
                    case 100: op = BLT;
                    case 101: op = BGE;
                    case 110: op = BLTU;
                    case 111: op = BGEU;
                }
            }
            case 0b0000011: {
                switch (func3)
                {
                    case 000: op = LB;
                    case 001: op = LH;
                    case 010: op = LW;
                    case 100: op = LBU;
                    case 101: op = LHU;
                }
            }
            case 0b0100011: {
                switch (func3)
                {
                    case 000: op = SB;
                    case 001: op = SH;
                    case 010: op = SW;
                }
            }
            case 0b0010011: {
                switch (func3)
                {
                    case 000: op = ADDI;
                    case 001: op = SLLI;
                    case 100: op = XORI;
                    case 111: op = ANDI;
                    case 010: op = SLTI;
                    case 011: op = SLTIU;
                    case 110: op = ORI;
                    case 101: {
                        if(func7 == 0)
                            op = SRLI;
                        else
                            op = SRAI;
                    }
                }
            }
            case 0b0110011: {
                switch (func3)
                {
                    case 000: {
                        if(func7 == 0)
                            op = ADD;
                        else
                            op = SUB;
                    }
                    case 001: op = SLL;
                    case 100: op = XOR;
                    case 101: {
                        if(func7 == 0)
                            op = SRL;
                        else
                            op = SRA;
                    }
                    case 110: op = OR;
                    case 111: op = AND;
                    case 010: op = SLT;
                    case 011: op = SLTU;
                }
            } 
        }
        
        ins_type = get_type[op];
        if(ins_type != J_TYPE && ins_type != U_TYPE)
            rs1 = (instruction & (1 << (19 + 1)) - 1) >> 15;  
        if(ins_type == R_TYPE || ins_type == B_TYPE || ins_type == S_TYPE)
            rs2 = (instruction & (1 << (24 + 1)) - 1) >> 20;
        if(ins_type != S_TYPE && ins_type != B_TYPE)
            rd = (instruction & (1 << (11 + 1)) - 1) >> 7;  
    }

    void init_imm() {

    }
    
};

#endif



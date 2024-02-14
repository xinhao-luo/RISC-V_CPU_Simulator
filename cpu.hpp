#ifndef CPU_HPP
#define CPU_HPP

#include <fstream>
#include <bitset>
#include "decoder.hpp"
#include "memory.hpp"
#include "register.hpp"



class CPU {
    Memory mem;
    Register reg;
    Decoder decoder;
public:
    void read_init(std::string filename) {
        std::ifstream file(filename);
        std::string line;
        unsigned address;
        unsigned value;

        while (std::getline(file, line)) {
            if (line[0] == '@') {
                address = std::stoi(line.substr(1), nullptr, 16);
            } else {
                for (int i = 0; i < line.size(); i+=3){
                    std::string data = line.substr(i, 2);
                    // 将指令读入memory
                    // std::cout<< address <<std::endl;
                    mem.writeins(address, data);
                    address++;
                }
            }
        }
    }

    void run() {
        while(true) {
            unsigned pc = reg.readpc();
            unsigned instruction = mem.getins(pc);
            // std::cout << "ins:" << instruction <<"  ";
            if(instruction == 0x0ff00513){
                unsigned output = reg.read(10) & 255u;
                std::cout << "ans: "<<output << std::endl;
                break;
            }
            decoder.decode(instruction);
            // std::cout << "op:" << decoder.op <<std::endl;
            // std::cout << "opcode:" << decoder.opcode <<std::endl;
            switch(decoder.op) {
                case LB : lb(decoder);
                    continue;
                case LH : lh(decoder);
                    continue;
                case LW : lw(decoder);
                    continue;
                case LBU : lbu(decoder);
                    continue;
                case LHU : lhu(decoder);
                    continue;
                case SB : sb(decoder);
                    continue;
                case SH : sh(decoder);
                    continue;
                case SW : sw(decoder);
                    continue;
                case ADD : add(decoder);
                    continue;
                case ADDI : addi(decoder);
                    continue;
                case SUB : sub(decoder);
                    continue;
                case LUI : lui(decoder);
                    continue;
                case AUIPC : auipc(decoder);
                    continue;
                case XOR : xor_(decoder);
                    continue;
                case XORI : xori(decoder);
                    continue;
                case OR : or_(decoder);
                    continue;
                case ORI : ori(decoder);
                    continue;
                case AND : and_(decoder);
                    continue;
                case ANDI : andi(decoder);
                    continue;
                case SLL : sll(decoder);
                    continue;
                case SLLI : slli(decoder);
                    continue;
                case SRL : srl(decoder);
                    continue;
                case SRLI : srli(decoder);
                    continue;
                case SRA : sra(decoder);
                    continue;
                case SRAI : srai(decoder);
                    continue;
                case SLT : slt(decoder);
                    continue;
                case SLTI : slti(decoder); 
                    continue;
                case SLTU : sltu(decoder);
                    continue;
                case SLTIU : sltiu(decoder);
                    continue;
                case BEQ : beq(decoder);
                    continue;
                case BNE : bne(decoder);
                    continue;
                case BLT : blt(decoder);
                    continue;
                case BGE : bge(decoder);
                    continue;
                case BLTU : bltu(decoder);
                    continue;
                case BGEU : bgeu(decoder);
                    continue;
                case JAL : jal(decoder);
                    continue;
                case JALR : jalr(decoder);
                    continue;
            }
        }
    }

    void lb(Decoder decoder) {
        unsigned address = (signed) decoder.imm + reg.read(decoder.rs1);
        unsigned rd_write = mem.read(address);
        rd_write = (rd_write & (1 << (7 + 1)) - 1); if (rd_write & (1 << 7)) rd_write = rd_write | 0xffffff00;
        // std::cout<<"lb"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void lh(Decoder decoder) {
        unsigned address1 = (signed) decoder.imm + reg.read(decoder.rs1);
        unsigned address2 = address1 + 1;
        unsigned rd_write1 = mem.read(address1);
        unsigned rd_write2 = mem.read(address2);
        unsigned rd_write3 = (rd_write2 << 8) + rd_write1;
        rd_write3 = (rd_write3 & (1 << (15 + 1)) - 1); if (rd_write3 & (1 << 15)) rd_write3 = rd_write3 | 0xffff0000;
        // std::cout<<"lh"<<std::endl;
        reg.write(decoder.rd, rd_write3);
        reg.updatepc(reg.readpc() + 4);
    }

    void lw(Decoder decoder) {
        unsigned address1 = (signed) decoder.imm + reg.read(decoder.rs1);
        unsigned address2 = address1 + 1;
        unsigned address3 = address1 + 2;
        unsigned address4 = address1 + 3;
        unsigned rd_write1 = mem.read(address1);
        unsigned rd_write2 = mem.read(address2);
        unsigned rd_write3 = mem.read(address3);
        unsigned rd_write4 = mem.read(address4);
        unsigned rd_write5 = (rd_write4 << 24) + (rd_write3 << 16) + (rd_write2 << 8) + rd_write1;
        // std::cout<<"lw"<<std::endl;
        reg.write(decoder.rd, rd_write5);
        reg.updatepc(reg.readpc() + 4);
    }

    void lbu(Decoder decoder) {
        unsigned address = decoder.imm + reg.read(decoder.rs1);
        unsigned rd_write = mem.read(address);
        // rd_write = (rd_write & (1 << (7 + 1)) - 1); 
        // std::cout<<"lbu"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void lhu(Decoder decoder) {
        unsigned address1 = decoder.imm + reg.read(decoder.rs1);
        unsigned address2 = address1 + 1;
        unsigned rd_write1 = mem.read(address1);
        unsigned rd_write2 = mem.read(address2);
        unsigned rd_write3 = (rd_write2 << 8) + rd_write1;
        // rd_write3 = (rd_write3 & (1 << (15 + 1)) - 1);
        // std::cout<<"lhu"<<std::endl;
        reg.write(decoder.rd, rd_write3);
        reg.updatepc(reg.readpc() + 4);
    }

    void sb(Decoder decoder) {
        // std::cout<<"sb"<<std::endl;
        unsigned char mem_write = reg.read(decoder.rs2);
        unsigned mem_address = reg.read(decoder.rs1) + decoder.imm;
        mem.write(mem_address, mem_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void sh(Decoder decoder) {
        // std::cout<<"sh"<<std::endl;
        unsigned char mem_write1 = reg.read(decoder.rs2);
        unsigned char mem_write2 = reg.read(decoder.rs2) >> 8;
        // mem_write = (mem_write & (1 << (7 + 1)) - 1); 
        // std::cout<<"imm: "<<decoder.imm<<std::endl;
        unsigned mem_address1 = reg.read(decoder.rs1) + decoder.imm;
        unsigned mem_address2 = reg.read(decoder.rs1) + decoder.imm + 1;

        // std::cout<<"address: "<<reg.read(decoder.rs1)<<std::endl;
        mem.write(mem_address1, mem_write1);
        mem.write(mem_address2, mem_write2);
        reg.updatepc(reg.readpc() + 4);
    }

    void sw(Decoder decoder) {
        // std::cout<<"sw"<<std::endl;
        unsigned char mem_write1 = reg.read(decoder.rs2);
        unsigned char mem_write2 = reg.read(decoder.rs2) >> 8;
        unsigned char mem_write3 = reg.read(decoder.rs2) >> 16;
        unsigned char mem_write4 = reg.read(decoder.rs2) >> 24;
        // mem_write = (mem_write & (1 << (7 + 1)) - 1); 
        // std::cout<<"imm: "<<decoder.imm<<std::endl;
        unsigned mem_address1 = reg.read(decoder.rs1) + decoder.imm;
        unsigned mem_address2 = reg.read(decoder.rs1) + decoder.imm + 1;
        unsigned mem_address3 = reg.read(decoder.rs1) + decoder.imm + 2;
        unsigned mem_address4 = reg.read(decoder.rs1) + decoder.imm + 3;
        // std::cout<<"address: "<<reg.read(decoder.rs1)<<std::endl;
        mem.write(mem_address1, mem_write1);
        mem.write(mem_address2, mem_write2);
        mem.write(mem_address3, mem_write3);
        mem.write(mem_address4, mem_write4);
        reg.updatepc(reg.readpc() + 4);
    }

    void add(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) + reg.read(decoder.rs2);
        // std::cout<<"add"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void addi(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) + (signed)decoder.imm;
        // std::cout<<"addi"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void sub(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) - reg.read(decoder.rs2);
        // std::cout<<"sub"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void lui(Decoder decoder) {
        unsigned rd_write = (decoder.imm & (1 << (31 + 1)) - 1) >> 12 << 12;
        // std::cout<<"lui"<<std::endl; 
        // std::cout<<"imm: "<<decoder.imm<<std::endl; 
        // std::cout<<"rd: "<<decoder.rd<<std::endl; 
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void auipc(Decoder decoder) {
        unsigned rd_write = (decoder.imm & (1 << (31 + 1)) - 1) >> 12 << 12 + reg.readpc(); 
        // std::cout<<"auipc"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void xor_(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) ^ reg.read(decoder.rs2);
        // std::cout<<"xor"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void xori(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) ^ decoder.imm;
        // std::cout<<"xori"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void or_(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) | reg.read(decoder.rs2);
        // std::cout<<"or"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void ori(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) | decoder.imm;
        // std::cout<<"ori"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void and_(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) & reg.read(decoder.rs2);
        // std::cout<<"and"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void andi(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) & decoder.imm;
        // std::cout<<"andi"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void sll(Decoder decoder) {
        unsigned rs2_read = reg.read(decoder.rs2) & (1 << (4 + 1)) - 1;
        unsigned rd_write = reg.read(decoder.rs1) << rs2_read;
        // std::cout<<"sll"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void slli(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) << decoder.shamt;
        // std::cout<<"slli"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void srl(Decoder decoder) {
        unsigned rs2_read = reg.read(decoder.rs2) & (1 << (4 + 1)) - 1;
        unsigned rd_write = reg.read(decoder.rs1) >> rs2_read;
        // std::cout<<"srl"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void srli(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) >> decoder.shamt;
        // std::cout<<"srli"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void sra(Decoder decoder) {
        unsigned rs2_read = reg.read(decoder.rs2) & (1 << (4 + 1)) - 1;
        unsigned rd_write = reg.read(decoder.rs1) >> rs2_read;
        unsigned top = reg.read(decoder.rs1) >> 31;
        unsigned sext = -(1 << (31 - rs2_read));
        if(top)
           rd_write = rd_write | sext;
        // std::cout<<"sra"<<std::endl; 
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void srai(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) >> decoder.shamt;
        unsigned top = reg.read(decoder.rs1) >> 31;
        unsigned sext = -(1 << (31 - decoder.shamt));
        if(top)
           rd_write = rd_write | sext;
        // std::cout<<"srai"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void slt(Decoder decoder) {
        unsigned rd_write = (int32_t)reg.read(decoder.rs1) < (int32_t)reg.read(decoder.rs2);
        // std::cout<<"slt"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void slti(Decoder decoder) {
        unsigned rd_write = (signed) reg.read(decoder.rs1) < (signed) decoder.imm;
        // std::cout<<"slti"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void sltu(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) < reg.read(decoder.rs2);
        // std::cout<<"sltu"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void sltiu(Decoder decoder) {
        unsigned rd_write = reg.read(decoder.rs1) < decoder.imm;
        // std::cout<<"sltiu"<<std::endl;
        reg.write(decoder.rd, rd_write);
        reg.updatepc(reg.readpc() + 4);
    }

    void beq(Decoder decoder) {
        // std::cout<<"beq"<<std::endl;
        if(reg.read(decoder.rs1) == reg.read(decoder.rs2))
            reg.updatepc(reg.readpc() + decoder.imm);
        else
            reg.updatepc(reg.readpc() + 4); 
    }

    void bne(Decoder decoder) {
        // std::cout<<"bne"<<std::endl;
        if(reg.read(decoder.rs1) != reg.read(decoder.rs2))
            reg.updatepc(reg.readpc() + decoder.imm);
        else
            reg.updatepc(reg.readpc() + 4); 
    }

    void blt(Decoder decoder) {
        // std::cout<<"blt"<<std::endl;
        if((signed) reg.read(decoder.rs1) < (signed) reg.read(decoder.rs2))
            reg.updatepc(reg.readpc() + decoder.imm);
        else
            reg.updatepc(reg.readpc() + 4); 
    }

    void bge(Decoder decoder) {
        // std::cout<<"bge"<<std::endl;
        if((signed) reg.read(decoder.rs1) >= (signed) reg.read(decoder.rs2))
            reg.updatepc(reg.readpc() + decoder.imm);
        else
            reg.updatepc(reg.readpc() + 4); 
    }

    void bltu(Decoder decoder) {
        // std::cout<<"bltu"<<std::endl;
        if(reg.read(decoder.rs1) < reg.read(decoder.rs2))
            reg.updatepc(reg.readpc() + decoder.imm);
        else
            reg.updatepc(reg.readpc() + 4); 
    }

    void bgeu(Decoder decoder) {
        // std::cout<<"bgeu"<<std::endl;
        // std::cout<<"1: "<<reg.read(decoder.rs1)<<" "<<decoder.rs2<<std::endl;
        if(reg.read(decoder.rs1) >= reg.read(decoder.rs2))
            reg.updatepc(reg.readpc() + decoder.imm);
        else
            reg.updatepc(reg.readpc() + 4); 
        // std::cout<<"pc:"<<reg.readpc()<<std::endl;
    }

    void jal(Decoder decoder) {
        // std::cout<<"jal"<<std::endl;
        reg.write(decoder.rd, reg.readpc() + 4);
        reg.updatepc(reg.readpc() + decoder.imm);
    }

    void jalr(Decoder decoder) {
        // std::cout<<"jalr"<<std::endl;
        // std::cout<<reg.readpc()<<std::endl;
        reg.write(decoder.rd, reg.readpc() + 4);
        // std::cout<<decoder.rs1<<"  "<<decoder.imm<<std::endl;
        reg.updatepc((reg.read(decoder.rs1) + decoder.imm) & (~(unsigned) 1));
        // std::cout<<reg.readpc()<<std::endl;
    }
};

#endif
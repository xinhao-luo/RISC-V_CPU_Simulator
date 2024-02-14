#include "pipeline.h"
#include "cpu.hpp"
#include <iostream>

void IF::run() {
    // std::cout<<"IF run"<<std::endl;
    buffer.pc = cpu->pc;
    buffer.ins = cpu->mem->getins(buffer.pc);
    cpu->Predict_->predict(buffer.pc, buffer.ins);
    buffer.pred_pc = cpu->Predict_->get();
    // std::cout<<buffer.ins<<" "<<buffer.pred_pc<<std::endl;
}

void ID::run() {
    // std::cout<<"ID run"<<std::endl;
    cpu->decoder->decode(buffer_.ins);
    buffer.ins = buffer_.ins;
    buffer.pc = buffer_.pc;
    buffer.imm = cpu->decoder->imm;
    buffer.instype = cpu->decoder->op;
    buffer.pred_pc = buffer_.pred_pc;
    // std::cout<<"ID: "<<buffer_.ins<<std::endl;
    // std::cout<<"ID debug ins: "<<buffer.ins<<std::endl;
    if(buffer_.ins == 0) {
        // std::cout<<"ID return"<<std::endl;
        return;
    }
    switch(cpu->decoder->ins_type) {
        case R_TYPE:
        case I_TYPE:
        case U_TYPE:
        case J_TYPE:
            buffer.rd = cpu->decoder->rd;
            break;
        default:
            buffer.rd = -99;
            break;
    }

    switch(cpu->decoder->ins_type) {
        case R_TYPE:
        case I_TYPE:
        case S_TYPE:
        case B_TYPE:
            buffer.rs1 = cpu->decoder->rs1;
            buffer.rv1 = cpu->reg->read(buffer.rs1);
            break;
        default:
            buffer.rs1 = -99;
            buffer.rv1 = -99;
            break;
    }

    switch(cpu->decoder->ins_type) {
        case R_TYPE:
        case S_TYPE:
        case B_TYPE:
            buffer.rs2 = cpu->decoder->rs2;
            buffer.rv2 = cpu->reg->read(buffer.rs2);
            break;
        default:
            buffer.rs2 = -99;
            buffer.rv2 = -99;
            break;
    }

    forward();

    // std::cout<<"after ID pc: "<<buffer.pc<<std::endl;
}

void ID::forward() {
    if(cpu->WB_->buffer_.rd == buffer.rs1 && buffer.rs1)
        buffer.rv1 = cpu->WB_->buffer_.output;
    if(cpu->WB_->buffer_.rd == buffer.rs2 && buffer.rs2)
        buffer.rv2 = cpu->WB_->buffer_.output;
}

void EX::run() {
    // std::cout<<"EX run"<<std::endl;
    // std::cout<<"EX debug pc: "<<buffer_.pc<<std::endl;
    // std::cout<<"EX debug ins: "<<buffer_.ins<<std::endl;
    buffer.ins = buffer_.ins;
    buffer.instype = buffer_.instype;
    buffer.pc = buffer_.pc;
    buffer.rd = buffer_.rd;
    buffer.newpc = buffer_.pc + 4;
    buffer.output = 0;
    buffer.rv2 = buffer_.rv2;
    unsigned rv1 = buffer_.rv1;
    unsigned rv2 = buffer_.rv2;
    unsigned imm = buffer_.imm;
    bool ifbranch = false;

    
    if(buffer_.instype == STALL){
        // std::cout<<"EX: ins: "<<buffer_.ins<<std::endl;
        // std::cout<<"ex return: "<<std::endl;
        cpu->wrong_predict = false;
        return;
    }
    // std::cout<<"EX: not return ins: "<<buffer_.ins<<std::endl;
    // std::cout<<"EX not return"<<std::endl;
    // std::cout<<"EX debug1: "<<cpu->final_buffer.rd<<" "<<buffer_.rs1<<std::endl;
    // std::cout<<"EX debug1: "<<cpu->WB_->buffer_.rd<<" "<<buffer_.rs1<<std::endl;
    // std::cout<<"EX debug1: "<<cpu->MEM_->buffer_.rd<<" "<<buffer_.rs1<<std::endl;
    
    // opcode op2 = cpu->MEM_->buffer_.instype;
    // bool flag = false;
    // if(op2 == LB || op2 == LH || op2 == LW || op2 == LBU || op2 == LHU)
    //     flag = true;
    // bool flag1 = (cpu->MEM_->buffer_.rd == buffer_.rs1 && buffer_.rs1);
    // std::cout<<"EX debug1: "<<flag<<"  "<<flag1<<"  "<<cpu->MEM_->buffer_.instype<<std::endl;
    if (cpu->final_buffer.rd == buffer_.rs1 && buffer_.rs1)
        rv1 = cpu->final_buffer.output;
    if (cpu->WB_->buffer_.rd == buffer_.rs1 && buffer_.rs1)
        rv1 = cpu->WB_->buffer_.output;
    if(cpu->stall_)
        cpu->stall_ = false;
    if(cpu->MEM_->buffer_.rd == buffer_.rs1 && buffer_.rs1){
        opcode op = cpu->MEM_->buffer_.instype;
        // std::cout<<"EX debug2: "<<op<<std::endl;
        if(op == LB || op == LH || op == LW || op == LBU || op == LHU){
            cpu->stall_ = true;
            return;
        } else {
            rv1 = cpu->MEM_->buffer_.output;
        }
    }
    
    if (cpu->final_buffer.rd == buffer_.rs2 && buffer_.rs2)
        rv2 = cpu->final_buffer.output;
    if (cpu->WB_->buffer_.rd == buffer_.rs2 && buffer_.rs2)
        rv2 = cpu->WB_->buffer_.output;
    if(cpu->stall_)
        cpu->stall_ = false;
    if(cpu->MEM_->buffer_.rd == buffer_.rs2 && buffer_.rs2){
        opcode op = cpu->MEM_->buffer_.instype;
        if(op == LB || op == LH || op == LW || op == LBU || op == LHU){
            cpu->stall_ = true;
            return;
        } else {
            rv2 = cpu->MEM_->buffer_.output;
        }
    }

    buffer.rv2 = rv2;
    unsigned rv2_;
    unsigned output_;
    unsigned top;
    unsigned sext;
    // std::cout<<"EX debug2: "<<buffer_.instype<<std::endl;
    
    switch(buffer_.instype) {
        case LB: 
        case LH: 
        case LW:
        case LBU:
        case LHU:
            // std::cout<<"LH: "<<imm<<"  "<<rv1<<std::endl;
            buffer.output = imm + rv1;
            break;
        case SB:
        case SH:
        case SW:
        case ADDI:
            buffer.output = imm + rv1;
            break;
        case ADD:
            buffer.output = rv1 + rv2;
            break;
        case SUB:
            buffer.output = rv1 - rv2;
            break;
        case LUI:
            buffer.output = (imm & (1 << (31 + 1)) - 1) >> 12 << 12;
            break;
        case AUIPC:
            buffer.output = (imm & (1 << (31 + 1)) - 1) >> 12 << 12 + buffer.pc;
            break;
        case XOR:
            buffer.output = rv1 ^ rv2;
            break;
        case OR:
            buffer.output = rv1 | rv2;
            break;
        case XORI:
            buffer.output = rv1 ^ imm;
            break;
        case ORI:
            buffer.output = rv1 | imm;
            break;
        case AND:
            buffer.output = rv1 & rv2;
            break;
        case ANDI:
            buffer.output = rv1 & imm;
            break;
        case SLL:
            rv2_ = rv2 & (1 << (4 + 1)) - 1; 
            buffer.output = rv1 << rv2_;
            break;
        case SLLI:
            // std::cout<<"EX slli: "<< rv1<<" "<<((imm & 0x00000fff))<<std::endl;
            buffer.output = rv1 << (imm & 0x00000fff);
            break;
        case SRL:
            rv2_ = rv2 & (1 << (4 + 1)) - 1; 
            buffer.output = rv1 >> rv2_;
            break;
        case SRLI:
            // std::cout<<"SRLI: "<<rv1<<"  "<<(imm & 0x0000001f)<<std::endl;
            buffer.output = rv1 >> (imm & 0x0000001f);
            break;
        case SRA:
            rv2_ = rv2 & (1 << (4 + 1)) - 1;
            output_ = rv1 >> rv2_;
            top = rv1 >> 31;
            sext = -(1 << (31 - rv2_));
            if(top)
                output_ = output_ | sext;
            buffer.output = output_;
            break;
        case SRAI:
            output_ = (signed)rv1 >> (imm & 0x0000001f);
            // output_ = rv1 >> cpu->decoder->shamt;
            // top = rv1 >> 31;
            // sext = -(1 << (31 - cpu->decoder->shamt));
            // if(top)
            //     output_ = output_ | sext;
            buffer.output = output_;
            break;
        case SLT:
            buffer.output = (signed) rv1 < (signed) rv2;
            break;
        case SLTI:
            buffer.output = (signed) rv1 < (signed) imm;
            break; 
        case SLTU:
            buffer.output = rv1 < rv2;
            break;
        case SLTIU:
            buffer.output = (unsigned)rv1 < (unsigned)imm;
            break;
        case BEQ:
            if(rv1 == rv2){
                ifbranch = true;
                buffer.newpc = buffer_.pc + imm;
            }
            break;
        case BNE:
            // std::cout<<"BNE: "<<buffer_.pc<<"  "<<imm<<"  "<<rv1<<"  "<<rv2<<"  "<<buffer_.rs1<<"  "<<buffer_.rs2<<std::endl;
            
            if(rv1 != rv2){
                ifbranch = true;
                buffer.newpc = buffer_.pc + imm;
            }
            break;
        case BLT:
            // std::cout<<"EX debug predict2: "<<buffer_.pc<<"  "<<imm<<"  "<<(signed)rv1<<"  "<<(signed)rv2<<"  "<<buffer_.rs1<<"  "<<buffer_.rs2<<std::endl;
            
            if((signed)rv1 < (signed)rv2){
                ifbranch = true;
                buffer.newpc = buffer_.pc + imm;
            }
            break;
        case BGE:
            if((signed)rv1 >= (signed)rv2){
                ifbranch = true;
                buffer.newpc = buffer_.pc + imm;
            }
            break;
        case BLTU:
            // std::cout<<"BLTU: "<<(unsigned)rv1<<" "<<(unsigned)rv2<<" "<<buffer_.rs1<<"  "<<buffer_.rs2<<std::endl;
            if((unsigned)rv1 < (unsigned)rv2){
                ifbranch = true;
                buffer.newpc = buffer_.pc + imm;
            }
            break;
        case BGEU:
            if((unsigned)rv1 >= (unsigned)rv2){
                ifbranch = true;
                buffer.newpc = buffer_.pc + imm;
            }
            break;
        case JAL:
            buffer.output = buffer_.pc + 4;
            buffer.newpc = buffer_.pc + imm;
            break;
        case JALR:
            buffer.output = buffer_.pc + 4;
            buffer.newpc = (rv1 + imm) & (~(unsigned) 1);
            break;
        default:
            break;
    }  
    // std::cout<<"EX debug predict: "<<buffer.newpc<<"  "<<buffer.output<<std::endl;
    cpu->wrong_predict = (buffer_.pred_pc != buffer.newpc);
    if((buffer_.ins & 0x7fu) == 0b1100011u)
        cpu->Predict_->run(buffer_.pc, ifbranch, !cpu->wrong_predict, buffer.newpc);
}

void MEM::run() {
    // std::cout<<"MEM run"<<std::endl;

    buffer.ins = buffer_.ins;
    buffer.instype = buffer_.instype;
    buffer.rd = buffer_.rd;
    cpu->stall_MEM = 0;
    // std::cout<<"MEM debug: "<<buffer.instype<<"  "<<buffer_.output<<"  "<<buffer_.instype<<std::endl;
    if(buffer_.instype == STALL) {
        // std::cout<<"MEM return"<<std::endl;
        return;
    }
    // std::cout<<"MEM run2"<<std::endl;
    cpu->stall_MEM = 3;
    unsigned rd_write1;
    unsigned rd_write2;
    unsigned rd_write3;
    unsigned rd_write4;
    unsigned rd_write5;
    unsigned char mem_write1;
    unsigned char mem_write2;
    unsigned char mem_write3;
    unsigned char mem_write4;
    unsigned address1;
    unsigned address2;
    unsigned address3;
    unsigned address4;
    unsigned mem_address1;
    unsigned mem_address2;
    unsigned mem_address3;
    unsigned mem_address4;
    unsigned output_;
    unsigned char mem_write;
    switch(buffer.instype) {
        case LB:
            output_ = cpu->mem->read(buffer_.output);
            output_ = (output_ & (1 << (7 + 1)) - 1); if (output_ & (1 << 7)) output_ = output_ | 0xffffff00;
            buffer.output = output_;
            break;
        case LH:
            address1 = buffer_.output;
            address2 = address1 + 1;
            rd_write1 = cpu->mem->read(address1);
            rd_write2 = cpu->mem->read(address2);
            rd_write3 = (rd_write2 << 8) + rd_write1;
            rd_write3 = (rd_write3 & (1 << (15 + 1)) - 1); if (rd_write3 & (1 << 15)) rd_write3 = rd_write3 | 0xffff0000;
            buffer.output = rd_write3;
            break;
        case LW:
            address1 = buffer_.output;
            address2 = address1 + 1;
            address3 = address1 + 2;
            address4 = address1 + 3;
            rd_write1 = cpu->mem->read(address1);
            rd_write2 = cpu->mem->read(address2);
            rd_write3 = cpu->mem->read(address3);
            rd_write4 = cpu->mem->read(address4);
            buffer.output = (rd_write4 << 24) + (rd_write3 << 16) + (rd_write2 << 8) + rd_write1;
            break;
        case LBU:
            output_ = cpu->mem->read(buffer_.output);
            buffer.output = output_;
            break;
        case LHU:
            address1 = buffer_.output;
            address2 = address1 + 1;
            rd_write1 = cpu->mem->read(address1);
            rd_write2 = cpu->mem->read(address2);
            rd_write3 = (rd_write2 << 8) + rd_write1;
            buffer.output = rd_write3;
            break;
        case SB:
            mem_write =  buffer_.rv2; 
            cpu->mem->write(buffer_.output, mem_write);
            break;
        case SH:
            mem_write1 = buffer_.rv2;
            mem_write2 = buffer_.rv2 >> 8;
            mem_address1 = buffer_.output;
            mem_address2 = mem_address1 + 1;
            cpu->mem->write(mem_address1, mem_write1);
            cpu->mem->write(mem_address2, mem_write2);
            break;
        case SW:
            mem_write1 = buffer_.rv2;
            mem_write2 = buffer_.rv2 >> 8;
            mem_write3 = buffer_.rv2 >> 16;
            mem_write4 = buffer_.rv2 >> 24;
            mem_address1 = buffer_.output;
            mem_address2 = buffer_.output + 1;
            mem_address3 = buffer_.output + 2;
            mem_address4 = buffer_.output + 3;
            cpu->mem->write(mem_address1, mem_write1);
            cpu->mem->write(mem_address2, mem_write2);
            cpu->mem->write(mem_address3, mem_write3);
            cpu->mem->write(mem_address4, mem_write4);
            break;
        default:
            // std::cout<<"MEM debug2: "<<buffer_.output<<std::endl;
            buffer.output = buffer_.output;
            cpu->stall_MEM = 0;
            break;

    }       
}

void WB::run() {
    // std::cout<<"WB run"<<std::endl;
    if(buffer_.ins == 0x0ff00513){
        unsigned output = cpu->reg->read(10) & 255u;
        std::cout << "ans: "<<output << " ";
        // stop all
        cpu->flush_all = true;
    }

    buffer.rd = buffer_.rd;
    buffer.output = buffer_.output;
    // std::cout<<"WB debug res: "<<buffer_.output<<std::endl;
    // std::cout<<"WB debug inscode: "<<buffer_.instype<<std::endl;
    if(buffer_.instype == STALL)
        return;
    ins_types ins_type = cpu->decoder->get_type[buffer_.instype];
    switch(ins_type) {
        case R_TYPE:
        case I_TYPE:
        case U_TYPE:
        case J_TYPE:
            if(buffer.rd != 0)
                cpu->reg->write(buffer.rd, buffer.output);
            break;
        default:
            break;
    }
}

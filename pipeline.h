#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "decoder.hpp"

class CPU;

class Buffer {
    friend class CPU;
public: 
    struct IF_ID {
        unsigned ins, pc, pred_pc;
        void flush() {
            ins = pc = pred_pc = 0;
        }
    };

    struct ID_EX {
        unsigned rv1, rv2, imm, rd, rs1, rs2, ins, pc, pred_pc;
        opcode instype;
        void flush() {
            ins = pc = pred_pc = rv1 = rv2 = imm = 0;
            instype = STALL;
            rd = rs1 = rs2 = -99;
        }
    };

    struct EX_MEM {
        unsigned ins, output, pc, newpc, rd, rv2;
        opcode instype;
        void flush() {
            pc = newpc = rv2 = output = 0;
            instype = STALL;
            rd = -99;
        }
    };

    struct MEM_WB {
        unsigned ins, output, rd;
        opcode instype;
        void flush() {
            output = 0;
            rd = -99;
            instype = STALL;
        }
    };

    struct final_ {
        unsigned output, rd;
        // void flush() {
        //     output = 0;
        //     rd = -99;
        // }
    };
    
    CPU* cpu;

    Buffer(CPU* cpu_) : cpu(cpu_) { }
};


class IF : public Buffer {
    friend class CPU;
public:
    IF_ID buffer;
    IF(CPU* cpu) : Buffer(cpu), buffer{} { }
    virtual void run();

};

class ID : public Buffer {
    friend class CPU;
public:
    ID_EX buffer;
    IF_ID buffer_;
    ID(CPU* cpu) : Buffer(cpu), buffer{}, buffer_{} { }
    virtual void run();
    void forward();
};

class EX : public Buffer {
    friend class CPU;
public:
    EX_MEM buffer;
    ID_EX buffer_;
    EX(CPU* cpu) : Buffer(cpu), buffer{}, buffer_{} { }

    virtual void run();
};

class MEM : public Buffer {
    friend class CPU;
public:
    MEM_WB buffer;
    EX_MEM buffer_;
    MEM(CPU* cpu) : Buffer(cpu), buffer{}, buffer_{} { }
    virtual void run();


};

class WB : public Buffer{
    friend class CPU;
public:
    final_ buffer;
    MEM_WB buffer_;
    WB(CPU* cpu) : Buffer(cpu), buffer{}, buffer_{} { }
    virtual void run();
};
#endif
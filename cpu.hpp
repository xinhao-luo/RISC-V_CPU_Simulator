#ifndef CPU_HPP
#define CPU_HPP

#include <fstream>
#include <bitset>
#include <iostream>
#include "decoder.hpp"
#include "memory.hpp"
#include "register.hpp"
#include "pipeline.h"
#include "predict.hpp"

class CPU {
    friend class Predict;
    friend class Buffer;
    friend class IF;
    friend class ID;
    friend class EX;
    friend class MEM;
    friend class WB;
private:
    Memory* mem;
    Register* reg;
    Decoder* decoder;
    IF* IF_;
    ID* ID_;
    EX* EX_;
    MEM* MEM_;
    WB* WB_;
    Predict* Predict_;
    unsigned clk;
    unsigned pc;
    Buffer::final_ final_buffer;
    bool wrong_predict;
    unsigned stall_MEM;// mem if stall
    bool stall_; // other stage if stall
    bool flush_all;

private:
    void flush() {
        IF_->buffer.flush();
        ID_->buffer.flush();
        // EX_->buffer.flush();
        // MEM_->buffer.flush();
        // WB_->buffer.flush();
    }

    void wire_buffer() {
        // std::cout<<"buffer start"<<std::endl;
        if(stall_MEM)
            return;

        if(!stall_) {
            if(wrong_predict) {
                pc = EX_->buffer.newpc;
                flush();
                // std::cout<<"after flush: "<<ID_->buffer.ins<<std::endl;
            } else {
                pc = Predict_->get();
            }
            // std::cout<<"wire!!"<<std::endl;
            ID_->buffer_ =  IF_->buffer;
            EX_->buffer_ = ID_->buffer;
            // std::cout<<"buffer ex: "<<EX_->buffer_.ins<<std::endl;
            MEM_->buffer_ = EX_->buffer;
        } else 
            MEM_->buffer_.flush();
        WB_->buffer_ = MEM_->buffer;
        final_buffer = WB_->buffer;
        // std::cout<<"after buffer pc: "<<pc<<std::endl;
        // std::cout<<"after buffer mem: "<<MEM_->buffer_.ins<<std::endl;
    }

    void wire_stage() {
        // std::cout<<"stage start"<<std::endl;
        if (stall_MEM)
            return;
        // std::cout<<"IF start"<<std::endl;
        IF_->run();
        // std::cout<<IF_->buffer.pc<<std::endl;
        // std::cout<<"IF end"<<std::endl;
        ID_->run();
        // std::cout<<ID_->buffer.pc<<std::endl;
        EX_->run();
        MEM_->run();
        WB_->run();
    }

    void get_totstall() {
        if(stall_MEM) {
            stall_MEM--;
            return;
        }
    } 

public:
    CPU() : IF_(new IF(this)),
            ID_(new ID(this)),
            EX_(new EX(this)),
            MEM_(new MEM(this)),
            WB_(new WB(this)),
            mem(new Memory()),
            reg(new Register()),
            decoder(new Decoder()),
            final_buffer{},
            Predict_(new Predict()),
            pc(0),
            stall_MEM(0),
            stall_(0),
            wrong_predict(false),
            flush_all(false),
            clk(0){}

    void read_init(std::string filename) {
        std::ifstream file(filename);
        std::string line;
        unsigned address;
        unsigned value;
        // unsigned i = mem->getins(0);
        while (std::getline(file, line)) {
            if (line[0] == '@') {
                address = std::stoi(line.substr(1), nullptr, 16);
            } else {
                for (int i = 0; i < line.size(); i+=3){
                    std::string data = line.substr(i, 2);
                    // 将指令读入memory
                    // std::cout<<address<<std::endl;
                    mem->writeins(address, data);
                    address++;
                }
            }
        }
    }

    void run() {
        while(!flush_all) {
            // std::cout<<stall_MEM<<" "<<stall_<<" "<<wrong_predict<<" "<<pc<<std::endl;
            // std::cout<<"reg: "<<reg->read(12)<<std::endl;
            clk++;
            wire_buffer();
            wire_stage();
            get_totstall();
        }

        std::cout<< "acc: " <<(double) Predict_->correct_pred / Predict_->total_pred<<" total: "<<Predict_->total_pred<<" correct: "<< Predict_->correct_pred<<std::endl;
    }


    
};

#endif
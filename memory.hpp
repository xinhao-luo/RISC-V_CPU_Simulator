#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstring>



class Memory {
private:
    unsigned char mem_[500000] = {};
public:
    Memory(){
        std::memset(mem_, 0, sizeof(mem_));
    }
    void writeins(unsigned address, std::string ins) {
        mem_[address] = std::stoi(ins, nullptr, 16);
    }

    unsigned read(unsigned address) {
        return mem_[address];
    }

    void write(unsigned address, unsigned char data) {
        mem_[address] = data;
    }

    unsigned getins(unsigned pc) {
        unsigned instruction = 0;
        // std::cout<<"get ins"<<std::endl;
        // for (int i = 0; i < 100; i++) std::cout << (int32_t)mem_[i] << " ";
        for (int i = 0; i < 4; i++){
            instruction = instruction | (mem_[pc + i] << (8 * i));
        }
        return instruction;
    }
};




#endif
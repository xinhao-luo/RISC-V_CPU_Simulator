#ifndef MEMORY_HPP
#define MEMORY_HPP

unsigned char mem[500000] = {};


class Memory {
public:
    void writeins(unsigned address, std::string ins) {
        mem[address] = std::stoi(ins, nullptr, 16);
    }

    unsigned read(unsigned address) {
        return mem[address];
    }

    void write(unsigned address, unsigned char data) {
        mem[address] = data;
    }

    unsigned getins(unsigned pc) {
        unsigned instruction = 0;
        // for (int i = 0; i < 100; i++) std::cout << (int32_t)mem[i] << " ";
        for (int i = 0; i < 4; i++){
            instruction = instruction | (mem[pc + i] << (8 * i));

        }
        
        return instruction;
    }
};




#endif
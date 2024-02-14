#ifndef REGISTER_HPP
#define REGISTER_HPP

unsigned regs[32] = {};

class Register {
    unsigned pc = 0;
public:
    unsigned readpc() {
        return pc;
    }

    unsigned read(unsigned index) {
        // for (int i = 0; i < 32; i++)std::cout << (int32_t)regs[i] << " ";
        return regs[index];
    }

    void write(unsigned index, unsigned data) {
        // for (int i = 0; i < 32; i++)std::cout << (int32_t)regs[i] << " ";
        // std::cout<<std::endl;
        if(index == 0){
            // for (int i = 0; i < 32; i++)std::cout << (int32_t)regs[i] << " ";
            // std::cout<<std::endl;
            // std::cout<<pc<<"  "<<index<<"  "<<data<<"  "<<std::endl;
            std::cout<< "illegal write (x0)"<<std::endl;
            return;
        }
        regs[index] = data;
    }

    void updatepc(unsigned new_pc) {
        pc = new_pc;
    }
};


#endif
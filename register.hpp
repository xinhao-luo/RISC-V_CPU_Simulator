#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <iostream>



class Register {
private:
    unsigned regs[32] = {};
public:
    Register(){
        std::memset(regs, 0, sizeof(regs));
    }

    unsigned read(unsigned index) {
        // for (int i = 0; i < 32; i++)std::cout << (int32_t)regs[i] << " ";
        return regs[index];
    }

    void write(unsigned index, unsigned data) {
        // for (int i = 0; i < 32; i++)std::cout << (int32_t)regs[i] << " ";
        // std::cout<<std::endl;
        // if(index == 0){
        //     // for (int i = 0; i < 32; i++)std::cout << (int32_t)regs[i] << " ";
        //     // std::cout<<std::endl;
        //     // std::cout<<pc<<"  "<<index<<"  "<<data<<"  "<<std::endl;
        //     std::cout<< "illegal write (x0)"<<std::endl;
        //     return;
        // }
        // std::cout<<"reg write: "<<index<<"  "<<data<<std::endl;
        regs[index] = data;
    }
};


#endif
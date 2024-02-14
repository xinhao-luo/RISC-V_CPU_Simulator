#ifndef PREDICT_HPP
#define PREDICT_HPP


// 参考了https://zhuanlan.zhihu.com/p/504327998

class Predict {
    friend class CPU;
private:
    unsigned predicted_pc;
    unsigned char PHTs[256][64] = {};
    unsigned char BHT[256] = {};
    unsigned BTB[256] = {};
    unsigned total_pred = 0, correct_pred = 0;

public:
    Predict() : predicted_pc(0), total_pred(0), correct_pred(0) {
        std::memset(BHT, 0, sizeof(BHT));
        std::memset(PHTs, 0b01u, sizeof(PHTs));
        std::memset(BTB, 0, sizeof(BTB));
    }

    void run(unsigned pc, bool ifbranch, bool ifcorrect, unsigned newpc) {
        unsigned index = (pc >> 2) & 0xff;
        unsigned BHR = BHT[index];
        if (ifbranch) {
            if (PHTs[index][BHR] < (unsigned) 0b11)
                PHTs[index][BHR]++;
            BHT[index] = BHT[index] >> 1;
            BHT[index] |= (unsigned) 0b100000;
            BTB[index] = newpc;            
        } else {
            if (PHTs[index][BHR] > (unsigned) 0b00)
                PHTs[index][BHR]--;
            BHT[index] = BHT[index] >> 1;
        }
        total_pred++;
        correct_pred += ifcorrect;
    }

    void predict(unsigned pc, unsigned ins) {
        unsigned index = (pc >> 2) & 0xff;
        unsigned BHR = BHT[index];
        unsigned opcode = (ins & (unsigned)0x7f);
        if(opcode == (unsigned)0b1100011 && (PHTs[index][BHR] & (unsigned)0x10))
            predicted_pc = BTB[index];
        else
            predicted_pc = pc + 4;
    }

    unsigned get() {
        return predicted_pc;
    }

};


#endif
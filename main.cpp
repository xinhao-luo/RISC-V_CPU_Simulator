#include <iostream>
#include "cpu.hpp"


int main() {
    std::string testcases[] = {"array_test1.data", "array_test2.data", "basicopt1.data", "bulgarian.data", "expr.data",
                                  "gcd.data", "hanoi.data", "lvalue2.data", "magic.data", "manyarguments.data",
                                  "multiarray.data", "naive.data", "pi.data", "qsort.data", "queens.data",
                                  "statement_test.data", "superloop.data", "tak.data"};
    freopen("output.txt", "w", stdout);
    for (auto t : testcases) {
        std::cout<<t<<std::endl;
        CPU cpu;
        cpu.read_init("testcases/" + t);
        cpu.run();
    }
    
    return 0;
}
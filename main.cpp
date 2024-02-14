#include <iostream>
#include "cpu.hpp"

using namespace std;

int main() {
    CPU cpu;
    cpu.read_init("testcases/array_test1.data");
    freopen("output.txt", "w", stdout);
    cpu.run();
    return 0;
}
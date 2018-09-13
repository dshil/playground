#include <iostream>

#include "shared_ptr.h"

int main(int argc, char** argv) {
    SharedPtr<int> ptr(new int(10));
    SharedPtr<int> newptr = ptr;

    /* std::cout << "value: " << *ptr << std::endl; */

    return 0;
}

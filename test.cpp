#include <iostream>
#include <cstdlib>

#include "Flp.h"

int main(int argc, char * argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " file.flp" << std::endl;
        std::exit(1);
    }
    Flp flp(argv[1]);

    if (flp.good()) {
        std::cout << "Good!" << std::endl;
    } else {
        std::cout << "Error: " << flp.errorMessage() << std::endl;
    }

}


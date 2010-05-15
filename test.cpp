#include <iostream>
#include <cstdlib>

#include "Flp.h"

int main(int argc, char * argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <file.flp>" << std::endl;
        std::exit(1);
    }
    Flp flp(argv[1]);

    if (flp.good()) {
        std::cout << "Good!" << std::endl;
        std::cout << "Beats per minute: " << flp.tempo() << std::endl;
        std::cout << "Project title: " << flp.title() << std::endl;
        std::cout << "Project version: " << flp.versionString() << std::endl;

        std::vector<std::string> * channels = flp.channels();
        std::cout << "Channels:\n-------\n";
        for (int i=0; i<channels->size(); ++i) {
            std::cout << channels->at(i) << std::endl;
        }

        std::vector<std::string> * samples = flp.samples();
        std::cout << "\nSamples:\n--------\n";
        for (int i=0; i<samples->size(); ++i) {
            std::cout << samples->at(i) << std::endl;
        }

        std::vector<std::string> * effects = flp.effects();
        std::cout << "\nEffects:\n--------\n";
        for (int i=0; i<effects->size(); ++i) {
            std::cout << effects->at(i) << std::endl;
        }

    } else {
        std::cout << "Error: " << flp.errorMessage() << std::endl;
    }
}


#include <iostream>
#include <cstdlib>

#include "Flp.h"

int main(int argc, char * argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <file.flp>" << std::endl;
        std::exit(1);
    }

    if (Flp::isValid(argv[1])) {
        std::cout << "file looks valid" << std::endl;
    } else {
        std::cout << "invalid file." << std::endl;
    }

    Flp flp(argv[1]);

    if (flp.good()) {
        std::cout << "Good!" << std::endl;
        std::cout << "Beats per minute: " << flp.tempo() << std::endl;
        std::cout << "Project title: " << flp.title() << std::endl;
        std::cout << "Project version: " << flp.versionString() << std::endl;

        std::cout << "Channels:\n-------\n";
        for (int i=0; i<flp.channelCount(); ++i) {
            std::cout << flp.channelName(i) << " (" <<
                flp.channelPluginName(i) << ")" << std::endl;
        }

        std::cout << "\nSamples:\n--------\n";
        for (int i=0; i<flp.sampleCount(); ++i) {
            std::cout << flp.sampleFileName(i) << std::endl;
        }

        std::cout << "\nEffects:\n--------\n";
        for (int i=0; i<flp.effectCount(); ++i) {
            std::cout << flp.effectPluginName(i) << std::endl;
        }

    } else {
        std::cout << "Error: " << flp.errorMessage() << std::endl;
    }
}


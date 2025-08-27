
#include <iostream>
#include <ranges>
#include "WavReader.hpp"

int main(int argc, char* argv[])
{   
    std::cout << "Number of arguments: " << argc << std::endl;
    std::cout << "Arguments : " << std::endl;

    for (auto i : std::views::iota(0, argc))
    {
        std::cout << "#" << i << ": " << argv[i] << std::endl;
    }

    std::cout << std::endl;

    if (argc != 2)
    {
        std::cout << "WavReader program to read an analyze a WAV file" << std::endl;
        std::cout << "Usage: WavReader path/to/wav" << std::endl;

        return 1;
    }

    // TO DO : wav reading
    wav::analyzeWav(argv[1]);

    return 0;
}
#include <iostream>
#include "SatSudoku.hpp"
int main(int argc, char** argv)
{
    // Parsing arguments
    if (argc  == 1)
    {
        std::cout << "Usage:\n\tSatSudoku <time_in_seconds> <path_to_sudoku_file>" << std::endl;
        return 0;
    }
    else if (argc < 3)
    {
        std::cerr << "Not enough arguments" << std::endl;
        return 1;
    }
    else if (argc > 3)
    {
        std::cerr << "Too many  arguments" << std::endl;
        return 1;
    }

    // Try to read first argument
    float time = 0;
    auto result = sscanf(argv[1], "%f", &time);
    if (result == 0)
    {
        std::cerr << "Invalid value for time: " << argv[1] << std::endl;
        std::cerr << "Expected float" <<  std::endl;
        return 1;
    }

    // Read second argument
    std::string file_path(argv[2]);
    SatSudoku program(time, file_path);
    program.run();


    return 0;
}
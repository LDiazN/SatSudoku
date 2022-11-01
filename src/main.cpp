#include <iostream>
#include "SatSudoku.hpp"
#include <set>
#include <string>

int main(int argc, char** argv)
{
    // Parsing arguments
    if (argc  == 1)
    {
        std::cout << "Usage:\n\tSatSudoku <time_in_seconds> <path_to_sudoku_file> [optional_flags]\n";
        std::cout << "Available flags:\n";
        std::cout << "\t--SAT: Interpret file input as a SAT file, and solve such file directly\n";
        return 0;
    }
    else if (argc < 3)
    {
        std::cerr << "Not enough arguments" << std::endl;
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

    // Read additional flags
    std::set<std::string> flags;
    for(int i = 3; i < argc; i++)
        flags.emplace(argv[i]);

    // Read flags

    bool is_sudoku = flags.find("--SAT") == flags.end();
    bool dump_sat = flags.find("--dump-sat") != flags.end();
    // Read second argument
    std::string file_path(argv[2]);
    SatSudoku program(time, file_path, is_sudoku, dump_sat);
    program.run();


    return 0;
}
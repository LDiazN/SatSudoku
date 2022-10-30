
#include "SatSudoku.hpp"
#include "Sudoku.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <filesystem>

SatSudoku::SatSudoku(float max_time, const std::string& file)
    : _file(file)
    , _time(max_time)
{ }

void SatSudoku::run()
{
    // Sanity check
    if (!std::filesystem::exists(_file))
    {
        std::cerr << "The specified file " << _file << " Does not exists\n";
        return;
    }

    if(_time < 0)
    {
        std::cerr << "Time should be a positive number or 0. Provided time: " << _time << std::endl;
        return;
    }

    std::ifstream fs(_file);
    std::string line;

    // Just a line counter for debug
    size_t n_lines = 0;
    std::cout << "Processing file: " << _file << std::endl;
    while(std::getline(fs, line)) // Iterate over each sudoku line in file
    {
        n_lines++;
        std::cout << "Reading Sudoku at line " << n_lines << std::endl;

        std::stringstream ss(line);
        size_t sudoku_order = 0;
        // Parse sudoku order
        ss >> sudoku_order;
        std::string next_num_str;

        // Create sudoku with specified size
        Sudoku sudoku(sudoku_order);

        // Parse numbers separated by -
        size_t num_index = 0;
        auto &board = sudoku.get_board();
        auto board_size = sudoku_order * sudoku_order;
        while(std::getline(ss, next_num_str, '-'))
        {
            uint next_num;
            size_t board_row = num_index / board_size;
            size_t board_col = num_index % (board_size);

            auto result = sscanf(next_num_str.c_str(), "%u", &next_num);
            if(result == 0)
            {
                std::cerr << "Invalid number in position " << num_index << ". Given number: " << next_num_str << std::endl;
                return;
            }

            board.set(board_row, board_col, next_num);

            num_index++;
        }
        sudoku.display();
    }

}
#ifndef SATSUDOKU_HPP
#define SATSUDOKU_HPP
#include <string>
#include "Sudoku.hpp"

/// @brief Maion class wrapping application flow
class SatSudoku
{
    public:
        SatSudoku(float max_time, const std::string& file);

        /// @brief Run application logic
        void run();

    private:
        /// @brief Solve a sudoku by turn it into a SAT problem and then solving the sat problem, and then converting it 
        /// back to sudoku
        /// @param sudoku sudoku to be solved
        /// @return a solved sudoku
        static Sudoku solve_sudoku(const Sudoku& sudoku);

    private: 
        std::string _file;
        float _time;

};

#endif
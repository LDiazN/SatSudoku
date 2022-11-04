#ifndef SATSUDOKU_HPP
#define SATSUDOKU_HPP
#include <string>

#include "SatSolver.hpp"
#include "Sudoku.hpp"

#define SUCCESS 1
#define FAILURE 0
#define STATUS int


class Sudoku;

/// @brief Maion class wrapping application flow
class SatSudoku
{
    public:
        SatSudoku(float max_time, const std::string& file, bool file_is_sudoku = true, bool dump_sat = false);

        /// @brief Run application logic
        void run();


    private:
        /// @brief Solve a sudoku by turn it into a SAT problem and then solving the sat problem, and then converting it 
        /// back to sudoku
        /// @param sudoku sudoku to be solved
        /// @return a solved sudoku
        static Sudoku solve_sudoku(Sudoku& sudoku, bool dump_sat = false);

        /// @brief Run a sudoku solving with the specified configuration
        void run_sudoku_solver();

        /// @brief run a sat solver with the specified configuration
        void run_sat_solver();

    private: 
        std::string _file;
        float _time;
        // If input file is a sudoku, otherwise it's a SAT file. True by default
        bool _file_is_sudoku;
        bool _dump_sat;

};

#endif
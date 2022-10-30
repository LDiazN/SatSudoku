#ifndef SUDOKU_HPP
#define SUDOKU_HPP
#include "SatSolver.hpp"
#include <string>
#include "Array2D.hpp"

class Sudoku
{
    public:
    Sudoku(size_t n);

    /// @brief Convert this sudoku to a SAT Solver 
    /// @return a SatSolver instance that mirrors this sudoku
    SatSolver as_sat() const;

    /// @brief Parse a sudoku from a string, as specified in the project document
    /// @param sudoku_str string correctly formated as a sudoku str
    /// @return Sudoku instance representing the given string
    static Sudoku from_str(const std::string& sudoku_str);

    /// @brief Create a sudoku from a sat solution, assuming the sat solution is consistent with a sudoku
    /// @param sat_solution A solution for a sat converted sudoku
    /// @return A solved sudoku matched the given sat solution
    static Sudoku from_sat_sol(const SatSolution& sat_solution);

    Array2D<uint32_t>& get_board() { return _board; }

    /// @brief Print this sudoku in the terminal correctly formatted
    void display();

    private:
    Array2D<uint32_t> _board;
    size_t _order;
};


#endif
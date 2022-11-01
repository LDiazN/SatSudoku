#ifndef SUDOKU_HPP
#define SUDOKU_HPP
#include "SatSolver.hpp"
#include <string>
#include "Array2D.hpp"


using Variable = int;
class SatSolution;
class SatSolver;

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

    /// @brief Transform from board position and value to a variable number
    /// @param i position i in board
    /// @param j position d in board
    /// @param d value of position i,j in board
    /// @return a number in range [0, n^6)
    int cell_to_variable(int i, int j, int d) const;

    /// @brief Convert from a variable in range [0, n^6) to a valid board position and value
    /// @param var variable to convert back to board position and value
    /// @param out_i position i in board
    /// @param out_j position j in board
    /// @param out_d value of position i,j
    void variable_to_cell(Variable var, int& out_i, int& out_j, int& out_d) const;

    /// @brief Get the max number of variables for this sudoku
    /// @return 
    size_t get_n_variables() const { return _order * _order * _order * _order * _order * _order; }

    private: 
    /// @brief Add completeness clauses to the specified clauses set
    /// @param clauses A vector that will be modified by adding new clauses
    void add_completeness_clauses(std::vector<Clause>& clauses) const;

    /// @brief Add uniqueness clauses to the specified clauses set
    /// @param clauses A vector that will be modified by adding new clauses
    void add_uniqueness_clauses(std::vector<Clause>& clauses) const;

    /// @brief Add validity clauses to the specified clauses set
    /// @param clauses A vector that will be modified by adding new clauses
    void add_validity_clauses(std::vector<Clause>& clauses) const;

    private:
    Array2D<uint32_t> _board;
    size_t _order;
};


#endif
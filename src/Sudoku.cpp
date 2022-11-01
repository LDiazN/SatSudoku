#include <iostream>
#include "Sudoku.hpp"
#include "Array2D.hpp"

Sudoku::Sudoku(size_t n)
    : _board(Array2D<uint>(n*n, n*n))
    , _order(n)
{ }

SatSolver Sudoku::as_sat() const
{
    std::cout << "Not implemented" << std::endl;
    return SatSolver(std::vector<SatSolver::Clause>(),0);
}

Sudoku Sudoku::from_str(const std::string& sudoku_str)
{
    std::cout << "Not implemented" << std::endl;
    return Sudoku(3);
}

Sudoku Sudoku::from_sat_sol(const SatSolution& sat_solution)
{
    std::cout << "Not implemented" << std::endl;
    return Sudoku(3);
}

void Sudoku::display()
{
    std::cout << "Order: " <<  _order << std::endl;
    for(size_t i = 0; i < _board.get_rows(); i++)
    {
        for(size_t j = 0; j < _board.get_cols(); j++)
        {
            if(j % _order == 0)
                std::cout << " ";
            std::cout << _board.get(i,j) << " ";
        }
        std::cout << std::endl;
        if ((i+1) % _order == 0)
            std::cout << std::endl;
    }
}
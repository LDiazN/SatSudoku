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
    return SatSolver(std::vector<Clause>(),0);
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

int Sudoku::cell_to_variable(int i, int j, int d)
{
    auto n2 = _order * _order;
    auto n4 = n2 * n2;

    assert(0 <= i && i < n2 && "invalid range for i");
    assert(0 <= j && j < n2 && "invalid range for j");
    assert(1 <= d && d <= n2 && "invalid range for i");

    return n4 * i + n2 * j + d - 1;
}

void Sudoku::variable_to_cell(Variable var, int& out_i, int& out_j, int& out_d)
{
    auto n2 = _order * _order;
    auto n4 = n2 * n2;
    assert(0 <= var < n4*n2 && "Invalid range for variable");

    out_d = var % n2;
    out_j = ((var - out_d) / n2) % n2;
    out_i = (var - out_d - n2 * out_j) / n4;
    out_d++;
}
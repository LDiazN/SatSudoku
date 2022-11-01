#include <iostream>
#include "Sudoku.hpp"
#include "Array2D.hpp"

Sudoku::Sudoku(size_t n)
    : _board(Array2D<uint>(n*n, n*n))
    , _order(n)
{ }

SatSolver Sudoku::as_sat() const
{
    std::vector<Clause> clauses;

    add_completeness_clauses(clauses);
    add_uniqueness_clauses(clauses);
    return SatSolver(clauses,0);
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

int Sudoku::cell_to_variable(int i, int j, int d) const
{
    auto order = static_cast<int>(_order);
    auto n2 = order * order;
    auto n4 = n2 * n2;

    assert(0 <= i && i < n2 && "invalid range for i");
    assert(0 <= j && j < n2 && "invalid range for j");
    assert(1 <= d && d <= n2 && "invalid range for i");

    return n4 * i + n2 * j + d - 1;
}

void Sudoku::variable_to_cell(Variable var, int& out_i, int& out_j, int& out_d) const
{
    auto order = static_cast<int>(_order);
    auto n2 = order * order;
    auto n4 = n2 * n2;
    assert(0 <= var && var < n4*n2 && "Invalid range for variable");

    out_d = var % n2;
    out_j = ((var - out_d) / n2) % n2;
    out_i = (var - out_d - n2 * out_j) / n4;
    out_d++;
}

void Sudoku::add_completeness_clauses(std::vector<Clause>& clauses) const
{
    auto order = static_cast<int>(_order);
    auto n2 = order * order;
    Clause next_clause;
    for(int i = 0; i < n2; i++)
        for(int j = 0; j < n2; j++)
        {
            for (int d = 1; i <= n2; d++)
                next_clause.push_back(cell_to_variable(i,j,d));
            clauses.emplace_back(next_clause);
            next_clause.clear();
        }
}

void Sudoku::add_uniqueness_clauses(std::vector<Clause>& clauses) const
{
    auto order = static_cast<int>(_order);
    auto n2 = order * order;
    for (int i = 0; i < n2; i++)
        for (int j = 0; j < n2; j++)
            for (int d = 1; d <= n2; d++)
                for (int d_ = 1; d_ <= n2; d_++)
                {
                    if (d == d_) continue;
                    clauses.emplace_back(Clause{-cell_to_variable(i,j,d), -cell_to_variable(i,j,d_)});
                }
}
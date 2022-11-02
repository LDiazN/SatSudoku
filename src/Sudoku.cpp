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
    add_validity_clauses(clauses);

    auto n = static_cast<int>(_order);
    auto n2 = n * n; 

    return SatSolver(clauses, cell_to_variable(n2-1, n2-1, n2) + 1);
}

Sudoku Sudoku::from_str(const std::string& sudoku_str)
{
    std::cout << "Not implemented" << std::endl;
    return Sudoku(3);
}

Sudoku Sudoku::from_sat_sol(const SatSolution& sat_solution)
{
    // TODO check this function when a sat solver is ready
    // Convert from number of variables to actual order
    auto order = order_for_n_vars(sat_solution.n_variables);
    Sudoku sudoku(order);
    auto& board = sudoku.get_board();

    // If can't solve sudoku, just return empty sudoku
    if (sat_solution.satisfiable == SatSatisfiable::UNKNOWN || sat_solution.satisfiable == SatSatisfiable::UNSATISFIABLE)
        return sudoku;

    // Fill 
    for(Variable variable : sat_solution.variable_states)
    {
        int i,j,d;
        if (variable < 0) // false, we don't want it
            continue;

        sudoku.variable_to_cell(abs(variable), i,j,d);
        board.set(i,j,d);
    }

    return sudoku;
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
    assert(1 <= d && d <= n2 && "invalid range for d");

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

int  Sudoku::order_for_n_vars(int n_variables)
{
    // order of sudoku can't be that big, right?
    // ... right???
    for (int n = 0; true; n++)
    {
        auto n2 = n * n;
        auto n4 = n2 * n2;
        auto N = n4 * (n-1) + n2 * (n-1) + n2;
        if (N == n_variables)
            return n;

        //Sanity check for debugging
        assert( n < 100 && "couldn't find actual order of variables ");
    }
}

void Sudoku::add_completeness_clauses(std::vector<Clause>& clauses) const
{
    auto order = static_cast<int>(_order);
    auto n2 = order * order;
    Clause next_clause;
    for(int i = 0; i < n2; i++)
        for(int j = 0; j < n2; j++)
        {
            for (int d = 1; d <= n2; d++)
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
                for (int d_ = d+1; d_ <= n2; d_++)
                {
                    if (d == d_) continue;
                    clauses.emplace_back(Clause{-cell_to_variable(i,j,d), -cell_to_variable(i,j,d_)});
                }
}

void  Sudoku::add_validity_clauses(std::vector<Clause>& clauses) const
{
    auto n = static_cast<int>(_order);
    auto n2 = n * n;
    // Iterate over rows 
    for (int i = 0; i < n2; i++)
        for(int j = 0; j < n2; j++)
            for(int j_ = j+1; j_ < n2; j_++)
                for(int d = 1; d <= n2; d++)
                    clauses.emplace_back(Clause{-cell_to_variable(i,j,d), -cell_to_variable(i,j_,d)});

    // Iterate over cols 
    for (int j = 0; j < n2; j++)
        for(int i = 0; i < n2; i++)
            for(int i_ = i+1; i_ < n2; i_++)
                for(int d = 1; d <= n2; d++)
                    clauses.emplace_back(Clause{-cell_to_variable(i,j,d), -cell_to_variable(i_,j,d)});
                    
    // Iterate over regions
    for(int i = 0; i < n2; i += n)
        for(int j = 0; j < n2; j += n)
            for(int i_ = i; i_ < i + n; i_++)
                for(int j_ = j; j_ < j + n; j_++)
                    for(int i_2 = i_ + 1; i_2 < i + n; i_2 ++)
                        for(int j_2 = j_ + 1; j_2 < j + n; j_2 ++)
                            for(int d = 1; d <= n2; d++)
                                clauses.emplace_back(Clause{-cell_to_variable(i_, j_, d), -cell_to_variable(i_2, j_2, d)});
}

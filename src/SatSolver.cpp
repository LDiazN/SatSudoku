#include "SatSolver.hpp"
#include <iostream>

std::string SatSolution::as_str()
{
    return "Not Implemented";
}

SatSolver::SatSolver()
{
    std::cout << "Creating a SAT Solver" << std::endl;
}

SatSolver SatSolver::from_str(const std::string& sat_str)
{
    return SatSolver();
}

SatSolution SatSolver::solve()
{
    return SatSolution();
}
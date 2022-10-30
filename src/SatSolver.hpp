#ifndef SAT_SOLVER_HPP
#define SAT_SOLVER_HPP

#include <string>


/// @brief Output of a Sat solution
struct SatSolution
{

    /// @brief String representation of this solution. Useful to be parsed by the SAT2Sudoku function 
    /// @return string formated as a valid SAT solution file
    std::string as_str();
};

class SatSolver
{
    public:
        SatSolver();

        /// @brief Create a SatSolver instance from a string representation as specified in 
        /// the project document
        /// @param sat_str string with an input SAT expression
        /// @return A SatSolver matching the specified string
        static SatSolver from_str(const std::string& sat_str);

        /// @brief Try to solve the SAT problem
        /// @return A solution representing the valid solution
        SatSolution solve();
};

#endif
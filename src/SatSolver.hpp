#ifndef SAT_SOLVER_HPP
#define SAT_SOLVER_HPP

#include <string>
#include <vector>
#include "SatSudoku.hpp"

#ifndef STATUS
    #define SUCCESS 1
    #define FAILURE 0
    #define STATUS int
#endif

/// @brief Output of a Sat solution
struct SatSolution
{

    /// @brief String representation of this solution. Useful to be parsed by the SAT2Sudoku function 
    /// @return string formated as a valid SAT solution file
    std::string as_str();
};

/// Possible SAT formats, it's here just for completeness
enum SATFormat
{
    CNF
};

/// @brief A SAT problem
class SatSolver
{
    public: 
        using Variable = int;
        using Clause = std::vector<Variable>;
    public:
        SatSolver(const std::vector<Clause>& clauses, size_t n_variables, SATFormat format = SATFormat::CNF);

        /// @brief Create a SatSolver instance from a string representation as specified in 
        /// the project document
        /// @param sat_str string with an input SAT expression
        /// @param outResult A SatSolver matching the specified string if parsing was successful, nothing otherwise
        /// @return Parsing status, 1 for success, 0 for failure
        static STATUS from_str(const std::string& sat_str, SatSolver& outResult);

        /// @brief Try to solve the SAT problem
        /// @return A solution representing the valid solution
        SatSolution solve();

    private:
    size_t _n_variables;
    size_t _n_clauses;
    SATFormat _format;
    std::vector<Clause> _clauses;
};

#endif
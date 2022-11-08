#ifndef SAT_SOLVER_HPP
#define SAT_SOLVER_HPP

#include <string>
#include <vector>
#include "SatSudoku.hpp"
#include <iostream>
#include <queue>
#include <map>
#include <assert.h>

#ifndef STATUS
    #define SUCCESS 1
    #define FAILURE 0
    #define STATUS int
#endif

using Variable = int;
using Clause = std::vector<Variable>;

enum SatSatisfiable
{
    SATISFIABLE = 1,
    UNSATISFIABLE = 0,
    UNKNOWN = -1
};

/// Possible SAT formats, it's here just for completeness
enum SATFormat
{
    CNF
};


/// @brief Output of a Sat solution
struct SatSolution
{

    /// @brief String representation of this solution. Useful to be parsed by the SAT2Sudoku function 
    /// @return string formated as a valid SAT solution file
    std::string as_str();

    void display() { std::cout << as_str() << std::endl; }

    public:
        SatSatisfiable satisfiable;
        size_t n_variables;
        std::vector<Variable> variable_states;
        SATFormat format;
};


/// @brief A SAT problem
class SatSolver
{
    public:
        SatSolver(const std::vector<Clause>& clauses, size_t n_variables, SATFormat format = SATFormat::CNF);

        /// @brief Create a SatSolver instance from a string representation as specified in 
        /// the project document
        /// @param sat_str string with an input SAT expression
        /// @param outResult A SatSolver matching the specified string if parsing was successful, nothing otherwise
        /// @return Parsing status, 1 for success, 0 for failure
        static STATUS from_str(const std::string& sat_str, SatSolver& outResult);

        /// @brief Create a SatSolver instance from a string representation as specified in 
        /// the project document
        /// @param sat_str sat_str_stream stringstream with an input SAT expression
        /// @param outResult A SatSolver matching the specified string if parsing was successful, nothing otherwise
        /// @return Parsing status, 1 for success, 0 for failure
        static STATUS from_str_stream(std::stringstream& sat_str_stream, SatSolver& outResult);

        /// @brief Return a string representing this sat problem according to the specification 
        /// @return a string with a valid SAT file
        std::string as_str() const;

        /// @brief Try to reduce sat as much as possible using symlogic properties
        void simplify();

        /// @brief Try to solve the SAT problem
        /// @return A solution representing the valid solution
        SatSolution solve();

        /// @brief Get expected value for this variable for it to be true. If negated, value is false, if not, is true
        /// @param var var to get value for
        /// @return expected value so that this variable evaluates to true
        static int expected_value(Variable var);

    private:

        // A list of lists of clause indices
        using Watchlist = std::vector<std::vector<size_t>>;

        /// @brief transform clauses tu literal format
        void clauses_to_literal();

        /// @brief Map a literal in the format p or -p to 2p for p, or 2p+1 for -p
        /// @param literal a literal, as in clauses 
        /// @return mapped literal, 2p or 2p + 1 accordingly
        int input_literal_to_mapped_literal(int literal)
        {
            if (literal > 0) return 2 * literal;
            if (literal < 0) return 2 * literal + 1;
            assert(false && "literals can't be zero");
        }

        static bool literal_is_negated(int literal)
        {
            assert(literal > 0 && "Invalid literal, you forgot to map it?");
            return (literal & 1) == 1;
        }

        static int literal_to_variable(int literal)
        {
            assert(literal > 0 && "Invalid literal, you forgot to map it?");
            return literal >> 1;
        }

        static int variable_to_literal(int variable)
        {
            auto var_pos = abs(variable);
            return variable < 0 ? var_pos << 1 | 1 : var_pos << 1;
        }

        Watchlist create_watchlist(const std::vector<int>& state) const;

        static bool check_watchlist_invariant(const std::vector<int>& state, const Watchlist& watchlist);

        bool update_watchlist(Watchlist& watchlist, int neg_literal, std::vector<int>& state, std::vector<int>& implications) const;

        bool solve_by_watchlist(Watchlist& watchlist, std::vector<int>& state, const std::vector<Variable>& variables, size_t next_var_index = 0);

        bool solve_by_watchlist_iter(Watchlist& watchlist, std::vector<int>& state, const std::vector<Variable>& variables, const std::vector<size_t>& positive_reps, const std::vector<size_t>& negative_reps,size_t next_var_index = 0 );

        static void reduce_unit_clauses(std::vector<Clause>& clauses, std::vector<int>& state);

        /// @brief Assign constant value to all variables that show up always possitive, or always negated, and simplify expression
        /// @param clauses 
        /// @param memo 
        static void literal_elimination(std::vector<Clause>& clauses, std::vector<int>& state);

        /// @brief Perform constant reduction: if a variable is constant, replace in expression with constant
        /// and simplify properly
        /// @param clauses clauses to simplify 
        /// @param state state of variables
        static void constant_reduction(std::vector<Clause>& clauses, std::vector<int>& state);

        /// @brief Eval this sat expression and check its value
        /// @param clauses List of clauses from the CNF form
        /// @param state state of variables
        /// @return 1 if true, 0 if not, -1 if can't tell
        static int eval(const std::vector<Clause>& clauses ,const std::vector<int>& state);
    private:
        size_t _n_variables;
        size_t _n_clauses;
        SATFormat _format;
        std::vector<Clause> _clauses;
        std::vector<int> _literals;
};

#endif
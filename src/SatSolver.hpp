#ifndef SAT_SOLVER_HPP
#define SAT_SOLVER_HPP

#include <string>
#include <vector>
#include "SatSudoku.hpp"
#include <iostream>
#include <queue>
#include <map>

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

        /// @brief Try to solve the SAT problem
        /// @return A solution representing the valid solution
        SatSolution solve();

        /// @brief Try to reduce sat as much as possible using symlogic properties
        void simplify();

        /// @brief Return a string representing this sat problem according to the specification 
        /// @return a string with a valid SAT file
        std::string as_str() const;

        /// @brief Get expected value for this variable for it to be true. If negated, value is false, if not, is true
        /// @param var var to get value for
        /// @return expected value so that this variable evaluates to true
        static int expected_value(Variable var);

    private:
        /// @brief Utility function to find if a list of clauses is satisfiable or not using backtracking 
        /// @param clauses list of clauses to check for satisfiability
        /// @param memo state of variables so far. 1 for true, 0 for false, -1 for unset
        /// @param current_clause start of clause in vector of clauses
        /// @return if this set of clauses is satisfiable
        bool is_satisfiable(const std::vector<Clause>& clauses,std::vector<int>& memo, size_t current_clause = 0);

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

        /// @brief This implementation of `is_satisfiable` is variable-first, instead of clause-first.
        /// @param clauses clauses forming this SAT expression
        /// @param memo state of variables
        /// @return true if expression is satisfiable, false if it isn't
        bool is_satisfiable_v2(const std::vector<Clause>& clauses, std::vector<int>& memo, std::priority_queue<std::pair<size_t, Variable>>& variables_per_reps,const std::vector<size_t>& negative_reps, const std::vector<size_t>& positive_reps);

        // void add_clause_count(size_t clause_to_count)
        // {
        //     if(_clause_frequence_counter.find(clause_to_count) == _clause_frequence_counter.end())
        //         _clause_frequence_counter[clause_to_count] = 1;
        //     else 
        //         _clause_frequence_counter[clause_to_count]++;
        // }
    private:
        size_t _n_variables;
        size_t _n_clauses;
        SATFormat _format;
        std::vector<Clause> _clauses;
        // std::map<size_t, size_t> _clause_frequence_counter;
};

#endif
#include "SatSolver.hpp"
#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>

std::string SatSolution::as_str()
{
    std::string format_str;
    switch (format)
    {
    case SATFormat::CNF:
        format_str = "cnf";
        break;
    default:
        assert(false && "format not supported");
        break;
    }
    std::stringstream ss;
    ss << "s " << format_str << " " << satisfiable << " " << variable_states.size() << std::endl;
    for(auto v : variable_states)
        ss << "v " << v << std::endl;

    return ss.str();
}

SatSolver::SatSolver(const std::vector<Clause>& clauses, size_t n_variables, SATFormat format)
    : _n_variables(n_variables) 
    , _n_clauses(clauses.size())
    , _format(format)
    , _clauses(clauses)

{ }

STATUS SatSolver::from_str(const std::string& sat_str, SatSolver& out_result)
{
    std::stringstream ss(sat_str);
    return from_str_stream(ss, out_result);
}

STATUS SatSolver::from_str_stream(std::stringstream& sat_str_stream, SatSolver& out_result)
{
    std::string line;
    std::string format;
    std::string word;
    std::vector<Clause> clauses;
    size_t n_variables = 0, n_clauses = 0;
    std::vector<Variable> next_clause;

    bool preamble_ready = false;

    while(std::getline(sat_str_stream, line))
    {
        // Do nothing if first char is c, it's a comment 
        if (line[0] == 'c')
            continue;
        else if (line[0] == 'p' && preamble_ready)
            return FAILURE; // Already parsed preable
        else if (line[0] == 'p')
        {
            std::stringstream ss(line);
             if (!(ss >> word >> format >> n_variables >> n_clauses))
                return FAILURE;

            preamble_ready = true;;

            // Finished parsing preamble
            continue;
        }

        std::stringstream line_stream(line);
        // parsing numbers from this line
        while (std::getline(line_stream, word, ' ') && clauses.size() < n_clauses)
        {
            int current_variable;
            try 
            {
                current_variable = std::stoi(word);
            }
            catch(std::invalid_argument const& ex)
            {
                return FAILURE;
            }

            // If 0 and next clause is not empty, add it to list of clauses
            if (current_variable == 0 && !next_clause.empty())
            {
                clauses.emplace_back(next_clause);
                next_clause.clear();
            }

            // If 0, then skip
            if (current_variable == 0) continue;
            
            next_clause.push_back(current_variable);
        }

    }
    if (!next_clause.empty())
        clauses.emplace_back(next_clause);

    assert(format == "cnf" && "Only supported format is 'cnf'");

    // Create result 
    out_result = SatSolver(clauses, n_variables);
    return SUCCESS;
}

SatSolution SatSolver::solve()
{
    std::vector<int> state(_n_variables + 1);
    // Initialize state as -1, each variable can be modified as needed
    for(size_t i = 0; i < state.size(); i++)
        state[i] = -1;

    // Initialize unit clauses to their corresponding value to save computation
    for (auto const& clause : _clauses)
        if (clause.size() == 1)
        {
            Variable var = clause[0];
            int expected_state = expected_value(var);
            state[abs(var)] = expected_state;
        }
    // Filter unit clauses, those won't change 
    std::vector<Clause> new_clauses;
    for(auto const& clause : _clauses)
        if (clause.size() > 1)
            new_clauses.emplace_back(clause);

    // Perform backtracking to solve this sat
    if (!is_satisfiable(new_clauses, state, 0))
        // If we couldn't make it, return empty response
        return SatSolution{SatSatisfiable::UNSATISFIABLE, 0, std::vector<Variable>(), SATFormat::CNF};

    new_clauses.clear();

    // Build result
    std::vector<Variable> result(_n_variables);
    for(int i = 0; static_cast<size_t>(i) < result.size(); i++)
        if (state[i+1] == 1)
            result[i] = (i+1); 
        else
            result[i] = -(i+1); // set insecure variables to false, we want the minimum amount of trues
        
    return SatSolution{SatSatisfiable::SATISFIABLE, _n_variables, result, SATFormat::CNF};
}

void SatSolver::simplify()
{
    reduce_unit_clauses(_clauses, _n_variables);

    // Now sort every clause, so we can delete duplicates
    for(auto &clause : _clauses)
        std::sort(clause.begin(), clause.end());

    // remove duplicates
    std::sort(_clauses.begin(), _clauses.end());
    _clauses.erase(std::unique(_clauses.begin(), _clauses.end()), _clauses.end());


   // When you sort all clauses, there's a lot of clauses that end like -p ^ (-p v _),
   // and therefore you can remove them, since we only care about true variables
    std::vector<Clause> final_clauses;
    Variable next_clause_deleter = 0;
    for (auto const & clause : _clauses)
    {
        assert(clause.size() > 0 && "Invalid empty clause");

        if (clause.size() == 1)
        {
            next_clause_deleter = clause[0];
            final_clauses.emplace_back(clause);
            continue;
        }
        else if (clause.size() > 1 && clause[0] != next_clause_deleter) // if can't be deleted by this deleter, add it to result
        {
            final_clauses.emplace_back(clause);
            continue;
        }
    }
}

std::string SatSolver::as_str() const
{
    std::stringstream ss;

    std::string format_str;
    switch (_format)
    {
    case SATFormat::CNF:
        format_str = "cnf";
        break;
    default:
        assert(false && "format not supported");
        break;
    }

    // Write header
    ss << "p " << format_str << " " << _n_variables << " " << _n_clauses << std::endl;
    for (const auto &clause : _clauses )
    {
        for (const auto &var : clause)
            ss << var << " "; 
        ss << "0\n";
    }

    return ss.str();
}

bool SatSolver::is_satisfiable(const std::vector<Clause>& clauses, std::vector<int>& memo, size_t current_clause) const
{
    assert(memo.size() == _n_variables + 1 && "Not the right amount of variables");
    auto const& clause = clauses[current_clause];
    // A bool expresion is satisfiable if this clause is satisfiable and all the clauses are satisfiable

    if (current_clause >= clauses.size())
        return true;                        // assume last clause is true, neutral of and

    if(clause.size() == 2) // Special optimization for 2-sized clauses
    {
        size_t next_clause = current_clause + 1;
        // Search the next clause not 2-sized or that doesn't starts with first variable of current_clause
        Variable first_variable = clause[0];

        while(clauses[next_clause].size() == 2 && clauses[next_clause][0] == first_variable && next_clause < clauses.size())
            next_clause++;

        // applying rule (p v q) ^ (p v r) == p v (q ^ r) over every clause with the same starting p:
        
        // Try with first_variable set to its expected value
        auto first_variable_index = abs(first_variable);
        if (memo[first_variable_index] == -1)
        {
            memo[first_variable_index] = expected_value(first_variable);

            if (is_satisfiable(clauses, memo, next_clause))
                return true;
            
            memo[first_variable_index] = -1;
        }
        else if (memo[first_variable_index] == expected_value(first_variable))
        {
            return is_satisfiable(clauses, memo, next_clause);
        }

        // Check if all right side variables have memo == -1 or memo == expected.
        // If there is a variable such that memo != -1 && != expected, then we can't 
        // satisfy it and therefore the sat cannot be satisfied
        for(size_t i = current_clause; i < next_clause; i++)
        {   
            auto clause_index = abs(clauses[i][1]);
            if (memo[clause_index] != -1 && memo[clause_index] != expected_value(clauses[i][1]))
                return false;
        }
        
        // now that we know that all variables are either in their expected value
        // or able to change, change them 
        std::set<Variable> changed_indices; // to keep track which variables to rollback in case of failure
        for(size_t i = current_clause; i < next_clause; i++)
        {
            auto right_side = clauses[i][1];
            auto right_side_index = abs(right_side);
            auto right_side_expected = expected_value(right_side);

            if (memo[right_side_index] == -1)
            {
                changed_indices.insert(right_side_index);
                memo[right_side_index] = right_side_expected;
            }
        }

        if (is_satisfiable(clauses, memo, next_clause))
            return true;
        
        // We failed and sat can't be satisfied, but we have to rollback
        for(auto const chanched_index : changed_indices)
            memo[chanched_index] = -1;

        return false;
    }
    // -- End Optimization -----------------------------------------------------

    for(auto const variable : clause)
    {
        assert(variable != 0 && "Variable can't be 0");
        auto const var_index = abs(variable);

        // Select expected value for this variable
        int expected_value_for_var = expected_value(variable);
        
        // if value value of this variable is true, this clause is already satisfiable 
        if (memo[var_index] == -1)
        {
            // We can change it, but with responsability. 
            // if we win, we leave memo as it is. If not, we roll back our changes
            memo[var_index] = expected_value_for_var;
            if (is_satisfiable(clauses, memo, current_clause + 1))
            {
                return true;
            }
            
            // Rollback, we didn't win :(
            memo[var_index] = -1;
        }
        else if (memo[var_index] == expected_value_for_var)
            return is_satisfiable(clauses, memo, current_clause + 1);
        else  // If value of this variable is false, then we have to keep checking remaining variables 
            continue;
    }

    return false; // If we couldn't make this 
}

int SatSolver::expected_value(Variable var)
{
    int expected_value;
    if (var < 0)       // if variable is negated, we want it to be 0 so the result is true
        expected_value = 0;
    else if (var > 0)
        expected_value = 1; // If variable is not negated, we want it to just be true
    else 
        assert(false && "Variable shouldn't be zero");

    return expected_value;
}

void SatSolver::reduce_unit_clauses(std::vector<Clause>& clauses, size_t n_variables)
{
    // Init a vector with the state of variables, we will try to remove clauses that are already constant (they eval to true)
    std::set<Variable> unit_variables;
    bool change = true;
    while(change)
    {
        change = false;
        for(auto const& clause : clauses)
        {
            if (clause.size() == 1)
            {
                auto const elem = clause[0];
                if (unit_variables.find(elem) == unit_variables.end())
                {
                    change = true;
                    unit_variables.insert(elem);
                }
                continue;
            }
            
            // count how many clauses already are unit in this clause
            size_t units = 0;
            Variable possible_unit = 0;
            for (auto const elem : clause)
                if (unit_variables.find(elem) != unit_variables.end())
                    units++;
                else
                    possible_unit = elem;
            
            // Then we have a unit-able clause
            if (units == clause.size() - 1)
            {
                unit_variables.insert(possible_unit);
                change = true;
            }
        }
    }

    std::vector<Clause> new_clauses;
    for (auto const& clause : clauses)
    {
        bool has_units = false;
        for (auto const var : clause)
        {
            if (unit_variables.find(var) != unit_variables.end())
            {
                new_clauses.emplace_back(Clause{var});
                has_units = true;
            }
        }
        if (!has_units)
            new_clauses.emplace_back(clause);
    }

    clauses = new_clauses;
}
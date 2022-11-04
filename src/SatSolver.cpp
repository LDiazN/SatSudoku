#include "SatSolver.hpp"
#include <iostream>
#include <sstream>


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
            int expected_state;
            Variable var = clause[0];
            if (var < 0)
                expected_state = 0;
            else if (var > 0)
                expected_state = 1;
            else 
                assert(false && "variables shouldn't be zero");
            
            state[abs(var)] = expected_state;
        }

    // Perform backtracking to solve this sat
    if (!is_satisfiable(_clauses, state, 0))
        // If we couldn't make it, return empty response
        return SatSolution{SatSatisfiable::UNSATISFIABLE, 0, std::vector<Variable>(), SATFormat::CNF};

    // Build result
    std::vector<Variable> result(_n_variables);
    for(int i = 0; static_cast<size_t>(i) < result.size(); i++)
        if (state[i+1] == 1)
            result[i] = (i+1); 
        else
            result[i] = -(i+1); // set insecure variables to false, we want the minimum amount of trues
        
    return SatSolution{SatSatisfiable::SATISFIABLE, _n_variables, result, SATFormat::CNF};
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

    for(auto const variable : clause)
    {
        assert(variable != 0 && "Variable can't be 0");
        auto const var_index = abs(variable);

        // Select expected value for this variable
        int expected_value;
        if (variable < 0)       // if variable is negated, we want it to be 0 so the result is true
            expected_value = 0;
        if (variable > 0)
            expected_value = 1; // If variable is not negated, we want it to just be true
        
        // if value value of this variable is true, this clause is already satisfiable 
        if (memo[var_index] == -1)
        {
            // We can change it, but with responsability. 
            // if we win, we leave memo as it is. If not, we roll back our changes
            memo[var_index] = expected_value;
            if (is_satisfiable(clauses, memo, current_clause + 1))
            {
                return true;
            }
            
            // Rollback, we didn't win :(
            memo[var_index] = -1;
        }
        else if (memo[var_index] == expected_value)
            return is_satisfiable(clauses, memo, current_clause + 1);
        else  // If value of this variable is false, then we have to keep checking remaining variables 
            continue;
    }

    return false; // If we couldn't make this 
}
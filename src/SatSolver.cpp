#include "SatSolver.hpp"
#include <iostream>
#include <sstream>
#include <set>
#include <algorithm>
#include <map>
#include <queue>

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

SatSolution SatSolver::solve()
{
    // Initialize state as -1, each variable can be modified as needed
    std::vector<int> state(_n_variables + 1, -1);

    // Initialize unit clauses to their corresponding value to save computation
    reduce_unit_clauses(_clauses, state);

    // Reduce literals 
    literal_elimination(_clauses, state);
    simplify();

    // Convert clauses to literals
    clauses_to_literal();
    auto watchlist = create_watchlist(state);

    if (!solve_by_watchlist(watchlist, state))
    {
        return SatSolution{SatSatisfiable::UNSATISFIABLE, 0, std::vector<Variable>(), SATFormat::CNF};
    }

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
    _clauses = final_clauses;
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

void SatSolver::clauses_to_literal()
{
    for(auto& clause : _clauses)
        for (size_t i = 0; i < clause.size(); i++)
            clause[i] = variable_to_literal(clause[i]);
}

SatSolver::Watchlist SatSolver::create_watchlist(const std::vector<int>& state) const
{
    // ASSUME CLAUSES IS IN RIGHT FORMAT
    Watchlist watchlist(2 * (_n_variables + 1));
    for(auto const& clause : _clauses)
    {
        for(auto const elem : clause)
        {
            // Find first element that is either true or not assigned
            auto elem_index = (literal_to_variable(elem));
            if (state[elem_index] == -1 || state[elem_index] == 1)
            {
                watchlist[elem].push_back(clause);
                break;
            }
        }
    }

    return watchlist;
}

bool SatSolver::update_watchlist(Watchlist & watchlist, int neg_literal, const std::vector<int>& state) const
{
    while(!watchlist[neg_literal].empty())
    {
        auto const& clause = watchlist[neg_literal].back();
        bool alternative_found = false;
        bool remove_last = false;
        for(auto const alternative : clause)
        {
            auto variable = literal_to_variable(alternative);
            auto parity = alternative & 1;
            if (state[variable] == -1 || state[variable] == (parity ^ 1))
            {
                alternative_found = true;
                watchlist[alternative].push_back(clause);
                remove_last = true;
                break;
            }
        }

        if (remove_last)
            watchlist[neg_literal].pop_back();
        if (!alternative_found)
            return false;
    }

    return true;
}

bool SatSolver::solve_by_watchlist(Watchlist& watchlist, std::vector<int>& state, Variable next_var)
{
    if (static_cast<size_t>(next_var) == _n_variables + 1)
        return true;

    bool result = false;
    for(int i = 0; i < 2; i++)
    {
        state[next_var] = i;
        if (update_watchlist(watchlist, (next_var << 1) | i, state))
        {
            result = solve_by_watchlist(watchlist, state, next_var+1);
            if (result) break;
        }
    }

    if (result)
        return true;

    state[next_var] = -1;
    return false;
}

void SatSolver::reduce_unit_clauses(std::vector<Clause>& clauses, std::vector<int>& state)
{
    // Collect all unit clauses
    for(auto const& clause : clauses)
        if (clause.size() == 1)
        {
            auto const var = clause[0];
            auto const var_index = abs(var);
            state[var_index] = expected_value(var);
        }

    constant_reduction(clauses, state);
}

void SatSolver::literal_elimination(std::vector<Clause>& clauses, std::vector<int>& state)
{
    std::set<Variable> positive_variables;
    std::set<Variable> negative_variables;
    for(auto const& clause : clauses)
        for(auto const var : clause)
            if (var < 0)
                negative_variables.insert(abs(var));
            else 
                positive_variables.insert(abs(var));

    for(int i = 1; static_cast<size_t>(i) < state.size(); i++ )
    {
        bool in_positive = positive_variables.find(i) != positive_variables.end();
        bool in_negative = negative_variables.find(i) != negative_variables.end();

        if ( in_negative && !in_positive)
            state[i] = 0;
        else if (in_positive && !in_negative)
            state[i] = 1;
    }

    constant_reduction(clauses, state);
}

void SatSolver::constant_reduction(std::vector<Clause>& clauses, std::vector<int>& state)
{
     // Now perform constant reduction
    std::vector<Clause> new_clauses;
    Clause next_clause;
    for(auto const& clause : clauses)
    {
        next_clause.clear();

        for(auto const var : clause)
        {
            auto const var_index = abs(var);
            auto const var_state = state[var_index];

            // If 1, this clause adds nothing
            if ((var_state == 1 && var > 0) || (var_state == 0 && var < 0)) // only add variable to clause if not constant
            {
                next_clause.clear();
                break;
            }
            else if ((var_state == 1 && var < 0) || (var_state == 0 && var > 0)) // if variable evals to false, then don't add it to the clause
                continue;
            else if (var_state == -1) // if unassigned, just add it
                next_clause.push_back(var);
            else 
            {
                assert(false && "invalid state");
            }
        }

        if (!next_clause.empty())
            new_clauses.emplace_back(next_clause);
    }
    clauses = new_clauses;
}

int SatSolver::eval(const std::vector<Clause>& clauses,const std::vector<int>& state)
{
    for(auto const& clause : clauses)
    {
        bool clause_is = false;
        bool has_chance = false;
        for(auto const var : clause)
        {
            auto const var_index = abs(var);

            // If variable evals to what we want it to be, then we're done with this clause
            if (state[var_index] == expected_value(var))
            {
                clause_is = true;
                break;
            }
            if (state[var_index] == -1) // if a variable has a chance to be true with further assignments
                has_chance = true;
        }

        if (clause_is == false && !has_chance)
            return 0;
        else if (clause_is == false && has_chance)
            // we can't tell if entire expression is true, 
            // since we still don't know if we can set this clause to a true state 
            return -1; 
        // If clause is true, just keep going
    }

    return 1;
}
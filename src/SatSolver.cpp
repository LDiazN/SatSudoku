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
    return SatSolution();
}

std::string SatSolver::as_str() const
{
    std::stringstream ss;

    // Write header
    ss << "p " << _n_variables << " " << _n_clauses << std::endl;
    for (const auto &clause : _clauses )
    {
        for (const auto &var : clause)
            ss << var << " "; 
        ss << "0\n";
    }

    return ss.str();
}
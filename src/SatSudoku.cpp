
#include "SatSudoku.hpp"
#include "Sudoku.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <filesystem>
#include <chrono>
#include <thread>
#include <future>
#include "Colors.hpp"

SatSudoku::SatSudoku(float max_time, const std::string& file, bool file_is_sudoku)
    : _file(file)
    , _time(max_time)
    , _file_is_sudoku(file_is_sudoku)
{ }

void SatSudoku::run()
{
    // Sanity check
    if (!std::filesystem::exists(_file))
    {
        std::cerr << "The specified file " << _file << " Does not exists\n";
        return;
    }

    if(_time < 0)
    {
        std::cerr << "Time should be a positive number or 0. Provided time: " << _time << std::endl;
        return;
    }

    if (_file_is_sudoku)
        run_sudoku_solver();
    else 
        run_sat_solver();
}

Sudoku SatSudoku::solve_sudoku(const Sudoku& sudoku)
{
    // Time each step in this function
    std::cout << "Converting from sudoku to sat..." << std::endl;
    auto solve_start = std::chrono::high_resolution_clock::now();
    SatSolver sat = sudoku.as_sat();
    auto sudoku_2_sat_duration =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - solve_start);
    std::cout << "Convertion done in " << YELLOW << sudoku_2_sat_duration.count() << " ms\n" << RESET;

    std::cout << "Solving sudoku..." << std::endl;
    auto sat_solver_start = std::chrono::high_resolution_clock::now();
    auto solution = sat.solve();
    auto satsolver_duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - sat_solver_start);
    std::cout << "SAT Solver done in " << YELLOW << satsolver_duration.count() << " ms\n" << RESET;

    std::cout << "Converting from SAT back to sudoku..." << std::endl;
    auto sol_2_sudoku_start = std::chrono::high_resolution_clock::now();
    auto solved_sudoku = Sudoku::from_sat_sol(solution);
    auto sol_2_sudoku_duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - sol_2_sudoku_start);
    std::cout << "Convertion done in " << YELLOW << sol_2_sudoku_duration.count() << " ms\n" << RESET;
    std::cout << "Entire process finished in " << YELLOW << sudoku_2_sat_duration.count() + satsolver_duration.count() + sol_2_sudoku_duration.count() << " ms\n" << RESET;
    return solved_sudoku;
}

void SatSudoku::run_sudoku_solver()
{
    std::ifstream fs(_file);
    std::string line;
    // Just a line counter for debug
    size_t n_lines = 0;
    std::cout << "Processing file: " << _file << std::endl;
    while(std::getline(fs, line)) // Iterate over each sudoku line in file
    {
        n_lines++;
        std::cout << BLUE << "Reading Sudoku at line " << n_lines << RESET <<  std::endl;

        std::stringstream ss(line);
        size_t sudoku_order = 0;
        // Parse sudoku order
        ss >> sudoku_order;
        std::string next_num_str;

        // Create sudoku with specified size
        Sudoku sudoku(sudoku_order);

        // Parse numbers separated by -
        size_t num_index = 0;
        auto &board = sudoku.get_board();
        auto board_size = sudoku_order * sudoku_order;
        while(std::getline(ss, next_num_str, '-'))
        {
            uint next_num;
            size_t board_row = num_index / board_size;
            size_t board_col = num_index % (board_size);

            auto result = sscanf(next_num_str.c_str(), "%u", &next_num);
            if(result == 0)
            {
                std::cerr << "Invalid number in position " << num_index << ". Given number: " << next_num_str << std::endl;
                return;
            }

            board.set(board_row, board_col, next_num);

            num_index++;
        }

        // Show sudoku to be solved
        sudoku.display();
        Sudoku solution(0);
        if (_time == 0) // If time == 0, just solve it whenever it's ready
            solution = solve_sudoku(sudoku);
        else // Otherwise, wait for the specified ammount of time
        {
            std::future<Sudoku> solve_thread = std::async(SatSudoku::solve_sudoku, sudoku);
            auto result_ready = solve_thread.wait_for(std::chrono::seconds( (int) _time));
            if (result_ready != std::future_status::ready)
            {
                std::cout << RED << "Time Limit Exceeded, killing solve thread" << RESET << std::endl;
                // TODO kill process when time limit is exceeded
            }
            solution = solve_thread.get();
        }
        
        std::cout << GREEN << "Solved Sudoku: " << RESET << std::endl;
        solution.display();
    }
}
void SatSudoku::run_sat_solver()
{
    std::ifstream file_stream(_file);
    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    SatSolver sat(std::vector<Clause>(), 0);

    std::cout << BLUE << "Reading SAT from file: " << _file << "...\n" << RESET;
    auto status = SatSolver::from_str_stream(buffer, sat);
    if (status == FAILURE)
    {
        std::cerr << RED << "Unable to parse SAT from file " << _file << RESET << std::endl;
        return;
    }

    SatSolution solution;
    std::cout << BLUE << "Solving SAT..." << RESET << std::endl;
    if (_time == 0) // If time == 0, just solve it whenever it's ready
        solution = sat.solve();
    else // Otherwise, wait for the specified ammount of time
    {
        std::future<SatSolution> solve_thread = std::async(&SatSolver::solve, &sat);
        auto result_ready = solve_thread.wait_for(std::chrono::seconds( (int) _time));
        if (result_ready != std::future_status::ready)
        {
            std::cout << RED << "Time Limit Exceeded, killing solve thread" << RESET << std::endl;
            // TODO kill process when time limit is exceeded
        }
        solution = solve_thread.get();
    }
    
    std::cout << GREEN << "SAT Solved!" << RESET << std::endl;
    std::cout << GREEN << "Solution:" << RESET << std::endl;
    solution.display();
}


#include "Orchestration.hpp"
// #include "Colors.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <cmath>


Sudoku read_sudoku() {
    size_t sudoku_order = 0;

    // Parse sudoku order
    std::cin >> sudoku_order;

    std::string next_num_str;

    // Create sudoku with specified size
    Sudoku sudoku(sudoku_order);

    // Parse numbers separated by -
    size_t num_index = 0;
    auto &board = sudoku.get_board();
    auto board_size = sudoku_order * sudoku_order;
    while(std::getline(std::cin, next_num_str, '-'))
    {
        uint next_num = 0;
        size_t board_row = num_index / board_size;
        size_t board_col = num_index % (board_size);

        auto result = sscanf(next_num_str.c_str(), "%u", &next_num);
        if(result == 0)
        {
            std::cerr << "Invalid number in position " << num_index << ". Given number: " << next_num_str << std::endl;
            exit(-1); // ! OJO
        }

        board.set(board_row, board_col, next_num);

        num_index++;
    }

    return sudoku;
}

//---------------------------------------------------


void sudoku_to_sat() {
    Sudoku sudoku = read_sudoku(); // ! Ugly but we'll see
    SatSolver sat = sudoku.as_sat();

    sat.simplify();
    std::cout << sat.as_str();  // ! OJO
}



//---------------------------------------------------


// TODO part B
void solver_caller()  {

    signal(SIGUSR1, usr_sig_handler);

    // !READ PART
    std::stringstream buffer;
    buffer << std::cin.rdbuf();

    SatSolver sat(std::vector<Clause>(), 0);

    //std::cout << BLUE << "Reading SAT from file: " << _file << "...\n" << RESET;
    auto status = SatSolver::from_str_stream(buffer, sat);
    if (status == FAILURE) {
        // TODO handle FAILURE state
    }

    // ! SOLVE PART

    SatSolution solution;

    auto solve_start = std::chrono::high_resolution_clock::now();
    solution = sat.solve();
    auto solver_duration =  std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - solve_start);
    finished = 1 ;

    std::cerr<<"Solved. Elapsed time: "<<solver_duration.count()/pow(10,6)<<"ms\n";

    // ! DISPLAY PART

    solution.display();
/*
    switch (solution.satisfiable)
    {
    case SatSatisfiable::SATISFIABLE:
        std::cout << GREEN << "Sudoku has solution!" << RESET << std::endl;
        break;
    case SatSatisfiable::UNSATISFIABLE:
        std::cout << RED << "Sudoku has no solution  T.T" << RESET << std::endl;
        return sudoku;
        break;
    case SatSatisfiable::UNKNOWN:
        std::cout << RED << "Unable to find a solution for this sudoku" << RESET << std::endl;
        return sudoku;
        break;
    }
*/
}

//---------------------------------------------------

// TODO part C
void sat_to_sudoku() {
    // TODO read sudoku
    Sudoku sudoku = read_sudoku();
    std::cout<<"Sudoku instance: \n";
    sudoku.display();

    // TODO read SAT solution

    // TODO display unsolved sudoku
    // TODO display solved sudoku
}
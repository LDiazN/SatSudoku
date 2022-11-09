
#include "ForeGround.hpp"
// #include "Colors.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <cmath>

void ForeGround::init() {
    ForeGround::finished = 0;
}

STATUS ForeGround::read_sudoku(Sudoku &in_sudoku) {
    size_t sudoku_order = 0;

    // Parse sudoku order
    std::cin >> sudoku_order;

    // Create sudoku with specified size
    Sudoku sudoku(sudoku_order);

    std::string next_num_str;
    auto &board = sudoku.get_board();
    auto board_size = sudoku_order * sudoku_order;

    // Get current line to parse
    std::string line; 
    std::getline(std::cin, line); 
    std::stringstream ss(line);

    //Parse table
    for (size_t num_index=0 ; num_index < board_size*board_size; ++num_index) 
    {
        std::getline(ss, next_num_str, '-'); // Use - as separator

        uint next_num = 0;
        size_t board_row = num_index / board_size;
        size_t board_col = num_index % (board_size);

        auto result = sscanf(next_num_str.c_str(), "%u", &next_num);
        if(result == 0)
        {
            std::cerr << "Invalid number in position " << num_index << ". Given number: " << next_num_str << std::endl;
            return FAILURE; 
        }

        board.set(board_row, board_col, next_num);

    }

    in_sudoku = sudoku;
    return SUCCESS;
}

//---------------------------------------------------


STATUS ForeGround::sudoku_to_sat() {
    Sudoku sudoku(0);
    
    auto status = read_sudoku(sudoku); // ! Ugly but we'll see
    if (status == FAILURE)
        return FAILURE;

    SatSolver sat = sudoku.as_sat();

    sat.simplify();
    std::cout << sat.as_str();  // ! OJO

    return SUCCESS;
}



//---------------------------------------------------


void ForeGround::usr_sig_handler(int sig_id) {
    if (ForeGround::finished == 0) {
        exit(42);
    }
}

STATUS ForeGround::solver_caller()  {

    signal(SIGUSR1, ForeGround::usr_sig_handler);

    // * Read SAT 
    std::stringstream buffer;
    buffer << std::cin.rdbuf();

    SatSolver sat(std::vector<Clause>(), 0);

    auto status = SatSolver::from_str_stream(buffer, sat);
    if (status == FAILURE) 
    {
        return FAILURE;
    }

    // * Solve SAT

    SatSolution solution;

    auto solve_start = std::chrono::high_resolution_clock::now();
    solution = sat.solve();
    auto solver_duration =  std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - solve_start);
    ForeGround::finished = 1 ;

    std::cerr<<"Solved. Elapsed time: "<<solver_duration.count()/pow(10,6)<<"ms\n";

    // * Display solution

    solution.display();

    return SUCCESS;
}

//---------------------------------------------------

STATUS ForeGround::read_sat_solution(struct SatSolution& solution) {

    std::string line, mode;
    // Skiping comments
    while (std::getline(std::cin, line)) { 
        std::stringstream extractor(line);

        extractor >> mode;
        //std::cout<<line<<std::endl;

        if (mode.size() == 0)
        {
            std::cerr<<"Invalid format. Exiting.. \n";
            return FAILURE;
        }
            
        if (mode[0] != 'c')
            break;
    }

    if (mode[0] != 's') 
    { 
        std::cerr<<"Solution line comes after comments. Exiting..\n";
        return FAILURE;
    }

    // * Read preamble
        
    // Format validation
    std::string s_format, s_satisfiable, s_n_vars;
    std::stringstream extractor(line);
    if (!(extractor >> mode >> s_format >> s_satisfiable >> s_n_vars))
    {
        std::cerr<<"4 Fields are required for solution line. Exiting..\n";
        return FAILURE;
    }

    SATFormat format;
    SatSatisfiable satisfiable;
    size_t n_vars;

    // Types validation
    try 
    {
        int tmp;
        if (s_format.compare("cnf") != 0) 
        {
            std::cerr<<"Only CNF format is supported. Exiting..\n";
            return FAILURE;
        }
        else format = CNF;

        tmp = std::stoi(s_satisfiable);
        if (abs(tmp) > 1) 
        {
            std::cerr<<s_satisfiable<<" is not a valid solution specifier. Exiting..\n";
            return FAILURE;
        }
        else {
            switch (tmp) {
                case SATISFIABLE:
                    satisfiable = SATISFIABLE;
                break;
                case UNSATISFIABLE:
                    satisfiable = UNSATISFIABLE;
                break;
                case UNKNOWN:
                    satisfiable = UNKNOWN;
                break;
                default:
                    std::cerr<<s_satisfiable<<" is not a valid solution specifier. Exiting..\n";
                    return FAILURE;
                break;
            }
        }
        n_vars = std::stoi(s_n_vars);
    }
    catch(std::invalid_argument const& ex) 
    {
        std::cerr<<"Invalid format for solution line. Exiting..\n";
        return FAILURE;
    }

    if (satisfiable != 1) { // TODO discard invalid and unsatisfiable 
        solution.satisfiable = satisfiable;
        solution.n_variables = n_vars;
        solution.variable_states = std::vector<Variable>();
        solution.format = format;
        return SUCCESS;
    }

    std::vector<Variable> states(n_vars);

    // * Read variables
    for (int i=0; i<static_cast<int>(n_vars); ++i)  {
        std::getline(std::cin, line);
        std::stringstream extractor(line);

        if ( !(extractor>>mode>>states[i]) || mode.compare("v") != 0) 
        {
            std::cerr<<"Invalid variable specifier. Exiting..\n";
            return FAILURE;
        }
    }

    solution.satisfiable = satisfiable;
    solution.n_variables = n_vars;
    solution.variable_states = states;
    solution.format = format;

    return SUCCESS;
}

STATUS ForeGround::sat_to_sudoku() {
    // * Read and display sudoku instance
    Sudoku sudoku(0);

    STATUS status;
    
    status = read_sudoku(sudoku);

    if (status == FAILURE)
        return FAILURE;

    std::cout<<"Sudoku instance: \n";
    sudoku.display();

    // * Read SAT solution
    struct SatSolution solution;
    status = read_sat_solution(solution) ;

    if (status == FAILURE) 
        return FAILURE;

    solution.as_str();
    sudoku.add_sat_solution(solution);

    // * Display solved sudoku
    std::cout<<"Sudoku solution:\n";
    sudoku.display();

    return SUCCESS;
}
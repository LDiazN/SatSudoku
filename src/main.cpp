
#include<iostream>
#include<string>

#include"ForeGround.hpp"

int main(int argc, char** argv) 
{
    if (argc == 1) 
    {
        std::cout<<"Usage\n\t./SatSolver <options> <arguments>\n";
        std::cout<<"Valid options:\n\t--toSAT <sudoku> : where sudoku is a valid sudoku description\n";
        std::cout<<"\t--solve <cnf>: where cnf is a valid cnf SAT description\n";
        std::cout<<"\t--toSudoku <SAT>: where SAT is a valid SAT description\n";
        return 0;
    }

    const std::string option_string = argv[1];
    STATUS result;

    ForeGround call;
    call.init();

    if (option_string.compare("--toSAT") == 0) 
        result = call.sudoku_to_sat();
    else if (option_string.compare("--solve") == 0) 
        result = call.solver_caller();
    else if (option_string.compare("--toSudoku") == 0) 
        result = call.sat_to_sudoku();
    else  {
        std::cerr<<option_string<<" Is not a valid option of SatSolver\n";    
        return -1;
    }

    if (result == FAILURE)
        return 1;
    
    return 0;
}
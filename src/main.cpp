
#include<iostream>
#include<string>
#include"Orchestration.hpp"

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


    //std::cout<<"DBG::"<<option_string<<"\n";
    //std::cout<<"DBG::"<<line<<"\n";

    if (option_string.compare("--toSAT") == 0) 
        sudoku_to_sat();
    else if (option_string.compare("--solve") == 0) 
        solver_caller();
    else if (option_string.compare("--toSudoku") == 0) 
        sat_to_sudoku();
    else  {
        std::cerr<<option_string<<" Is not a valid option of SatSolver\n";    
        return -1;
    }

    // ! File existence validation is left out to orquestrator

    
    return 0;
}
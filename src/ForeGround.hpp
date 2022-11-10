#ifndef ORCHESTRATION_HPP
#define ORCHESTRATION_HPP

#include "Sudoku.hpp"

#include <iostream>
#include <csignal>

#define SIGUSR1 10

class ForeGround {
public:
    static sig_atomic_t finished ; 
    ForeGround(){};

    static void init();
    
    STATUS read_sudoku(Sudoku &in_sudoku);
    STATUS sudoku_to_sat();

    static void usr_sig_handler(int sig_id) ;
    static STATUS solver_caller();

    STATUS read_sat_solution(struct SatSolution& solution);
    STATUS sat_to_sudoku();
} ;


#endif
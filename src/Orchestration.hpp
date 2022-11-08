#ifndef ORCHESTRATION_HPP
#define ORCHESTRATION_HPP

#include "Sudoku.hpp"

#include <iostream>
#include <csignal>

sig_atomic_t finished = 0;

#define SIGUSR1 10

void usr_sig_handler(int sig_id) {
    if (finished == 0) {
        std::cout<<"s cnf -1 x";
        exit(42);
    }

}


Sudoku read_sudoku();
void sudoku_to_sat();
void solver_caller();
void sat_to_sudoku();

#endif
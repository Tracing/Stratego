#include <cstdbool>
#include <deque>
#include <iostream>
#include "state.h"
#include "move.h"
#include "enums.h"

#ifndef TESTS_H
#define TESTS_H

void do_test(int* total_tests, int* successes, bool success, string name);
bool all_but_square_empty(GameState state, int x, int y);
bool has_move(GameState state, Move move);
void erase_owner_matrix(GameState* state);
void run_do_move_tests();
void run_get_scout_moves_tests();
void run_get_move_normal_tests();
void run_deployment_tests();

#endif
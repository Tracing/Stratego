#include <cmath>
#include <iostream>
#include <cfloat>

#include "enums.h"
#include "helper.h"
#include "move.h"
#include "state.h"

#ifndef AI_FUNCTIONS_H
#define AI_FUNCTIONS_H
Player do_simulation(GameState* state, int max_depth);
double distance_to_unknown_pieces(GameState* state, Player player);
std::vector<DeploymentMove> get_deployment_plan(Player player, int n_simulations, int n_plans, int simulation_depth);
double heuristic(GameState* state, Player player);
void do_semirandom_deployment(GameState* state, std::vector<DeploymentMove>* deploymentMoves);
double move_heuristic(GameState* state, Move* move);
void sortMoves(GameState* state, std::vector<Move>* moves);
#endif

#include <cstdbool>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "ai_functions.h"
#include "enums.h"
#include "move.h"
#include "state.h"
#include "tests.h"

#ifndef ENGINE_H
#define ENGINE_H

void do_random_game();
void print_game_state(GameState* state);

class Agent {
    public:
        virtual void setup(void) {
            srand(time(NULL));
        }

        virtual void teardown(void) {
            ;
        }

        virtual void get_move(GameState* state, Move* bestMove) {
            *bestMove = state->getMoves().at(rand() % state->getMoves().size());
        }

        virtual void do_deployment(GameState* state) {
            std::vector<DeploymentMove> deploymentMoves;
            do_semirandom_deployment(state, &deploymentMoves);
        }
};

class Game {
    public:
        GameState state;
        void setAgent(Player player, Agent* agent);
        Player play(int turn_limit, bool verbose);
        GameState getGameState();
    private:
        Agent* redAgent;
        Agent* blueAgent;
        int lastRedMove;
        int lastBlueMove;
};

#endif

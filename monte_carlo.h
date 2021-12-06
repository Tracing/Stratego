#include "ai_functions.h"
#include "engine.h"
#include <pthread.h>

#ifndef MONTE_CARLO_H
#define MONTE_CARLO_H


class MonteCarloAgent : public Agent
{
public:
    MonteCarloAgent(int n, int d, int m, double betas[4]);
    MonteCarloAgent(int n, int d, int m);
    MonteCarloAgent(int n, int m);
    void setup(void) override {
        srand(time(NULL));
    }

    void teardown(void) override {
        ;
    }

    void get_move(GameState* state, Move* bestMove) override;
    void do_deployment(GameState* state) override;

    double beta1 = 0.25;
    double beta2 = 0.25;
    double beta3 = 0.25;
    double beta4 = 0.25;
private:
    int n_rollouts = 800;
    int rollout_depth = 1000;
    int n_best_moves = 5;
    double heuristic(GameState* state, Player player);
};

#endif // MONTE_CARLO_H

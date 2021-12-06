#ifndef MCTS_H
#define MCTS_H

#include <deque>

#include "ai_functions.h"
#include "engine.h"

struct MCTSResult {
    Move bestMove;
    int n_simulations;
    double root_reward;
    int move_node_visits;
    double move_node_reward;
};

struct MCTSNode {
    MCTSNode* parent = nullptr;
    GameState state;
    float rewards[2] = {0, 0};
    float visits = 0;
    vector<MCTSNode> children;
};

class MCTS: public Agent
{
public:
    MCTS(int n_deployment_plans, int n_deployment_simulations, int deployment_simulation_depth, int simulations_per_move) {
        this->simulations_per_move = simulations_per_move;
        this->n_deployment_plans = n_deployment_plans;
        this->n_deployment_simulations = n_deployment_simulations;
        this->deployment_simulation_depth = deployment_simulation_depth;
    }
    void get_move(GameState* state, Move* bestMove) override;
    void setup(void) override;
    int n_deployment_plans;
    int n_deployment_simulations;
    int deployment_simulation_depth;
    int simulations_per_move;
    std::deque<DeploymentMove> deploymentPlan;
    double C = 1 / sqrt(2);
private:
    struct MCTSResult search(GameState* state, Move* bestMove);
    float ucb_score(MCTSNode* p, MCTSNode* c);
    MCTSNode* traverse(MCTSNode* tree);
};

#endif // MCTS_H

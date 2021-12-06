#include "mcts.h"

MCTSNode new_node(MCTSNode* parent, Move move) {
    MCTSNode node;
    GameState state = parent->state.copy();
    state.doMove(move);
    node.parent = parent;
    node.rewards[0] = 0.0;
    node.rewards[1] = 0.0;
    node.state = state;
    node.visits = 0.0;
    return node;
}

void MCTS::get_move(GameState* state, Move* bestMove) {
    std::vector<Move> moves;

    moves = state->getMoves();

}

float MCTS::ucb_score(MCTSNode* p, MCTSNode* c) {
    return p->rewards[p->state.getPlayerTurn()] / p->visits * C * sqrt(log(p->visits) / c->visits);
}

MCTSNode* MCTS::traverse(MCTSNode* tree) {
    MCTSNode* current = tree;
    int bestNode;
    float score;
    float bestScore = FLT_MIN;
    while(current->children.size() > 0) {
        for(int i = 0; i < (int) current->children.size(); i++) {
            score = ucb_score(current, &current[i]);
            if(score > bestScore) {
                bestScore = score;
                bestNode = i;
            }
        }
        current = &current->children[bestNode];
    }
    return current;
}

void expand(MCTSNode* tree, Move move) {
    MCTSNode node = new_node(tree, move);
    tree->children.push_back(node);
}

struct MCTSResult MCTS::search(GameState* state, Move* bestMove) {
    struct MCTSResult result;
    int simulation = 0;
    MCTSNode root;
    MCTSNode* current = &root;
    GameState _state = state->copy();
    while(simulation < simulations_per_move) {
        current = traverse(&root);

        simulation++;
    }
}

void MCTS::setup() {
    ;
}



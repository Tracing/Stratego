#include "ai_functions.h"

Player do_simulation(GameState* state, int max_depth) {
    std::vector<DeploymentMove> deploymentMoves;
    std::vector<Move> moves;
    int depth = 0;

    while(!state->hasWinner() && depth < max_depth) {
        if(state->state == deployment) {
            deploymentMoves = state->getDeploymentMoves();
            state->doDeploymentMove(deploymentMoves.at(rand() % deploymentMoves.size()));
        } else {
            moves = state->getMoves();
            state->doMove(moves.at(rand() % moves.size()));
        }
        depth += 1;
    }
    return state->getWinner();
}

void deployBombIfInBoard(GameState* state, int x, int y, std::vector<DeploymentMove>* deploymentMoves) {
    if(inBoard(x, y)) {
        state->doDeploymentMove(DeploymentMove(x, y, bomb));
        deploymentMoves->push_back(DeploymentMove(x, y, bomb));    
    }
}

void do_semirandom_deployment(GameState* state, std::vector<DeploymentMove>* deploymentMoves) {
    int flagx = rand() % 10;
    int flagy = state->player_turn == red ? rand() % 2 : 9 - rand() % 2;
    Player player = state->player_turn;
    DeploymentMove deploymentMove;

    state->doDeploymentMove(DeploymentMove(flagx, flagy, flag));
    deploymentMoves->push_back(DeploymentMove(flagx, flagy, flag));
    deployBombIfInBoard(state, flagx + 1, flagy, deploymentMoves);
    deployBombIfInBoard(state, flagx - 1, flagy, deploymentMoves);
    deployBombIfInBoard(state, flagx, flagy + 1, deploymentMoves);
    deployBombIfInBoard(state, flagx, flagy - 1, deploymentMoves);
    while(state->state == deployment && state->player_turn == player) {
        deploymentMove = state->getDeploymentMoves().at(rand() % state->getDeploymentMoves().size());
        state->doDeploymentMove(deploymentMove);
        deploymentMoves->push_back(deploymentMove);
    }
}

std::vector<DeploymentMove> get_deployment_candidate(Player player, int n_simulations, int simulation_depth, double* plan_score) {
    std::vector<DeploymentMove> deploymentPlan;
    std::vector<DeploymentMove> throwawayDeploymentPlan;
    std::vector<DeploymentMove>::iterator it;
    GameState currentState = GameState();
    Player otherPlayer = player == red ? blue : red;
    Player winner;
    double visits = 0;
    double score = 0;

    currentState.newGame();

    if(player == red) {
        assert(currentState.player_turn == red);
        do_semirandom_deployment(&currentState, &deploymentPlan);
        assert(currentState.player_turn == blue);
        do_semirandom_deployment(&currentState, &throwawayDeploymentPlan);
    } else {
        assert(currentState.player_turn == red);
        do_semirandom_deployment(&currentState, &throwawayDeploymentPlan);
        assert(currentState.player_turn == blue);
        do_semirandom_deployment(&currentState, &deploymentPlan);
    }
    assert(deploymentPlan.size() == 40);

    for(int i = 0; i < n_simulations; i++) {
        throwawayDeploymentPlan.clear();

        currentState = GameState();
        currentState.newGame();
        if(player == blue) {
            do_semirandom_deployment(&currentState, &throwawayDeploymentPlan);

            for(it = deploymentPlan.begin(); it < deploymentPlan.end(); it++) {
                currentState.doDeploymentMove(*it);
            }
        } else {
            for(it = deploymentPlan.begin(); it < deploymentPlan.end(); it++) {
                currentState.doDeploymentMove(*it);
            }
            do_semirandom_deployment(&currentState, &throwawayDeploymentPlan);
        }
        winner = do_simulation(&currentState, simulation_depth);
        visits += 1.0;
        score += winner == player ? 1.0 : (winner == otherPlayer ? 0.0 : 0.5);
    }
    *plan_score = score / visits;
    return deploymentPlan;
}

double distance_to_unknown_pieces(GameState* state, Player player) {
    double enemy_x, enemy_y = 0;
    double num_pieces = 0;
    double num_enemy_pieces = 0;
    double distance = 0;

    Player other_player = player == red ? blue : red;

    for(int x = 0; x < 10; x++) {
        for(int y = 0; y < 10; y++) {
            if(state->getOwner(x, y) == other_player && state->getVisibility(x, y) == unknown) {
                num_enemy_pieces += 1;
                enemy_x += (double) x;
                enemy_y += (double) y;
            }
        }
    }

    enemy_x = enemy_x / num_enemy_pieces;
    enemy_y = enemy_y / num_enemy_pieces;

    for(int x = 0; x < 10; x++) {
        for(int y = 0; y < 10; y++) {
            if(state->getOwner(x, y) == player) {
                num_pieces += 1;
                distance += sqrt(powf64((double) x - enemy_x, 2) + powf64((double) y - enemy_y, 2));
            }
        }
    }
    return distance / num_pieces;
}

double getPieceScore(Piece piece) {
    const double table[13] = {0.5, 0.1, 1.0, 0.2, 0.5, 1.0, 1.4, 1.75, 3.0, 4.0, 0.75, 0, 0};
    return table[piece];
}

double heuristic(GameState* state, Player player) {
    Player otherPlayer = player == red ? blue : red;
    if(state->hasWinner()) {
        return state->getWinner() == player ? 1.0 : 0.0;
    } else {
        return (float) state->getNumPieces(player) / ((float) state->getNumPieces(player) + (float) state->getNumPieces(otherPlayer));
    }
}

double move_heuristic(GameState* state, Move* move) {
    bool isCapture = state->getOwner(move->x, move->y) != state->getOwner(move->dx, move->dy) && !state->isEmpty(move->dx, move->dy);
    bool isMultiMove = abs(move->dx - move->x) > 1 || abs(move->dy - move->y) > 1;
    bool isScoutingMove = isCapture && state->getPieceAt(move->x, move->y) == scout && state->getVisibility(move->dx, move->dy) != visible;
    bool isBombAttack = isCapture && state->getPieceAt(move->dx, move->dy) == bomb;
    bool isBombSuicide = isBombAttack && (state->getPieceAt(move->x, move->x) != miner);
    bool isBackMove = state->player_turn == red ? (move->dy - move->y) < 0 : (move->dy - move->y) > 0;
    bool _isSuccessfulAttack = isSuccessfulAttack(state->getPieceAt(move->x, move->y), state->getPieceAt(move->dx, move->dy));
    if(isScoutingMove) {
        return 2.0;
    } else if(isMultiMove) {
        return -3.0;
    } else if(isBombAttack) {
        return isBombSuicide ? -3.0 : 3.0;
    } else if(isCapture && _isSuccessfulAttack) {
        return 3.0;
    } else if(isCapture && !_isSuccessfulAttack) {
        return -2.0;
    } else if(isBackMove && distanceFromHome(state->player_turn, move->y) <= 6) {
        return -1.0;
    } else {
        return 0;
    }
}

void sortMoves(GameState* state, std::vector<Move>* moves) {
    std::vector<std::pair<double, Move>> moves_score_pairs;
    Move move;
    for(int i = 0; i < moves->size(); i++) {
        move = moves->at(i);
        moves_score_pairs.push_back(std::pair<double, Move>(move_heuristic(state, &move) + ((float) rand() / (float) RAND_MAX), move));
    }
    std::qsort(moves_score_pairs.data(), moves_score_pairs.size(), sizeof(std::pair<double, Move>), [](const void *a, const void *b)->int {
        std::pair<double, Move>* p1 = (std::pair<double, Move>*) a;
        std::pair<double, Move>* p2 = (std::pair<double, Move>*) b;
        if(p1->first < p2->first) {
            return 1;
        } else if(p1->first > p2->first) {
            return -1;
        } else {
            return 0;
        }
    });
    moves->clear();
    for(int i = 0; i < moves_score_pairs.size(); i++) {
        moves->push_back(moves_score_pairs[i].second);
    }
}

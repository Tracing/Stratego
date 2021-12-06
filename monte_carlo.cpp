#include "monte_carlo.h"

MonteCarloAgent::MonteCarloAgent(int n, int d, int m, double betas[4])
{
    double beta_sum = 0;
    n_rollouts = n;
    rollout_depth = d;
    n_best_moves = m;

    for(int i = 0; i < 4; i++) {
        beta_sum += betas[i];
    }
    beta1 = betas[0] / beta_sum;
    beta2 = betas[1] / beta_sum;
    beta3 = betas[2] / beta_sum;
    beta4 = betas[3] / beta_sum;
}

MonteCarloAgent::MonteCarloAgent(int n, int d, int m) {
    double betas[4] = {0.19142, 0.3035, 0.20591, 0.29919};
    MonteCarloAgent(n, d, m, betas);
}

MonteCarloAgent::MonteCarloAgent(int n, int m) {
    MonteCarloAgent(n, 20, m);
}

struct ThreadData {
    GameState* determinization;
    Move* move;
    std::vector<double>* scores;
    double* score;
    int n_rollouts;
    int rollout_depth;
};

void *doRollouts(void* thread_data) {
    ThreadData* data = (ThreadData*) thread_data;
    GameState local_determinization;
    for(int i = 0; i < data->n_rollouts; i++) {
        local_determinization = data->determinization->copy();
        local_determinization.doMove(*(data->move));
        local_determinization.doRandomMoves(data->rollout_depth);
        if(local_determinization.hasWinner()) {
            if(local_determinization.getWinner() == local_determinization.getPlayerTurn()) {
                data->scores->push_back(1.0);
                *data->score += data->scores->back();
            }
        } else {
            data->scores->push_back(heuristic(&local_determinization, data->determinization->getPlayerTurn()));
            *data->score += data->scores->back();
        }
    }
    pthread_exit(0);
}

int heuristic_dist_from_home(GameState* state, Player player) {
    std::set<std::pair<char, char>>::iterator it;
    int d = 0;
    for(it = state->getPieceLocations(player)->begin(); it != state->getPieceLocations(player)->end(); it++) {
        if(state->getPieceAt(it->first, it->second) != scout) {
            d += distanceFromHome(player, it->second);
        }
    }
    return d;
}

double MonteCarloAgent::heuristic(GameState* state, Player player) {
    Player otherPlayer = player == red ? blue : red;
    double score;
    double n_pieces_score;
    double intelligence_score;
    double unknown_intelligence_score;
    double dist_from_home_score;

    if(state->hasWinner()) {
        score = state->getWinner() == player ? 1.0 : 0.0;
    } else {
        n_pieces_score = (double) state->getNumPieces(player) / ((double) state->getNumPieces(player) + (double) state->getNumPieces(otherPlayer));
        intelligence_score = (((double) (state->getNumPiecesOfVisibility(player, unknown) + state->getNumPiecesOfVisibility(player, unknown_movable))) /
                        ((double) (state->getNumPiecesOfVisibility(player, unknown) + state->getNumPiecesOfVisibility(player, unknown_movable) +
                                   state->getNumPiecesOfVisibility(otherPlayer, unknown) + state->getNumPiecesOfVisibility(otherPlayer, unknown_movable))));
        unknown_intelligence_score = ((double) state->getNumPiecesOfVisibility(player, unknown) /
                                ((double) state->getNumPiecesOfVisibility(player, unknown) + (double) state->getNumPiecesOfVisibility(otherPlayer, unknown)));
        dist_from_home_score = ((double) heuristic_dist_from_home(state, player) /
                                ((double) heuristic_dist_from_home(state, player) + (double) heuristic_dist_from_home(state, otherPlayer)));
        score = beta1 * n_pieces_score + beta2 * intelligence_score + beta3 * unknown_intelligence_score + beta4 * dist_from_home_score;
    }
    return score;
}

void MonteCarloAgent::get_move(GameState* state, Move* bestMove) {
    std::vector<Move> moves = state->getMoves();
    std::vector<double>* scores;
    std::vector<double> rollouts;
    std::vector<Move>::iterator it;
    GameState determinization = state->sampleDeterminization(state->getPlayerTurn());
    pthread_t* threads;
    ThreadData* datas;
    double* score_totals;
    double score;
    double best_score = -1;
    Move _bestMove;

    sortMoves(state, &moves);
    while(moves.size() > n_best_moves) {
        moves.pop_back();
    }

    threads = new pthread_t[moves.size()];
    datas = new ThreadData[moves.size()];
    scores = new std::vector<double>[moves.size()];
    score_totals = new double[moves.size()];

    for(int j = 0; j < moves.size(); j++) {
            datas[j].determinization = &determinization;
            datas[j].move = &moves[j];
            datas[j].scores = &scores[j];
            scores[j].clear();
            datas[j].n_rollouts = (int) (n_rollouts / moves.size());
            datas[j].rollout_depth = rollout_depth;
            datas[j].score = score_totals + j;
            *datas[j].score = 0;
        pthread_create(&threads[j], NULL, doRollouts, (void*) (&datas[j]));
    }
    for(int i = 0; i < moves.size(); i++) {
        pthread_join(threads[i], NULL);
    }
    for(int i = 0; i < moves.size(); i++) {
        score = *datas[i].score;

        if(score > best_score) {
            best_score = score;
            _bestMove = moves[i];
        }
    }
    *bestMove = _bestMove;
    delete [] threads;
    delete [] datas;
    delete [] scores;
    delete [] score_totals;
}

void MonteCarloAgent::do_deployment(GameState* state) {
    std::vector<DeploymentMove> deploymentMoves;
    do_semirandom_deployment(state, &deploymentMoves);
}

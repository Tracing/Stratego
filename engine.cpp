#include "engine.h"

void do_random_game() {
    GameState state;
    std::vector<DeploymentMove> deploymentMoves;
    std::vector<Move> moves;
    DeploymentMove deploymentMove;
    Move move;

    state.newGame();
    while(!state.hasWinner()) {
        if(state.state == deployment) {
            deploymentMoves = state.getDeploymentMoves();
            state.doDeploymentMove(deploymentMoves.at(rand() % deploymentMoves.size()));
        } else {
            moves = state.getMoves();
            state.doMove(moves.at(rand() % moves.size()));
        }
    }
    print_game_state(&state);
}

void print_game_state(GameState* state) {
    bool bold = false;
    for(int y = 9; y >= 0; y--) {
        cout << "\033[107m";
        for(int x = 0; x < 10; x++) {
            bold = ((state->lastMovesStack.size() > 0) && ((x == state->lastMovesStack.back().x && y == state->lastMovesStack.back().y) || (x == state->lastMovesStack.back().dx && y == state->lastMovesStack.back().dy)));
            if(bold) {
                if(state->getOtherPlayerTurn() == blue) {
                    cout << "\u001b[47m";
                } else {
                    cout << "\u001b[47m";
                }
            }
            if(isLake(x, y)) {
                cout << "\033[104m  \033[107m";
            } else if(state->isEmpty(x, y)) {
                cout << "  ";
            } else if(state->getOwner(x, y) == blue) {
                cout << "\033[34m" << getPieceChar(state->getPieceAt(x, y)) << " \033[107m";
            } else {
                cout << "\033[31m" << getPieceChar(state->getPieceAt(x, y)) << " \033[107m";
            }
            if(bold) {
                cout << "\033[107m";
            }
        }
        cout << "\033[0m\n";
    }
    cout << "\033[0m\n";
}

void Game::setAgent(Player player, Agent* agent) {
    if(player == red) {
        redAgent = agent;
    } else {
        blueAgent = agent;
    }
}

GameState Game::getGameState() {
    return state;
}

Player Game::play(int turn_limit, bool verbose) {
    Move move;
    state = GameState();
    
    state.newGame();
    redAgent->setup();
    blueAgent->setup();

    while(!state.hasWinner() && state.turn < turn_limit) {
        if(state.state == deployment) {
            if(state.player_turn == red) {
                redAgent->do_deployment(&state);
            } else {
                blueAgent->do_deployment(&state);
            }
        } else {
            if(state.player_turn == red) {
                redAgent->get_move(&state, &move);
            } else {
                blueAgent->get_move(&state, &move);
            }
            state.doMove(move);
        }
        if(verbose) {
            print_game_state(&state);
        }
    }
    if(verbose) {
        cout << "Player " << (state.getWinner() == red ? "Red" : "Blue") << " has won on turn " << state.turn << "!\n";
    }

    return state.getWinner();
}

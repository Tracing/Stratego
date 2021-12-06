#include "tests.h"

using namespace std;

void do_test(int* total_tests, int* successes, bool success, string name) {
    *total_tests += 1;
    if(success) {
        *successes += 1;
        //std::cout << "Test " << *test_number << "/" << total_tests << " passed\n";
    } else {
        std::cout << "Test " << *total_tests << " - \"" << name << "\" failed\n";
    }
}

bool all_but_square_empty(GameState state, int x, int y) {
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 10; j++) {
            if(i == x && j == y) {
                continue;
            }
            if(!state.isEmpty(i, j)) {
                return false;
            }
        }
    }
    return true;
}

bool has_move(GameState state, Move move) {
    std::vector<Move> moves = state.getMoves();
    std::vector<Move>::iterator it;
    for(it = moves.begin(); it != moves.end(); it++) {
        if(it->equals(&move)) {
            return true;
        }
    }
    return false;
}

void erase_owner_matrix(GameState* state) {
    for(int x = 0; x < 10; x++) {
        for(int y = 0; y < 10; y++) {
            state->owner[x][y] = none;
        }
    }
}

void run_do_move_tests() {
    int successes = 0;
    int total_tests = 0;
    GameState state = GameState();

    cout << "Running do_move() tests...\n";

    //Normal moves
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.doDeploymentMove(DeploymentMove(7, 2, general));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.state = movement;
    state.player_turn = red;
    state.doMove(Move(7, 2, 7, 3));
    do_test(&total_tests, &successes, state.getOwner(7, 2) == none, "state.getOwner(7, 2) == none");
    do_test(&total_tests, &successes, state.getOwner(7, 3) == red, "state.getOwner(7, 3) == red");
    do_test(&total_tests, &successes, state.getPieceAt(7, 2) == nopiece, "state.getPieceAt(7, 2) == none");
    do_test(&total_tests, &successes, state.getPieceAt(7, 3) == general, "state.getPieceAt(7, 3) == general");
    do_test(&total_tests, &successes, !state.hasWinner(), "!state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == none, "state.getWinner() == none");
    //Normal take weaker enemy
    state.player_turn = blue;
    state.state = deployment;
    state.doDeploymentMove(DeploymentMove(6, 3, miner));
    state.state = movement;
    state.player_turn = red;
    state.doMove(Move(7, 3, 6, 3));
    do_test(&total_tests, &successes, state.getOwner(7, 3) == none, "state.getOwner(7, 3) == none");
    do_test(&total_tests, &successes, state.getOwner(6, 3) == red, "state.getOwner(6, 3) == red");
    do_test(&total_tests, &successes, state.getPieceAt(7, 3) == nopiece, "state.getPieceAt(7, 3) == none");
    do_test(&total_tests, &successes, state.getPieceAt(6, 3) == general, "state.getPieceAt(6, 3) == general");
    do_test(&total_tests, &successes, !state.hasWinner(), "!state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == none, "state.getWinner() == none");
    //Normal lose vs stronger
    state.state = deployment;
    state.player_turn = blue;    
    state.doDeploymentMove(DeploymentMove(6, 2, marshal));
    state.state = movement;
    state.player_turn = red;
    state.doMove(Move(6, 3, 6, 2));
    do_test(&total_tests, &successes, state.getOwner(6, 3) == none, "state.getOwner(6, 3) == none");
    do_test(&total_tests, &successes, state.getOwner(6, 2) == blue, "state.getOwner(6, 2) == blue");
    do_test(&total_tests, &successes, state.getPieceAt(6, 3) == nopiece, "state.getPieceAt(6, 3) == nopiece");
    do_test(&total_tests, &successes, state.getPieceAt(6, 2) == marshal, "state.getPieceAt(6, 2) == marshal");
    do_test(&total_tests, &successes, !state.hasWinner(), "!state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == none, "state.getWinner() == none");
    //Normal lose both when equal
    state.player_turn = red;
    state.state = deployment;
    state.doDeploymentMove(DeploymentMove(5, 2, marshal));
    state.player_turn = blue;
    state.state = movement;
    state.doMove(Move(6, 2, 5, 2));
    do_test(&total_tests, &successes, state.getOwner(5, 2) == none, "state.getOwner(5, 2) == none");
    do_test(&total_tests, &successes, state.getOwner(6, 2) == none, "state.getOwner(6, 2) == none");
    do_test(&total_tests, &successes, state.getPieceAt(5, 2) == nopiece, "state.getPieceAt(5, 2) == nopiece");
    do_test(&total_tests, &successes, state.getPieceAt(6, 2) == nopiece, "state.getPieceAt(6, 2) == nopiece");
    do_test(&total_tests, &successes, !state.hasWinner(), "!state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == none, "state.getWinner() == none");
    //Normal lose when attacking bomb
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.doDeploymentMove(DeploymentMove(5, 4, bomb));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.doDeploymentMove(DeploymentMove(5, 5, general));
    state.state = movement;
    state.doMove(Move(5, 5, 5, 4));
    do_test(&total_tests, &successes, state.getOwner(5, 4) == red, "state.getOwner(5, 4) == red");
    do_test(&total_tests, &successes, state.getOwner(5, 5) == none, "state.getOwner(5, 5) == none");
    do_test(&total_tests, &successes, state.getPieceAt(5, 4) == bomb, "state.getPieceAt(5, 2) == nopiece");
    do_test(&total_tests, &successes, state.getPieceAt(5, 5) == nopiece, "state.getPieceAt(6, 2) == nopiece");
    do_test(&total_tests, &successes, !state.hasWinner(), "!state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == none, "state.getWinner() == none");
    //Miner win when attacking bomb
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.doDeploymentMove(DeploymentMove(5, 4, bomb));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.doDeploymentMove(DeploymentMove(5, 5, miner));
    state.state = movement;
    state.doMove(Move(5, 5, 5, 4));
    do_test(&total_tests, &successes, state.getOwner(5, 4) == blue, "state.getOwner(5, 4) == blue");
    do_test(&total_tests, &successes, state.getPieceAt(5, 4) == miner, "state.getPieceAt(5, 4) == miner");
    do_test(&total_tests, &successes, !state.hasWinner(), "!state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == none, "state.getWinner() == none");
    //Spy wins when attacking marshal
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.doDeploymentMove(DeploymentMove(8, 8, marshal));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.doDeploymentMove(DeploymentMove(7, 8, spy));
    state.state = movement;
    state.doMove(Move(7, 8, 8, 8));
    do_test(&total_tests, &successes, state.getOwner(7, 8) == none, "state.getOwner(7, 8) == none");
    do_test(&total_tests, &successes, state.getPieceAt(7, 8) == nopiece, "state.getPieceAt(7, 8) == nopiece");
    do_test(&total_tests, &successes, state.getOwner(8, 8) == blue, "state.getOwner(8, 8) == spy");
    do_test(&total_tests, &successes, state.getPieceAt(8, 8) == spy, "state.getPieceAt(8, 8) == blue");
    do_test(&total_tests, &successes, !state.hasWinner(), "!state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == none, "state.getWinner() == none");
    //Anything wins when attacking flag
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.doDeploymentMove(DeploymentMove(8, 9, scout));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.player_turn = red;
    state.state = movement;
    state.doMove(Move(8, 9, 9, 9));
    do_test(&total_tests, &successes, state.hasWinner(), "state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == red, "state.getWinner() == red");
    //If there are no moves, opponent wins
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.player_turn = red;
    state.state = movement;
    state.doMove(Move(true));
    do_test(&total_tests, &successes, state.hasWinner(), "state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == blue, "state.getWinner() == blue");
    //Normal scout move
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.doDeploymentMove(DeploymentMove(7, 2, scout));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.state = movement;
    state.player_turn = red;
    state.doMove(Move(7, 2, 1, 2));
    do_test(&total_tests, &successes, state.getOwner(7, 2) == none, "state.getOwner(7, 2) == none");
    do_test(&total_tests, &successes, state.getOwner(1, 2) == red, "state.getOwner(1, 2) == red");
    do_test(&total_tests, &successes, state.getPieceAt(7, 2) == nopiece, "state.getPieceAt(7, 2) == none");
    do_test(&total_tests, &successes, state.getPieceAt(1, 2) == scout, "state.getPieceAt(1, 2) == scout");
    do_test(&total_tests, &successes, !state.hasWinner(), "!state.hasWinner()");
    do_test(&total_tests, &successes, state.getWinner() == none, "state.getWinner() == none");

    cout << total_tests << " tests run\n";
    cout << total_tests - successes << " tests failed\n";
}

void run_get_scout_moves_tests() {
    int successes = 0;
    int total_tests = 0;
    GameState state = GameState();
    std::vector<Move> moves;

    cout << "Running get_move() tests for scouts...\n";

    //Blue sky
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.doDeploymentMove(DeploymentMove(1, 1, scout));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.state = movement;
    state.player_turn = red;
    moves = state.getMoves();
    do_test(&total_tests, &successes, state.getMoves().size() == 18, "state.getMoves().size() == 18");
    for(int i = 0; i < 10; i++) {
        if(i != 1) {
            do_test(&total_tests, &successes, has_move(state, Move(1, 1, i, 1)), "blue_sky_x");
            do_test(&total_tests, &successes, has_move(state, Move(1, 1, 1, i)), "blue_sky_y");
        }
    }

    //Friendly piece in the way
    state.state = deployment;
    state.doDeploymentMove(DeploymentMove(1, 5, bomb));
    state.state = movement;
    do_test(&total_tests, &successes, state.getMoves().size() == 13, "state.getMoves().size() == 13");
    for(int i = 5; i < 10; i++) {
        do_test(&total_tests, &successes, !has_move(state, Move(1, 1, 1, i)), "friendly_piece_in_way_y");
    }

    //Enemy piece in the way
    state.state = deployment;
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(4, 1, sergeant));
    state.player_turn = red;
    state.state = movement;
    do_test(&total_tests, &successes, state.getMoves().size() == 8, "state.getMoves().size() == 8");
    do_test(&total_tests, &successes, has_move(state, Move(1, 1, 4, 1)), "enemy_piece_in_way_capture");
    for(int i = 5; i < 10; i++) {
        do_test(&total_tests, &successes, !has_move(state, Move(1, 1, i, 1)), "enemy_piece_in_way_x");
    }

    //No swimming
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.doDeploymentMove(DeploymentMove(5, 4, scout));
    state.state = movement;
    moves = state.getMoves();
    do_test(&total_tests, &successes, state.getMoves().size() == 10, "state.getMoves().size() == 10");
    do_test(&total_tests, &successes, has_move(state, Move(5, 4, 4, 4)), "scout_no_swimming_x");
    for(int i = 0; i < 10; i++) {
        if(i != 4) {
            do_test(&total_tests, &successes, has_move(state, Move(5, 4, 5, i)), "scout_no_swimming_y");
        }
    }

    cout << total_tests << " tests run\n";
    cout << total_tests - successes << " tests failed\n";
}

void run_get_move_normal_tests() {
    int successes = 0;
    int total_tests = 0;
    GameState state = GameState();
    std::vector<Move> moves;

    cout << "Running normal get_move() tests...\n";

    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.state = movement;
    do_test(&total_tests, &successes, state.getMoves().size() == 1, "Empty_board_blue_no_moves");
    do_test(&total_tests, &successes, has_move(state, Move(true)), "has_move(state, Move(true))");
    state.player_turn = red;
    do_test(&total_tests, &successes, state.getMoves().size() == 1, "Empty_board_red_no_moves");
    do_test(&total_tests, &successes, has_move(state, Move(true)), "has_move(state, Move(true))");
    state.state = deployment;
    state.doDeploymentMove(DeploymentMove(2, 2, miner));
    state.state = movement;
    moves = state.getMoves();
    do_test(&total_tests, &successes, has_move(state, Move(2, 2, 2, 1)), "has_move(state, Move(2, 2, 2, 1))");
    do_test(&total_tests, &successes, has_move(state, Move(2, 2, 2, 3)), "has_move(state, Move(2, 2, 2, 3))");
    do_test(&total_tests, &successes, has_move(state, Move(2, 2, 3, 2)), "has_move(state, Move(2, 2, 3, 2))");
    do_test(&total_tests, &successes, has_move(state, Move(2, 2, 1, 2)), "has_move(state, Move(2, 2, 1, 2))");
    do_test(&total_tests, &successes, moves.size() == 4, "moves.size() == 4 - 1");
    state.state = deployment;
    state.doDeploymentMove(DeploymentMove(2, 1, bomb));
    state.state = movement;
    moves = state.getMoves();
    do_test(&total_tests, &successes, has_move(state, Move(2, 2, 3, 2)), "has_move(state, Move(2, 2, 3, 2)) - 2");
    do_test(&total_tests, &successes, has_move(state, Move(2, 2, 2, 3)), "has_move(state, Move(2, 2, 2, 3)) - 2");
    do_test(&total_tests, &successes, has_move(state, Move(2, 2, 1, 2)), "has_move(state, Move(2, 2, 1, 2)) - 2");
    do_test(&total_tests, &successes, moves.size() == 3, "moves.size() == 3");

    state = GameState();
    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(2, 2, flag));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(7, 7, flag));
    state.doDeploymentMove(DeploymentMove(9, 9, colonel));
    state.doDeploymentMove(DeploymentMove(0, 0, spy));
    state.state = movement;
    moves = state.getMoves();
    do_test(&total_tests, &successes, has_move(state, Move(9, 9, 9, 8)), "has_move(state, Move(9, 9, 9, 8))");
    do_test(&total_tests, &successes, has_move(state, Move(9, 9, 8, 9)), "has_move(state, Move(9, 9, 8, 9))");
    do_test(&total_tests, &successes, has_move(state, Move(0, 0, 0, 1)), "has_move(state, Move(9, 9, 0, 1))");
    do_test(&total_tests, &successes, has_move(state, Move(0, 0, 1, 0)), "has_move(state, Move(9, 9, 1, 0))");
    do_test(&total_tests, &successes, moves.size() == 4, "moves.size() == 4 - 2"),

    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.doDeploymentMove(DeploymentMove(2, 3, colonel));
    state.state = movement;
    moves = state.getMoves();
    do_test(&total_tests, &successes, !has_move(state, Move(2, 3, 2, 4)), "!has_move(state, Move(2, 3, 2, 4))");

    state.newGame();
    erase_owner_matrix(&state);
    state.doDeploymentMove(DeploymentMove(0, 0, flag));
    state.player_turn = blue;
    state.doDeploymentMove(DeploymentMove(9, 9, flag));
    state.doDeploymentMove(DeploymentMove(2, 3, colonel));
    state.player_turn = red;
    state.doDeploymentMove(DeploymentMove(2, 2, general));
    state.state = movement;
    moves = state.getMoves();
    do_test(&total_tests, &successes, has_move(state, Move(2, 2, 2, 3)), "has_move(state, Move(2, 2, 2, 3))");

    cout << total_tests << " tests run\n";
    cout << total_tests - successes << " tests failed\n";
}

void run_deployment_tests() {
    int successes = 0;
    int total_tests = 0;
    GameState state = GameState();
    std::vector<DeploymentMove> moves;
    DeploymentMove move;

    cout << "Running deployment stage tests...\n";

    state.newGame();
    moves = state.getDeploymentMoves();
    do_test(&total_tests, &successes, moves.size() == 480, "moves.size() == 480");
    do_test(&total_tests, &successes, all_but_square_empty(state, 11, 11), "all_but_square_empty(state, 11, 11)");

    state = GameState();
    state.newGame();
    move = DeploymentMove(2, 2, scout);
    state.doDeploymentMove(move);
    do_test(&total_tests, &successes, state.getPlayerTurn() == red, "state.getPlayerTurn() == red");
    do_test(&total_tests, &successes, state.getPieceAt(2, 2) == scout, "state.getPieceAt(2, 2) == scout");
    do_test(&total_tests, &successes, all_but_square_empty(state, 2, 2), "all_but_square_empty(state, 2, 2)");
    do_test(&total_tests, &successes, state.getPlayerTurn() == red, "state.getPlayerTurn() == red");
    do_test(&total_tests, &successes, state.getDeploymentMoves().size() == 468, "state.getDeploymentMoves().size() == 468");

    state = GameState();
    state.newGame();
    for(int i = 0; i < 40; i++) {
        moves = state.getDeploymentMoves();
        move = moves.at(0);
        state.doDeploymentMove(move);
    }
    do_test(&total_tests, &successes, state.getPlayerTurn() == blue, "state.getPlayerTurn() == blue");
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 4; j++) {
            do_test(&total_tests, &successes, !state.isEmpty(i, j), "!state.isEmpty(i, j) - 1");
        }
    }
    for(int i = 0; i < 40; i++) {
        moves = state.getDeploymentMoves();
        move = moves.at(0);
        state.doDeploymentMove(move);
    }
    do_test(&total_tests, &successes, state.getPlayerTurn() == red, "state.getPlayerTurn() == red");
    do_test(&total_tests, &successes, state.getState() == movement, "state.getState() == movement");

    for(int i = 0; i < 10; i++) {
        for(int j = 6; j < 10; j++) {
            do_test(&total_tests, &successes, !state.isEmpty(i, j), "!state.isEmpty(i, j) - 2");
        }
    }

    cout << total_tests << " tests run\n";
    cout << total_tests - successes << " tests failed\n";
}

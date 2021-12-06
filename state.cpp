#include "move.h"
#include "state.h"

using namespace std;

std::vector<DeploymentMove> GameState::getDeploymentMoves() {
    return deploymentMoves;
}

void GameState::replacePieceAt(char x, char y, Piece newPiece) {
    pieces[x][y] = newPiece;
    updatePieceLocationCache(x, y, newPiece, red);
    updatePieceLocationCache(x, y, newPiece, blue);
}

bool isPieceAtNoError(GameState* state, Player player, Piece piece, char x, char y) {
    return x >= 0 && x < 10 && y >= 0 && y < 10 && !isLake(x, y) && state->getOwner(x, y) == player && state->getPieceAt(x, y) == piece;
}

void GameState::doRandomMove() {
    if(state == deployment) {
        std::vector<DeploymentMove> deploymentMoves = getDeploymentMoves();
        doDeploymentMove(deploymentMoves.at(rand() % deploymentMoves.size()));
    } else {
        std::vector<Move> moves = getMoves();
        doMove(moves.at(rand() % moves.size()));
    }
}

void GameState::doRandomMoves(int max_depth) {
    int d = 0;
    while(d < max_depth && !hasWinner()) {
        doRandomMove();
        d += 1;
    }
}

void GameState::seedRNG(int seed) {
    srand(seed);
}

std::vector<Piece> GameState::getDeployablePieces() {
    std::vector<Piece> pieces;
    std::unordered_map<Piece, int>::iterator it;
    for(int i = 0; i < 12; i++) {
        if(pieces_to_deploy[player_turn][(Piece) i] > 0) {
            pieces.push_back((Piece) i);
        }
    }
    return pieces;
}

Player GameState::getOtherPlayerTurn() {
    return player_turn == red ? blue : red;
}

GameState GameState::copy() {
    GameState obj = GameState();
    obj.turn = turn;
    obj.player_turn = player_turn;
    obj.state = state;
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            obj.pieces[x][y] = pieces[x][y];
            obj.owner[x][y] = owner[x][y];
            obj.visibility[x][y] = visibility[x][y];
        }
    }
    obj.has_winner = has_winner;
    obj.winner = winner;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 12; j++) {
            obj.pieces_to_deploy[i][(Piece) j] = pieces_to_deploy[i][(Piece) j];
        }
    }
    obj.num_pieces_to_deploy = num_pieces_to_deploy;
    obj.num_moves_since_capture = num_moves_since_capture;
    if(obj.getState() == deployment) {
        obj.computeDeploymentMoves();
    } else if(obj.getState() == movement) {
        obj.computeMoves();
    } else {
        ;
    }
    obj.piece_location_cache[red] = std::set<std::pair<char, char>>(piece_location_cache[red]);
    obj.piece_location_cache[blue] = std::set<std::pair<char, char>>(piece_location_cache[blue]);
    return obj;
}

void GameState::addMove(std::vector<Move>* moves, char x, char y, char dx, char dy) {
    if(inBoard(dx, dy) && !isLake(dx, dy) && (isEmpty(dx, dy) || getOwner(dx, dy) != player_turn)) {
        moves->push_back(Move(x, y, dx, dy));
    }
}

void GameState::addNormalMoves(std::vector<Move>* moves, char x, char y) {
    addMove(moves, x, y, x, y - 1);
    addMove(moves, x, y, x, y + 1);
    addMove(moves, x, y, x - 1, y);
    addMove(moves, x, y, x + 1, y);
}

void GameState::addScoutMovesRecursive(std::vector<Move>* moves, char sx, char sy, char x, char y, char dx, char dy) {
    int x2 = x + dx;
    int y2 = y + dy;
    
    if(inBoard(x2, y2) && !isLake(x2, y2) && isEmpty(x2, y2)) {
        moves->push_back(Move(sx, sy, x2, y2));
        addScoutMovesRecursive(moves, sx, sy, x2, y2, dx, dy);
    } else if(inBoard(x2, y2) && !isLake(x2, y2) && getOwner(x2, y2) != player_turn) {
        moves->push_back(Move(sx, sy, x2, y2));
    }
}

void GameState::addScoutMoves(std::vector<Move>* moves, char x, char y) {
    addScoutMovesRecursive(moves, x, y, x, y, 1, 0);
    addScoutMovesRecursive(moves, x, y, x, y, -1, 0);
    addScoutMovesRecursive(moves, x, y, x, y, 0, 1);
    addScoutMovesRecursive(moves, x, y, x, y, 0, -1);    
}

std::vector<Move> GameState::getMoves() {
    return moves;
}

int GameState::getNumPieces(Player player) {
    return piece_location_cache[player].size();
}

int GameState::getNumPiecesOfVisibility(Player player, Visibility visibility) {
    std::set<std::pair<char, char>>::iterator it;
    int n = 0;
    for(it = piece_location_cache[player].begin(); it != piece_location_cache[player].end(); it++) {
        if(getVisibility(it->first, it->second) == visibility) {
            n += 1;
        }
    }
    return n;
}

const std::set<std::pair<char, char>>* GameState::getPieceLocations(Player player) {
    return &piece_location_cache[player];
}

int GameState::getNumPiecesofType(Player player, Piece piece) {
    std::set<std::pair<char, char>>::iterator it;
    int n = 0;
    for(it = piece_location_cache[player].begin(); it != piece_location_cache[player].end(); it++) {
        if(pieces[it->first][it->second] == piece) {
            n += 1;
        }
    }
    return n;
}

void GameState::removePieceAt(char x, char y) {
    visibility[x][y] = unknown;
    owner[x][y] = none;
    pieces[x][y] = nopiece;
    updatePieceLocationCache(x, y, nopiece, red);
    updatePieceLocationCache(x, y, nopiece, blue);
}

void GameState::movePiece(char x, char y, char dx, char dy) {
    visibility[dx][dy] = (visibility[x][y] == visible || abs(dx - x) > 1 || abs(dy - y) > 1) ? visible : unknown_movable;
    visibility[x][y] = unknown;
    owner[x][y] = none;
    owner[dx][dy] = player_turn;
    pieces[dx][dy] = pieces[x][y];
    pieces[x][y] = nopiece;
    updatePieceLocationCache(x, y, nopiece, player_turn);
    updatePieceLocationCache(dx, dy, nopiece, getOtherPlayerTurn());
    updatePieceLocationCache(dx, dy, pieces[dx][dy], player_turn);
}

void GameState::capturePiece(char x, char y, char dx, char dy) {
    movePiece(x, y, dx, dy);
    visibility[dx][dx] = visible;
}

void GameState::updatePieceLocationCache(char x, char y, Piece piece, Player player) {
    std::pair<char, char> cords = std::pair<char, char>(x, y);

    if(piece == nopiece) {
        if(piece_location_cache[player].find(cords) != piece_location_cache[player].end()) {
            piece_location_cache[player].erase(cords);
        }
    } else {
        piece_location_cache[player].insert(cords);
    }
}

void GameState::undoMove() {
    LastMove last_move = lastMovesStack.at(lastMovesStack.size() - 1);
    char x = last_move.x;
    char y = last_move.y;
    char dx = last_move.dx;
    char dy = last_move.dy;

    if(pieces[x][y] != last_move.from_piece || owner[x][y] != last_move.from_player) {
        updatePieceLocationCache(x, y, last_move.from_piece, red);
        updatePieceLocationCache(x, y, last_move.from_piece, blue);
    }
    if(pieces[dx][dy] != last_move.to_piece || owner[dx][dy] != last_move.from_player) {
        updatePieceLocationCache(dx, dy, last_move.to_piece, red);
        updatePieceLocationCache(dx, dy, last_move.to_piece, blue);
    }
    pieces[x][y] = last_move.from_piece;
    pieces[dx][dy] = last_move.to_piece;
    owner[x][y] = last_move.from_player;
    owner[dx][dy] = last_move.to_player;
    visibility[x][y] = last_move.from_visibility;
    visibility[dx][dy] = last_move.to_visibility;
    num_moves_since_capture = last_move.num_moves_since_capture;
    turn -= player_turn == red ? 1 : 0;
    player_turn = player_turn == blue ? red : blue;
    has_winner = false;
    winner = none;
    state = movement;

    lastMovesStack.pop_back();
    computeMoves();
}

void GameState::doMove(Move move) {
    Piece piece = getPieceAt(move.x, move.y);;
    Piece otherPiece = getPieceAt(move.dx, move.dy);;
    LastMove lastMove;

    lastMove.x = move.x;
    lastMove.y = move.y;
    lastMove.dx = move.dx;
    lastMove.dy = move.dy;
    lastMove.from_piece = piece;
    lastMove.to_piece = otherPiece;
    lastMove.from_visibility = getVisibility(move.x, move.y);
    lastMove.to_visibility = getVisibility(move.dx, move.dy);
    lastMove.from_player = getOwner(move.x, move.y);
    lastMove.to_player = getOwner(move.dx, move.dy);
    lastMove.num_moves_since_capture = num_moves_since_capture;
    lastMovesStack.push_back(lastMove);

    assert(state == movement);
    if(move.resign) {
        //Forced resignation
        has_winner = true;
        winner = player_turn == blue ? red : blue;
        state = end_of_game;
    } else {
        assert(move.x != move.dx || move.y != move.dy);
        if(isEmpty(move.dx, move.dy)) {
            //Movement
            movePiece(move.x, move.y, move.dx, move.dy);
            num_moves_since_capture += 1;
        } else if(otherPiece == flag) {
            //Victory
            has_winner = true;
            winner = player_turn;
            state = end_of_game;
        } else if(otherPiece == bomb) {
            //Attacking bomb
            num_moves_since_capture = 0;
            if(piece == miner) {
                //Attacking bomb with miner
                capturePiece(move.x, move.y, move.dx, move.dy);
            } else {
                //Attacking bomb with anything else
                removePieceAt(move.x, move.y);
                visibility[move.dx][move.dy] = visible;
            }
        } else if(otherPiece == marshal && piece == spy) {
            //Attacking marshal with spy
            capturePiece(move.x, move.y, move.dx, move.dy);
            num_moves_since_capture = 0;
        } else {
            assert(!isEmpty(move.dx, move.dy));
            //Normal combat
            if(getPieceRank(piece) > getPieceRank(otherPiece)) {
                capturePiece(move.x, move.y, move.dx, move.dy);
            } else if(getPieceRank(piece) == getPieceRank(otherPiece)) {
                removePieceAt(move.x, move.y);
                removePieceAt(move.dx, move.dy);
            } else {
                removePieceAt(move.x, move.y);
                visibility[move.dx][move.dy] = visible;
            }
            num_moves_since_capture = 0;
        }
        //Postprocessing
        player_turn = player_turn == blue ? red : blue;
        if(player_turn == red) {
            turn += 1;
        }
    }
    if(state != end_of_game) {
        computeMoves();
    } else {
        moves.clear();
    }
}

bool GameState::hasWinner() {
    return has_winner;
}

Player GameState::getWinner() {
    return winner;
}

Visibility GameState::getVisibility(char x, char y) {
    return visibility[x][y];
}

Piece GameState::getPieceAt(char x, char y) {
    return pieces[x][y];
}

bool GameState::isEmpty(char x, char y) {
    return pieces[x][y] == nopiece;
}

int GameState::cmpDeterminization(const void *a, const void *b) {
    const std::pair<float, Piece>* p1 = (std::pair<float, Piece>*) a;
    const std::pair<float, Piece>* p2 = (std::pair<float, Piece>*) b;
    if(p1->first < p2->first) {
        return -1;
    } else if(p1->first > p2->first) {
        return 1;
    } else {
        return 0;
    }
}

int GameState::determinizationPieceRank(Piece piece) {
    if(piece == spy || piece == miner) {
        return 6;
    } else if(piece == scout || piece == captain) {
        return 4;
    } else {
        return piece;
    }
}

bool GameState::tryReplacePieceAt(char x, char y, Piece newPiece) {
    if(inBoard(x, y) && !isLake(x, y)) {
        replacePieceAt(x, y, newPiece);
        return true;
    } else {
        return false;
    }
}

double determinizationFlagPlacementScore(Player player, char x, char y) {
    double score = 0;
    char y_distance_from_home = player == red ? y : 9 - y;
    score = 1000 - (double) y_distance_from_home * 100.0;
    score += ((double) rand() / (double) RAND_MAX);
    return score;
}

double determinizationBombPlacementScore(GameState* state, Player player, char x, char y) {
    double score = 1000;
    if(isPieceAtNoError(state, player, flag, x + 1, y) && state->getVisibility(x + 1, y) == unknown) {
        score += 100000;
    }
    if(isPieceAtNoError(state, player, flag, x - 1, y) && state->getVisibility(x - 1, y) == unknown) {
        score += 100000;
    }
    if(isPieceAtNoError(state, player, flag, x, y + 1) && state->getVisibility(x, y + 1) == unknown) {
        score += 100000;
    }
    if(isPieceAtNoError(state, player, flag, x, y - 1) && state->getVisibility(x, y - 1) == unknown) {
        score += 100000;
    }
    score += ((double) rand() / (double) RAND_MAX);
    if(state->getVisibility(x, y) != unknown) {
        score = 0;
    }
    return score;
}

double determinizationPieceValue(Piece piece) {
    if(piece == miner) {
        return 6;
    } else if(piece == spy) {
        return 6;
    } else if(piece == scout) {
        return 4;
    } else {
        return getPieceRank(piece);
    }
}

double determinizationPiecePlacementScore(GameState* state, Player player, Piece piece, char x, char y) {
    double y_distance_from_home = player == red ? y : 9 - y;
    double score = 0;
    if(state->getPieceAt(x, y) != nopiece) {
        score = -100000;
    } else if(piece == bomb) {
        score = determinizationBombPlacementScore(state, player, x, y);
    } else {
        if(isPieceAtNoError(state, player, flag, x + 1, y) && state->getVisibility(x + 1, y) == unknown) {
            score -= 1000;
        }
        if(isPieceAtNoError(state, player, flag, x - 1, y) && state->getVisibility(x - 1, y) == unknown) {
            score -= 1000;
        }
        if(isPieceAtNoError(state, player, flag, x, y + 1) && state->getVisibility(x, y + 1) == unknown) {
            score -= 1000;
        }
        if(isPieceAtNoError(state, player, flag, x, y - 1) && state->getVisibility(x, y - 1) == unknown) {
            score -= 1000;
        }
        score += pow(y_distance_from_home - (determinizationPieceValue(piece) / 2), 2) + ((double) rand() / (double) RAND_MAX);
    }
    return score;
}

GameState GameState::sampleDeterminization(Player player_pov) {
    GameState determinization = copy();
    Player otherPlayer = player_pov == red ? blue : red;
    std::set<std::pair<char, char>>::iterator it;
    std::vector<Piece>::iterator piece_it;
    std::vector<Piece> unknownPieces;
    std::set<std::pair<char, char>> unknownPieceLocations;
    char x, y;
    double bestScore;
    double score;
    std::pair<char, char> bestLocation;
    for(it = piece_location_cache[otherPlayer].begin(); it != piece_location_cache[otherPlayer].end(); it++) {
        x = it->first;
        y = it->second;
        if(getVisibility(x, y) != visible) {
            unknownPieceLocations.insert(*it);
            if(getPieceAt(x, y) != flag) {
                unknownPieces.push_back(getPieceAt(x, y));
            }
            determinization.replacePieceAt(x, y, nopiece);
        }
    }
    // Place flag
    bestScore = -1;
    for(it = unknownPieceLocations.begin(); it != unknownPieceLocations.end(); it++) {
        x = it->first;
        y = it->second;
        score = determinizationFlagPlacementScore(otherPlayer, x, y);
        if(score > bestScore) {
            bestScore = score;
            bestLocation = *it;
        }
    }
    unknownPieceLocations.erase(bestLocation);
    determinization.replacePieceAt(bestLocation.first, bestLocation.second, flag);
    // Place other pieces
    for(piece_it = unknownPieces.begin(); piece_it != unknownPieces.end(); piece_it++) {
        bestScore = -1;
        for(it = unknownPieceLocations.begin(); it != unknownPieceLocations.end(); it++) {
            x = it->first;
            y = it->second;
            score = determinizationPiecePlacementScore(&determinization, otherPlayer, *piece_it, x, y);
            if(score > bestScore) {
                bestScore = score;
                bestLocation = *it;
            }
        }
        determinization.replacePieceAt(bestLocation.first, bestLocation.second, *piece_it);
    }

    return determinization;
}

Player GameState::getPlayerTurn() {
    return player_turn;
}

Player GameState::getOwner(char x, char y) {
    return owner[x][y];
}

void GameState::computeDeploymentMoves() {
    std::vector<Piece> deployablePieces = getDeployablePieces();
    std::vector<Piece>::iterator it;
    assert(state == deployment);

    deploymentMoves.clear();

    for(int x = 0; x < 10; x++) {
        for(int y = (player_turn == red ? 0 : 6); y < (player_turn == red ? 4 : 10); y++) {
            if(isEmpty(x, y)) {
                for (it = deployablePieces.begin(); it != deployablePieces.end(); it++) {
                    deploymentMoves.push_back(DeploymentMove(x, y, *it));
                }
            }
        }
    }
}

void GameState::computeMoves() {
    Piece piece;
    std::set<std::pair<char, char>>::iterator it;
    char x, y;
    assert(state == movement);

    moves.clear();

    for(it = piece_location_cache[player_turn].begin(); it != piece_location_cache[player_turn].end(); it++) {
        x = it->first;
        y = it->second;
        if(getOwner(x, y) == player_turn) {
            assert(!isEmpty(x, y));
            piece = getPieceAt(x, y);
            if(isMovable(piece)) {
                if(piece == scout) {
                    addScoutMoves(&moves, x, y);
                } else {
                    addNormalMoves(&moves, x, y);
                }
            }
        }
    }
    if(moves.size() == 0) {
        moves.push_back(Move(true));
    }
}

State GameState::getState() {
    return state;
}

void GameState::newGame() {
    turn = 1;
    player_turn = red;
    state = deployment;
    has_winner = false;
    winner = none;
    num_pieces_to_deploy = 40;
    num_moves_since_capture = 0;
    lastMovesStack.clear();
    piece_location_cache[red].clear();
    piece_location_cache[blue].clear();

    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 10; y++) {
            pieces[x][y] = { nopiece };
            owner[x][y] = { none };
            visibility[x][y] = { unknown };
        }
    }

    for (int p = 0; p < 2; p++) {
        pieces_to_deploy[p][bomb] = 6;
        pieces_to_deploy[p][marshal] = 1;
        pieces_to_deploy[p][general] = 1;
        pieces_to_deploy[p][spy] = 1;
        pieces_to_deploy[p][flag] = 1;
        pieces_to_deploy[p][colonel] = 2;
        pieces_to_deploy[p][major] = 3;
        pieces_to_deploy[p][captain] = 4;
        pieces_to_deploy[p][lieutenant] = 4;
        pieces_to_deploy[p][sergeant] = 4;
        pieces_to_deploy[p][miner] = 5;
        pieces_to_deploy[p][scout] = 8;
    }
    for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 4; y++) {
            owner[x][y] = red;
        }
        for (int y = 6; y < 10; y++) {
            owner[x][y] = blue;
        }
    }
    seedRNG(0);
    computeDeploymentMoves();
}

void GameState::doDeploymentMove(DeploymentMove deploymentMove) {
    assert(state == deployment);
    removeDeploymentPiece(player_turn, deploymentMove.piece);
    pieces[deploymentMove.x][deploymentMove.y] = deploymentMove.piece;
    owner[deploymentMove.x][deploymentMove.y] = player_turn;
    updatePieceLocationCache(deploymentMove.x, deploymentMove.y, deploymentMove.piece, player_turn);

    if(num_pieces_to_deploy == 0) {
        if(player_turn == red) {
            player_turn = blue;
            num_pieces_to_deploy = 40;
            computeDeploymentMoves();
        } else {
            player_turn = red;
            state = movement;
            computeMoves();
        }
    } else {
        computeDeploymentMoves();
    }
}

void GameState::removeDeploymentPiece(Player player, Piece piece) {
    assert(pieces_to_deploy[player][piece] > 0);
    pieces_to_deploy[player][piece] -= 1;
    num_pieces_to_deploy -= 1;
}

int GameState::getTurn() {
    return turn;
}

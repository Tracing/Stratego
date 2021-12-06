#include <assert.h>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <iostream>
#include <random>
#include <set>
#include "helper.h"
#include "last_move.h"
#include "move.h"

#ifndef STATE_H
#define STATE_H

using namespace std;

class GameState {
    public:
        int turn = 1;
        Player player_turn = red;
        State state = deployment;
        bool has_winner = false;
        Player winner = none;
        int num_pieces_to_deploy = 40;
        int pieces_to_deploy[2][13];
        int num_moves_since_capture = 0;
        int n_threads = 1;
        GameState() {
            turn = 1;
            player_turn = red;
            state = deployment;
            has_winner = false;
            winner = none;
            num_pieces_to_deploy = 40;
            num_moves_since_capture = 0;
        }
        std::vector<LastMove> lastMovesStack;
        void doDeploymentMove(DeploymentMove deploymentMove);
        void doMove(Move move);
        void undoMove();
        void doRandomMove();
        void doRandomMoves(int max_depth);
        void seedRNG(int seed);
        std::vector<DeploymentMove> getDeploymentMoves();
        std::vector<Move> getMoves();
        void newGame();
        Piece getPieceAt(char x, char y);
        Player getOwner(char x, char y);
        bool isEmpty(char x, char y);
        Player getPlayerTurn();
        State getState();
        bool hasWinner();
        Player getWinner();
        Player getOtherPlayerTurn();
        Visibility getVisibility(char x, char y);
        GameState sampleDeterminization(Player player_pov);
        GameState copy();
        int getTurn();
        void replacePieceAt(char x, char y, Piece newPiece);
        bool tryReplacePieceAt(char x, char y, Piece newPiece);
        Piece pieces[10][10] = { nopiece };
        Player owner[10][10] = { none };
        Visibility visibility[10][10] = { unknown };
        int getNumPieces(Player player);
        int getNumPiecesofType(Player player, Piece piece);
        int getNumPiecesOfVisibility(Player player, Visibility visibility);
        const std::set<std::pair<char, char>>* getPieceLocations(Player player);
    private:
        void updatePieceLocationCache(char x, char y, Piece piece, Player player);
        void removePieceAt(char x, char y);
        void movePiece(char x, char y, char dx, char dy);
        void capturePiece(char x, char y, char dx, char dy);
        void removeDeploymentPiece(Player player, Piece piece);
        std::vector<Piece> getDeployablePieces();
        void addNormalMoves(std::vector<Move>* moves, char x, char y);
        void addMove(std::vector<Move>* moves, char x, char y, char dx, char dy);
        void addScoutMoves(std::vector<Move>* moves, char x, char y);
        void addScoutMovesRecursive(std::vector<Move>* moves, char sx, char sy, char x, char y, char dx, char dy);
        void computeDeploymentMoves();
        void computeMoves();
        std::vector<DeploymentMove> deploymentMoves;
        std::vector<Move> moves;
        std::set<std::pair<char, char>> piece_location_cache[2];
        static int cmpDeterminization(const void *a, const void *b);
        static int determinizationPieceRank(Piece piece);
};

bool isPieceAtNoError(GameState* state, char x, char y);

#endif

#include "helper.h"

const static char piece_rank[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 11, 0};

bool isMovable(Piece piece) {
    return piece != bomb && piece != flag;
}

bool isLake(char x, char y) {
    return (y == 4 || y == 5) && (x == 2 || x == 3 || x == 6 || x == 7);
}

bool inBoard(char x, char y) {
    return x >= 0 && x < 10 && y >= 0 && y < 10;
}

char getPieceRank(Piece piece) {
    return piece_rank[piece];
}

bool isSuccessfulAttack(Piece piece1, Piece piece2) {
    if(piece2 == flag) {
        return true;
    } else if(piece2 == bomb) {
        return piece1 == miner;
    } else if(piece1 == spy) {
        return piece2 == marshal;
    } else {
        return getPieceRank(piece1) > getPieceRank(piece2);
    }
}

int distanceFromHome(Player player, char y) {
    if(player == red) {
        return y;
    } else {
        return 9 - y;
    }
}

char getPieceChar(Piece piece) {
    if(piece == spy) {
        return 'S';
    } else if(piece == scout) {
        return '2';
    } else if(piece == miner) {
        return '3';
    } else if(piece == sergeant) {
        return '4';
    } else if(piece == lieutenant) {
        return '5';
    } else if(piece == captain) {
        return '6';
    } else if(piece == major) {
        return '7';
    } else if(piece == colonel) {
        return '8';
    } else if(piece == general) {
        return '9';
    } else if(piece == marshal) {
        return 'M';
    } else if(piece == flag) {
        return 'F';
    } else if(piece == bomb) {
        return 'B';
    } else {
        return '.';
    }
}

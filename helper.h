#include "enums.h"
#include <assert.h>

#ifndef HELPER_H
#define HELPER_H

bool isMovable(Piece piece);
bool isLake(char x, char y);
bool inBoard(char x, char y);
char getPieceRank(Piece piece);
char getPieceChar(Piece piece);
bool isSuccessfulAttack(Piece piece1, Piece piece2);
int distanceFromHome(Player player, char y);

#endif

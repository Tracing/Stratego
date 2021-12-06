#include "enums.h"

#ifndef LAST_MOVE_H
#define LAST_MOVE_H


class LastMove
{
public:
    int x, y, dx, dy, num_moves_since_capture;
    Player from_player, to_player;
    Visibility from_visibility, to_visibility;
    Piece from_piece, to_piece;

    LastMove() {
        ;
    }

    LastMove(int x, int y, int dx, int dy, Player from_player, Player to_player,
             Visibility from_visibility, Visibility to_visibility, Piece from_piece, Piece to_piece,
             int num_moves_since_capture) {
        this->x = x;
        this->dx = dx;
        this->y = y;
        this->dy = dy;
        this->from_player = from_player;
        this->to_player = to_player;
        this->from_visibility = from_visibility;
        this->to_visibility = to_visibility;
        this->from_piece = from_piece;
        this->to_piece = to_piece;
        this->num_moves_since_capture = num_moves_since_capture;
    }
};

#endif // LAST_MOVE_H

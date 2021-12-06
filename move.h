#include <cstdbool>
#include "enums.h"

#ifndef MOVE_H
#define MOVE_H

class DeploymentMove {
    public:
        char x;
        char y;
        Piece piece;

        DeploymentMove(char move_x, char move_y, Piece move_piece) {
            x = move_x;
            y = move_y;
            piece = move_piece;
        }

        DeploymentMove() {
            x = 0;
            y = 0;
            piece = nopiece;
        }
};

class Move {
    public:
        char x;
        char y;
        char dx;
        char dy;
        bool resign;

        Move(char _x, char _y, char _dx, char _dy) {
            x = _x;
            y = _y;
            dx = _dx;
            dy = _dy;
            resign = false;
        }

        Move(bool _resign) {
            x = 0;
            y = 0;
            dx = 0;
            dy = 0;
            resign = _resign;
        }

        Move() {
            //Null move
            x = 0;
            y = 0;
            dx = 0;
            dy = 0;
            resign = false;
        }

        bool equals(Move* other) {
            return x == other->x && y == other->y && dx == other->dx && dy == other->dy && resign == other->resign;
        }
};

#endif

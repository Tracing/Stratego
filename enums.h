#ifndef ENUMS_H
#define ENUMS_H

enum Piece { spy = 0, scout = 1, miner = 2, sergeant = 3, lieutenant = 4, captain = 5, major = 6,
colonel = 7, general = 8, marshal = 9, bomb = 10, flag = 11, nopiece = 12};

enum Player: int { red = 0, blue = 1, none = 2};

enum Visibility { unknown = 0, unknown_movable = 1, visible = 2};

enum State { deployment, movement, end_of_game };

#endif

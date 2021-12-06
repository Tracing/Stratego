#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "ai_functions.h"
#include "monte_carlo.h"
#include "engine.h"
#include "enums.h"
#include "move.h"
#include "state.h"

DeploymentMove newDeploymentMove(int move_x, int move_y, Piece move_piece) {
    return DeploymentMove(move_x, move_y, move_piece);
}

Move newMove(int x, int y, int dx, int dy, bool resign) {
    if(resign) {
        return Move(resign);
    } else {
        return Move(x, y, dx, dy);
    }
}

MonteCarloAgent newMonteCarloAgent(int n, int d, int m, double beta1, double beta2, double beta3, double beta4) {
    double betas[4];
    betas[0] = beta1;
    betas[1] = beta2;
    betas[2] = beta3;
    betas[3] = beta4;
    return MonteCarloAgent(n, d, m, betas);
}

PYBIND11_MODULE(stratego, m) {
    pybind11::class_<Agent>(m, "Agent")
            .def(pybind11::init<>())
            .def("do_deployment", &Agent::do_deployment)
            .def("get_move", &Agent::get_move);
    pybind11::class_<MonteCarloAgent, Agent>(m, "MonteCarloAgent")
            .def(pybind11::init(&newMonteCarloAgent))
            .def("do_deployment", &MonteCarloAgent::do_deployment)
            .def("get_move", &MonteCarloAgent::get_move);
    pybind11::class_<Game>(m, "Game")
            .def(pybind11::init<>())
            .def("setAgent", &Game::setAgent)
            .def("play", &Game::play)
            .def("getGameState", &Game::getGameState);
    pybind11::class_<DeploymentMove>(m, "DeploymentMove")
    .def(pybind11::init(&newDeploymentMove))
    .def_readonly("x", &DeploymentMove::x)
    .def_readonly("y", &DeploymentMove::y)
    .def_readonly("piece", &DeploymentMove::piece);

    pybind11::class_<Move>(m, "Move")
    .def(pybind11::init(&newMove))
    .def_readonly("x", &Move::x)
    .def_readonly("y", &Move::y)
    .def_readonly("dx", &Move::dx)
    .def_readonly("dy", &Move::dy)
    .def_readonly("resign", &Move::resign);

    pybind11::class_<GameState>(m, "GameState")
    .def(pybind11::init<>())
    .def("doDeploymentMove", &GameState::doDeploymentMove)
    .def("doMove", &GameState::doMove)
    .def("getDeploymentMoves", &GameState::getDeploymentMoves)
    .def("getMoves", &GameState::getMoves)
    .def("newGame", &GameState::newGame)
    .def("getPieceAt", &GameState::getPieceAt)
    .def("getOwner", &GameState::getOwner)
    .def("isEmpty", &GameState::isEmpty)
    .def("getPlayerTurn", &GameState::getPlayerTurn)
    .def("getState", &GameState::getState)
    .def("hasWinner", &GameState::hasWinner)
    .def("getWinner", &GameState::getWinner)
    .def("getOtherPlayerTurn", &GameState::getOtherPlayerTurn)
    .def("getVisibility", &GameState::getVisibility)
    .def("sampleDeterminization", &GameState::sampleDeterminization)
    .def("copy", &GameState::copy)
    .def("doRandomMove", &GameState::doRandomMove)
    .def("doRandomMoves", &GameState::doRandomMoves)
    .def("getNumPieces", &GameState::getNumPieces)
    .def("getNumPiecesOfVisibility", &GameState::getNumPiecesOfVisibility)
    .def("getNumPiecesofType", &GameState::getNumPiecesofType)
    .def("seedRNG", &GameState::seedRNG)
    .def("getTurn", &GameState::getTurn);

    pybind11::enum_<Piece>(m, "Piece")
    .value("spy", Piece::spy)
    .value("scout", Piece::scout)
    .value("miner", Piece::miner)
    .value("sergeant", Piece::sergeant)
    .value("lieutenant", Piece::lieutenant)
    .value("captain", Piece::captain)
    .value("major", Piece::major)
    .value("colonel", Piece::colonel)
    .value("general", Piece::general)
    .value("marshal", Piece::marshal)
    .value("bomb", Piece::bomb)
    .value("flag", Piece::flag)
    .value("nopiece", Piece::nopiece);

    pybind11::enum_<Player>(m, "Player")
    .value("red", Player::red)
    .value("blue", Player::blue)
    .value("none", Player::none);

    pybind11::enum_<Visibility>(m, "Visibility")
    .value("unknown", Visibility::unknown)
    .value("unknown_movable", Visibility::unknown_movable)
    .value("visible", Visibility::visible);

    pybind11::enum_<State>(m, "State")
    .value("deployment", State::deployment)
    .value("end_of_game", State::end_of_game)
    .value("movement", State::movement);

    m.def("heuristic", &heuristic);
}

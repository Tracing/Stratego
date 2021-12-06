#include "ai_functions.h"
#include "engine.h"
#include "monte_carlo.h"

int main(int ac, char** av) {
    MonteCarloAgent agent1 = MonteCarloAgent(5000, 20, 8);
    MonteCarloAgent agent2 = MonteCarloAgent(5000, 20, 8);
    Game g;
    g.setAgent(blue, &agent1);
    g.setAgent(red, &agent2);
    g.play(500, true);
    /*GameState s;
    GameState s2;
    GameState s3;
    std::vector<DeploymentMove> deploymentMoves;
    s.newGame();
    do_semirandom_deployment(&s, &deploymentMoves);
    do_semirandom_deployment(&s, &deploymentMoves);
    std::cout << "----First state----\n";
    print_game_state(&s);
    std::cout << "----Determinize from red pov----\n";
    s2 = s.sampleDeterminization(red);
    print_game_state(&s2);
    std::cout << "----Determinize from blue pov----\n";
    s3 = s.sampleDeterminization(blue);
    print_game_state(&s3);
    std::cout << "----First state----\n";
    print_game_state(&s);*/
    /*for(int i = 0; i < 20000; i++) {
        s = GameState();
        s.newGame();
        do_semirandom_deployment(&s, &deploymentMoves);
        do_semirandom_deployment(&s, &deploymentMoves);
        s.doRandomMoves(1500);
    }*/

}

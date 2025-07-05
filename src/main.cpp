#include "main.h"
#include "poker.h"
#include "network.h"
#include "client.h"

int main(int argc, char* argv[]){
    std::string arg1 = "";
    if(argc > 1){
        arg1 = argv[1];
    }
    if(argc > 1 && arg1 == "-c"){
        Client client("127.0.0.1", 9603);
    } else {
        Server server(9603);
        server.wait_for_players();
    }

    /*
    Poker poker_game;
    poker_game.setup();    
    poker_game.start_round();
    */
}
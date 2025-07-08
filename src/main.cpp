#include "main.h"
#include "poker.h"
#include "network.h"
#include "client.h"
#include <iostream>

int main(int argc, char* argv[]){
    std::string arg1 = "";
    if(argc > 1){
        arg1 = argv[1];
    }
    if(argc > 1 && arg1 == "-c"){
        std::string ip;
        std::cout << "enter server ip";
        std::cin >> ip;
        Client client(ip, 9603);
        client.run();
    } else {
        Server* server = new Server(9603);
        server->wait_for_players();
        Poker poker(server);
        poker.setup();
        poker.start_round();
        delete server;
    }

    /*
    Poker poker_game;
    poker_game.setup();    
    poker_game.start_round();
    */
}
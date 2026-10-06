#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>

#include "pokerStateMachine.h"

PokerServerStateMachine::PokerServerStateMachine(unsigned short port){
    if (listener.listen(port) != sf::Socket::Status::Done) {
        std::cerr << "Error: Could not bind the listener to port " << port << "\n";
        return;
    }

    std::cout << "Server is listening on port " << port << "...\n";

    selector.add(listener);
}

PokerServerStateMachine::~PokerServerStateMachine(){
    listener.close();
}

void PokerServerStateMachine::update(){
    switch(state){
        case PokerServerState::WaitingForPlayers:
            waitingForPlayers();
            break;
        case PokerServerState::GetStartChips:
            getStartChips();
            break;
        case PokerServerState::DealHandCards:
            DealHandCards();
            break;
    }
}

void PokerServerStateMachine::waitingForPlayers(){
    if(selector.wait(sf::milliseconds(20))){
        // New client is trying to connect
        if (selector.isReady(listener)) {
            auto client = std::make_unique<sf::TcpSocket>();
            if (listener.accept(*client) == sf::Socket::Status::Done) {
                auto new_player = new NetworkPlayer();
                client->setBlocking(false);
                selector.add(*client);
                new_player->player_socket = std::move(client);

                std::string message = "id ";
                if(clients.empty()){
                    new_player->is_master = true;
                    message += "master";
                } else {
                    message += "normal";
                }

                sf::TcpSocket& socket = *new_player->player_socket;
                if (!send_all(socket, message.c_str(), message.size())) {
                    std::cerr << "Warning: Failed to send data to client.\n";
                }


                clients.push_back(new_player);
                std::cout << "New client connected! Total clients: " << clients.size() << "\n";
            }
        }

        // Check each client for incoming data
        for (auto it = clients.begin(); it != clients.end(); ) {
            sf::TcpSocket& socket = *(*it)->player_socket;
            if (selector.isReady(socket)) {
                char buffer[1024];
                std::size_t received;
                sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);

                if (status == sf::Socket::Status::Done) {
                    std::string msg(buffer, received);
                    std::cout << "Received: " << msg << "\n";
                    if((*it)->is_master && msg == "start"){
                        for(auto client : clients){
                            if(!client->is_master){
                                std::string startId = "start";
                                send_all(*client->player_socket, startId.c_str(), startId.size());
                            }
                        }
                        initGetStartChips(); //go to next state
                    }
                } else if (status == sf::Socket::Status::Disconnected) {
                    std::cout << "Client disconnected.\n";
                    selector.remove(socket);
                    delete (*it);
                    it = clients.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }
}

void PokerServerStateMachine::initGetStartChips(){
    state = PokerServerState::GetStartChips;
    std::cout << "STATE getChipsState" << std::endl;

    int chips = -1;
    for(auto client : clients){
        if(client->is_master){
            masterClient = client;
        }
    }
    if(masterClient == nullptr){
        return;
    }
    std::string ask_msg = "aint startchips";
    masterClient->sendMessage(ask_msg);
    startTime = std::chrono::steady_clock::now();
}

void PokerServerStateMachine::getStartChips(){
    int startChips = -1;

    if(startChips > 0){ 
        setupRound(startChips); //goes to next state
    }


    if(std::chrono::steady_clock::now()-startTime > std::chrono::milliseconds(500)){
        startChips = 300;
    }
    if(!masterClient){
        startChips = 300;
        return; 
    }
    std::string msg;
    if(masterClient->tryReceive(msg)){
        std::cout << "message received: " << msg << std::endl; 
        startChips = atoi(msg.c_str());
    }
}

void PokerServerStateMachine::setupRound(unsigned int startChips){
    for(int i = 0; i < clients.size(); i++){
        Player* player = new Player(startChips, clients[i]);
        if(i==0){
            player->role = Role::dealer;
        } else if(i==1){
            player->role = Role::small_blind;
        } else if(i==2){
            player->role = Role::big_blind;
        }
        player->player_num = i;
        players.push_back(player);
    }
    std::cout << "generated all players" << std::endl;

    community_cards.clear();
    Deck new_deck(true);
    deck = new_deck;
    deck.shuffle();
    active_players = players.size();
}

void PokerServerStateMachine::initDealHandCards(){
    //deal 2 cards to each player
    for(Player* player : players){
        player->set_hand(deck.take_cards(2));
        player->network->send_deck(player->hand, "hand");
    }
}
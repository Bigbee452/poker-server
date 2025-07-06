#include "network.h"
#include <iostream>
#include <memory>

Server::Server(unsigned short port){
    if (listener.listen(port) != sf::Socket::Status::Done) {
        std::cerr << "Error: Could not bind the listener to port " << port << "\n";
        return;
    }

    std::cout << "Server is listening on port " << port << "...\n";

    selector.add(listener);
}

void Server::wait_for_players(){
    while (true) {
        if (selector.wait()) {
            // New client is trying to connect
            if (selector.isReady(listener)) {
                auto new_player = std::make_unique<NetworkPlayer>();
                auto client = std::make_unique<sf::TcpSocket>();
                if (listener.accept(*client) == sf::Socket::Status::Done) {
                    client->setBlocking(false);
                    selector.add(*client);
                    new_player->player_socket = std::move(client);

                    std::string message = "";
                    if(clients.empty()){
                        new_player->is_master = true;
                        message += "master";
                    } else {
                        message += "normal";
                    }

                    sf::TcpSocket& socket = *new_player->player_socket;
                    sf::Socket::Status sendStatus = socket.send(message.c_str(), message.size());
                    if (sendStatus != sf::Socket::Status::Done) {
                        std::cerr << "Warning: Failed to send data to client.\n";
                    }


                    clients.push_back(std::move(new_player));
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
                            return;
                        }
                    } else if (status == sf::Socket::Status::Disconnected) {
                        std::cout << "Client disconnected.\n";
                        selector.remove(socket);
                        it = clients.erase(it);
                        continue;
                    }
                }
                ++it;
            }
        }
    }
}
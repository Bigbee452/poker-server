#include "network.h"
#include <iostream>

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
                auto client = std::make_unique<sf::TcpSocket>();
                if (listener.accept(*client) == sf::Socket::Status::Done) {
                    client->setBlocking(false);
                    selector.add(*client);
                    clients.push_back(std::move(client));
                    std::cout << "New client connected! Total clients: " << clients.size() << "\n";
                }
            }

            // Check each client for incoming data
            for (auto it = clients.begin(); it != clients.end(); ) {
                sf::TcpSocket& socket = *(*it);
                if (selector.isReady(socket)) {
                    char buffer[1024];
                    std::size_t received;
                    sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);

                    if (status == sf::Socket::Status::Done) {
                        std::string msg(buffer, received);
                        std::cout << "Received: " << msg << "\n";

                        // Echo message back
                        sf::Socket::Status sendStatus = socket.send(msg.c_str(), msg.size());
                        if (sendStatus != sf::Socket::Status::Done) {
                            std::cerr << "Warning: Failed to send data to client.\n";
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
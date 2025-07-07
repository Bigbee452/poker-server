#include "network.h"
#include <SFML/System/Time.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

bool send_all(sf::TcpSocket& socket, const void* data, std::size_t totalSize) {
    const char* buffer = static_cast<const char*>(data);
    std::size_t sentTotal = 0;

    while (sentTotal < totalSize) {
        std::size_t sent = 0;
        sf::Socket::Status status = socket.send(buffer + sentTotal, totalSize - sentTotal, sent);
        if (status != sf::Socket::Status::Done) {
            return false; // Error or disconnected
        }
        sentTotal += sent;
    }

    return true;
}

bool receive_with_timeout(sf::TcpSocket& socket, std::string& outMessage, sf::Time timeout) {
    socket.setBlocking(false); // Make the socket non-blocking

    auto start = std::chrono::steady_clock::now();
    char buffer[1024];
    std::size_t received;

    while (true) {
        sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);
        if (status == sf::Socket::Status::Done) {
            outMessage = std::string(buffer, received);
            return true; // Successfully received
        } else if (status == sf::Socket::Status::Disconnected) {
            std::cerr << "Disconnected from server.\n";
            return false;
        }

        // Check timeout
        auto now = std::chrono::steady_clock::now();
        if (now - start > std::chrono::milliseconds(timeout.asMilliseconds())) {
            std::cout << "Timeout reached. No data received.\n";
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // avoid busy waiting
    }
}

int NetworkPlayer::ask_int(std::string& msg_str){
    std::string id = "aint";
    if(!send_all(*player_socket, id.c_str(), id.size())){
        std::cout << "failed to send message to client!" << std::endl;
        return -1;
    }
    std::string received;
    receive_with_timeout(*player_socket, received, sf::seconds(5));
    if(!(received == "aint ok")){
        return -1;
    }
    if(!send_all(*player_socket, msg_str.c_str(), msg_str.size())){
        std::cout << "failed to send message to client!" << std::endl;
        return -1;
    }
    receive_with_timeout(*player_socket, received, sf::seconds(60));
    return atoi(received.c_str());
}  

Server::Server(unsigned short port){
    if (listener.listen(port) != sf::Socket::Status::Done) {
        std::cerr << "Error: Could not bind the listener to port " << port << "\n";
        return;
    }

    std::cout << "Server is listening on port " << port << "...\n";

    selector.add(listener);
}

Server::~Server(){
    listener.close();
}

void Server::wait_for_players(){
    while (true) {
        if (selector.wait()) {
            // New client is trying to connect
            if (selector.isReady(listener)) {
                auto client = std::make_unique<sf::TcpSocket>();
                if (listener.accept(*client) == sf::Socket::Status::Done) {
                    auto new_player = new NetworkPlayer();
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
                            return;
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
}
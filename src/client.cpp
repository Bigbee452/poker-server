#include "client.h"
#include <iostream>

Client::Client(const std::string& serverIp, unsigned short port){
    std::cout << "Connecting to server " << serverIp << ":" << port << "...\n";
    sf::IpAddress ip = sf::IpAddress::resolve(serverIp).value();
    if (socket.connect(ip, port) != sf::Socket::Status::Done) {
        std::cerr << "Error: Failed to connect to server.\n";
        return;
    }

    std::cout << "Connected to server!\n";
}

Client::~Client(){
    socket.disconnect();
}

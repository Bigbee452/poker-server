#pragma once
#include <SFML/Network.hpp>

class Client {
    public:
        Client(const std::string& serverIp, unsigned short port);
        ~Client();
    private:
        sf::TcpSocket socket;
};
#pragma once
#include <SFML/Network.hpp>

class Client {
    public:
        Client(const std::string& serverIp, unsigned short port);
        ~Client();
        void run();
        void return_int();
        void get_hand();
        bool is_master = false;
        bool disconnected = false;
    private:
        sf::TcpSocket socket;
        bool receiveWithTimeout(sf::TcpSocket& socket, std::string& outMessage, sf::Time timeout);
};
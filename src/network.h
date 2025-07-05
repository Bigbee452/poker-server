#pragma once
#include <SFML/Network.hpp>

class Server {
    public:
        std::vector<std::unique_ptr<sf::TcpSocket>> clients;

        Server(unsigned short port);
        void wait_for_players();
    private:
        sf::TcpListener listener;
        sf::SocketSelector selector;
};
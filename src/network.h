#pragma once
#include <SFML/Network.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <memory>

class NetworkPlayer {
    public:
        bool is_master = false;
        std::unique_ptr<sf::TcpSocket> player_socket;
};

class Server {
    public:
        std::vector<std::unique_ptr<NetworkPlayer>> clients;

        Server(unsigned short port);
        void wait_for_players();
    private:
        sf::TcpListener listener;
        sf::SocketSelector selector;
};
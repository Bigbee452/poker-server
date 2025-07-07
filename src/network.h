#pragma once
#include <SFML/Network.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <memory>

#include "cards.h"

bool send_all(sf::TcpSocket& socket, const void* data, std::size_t totalSize);
bool receive_with_timeout(sf::TcpSocket& socket, std::string& outMessage, sf::Time timeout);

class NetworkPlayer {
    public:
        bool is_master = false;
        std::unique_ptr<sf::TcpSocket> player_socket;
        int ask_int(std::string& msg_str);
        void send_deck(Deck& deck, std::string id);
        int get_bet(bool can_raise);
};

class Server {
    public:
        std::vector<NetworkPlayer*> clients;

        Server(unsigned short port);
        ~Server();
        void wait_for_players();
    private:
        sf::TcpListener listener;
        sf::SocketSelector selector;
};
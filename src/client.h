#pragma once
#include "cards.h"
#include <SFML/Network.hpp>

class Client {
    public:
        Client(const std::string& serverIp, unsigned short port);
        ~Client();
        void run();
        void return_int();
        Deck get_cards();
        void get_hand();
        void get_community_cards();
        void get_bet();
        void get_last_bet();
        void get_chips();
        bool is_master = false;
        bool disconnected = false;
    private:
        sf::TcpSocket socket;
        bool receiveWithTimeout(sf::TcpSocket& socket, std::string& outMessage, sf::Time timeout);
};
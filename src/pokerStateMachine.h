#pragma once
#include <SFML/Network.hpp>
#include <SFML/Network/TcpSocket.hpp>

#include "network.h"
#include "player.h"


enum class PokerServerState {
    WaitingForPlayers,
    GetStartChips,
    DealHandCards,
};

enum class PokerGameState {
    PreFlop,
    Flop,
    Turn,
    River
};

class PokerServerStateMachine {
    public: 
        PokerServerStateMachine(unsigned short port);
        ~PokerServerStateMachine();
        void update();
    private:
        PokerServerState state = PokerServerState::WaitingForPlayers;

        void waitingForPlayers();

        void initGetStartChips();
        void getStartChips();

        void setupRound(unsigned int startChips); //always called from getStartChips

        void initDealHandCards();
        void DealHandCards();

        sf::TcpListener listener;
        sf::SocketSelector selector;
        std::vector<NetworkPlayer*> clients;

        std::vector<Player*> players;
        int bettings = 0;
        Deck community_cards = {};
        Deck deck;
        int active_players = 0;

        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

        //getStartChips
        NetworkPlayer* masterClient = nullptr;
};
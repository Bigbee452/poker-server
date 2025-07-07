#pragma once
#include "cards.h"
#include "player.h"
#include "network.h"
#include <vector>

class Poker {
    public:
        int min_bet = 10;
        void setup();
        void start_round();
        Poker(Server* server) : server(server) {};
        ~Poker();
    private:
        Server* server;
        std::vector<Player*> players;
        int bettings = 0;
        Deck community_cards = {};
        Deck deck;
        int active_players = 0;
        int last_bet = 0;
        int number_of_bets = 0;

        void get_players();
        void pre_flop();
        void flop();
        void turn();
        void river();
};
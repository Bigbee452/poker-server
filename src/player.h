#pragma once
#include "cards.h"

enum class Role {
    normal,
    dealer,
    small_blind,
    big_blind
};

class Player {
    public:
        Role role = Role::normal;
        int player_num;
        bool folded = false;

        Player(int in_chips);
        void set_hand(std::vector<Card> in_hand);
        std::vector<Card> get_hand();
        int get_bet(int last_bet, int& bettings);
        bool has_highest_bet(int last_bet);
        void take_role_bet(int min_bet, int& bettings);
        void sub_round_reset();
        Deck hand;
    private:
        int chips;
        int already_bet = 0;
};
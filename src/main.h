#pragma  once
#include <vector>
#include <string>
#include <phevaluator/phevaluator.h>

struct Card {
    int card_id; //spades 0=ace, 2=2, ..., 12=king, Hearts 13=1, ..., Diamonds, Cubs
    int suit() const {
        return card_id/13; //spades hearts diamonds cubs
    }
    int rank() const {
        return card_id%13;
    }
    std::string card_str(){
        std::string suits[] = {"♠Spades", "♥Hearts", "♦Diamonds", "♣Cubs "};
        std::string ranks[] = {"Jack", "Queen", "King"};
        std::string suit = "";
        std::string rank = "";

        if(card_id%13 < 10){
            rank = std::to_string(card_id%13 + 1);
        } else {
            rank = ranks[card_id%13-10];
        } 
        suit = suits[card_id/13];
        std::string return_string = suit + " " + rank;
        return return_string;
    }

    std::string card_str_short(){
        std::string suits[] = {"s", "h", "d", "c"};
        std::string ranks[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K"}; 
        std::string return_string = ranks[rank()]+suits[suit()];
        return return_string;      
    }
};

class Deck {
    public:
        Deck(bool start_full);
        Deck();
        void shuffle();
        void clear();
        void add_cards(std::vector<Card> in_cards);
        std::vector<Card> take_cards(int number);
        std::vector<Card> vieuw_cards(int number);
        void print_deck();
        phevaluator::Rank get_poker_score();
    private:
        std::vector<Card> cards = {};

        void sort_rank();
        void sort_suit();
};

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

class Poker {
    public:
        int min_bet = 10;
        void setup();
        void start_round();
        ~Poker();
    private:
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


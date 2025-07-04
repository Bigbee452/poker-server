#include "main.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <vector>

Deck::Deck(bool start_full){
    if(start_full){
        for(int i = 0; i < 52;  i++){
            Card card;
            card.card_id = i;
            cards.push_back(card);
        }
    }
}

Deck::Deck(){

}

void Deck::shuffle(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<Card> shuffled_cards = {};
    int deck_size = cards.size();
    for(int i = 0; i < deck_size; i++){
        std::uniform_int_distribution<> dist(0, cards.size()-1);
        int random_index = dist(gen);
        Card card = cards[random_index];
        cards.erase(cards.begin()+random_index);
        shuffled_cards.push_back(card);
    }
    cards.clear();
    cards = std::move(shuffled_cards);
}

void Deck::clear(){
    cards.clear();
}

std::vector<Card> Deck::take_cards(int number){
    std::vector<Card> top_cards = {};
    for(int i = 0; i < number; i++){
        if(!cards.empty()){
            top_cards.push_back(cards.back());
            cards.pop_back();
        } else {
            std::cerr << "trying to take cards out of an empty deck" << std::endl;
        }
    }
    return top_cards;
}

std::vector<Card> Deck::vieuw_cards(int number){
    std::vector<Card> top_cards = {};
    for(int i = 1; i <= number; i++){
        if(i >= 0){
            top_cards.push_back(cards[cards.size()-i]);
        } else {
            std::cerr << "trying to take cards out of an empty deck" << std::endl;
        }
    }
    return top_cards;
}

void Deck::add_cards(std::vector<Card> in_cards){
    for(Card card : in_cards){
        cards.push_back(card);
    }
}

void Deck::print_deck(){
    for(Card card : cards){
        std::cout << card.card_str() << std::endl;
    }
}

phevaluator::Rank Deck::get_poker_score(){
    if(cards.size() != 7){
        std::cerr << "wrong number of cards to evaluate hand score!\n expected 7 got " << cards.size() << std::endl;
    }
    phevaluator::Rank rank = phevaluator::EvaluateCards(
                                                cards[0].card_str_short(), 
                                                cards[1].card_str_short(), 
                                                cards[2].card_str_short(), 
                                                cards[3].card_str_short(), 
                                                cards[4].card_str_short(), 
                                                cards[5].card_str_short(), 
                                                cards[6].card_str_short());

    std::cout << rank.describeRank() << std::endl;
     
    return rank;
}

void Deck::sort_rank(){
    std::sort(cards.begin(), cards.end(), [](const Card& a, const Card& b){return a.rank() < b.rank();});
}

void Deck::sort_suit(){
    std::sort(cards.begin(), cards.end(), [](const Card& a, const Card& b){return a.suit() < b.suit();});
}

Player::Player(int in_chips){
    chips = in_chips;
}

void Player::set_hand(std::vector<Card> in_hand){
    hand.clear();
    hand.add_cards(in_hand);
    std::cout << "player " << player_num+1 << " cards:" << std::endl;
    hand.print_deck();
}

void Player::take_role_bet(int min_bet, int& bettings){
    if(role == Role::big_blind){
        if(chips >= min_bet){
            chips -= min_bet;
            already_bet = min_bet;
            bettings += min_bet;
        } else {
            bettings += chips;
            already_bet = chips;
            chips = 0;
        }
    } else if(role == Role::small_blind){
        if(chips >= min_bet/2){
            chips -= min_bet/2;
            already_bet = min_bet/2;
            bettings += min_bet/2;
        } else {
            bettings += chips;
            already_bet = chips;
            chips = 0;
        }    
    }
}

int Player::get_bet(int last_bet, int& bettings){
    std::cout << "Player " << player_num+1 << " chips: " << chips << " last bet: " << last_bet << std::endl;
    std::cout << "(1) fold" << std::endl;
    std::cout << "(2) call" << std::endl;
    if(chips > last_bet){
        std::cout << "(3) raise" << std::endl;       
    }
    while(true){
        std::cout << "enter choice: ";
        int choice;
        std::cin >> choice;

        if(choice == 1){
            folded = true;
            return -1;
        } else if(choice == 2){
            if(chips < last_bet-already_bet){
                int bet = chips;
                already_bet += chips;
                bettings += chips;
                chips = 0;
                return bet;
            } else {
                chips -= last_bet-already_bet;
                bettings += last_bet-already_bet;
                already_bet = last_bet;
                return last_bet;
            }
        } else if(choice == 3 && chips > last_bet){
            std::cout << "enter raise amount: ";
            int raise;
            std::cin >> raise;
            if(chips < last_bet+raise-already_bet){
                already_bet += chips;
                bettings += chips;
                int bet = chips;
                chips = 0;
                return bet;
            } else {
                bettings += last_bet+raise-already_bet;
                chips -= last_bet+raise-already_bet;
                already_bet = last_bet+raise;
                return last_bet+raise;
            }
        } else {
            std::cout << "invallid choice, try again." << std::endl;
            std::cin.clear(); 
            std::cin.ignore(10000, '\n');
        }
    }
}

void Player::sub_round_reset(){
    already_bet = 0;
}

Poker::~Poker(){
    for(Player* player : players){
        delete player;
    }
}

void Poker::setup(){
    get_players();
}

void Poker::get_players(){
    std::cout << "how many players participate in this game (minimum 2): ";
    int num_players;
    std::cin >> num_players;
    std::cout << "starting game with " << num_players << " players" << std::endl;

    for(int i = 0; i < num_players;  i++){
        std::cout << "enter player " << i+1 << " chips: ";
        int chips;
        std::cin >> chips;
        Player* player = new Player(chips);
        if(i==0){
            player->role = Role::dealer;
        } else if(i==1){
            player->role = Role::small_blind;
        } else if(i==2){
            player->role = Role::big_blind;
        }
        player->player_num = i;
        players.push_back(player);
    }
}

void Poker::start_round(){
    community_cards.clear();
    Deck new_deck(true);
    deck = new_deck;
    deck.shuffle();
    active_players = players.size();

    pre_flop();
    flop();
    turn();
    river();

    if(active_players > 1){
        phevaluator::Rank rank;
        int prev_rank_value = -1;
        int winning_player_id = -1;
        for(Player* player : players){
            if(!player->folded){
                Deck check_deck;
                check_deck.add_cards(community_cards.vieuw_cards(5));
                check_deck.add_cards(player->hand.vieuw_cards(2));
                phevaluator::Rank player_rank = check_deck.get_poker_score();
                if(player_rank.value() < prev_rank_value || prev_rank_value == -1){
                    rank = player_rank;
                    prev_rank_value = player_rank.value();
                    winning_player_id = player->player_num;
                }
            }
        }
        std::cout << "player" << winning_player_id+1 << " won the game with a " << rank.describeRank() << std::endl;
    } else if(active_players == 1){
        int winning_player_id = -1;
        for(Player* player : players){
            if(!player->folded){
                winning_player_id = player->player_num;
            }
        }
        std::cout << "player" << winning_player_id +1 << " won the game because everybody else folded" << std::endl;
    } else {
        std::cerr << "nobody won?" << std::endl;
    }
}

void Poker::pre_flop(){
    //deal 2 cards to each player
    for(Player* player : players){
        player->set_hand(deck.take_cards(2));
    }

    //bets code
    number_of_bets = 0;
    //find index of first one to bet
    int start_index = 0;
    if(active_players <= 3){
        for(int i = 0; i < players.size(); i++){
            if(players[i]->role == Role::dealer){
                start_index = i;
            }
        }
    } else if(active_players > 3){
        for(int i = 0; i < players.size(); i++){
            if(players[i]->role == Role::big_blind){
                start_index = i+1;
            }
        }    
        number_of_bets = 1;
    } else {
        std::cerr << "invalid number of players" << std::endl;
    }

    //let big and small blind place their bets
    for(Player* player : players){
        player->take_role_bet(min_bet, bettings);
    }

    last_bet = min_bet;
    //let players place their bets till they all agree 
    while(number_of_bets < active_players && active_players > 1){
        if(!players[start_index%players.size()]->folded){ //players that folded dont participate
            int player_bet = players[start_index%players.size()]->get_bet(last_bet, bettings);
            if(player_bet > last_bet){
                last_bet = player_bet;
                number_of_bets = 1;
            } else if(player_bet == -1){
                active_players -= 1;
            } else {
                number_of_bets++;
            }
        }
        start_index++;
    }

    std::cout << "total bettings: " << bettings << std::endl;
}

void Poker::flop(){
    if(active_players > 1){
        //deal community cards
        community_cards.add_cards(deck.take_cards(3));
        std::cout << "comunity cards: " << std::endl;
        community_cards.print_deck();

        //reset players, last bet and number of bets
        for(Player* player : players){
            player->sub_round_reset();
        }
        last_bet = 0;
        number_of_bets = 0;

        //find index of first one to bet
        int start_index = 0;
        for(int i = 0; i < players.size(); i++){
            if(players[i]->role == Role::dealer){
                start_index = i+1;
            }
        }

        //let the player bet
        while(number_of_bets < active_players && active_players > 1){
            if(!players[start_index%players.size()]->folded){ //players that folded dont participate
                int player_bet = players[start_index%players.size()]->get_bet(last_bet, bettings);
                if(player_bet > last_bet){
                    last_bet = player_bet;
                    number_of_bets = 1;
                } else if(player_bet == -1){
                    active_players -= 1;
                } else {
                    number_of_bets++;
                }
            }
            start_index++;
        }

        std::cout << "total bettings: " << bettings << std::endl;
    }
}

void Poker::turn(){
    if(active_players > 1){
        //deal community cards
        community_cards.add_cards(deck.take_cards(1));
        std::cout << "comunity cards: " << std::endl;
        community_cards.print_deck();

        //reset players, last bet and number of bets
        for(Player* player : players){
            player->sub_round_reset();
        }
        last_bet = 0;
        number_of_bets = 0;

        //find index of first one to bet
        int start_index = 0;
        for(int i = 0; i < players.size(); i++){
            if(players[i]->role == Role::dealer){
                start_index = i+1;
            }
        }

        //let the player bet
        while(number_of_bets < active_players && active_players > 1){
            if(!players[start_index%players.size()]->folded){ //players that folded dont participate
                int player_bet = players[start_index%players.size()]->get_bet(last_bet, bettings);
                if(player_bet > last_bet){
                    last_bet = player_bet;
                    number_of_bets = 1;
                } else if(player_bet == -1){
                    active_players -= 1;
                } else {
                    number_of_bets++;
                }
            }
            start_index++;
        }

        std::cout << "total bettings: " << bettings << std::endl;
    }
}

void Poker::river(){
    turn();
}

int main(){
    Poker poker_game;
    poker_game.setup();    
    poker_game.start_round();
}
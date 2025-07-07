#include "poker.h"
#include <iostream>
#include <string>

Poker::~Poker(){
    for(Player* player : players){
        delete player;
    }
}

void Poker::setup(){
    get_players();
}

void Poker::get_players(){
    int chips = 300;
    for(auto client : server->clients){
        if(client->is_master){
            std::string ask_msg = "enter start chips: ";
            chips = client->ask_int(ask_msg);
            std::cout << chips << std::endl;
        }
    }

    /*
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
    */
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
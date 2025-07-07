#include <iostream>

#include "player.h"
#include "network.h"

Player::Player(int chips, NetworkPlayer* network) : chips(chips), network(network) {
    
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
    bool can_raise = chips > last_bet;
    int choice = network->get_bet(can_raise);
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
    } else if(choice > 2){
        if(chips <= last_bet){
            int bet = chips;
            already_bet += chips;
            bettings += chips;
            chips = 0;
            return bet;
        }
        int raise = choice-3;
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
        std::cout << "invallid betting choise, folding player" << std::endl;
        folded = true;
        return -1;
    }
    /*
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
    */
}

void Player::sub_round_reset(){
    already_bet = 0;
}
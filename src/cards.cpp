#include <random>
#include <iostream>
#include <algorithm>

#include "cards.h"


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

     
    return rank;
}

void Deck::sort_rank(){
    std::sort(cards.begin(), cards.end(), [](const Card& a, const Card& b){return a.rank() < b.rank();});
}

void Deck::sort_suit(){
    std::sort(cards.begin(), cards.end(), [](const Card& a, const Card& b){return a.suit() < b.suit();});
}
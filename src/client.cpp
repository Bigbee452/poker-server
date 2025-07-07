#include "client.h"
#include <SFML/System/Time.hpp>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include <sstream>

#include "cards.h"
#include "network.h"

Client::Client(const std::string& serverIp, unsigned short port){
    std::cout << "Connecting to server " << serverIp << ":" << port << "...\n";
    sf::IpAddress ip = sf::IpAddress::resolve(serverIp).value();
    if (socket.connect(ip, port) != sf::Socket::Status::Done) {
        std::cerr << "Error: Failed to connect to server.\n";
        return;
    }

    std::cout << "Connected to server!\n";
    std::string message;
    if(receiveWithTimeout(socket, message, sf::seconds(5))){
        std::cout << "received identifier: " << message << std::endl;
        if(message == "master"){
            is_master = true;
        } else {
            is_master = false;
        }
    } else {
        std::cerr << "did not receive identifier message, disconnecting" << std::endl;
        disconnected = true;
        return;
    }
}

Client::~Client(){
    socket.disconnect();
}

void Client::return_int(){
    std::string ok_msg = "aint ok";
    send_all(socket, ok_msg.c_str(), ok_msg.size());
    std::string msg_in;
    int my_int = -2;
    if(receive_with_timeout(socket, msg_in, sf::seconds(5))){
        std::cout << msg_in;
        std::cin.clear();
        std::cin >> my_int;
    }
    std::string msg_out = std::to_string(my_int);
    send_all(socket, msg_out.c_str(), msg_out.size());
}

void Client::get_hand(){
    std::string ok_msg = "hand ok";
    send_all(socket, ok_msg.c_str(), ok_msg.size());
    std::string msg_in;
    if(!receive_with_timeout(socket, msg_in, sf::seconds(5))){
        std::cout << "get hand failed" << std::endl;
        return;
    }
    int number_of_cards = atoi(msg_in.c_str());
    std::cout << "number of receiving cards: " << number_of_cards << std::endl;
    send_all(socket, ok_msg.c_str(), ok_msg.size());

    if(!receive_with_timeout(socket, msg_in, sf::seconds(5))){
        std::cout << "get hand failed" << std::endl;
        return;
    }
    std::vector<Card> cards;
    std::string card_str;
    std::stringstream ss(msg_in);

    while (std::getline(ss, card_str, ',')) {
        Card card;
        card.card_id = atoi(card_str.c_str());
        cards.push_back(card);
    }
    Deck deck;
    deck.add_cards(cards);

    std::cout << "hand cards: " << std::endl;
    deck.print_deck();

    send_all(socket, ok_msg.c_str(), ok_msg.size());
}

void Client::run(){
    if(disconnected){
        return;
    }
    if(is_master){
        int chips;
        std::cout << "enter 1 to start the game: ";
        std::cin >> chips;
        std::string start_msg = "start";
        sf::Socket::Status sendStatus = socket.send(start_msg.c_str(), start_msg.size());
        if (sendStatus != sf::Socket::Status::Done) {
            std::cerr << "Warning: Failed to send data to client.\n";
        }
    }
    
    while(true){
        char buffer[1024];
        std::size_t received;
        socket.setBlocking(false); 
        std::string msg_in;
        sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);
        if (status == sf::Socket::Status::Done) {
            msg_in = std::string(buffer, received);
            if(msg_in == "aint"){
                return_int();
            } else if(msg_in == "hand"){
                get_hand();
            }
        } else if (status == sf::Socket::Status::Disconnected) {
            std::cerr << "Disconnected from server.\n";
            return;
        }
    }
}

bool Client::receiveWithTimeout(sf::TcpSocket& socket, std::string& outMessage, sf::Time timeout) {
    socket.setBlocking(false); // Make the socket non-blocking

    auto start = std::chrono::steady_clock::now();
    char buffer[1024];
    std::size_t received;

    while (true) {
        sf::Socket::Status status = socket.receive(buffer, sizeof(buffer), received);
        if (status == sf::Socket::Status::Done) {
            outMessage = std::string(buffer, received);
            return true; // Successfully received
        } else if (status == sf::Socket::Status::Disconnected) {
            std::cerr << "Disconnected from server.\n";
            return false;
        }

        // Check timeout
        auto now = std::chrono::steady_clock::now();
        if (now - start > std::chrono::milliseconds(timeout.asMilliseconds())) {
            std::cout << "Timeout reached. No data received.\n";
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // avoid busy waiting
    }
}

#include "client.h"
#include <SFML/System/Time.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <thread>

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

void Client::run(){
    if(disconnected){
        return;
    }
    if(is_master){
        std::string tempStr;
        std::cout << "press enter to start the game";
        std::cin >> tempStr;
        std::string start_msg = "start";
        sf::Socket::Status sendStatus = socket.send(start_msg.c_str(), start_msg.size());
        if (sendStatus != sf::Socket::Status::Done) {
            std::cerr << "Warning: Failed to send data to client.\n";
        }
    }
    while(true){

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

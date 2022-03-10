#include "crypto.h"
#include "server.h"
#include <iostream>
#include <string>
#include <random>
#include <memory>
#include <map>
#include <sstream>

std::vector<std::string> pending_trxs; 

Server :: Server() {};

std::shared_ptr<Client> Server::add_client(std::string id)
{
    if(get_client(id) != nullptr)
    {
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<int> distr(1000, 9999); // define the range
        id += std::to_string( distr(gen) );
    }
    // create new client and add it to previous clients 
    Client new_client { id, *this };
    std::shared_ptr <Client> new_ptr // ptr to new client
    { std :: make_shared <Client> (new_client) };
    clients.insert( std::pair<std :: shared_ptr <Client>, double>
    (new_ptr, 5.0 ));
    return new_ptr;
}

std::shared_ptr<Client> Server :: get_client(std::string id) const
{   
    
    for(auto itr {clients.begin()}; itr != clients.end(); ++itr) // find  client with input id
        if(id == itr->first->get_id())
            return itr->first; // return client ptr
    return nullptr;
}

double Server :: get_wallet(std::string id) const
{   
    return clients.find(get_client(id))->second; // find client by id and return it's wallet
}

bool Server::parse_trx(std::string trx, std::string &sender, std::string &receiver, double &value) 
{
    std::istringstream ss(trx);
    std::string s_val ;
    std::getline(ss, sender, '-'); // extract sender
    std::getline(ss, receiver, '-'); // extract receiver
    std::getline(ss, s_val, '-'); // extract value(string)

    if (!(trx == sender + "-" + receiver + "-" + s_val)) // checking concattion format
        throw std::runtime_error{"wrong trx"};

    if(s_val.empty()) // cheking value emptyness
        throw std::runtime_error{"wrong trx"};

    for(auto c : s_val) // cheking valid value
        if(!(std::isdigit(c) || c == '.'))
                throw std::runtime_error{"wrong trx"};

    value = std::stod(s_val); // turn value(string) to value(double)
    return true;
}


bool Server::add_pending_trx(std::string trx, std::string signature) const
{   
    // extracting sender receiver and value
    std::string sender, receiver;
    double value;
    parse_trx(trx, sender, receiver, value) ;

    std::shared_ptr<Client> sender_ptr { get_client(sender) }; // ptr to sender
    bool authentic = crypto::verifySignature(sender_ptr->get_publickey(), trx, signature); // signature check

    if((value <= get_wallet(sender) ) && authentic) // checking value and authentic
    {
        pending_trxs.push_back(trx); // add valid trx to pending_trx
        return true;   
    }
    return false;
}

size_t Server::mine()
{   
    //create mempool
    std::string mempool{};
    for(auto pnd_trx : pending_trxs)
        mempool += pnd_trx;

    while(true)
    {
        for (auto& client : clients )
        {   
            // get nonce and create a hash with that and mempool
            size_t client_nonce {client.first->generate_nonce()};
            std::string temp_pool {mempool + std::to_string(client_nonce) };
            std::string hash{crypto::sha256(temp_pool)};
            // check success  of mine
            bool success = false;
            if(hash.substr(0,10).find("000") != std::string::npos )
                success = true;
            if(success)
            {   
                // modify wallets according to trxs
                client.second += 6.25; // reward miner
                for (const auto trx : pending_trxs)
                {
                    std::string sender, receiver;
                    double value;
                    parse_trx(trx, sender, receiver, value);
                    std::shared_ptr<Client> sender_ptr { get_client(sender) } ;
                    clients.find(sender_ptr)->second -= value;
                    std::shared_ptr<Client> receiver_ptr { get_client(receiver) };
                    clients.find(receiver_ptr)->second += value;
                }
                std::cout << client.first->get_id() << std::endl;
                pending_trxs.clear(); // clearing pending_trx
                return client_nonce; // return miner nonce
            }
        }
    }
    return 0;
}

void  show_wallets(const  Server& server)
{
    std::cout << std::string(20, '*') << std::endl; 
 	for(const auto& client: server.clients)
 		std::cout << client.first->get_id() <<  " : "  << client.second << std::endl;
 	std::cout << std::string(20, '*') << std::endl;
}


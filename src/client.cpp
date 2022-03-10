
#include "client.h"
#include "crypto.h"
#include <random>

Client :: Client(std::string _id, const Server& _server)
: server {&_server }
, id {_id}
{  
    // initiallizing public_key and private_key
    crypto::generate_key(this->public_key, this->private_key);
}

std::string Client :: get_id()
{
    return id;
}

std::string Client :: get_publickey() const
{
    return public_key;
}

double Client::get_wallet()
{
    return server->get_wallet(id);
}

std::string Client::sign(std::string txt) const
{
    std::string signature = crypto::signMessage(this->private_key, txt); // sign trx
    return signature;
}

bool Client::transfer_money(std::string receiver, double value)
{   
    if(server->get_client(receiver)== nullptr) // check receiver existance
        return false;
    std::string trx{};
    trx = id + "-" + receiver + "-" + std::to_string(value); // create trx
    std::string signature {sign(trx)}; // sign trx
    if(server->add_pending_trx(trx, signature)) // pending trx
        return true;
    return false;
}

size_t Client::generate_nonce()
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<size_t> distr;
    return distr(gen);
}
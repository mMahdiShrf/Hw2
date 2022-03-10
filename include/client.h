#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include "server.h"
#include "crypto.h"

class Server; // depentent to Client(Class)

class Client
{
public  :
	Client(std::string _id, const Server& _server);
	std::string get_id();
	std::string get_publickey() const;
	double get_wallet();
	std::string sign(std::string txt) const;
	bool transfer_money(std::string receiver, double value);
	size_t generate_nonce();
private :
    Server const* const server;
	const std::string id;
	std::string public_key;
	std::string private_key;
};

#endif 
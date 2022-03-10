
#include <iostream>
#include <gtest/gtest.h>
#include "client.h"
#include "server.h"

void  show_pending_transactions();



int main(int argc, char **argv)
{
    if (false) // make false to run unit-tests
    {   
        Server iran{};
        std::string sender;
        std::string reciver;
        double value{};
        iran.add_client("mamad");
        iran.add_client("mahdi");
        iran.add_client("ali");
        iran.get_client("mahdi")->transfer_money("mamad",2.0);
        iran.get_client("ali")->transfer_money("mamad", 2.0);
        show_pending_transactions();
        show_wallets(iran);
        iran.mine();
        show_wallets(iran);
    }
    else
    {
        ::testing::InitGoogleTest(&argc, argv);
        std::cout << "RUNNING TESTS ..." << std::endl;
        int ret{RUN_ALL_TESTS()};
        if (!ret)
            std::cout << "<<<SUCCESS>>>" << std::endl;
        else
            std::cout << "FAILED" << std::endl;
    }
    return 0;   
}

void  show_pending_transactions()
{
 	std::cout  <<  std::string(20, '*') <<  std::endl;
 	for(const  auto& trx : pending_trxs)
 		std::cout << trx <<  std::endl;
 	std::cout  <<  std::string(20, '*') <<  std::endl;
}

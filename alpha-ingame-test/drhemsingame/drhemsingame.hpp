#include <math.h>
/*#include <string>*/
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include "resources.hpp"

#include "../atomicassets/atomicdata.hpp"
#include "config.hpp"
#include "players.hpp"

using namespace eosio;
using namespace std;
using namespace atomicdata;

class [[eosio::contract("drhemsingame")]] drhemsingame : public contract
{
public:
   drhemsingame(name receiver, name code, datastream<const char *> ds);

   //For managing recipes
   [[eosio::action("lastwd")]] void lastwd(const name& user, time_point_sec value);

   [[eosio::action("balance")]] void balance(const name& user, int64_t quantity);

   [[eosio::action("earn")]] void earn(const name& user, int64_t quantity);

   [[eosio::action("spend")]] void spend(const name& user, int64_t quantity);

   // [[eosio::action("withdrawn")]] void withdrawn(const name& user, int64_t quantity);

   [[eosio::on_notify("drhemstokenx::transfer")]] void on_transfer(name from, name to, asset quantity, string memo);

private:
    config get_config();
};

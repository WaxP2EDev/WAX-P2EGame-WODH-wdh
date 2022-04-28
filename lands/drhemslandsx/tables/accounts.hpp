#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("drhemslandsx"), eosio::table]] account
{
  name user;
  uint64_t type;
  int64_t count;
  int64_t free;

  uint64_t primary_key() const { return user.value; };
};

using accounts = multi_index<name("accounts"), account>;

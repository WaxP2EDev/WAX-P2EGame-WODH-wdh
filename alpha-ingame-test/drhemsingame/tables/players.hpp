#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("drhemsingame"), eosio::table]] player
{
    name user;
    asset balance;
    time_point_sec last_wd;

    uint64_t primary_key() const { return user.value; };
};

using players = multi_index<name("players"), player>;

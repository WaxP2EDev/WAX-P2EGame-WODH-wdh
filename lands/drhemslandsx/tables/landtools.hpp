#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by user.value
struct [[eosio::contract("drhemslandsx"), eosio::table]] landtool
{
    uint64_t id;
    string data;

    uint64_t primary_key() const { return id; };
};

using landtools = multi_index<name("landtool"), landtool>;

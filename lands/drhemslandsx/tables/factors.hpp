#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Remove after deleting rows

struct [[eosio::contract("drhemsworlds"), eosio::table]] factor
{
    int32_t template_id;
    string data;

    uint64_t primary_key() const { return (uint64_t)template_id; };
};

using factors = multi_index<name("factors"), factor>;

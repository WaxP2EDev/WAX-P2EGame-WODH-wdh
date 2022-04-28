#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("drhemslandsx"), eosio::table]] land
{
    uint64_t template_id;
    int64_t total;
    int64_t sold;
    int64_t free;
    asset price;

    uint64_t primary_key() const { return template_id; };
};

using lands = multi_index<name("lands"), land>;

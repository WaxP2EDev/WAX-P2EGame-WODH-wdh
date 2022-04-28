#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("drhemslandsx"), eosio::table]] type
{
    uint64_t type_id;
    int32_t discount;
    int32_t total_free;
    time_point_sec start;
    time_point_sec end;

    uint64_t primary_key() const { return type_id; };
};

using types = multi_index<name("types"), type>;

#pragma once
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

struct [[eosio::contract("drhemsingame"), eosio::table]] config
{
    uint32_t max_wd;
    uint32_t wd_fee;
    uint32_t wd_min_fee;
    time_point_sec wd_cooldown;
};

using config_table = singleton<name("config"), config>;

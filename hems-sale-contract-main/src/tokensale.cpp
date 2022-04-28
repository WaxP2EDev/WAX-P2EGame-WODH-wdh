#include "tokensale.hpp"

// this action configure new sale. pvsale || pubsale || airdrop
ACTION drhemsdropsx::configsale(time_point start_date, time_point end_date, uint64_t amount, asset price, string type, string h1, string h2, string h3, uint32_t template_id, time_point cooldown, uint64_t sold, uint64_t max_sell)
{
    eosio::check(has_auth(_self), "need admin authorization");
    sale_index _sale = sale_index(_self, get_self().value);
    _sale.emplace(_self, [&](auto &u)
                  {
                       u.id = _sale.available_primary_key();
                       u.start_date = start_date;
                       u.end_date = end_date;
                       u.amount = amount;
                       u.price = price;
                       u.type = type;
                       u.h1 = h1;
                       u.h2 = h2;
                       u.h3 = h3;
                       u.template_id = template_id;
                       u.cooldown = cooldown;
                       u.sold = sold;
                       u.max_sell = max_sell;
                  });
}

// This action remove existing sale entry
ACTION drhemsdropsx::rmsale(uint64_t sale_id)
{
    check(has_auth(_self), "Need contract authorization");
    sale_index _sale = sale_index(_self, get_self().value);
    auto itr = _sale.find(sale_id);
    check(itr != _sale.end(), "no sale found");
    _sale.erase(itr);
}

ACTION drhemsdropsx::rmlimit(name limit_id)
{
    check(has_auth(_self), "Need contract authorization");
    limits_index _limit = limits_index(_self, get_self().value);
    auto itr = _limit.find(limit_id.value);
    check(itr != _limit.end(), "no limit found");
    _limit.erase(itr);
}

// This action modify the existing sale entry
ACTION drhemsdropsx::modifysale(uint64_t sale_id, time_point start_date, time_point end_date, uint64_t amount, asset price, string type, string h1, string h2, string h3, uint64_t max_sell)
{
    check(has_auth(_self), "Need contract authorization");
    sale_index _sale = sale_index(_self, get_self().value);
    auto itr = _sale.find(sale_id);
    check(itr != _sale.end(), "Sale not found");
    _sale.modify(itr, _self, [&](auto &u)
                 {
                       u.start_date = start_date;
                       u.end_date = end_date;
                       u.amount = amount;
                       u.price = price;
                       u.type = type;
                       u.h1 = h1;
                       u.h2 = h2;
                       u.h3 = h3;
                       u.max_sell = max_sell;
                  });
}

// This action add whitelist for pvsale and airdrop depend upon type
ACTION drhemsdropsx::addwhitelist(uint64_t sale_id, vector<name> accounts, string type)
{
    eosio::check(has_auth(_self), "need admin authorization");
    check(type == "pvsale" || type == "airdrop", "Invalid type");
    whitelist_index _whitelist = whitelist_index(_self, get_self().value);
    auto type_index = _whitelist.get_index<"saletype"_n>();
    auto itr = type_index.find(sha256(&type[0], type.length()));
    if (itr == type_index.end())
    {
        _whitelist.emplace(_self, [&](auto &u)
                           {
                      u.sale_id = sale_id;
                      u.accounts = accounts;
                      u.type = type; });
    }
    else
    {
        type_index.modify(itr, _self, [&](auto &u)
                          { u.accounts.insert(u.accounts.end(), accounts.begin(), accounts.end()); });
    }
}

// This action remove whitelisted user from sale
ACTION drhemsdropsx::rmwhitelist(uint64_t sale_id, name account)
{
    check(has_auth(_self), "need admin authorization");
    whitelist_index _whitelist = whitelist_index(_self, get_self().value);
    auto itr = _whitelist.find(sale_id);
    check(itr != _whitelist.end(), "no sale id found");
    _whitelist.modify(itr, _self, [&](auto &u)
                      { u.accounts.erase(std::remove(u.accounts.begin(), u.accounts.end(), account), u.accounts.end()); });
}

// This action remove whitelisted entry
ACTION drhemsdropsx::rmwhitesale(uint64_t sale_id)
{
    check(has_auth(_self), "need admin authorization");
    whitelist_index _whitelist = whitelist_index(_self, get_self().value);
    auto itr = _whitelist.find(sale_id);
    check(itr != _whitelist.end(), "no sale id found");
    _whitelist.erase(itr);
}

void drhemsdropsx::buytoken(name from, name to, asset quantity, string memo)
{
    if (_first_receiver == _self || from == _self || to != _self)
        return;
    string delimiter = ":";
    int pos = memo.find(delimiter);
    string type = memo.substr(0, pos);
    // memo should be only pvsale or pubsale otherwise return
    if (type != "pvsale" && type != "pubsale")
        return;
    if (type == "pvsale")
    {
        // checking user is whitelisted
        whitelist_index _whitelist = whitelist_index(_self, get_self().value);
        auto type_index = _whitelist.get_index<"saletype"_n>();
        auto itr = type_index.find(sha256(&type[0], type.length()));
        check(itr != type_index.end(), "Whitelist for pvt do not exist");
        vector<name> accounts = itr->accounts;
        check(std::find(accounts.begin(), accounts.end(), from) != accounts.end(), "User is not whitelisted for pvsale");
    }

    memo.erase(0, pos + 1);
    uint64_t amount = stoi(memo);
    sale_index _sale = sale_index(_self, get_self().value);
    // getting secondary index by type
    auto type_index = _sale.get_index<"saletype"_n>();
    auto itr = type_index.find(sha256(&type[0], type.length()));
    check(itr != type_index.end(), "Sale not exist");
    check(itr->amount >= amount, "Invalid amount" );
    check(current_time_point() >= itr->start_date, "Sale is not started yet");
    check(current_time_point() < itr->end_date, "Sale has been ended");
    check(quantity.amount >= itr->price.amount * amount, "Transfer amount is lower than sale token price");

    check(amount+itr->sold <= itr->max_sell, "Quota reached");

    uint64_t alreadyCheck = itr->amount;

    limits_index _limits = limits_index(_self, get_self().value);
    auto limitItr = _limits.find(from.value);
    if(limitItr != _limits.end()){
      if(limitItr->limit_end > current_time_point()){
        alreadyCheck = alreadyCheck - limitItr->already;
      }
//      check(limitItr->captcha != 1, "Account locked");
      check(limitItr->limit_end <= current_time_point() || alreadyCheck >= 0, "Wait for cool down");

    }

    check(amount <= alreadyCheck, "Limit reached");

    bool resetAlready = true;
    if(limitItr->limit_end > current_time_point()){
      resetAlready = false;
    }

    /*bool captchaValue = 1;
    if(type == "pvsale"){
      captchaValue = 0;
    }*/


    time_point_sec limitEndValue = current_time_point() + itr->cooldown;

    if(limitItr == _limits.end()){
      _limits.emplace(_self, [&](auto &u)
                        {
                          u.user = from;
                          u.limit_end = limitEndValue;
                          u.captcha = 0;
                          if(resetAlready){
                            u.already = amount;
                          }else{
                            u.already += amount;
                          }
                        });
    }else{
      _limits.modify(limitItr, _self, [&](auto &u)
                        {
                          u.limit_end = limitEndValue;
                          u.captcha = 0;
                          if(resetAlready){
                            u.already = amount;
                          }else{
                            u.already += amount;
                          }
                        });
    }

    type_index.modify(itr, _self, [&](auto &u)
                      {
                        u.sold += amount;
                      });

    uint64_t hems_amount = amount*10000;
    asset hemstokens = asset(hems_amount, symbol(TOKEN_SYMBOL));
    // Sending hems token to user
    action(
        permission_level{_self, "active"_n},
        TOKEN_CONTRACT,
        "transfer"_n,
        std::make_tuple(_self, from, hemstokens, string("")))
        .send();
}

void drhemsdropsx::airdropclaim(name account, uint64_t sale_id)
{
    eosio::check(has_auth(account), "need account authorization");
    // checking if user is whitelisted
    whitelist_index _whitelist = whitelist_index(_self, get_self().value);
    auto type_index = _whitelist.get_index<"saletype"_n>();
    string type = "airdrop";
    auto itr = type_index.find(sha256(&type[0], type.length()));
    check(itr != type_index.end(), "Whitelist for airdrop do not exist");
    vector<name> accounts = itr->accounts;
    check(std::find(accounts.begin(), accounts.end(), account) != accounts.end(), "User is not whitelisted for airdrop");
    // Check sale asserts
    sale_index _sale = sale_index(_self, get_self().value);
    // getting secondary index by type
    auto sale_type_index = _sale.get_index<"saletype"_n>();
    auto sale_type_itr = sale_type_index.find(sha256(&type[0], type.length()));
    check(sale_type_itr != sale_type_index.end(), "Sale not exist");
    check(current_time_point() >= sale_type_itr->start_date, "Sale is not started yet");
    check(current_time_point() < sale_type_itr->end_date, "Sale has been ended");
    // Sending airdrop hems token to whitelisted claimer
    type_index.modify(itr, _self, [&](auto &u)
                      { u.accounts.erase(std::remove(u.accounts.begin(), u.accounts.end(), account), u.accounts.end()); });
    
    action(
        permission_level{_self, "active"_n},
        TOKEN_CONTRACT,
        "transfer"_n,
        std::make_tuple(_self, account, sale_type_itr->price, string("")))
        .send();
}

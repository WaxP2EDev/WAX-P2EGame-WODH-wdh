#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>

using namespace eosio;
using namespace std;

constexpr auto TOKEN_SYMBOL = eosio::symbol("HEMS", 4);
// constexpr auto TOKEN_SYMBOL = eosio::symbol("HEMZ", 4);
name TOKEN_CONTRACT = name("drhemstokenx");
// asset AIR_DROP_TOKEN = asset(10 * 100000000, symbol(TOKEN_SYMBOL));

CONTRACT drhemsdropsx : public contract
{
public:
    using contract::contract;

    // Contract ACTIONS
    ACTION configsale(time_point start_date, time_point end_date, uint64_t amount, asset price, string type, string h1, string h2, string h3, uint32_t template_id, time_point cooldown, uint64_t sold, uint64_t max_sell);
    ACTION rmsale(uint64_t sale_id);
    ACTION modifysale(uint64_t sale_id, time_point start_date, time_point end_date, uint64_t amount, asset price, string type, string h1, string h2, string h3, uint64_t max_sell);
    ACTION addwhitelist(uint64_t sale_id, vector<name> accounts, string type);
    ACTION rmwhitelist(uint64_t sale_id, name account);
    ACTION rmwhitesale(uint64_t sale_id);
    ACTION rmlimit(name limit_id);
    [[eosio::on_notify("eosio.token::transfer")]] void buytoken(name from, name to, asset quantity, string memo);
    ACTION airdropclaim(name account, uint64_t sale_id);

    // User bag table struct
    struct [[eosio::table]] sale
    {
        uint64_t id;
        time_point start_date;
        time_point end_date;
        uint64_t amount;
        asset price;
        string type;
        string h1;
        string h2;
        string h3;
        uint32_t template_id;
        time_point cooldown;
        uint64_t sold;
        uint64_t max_sell;

        uint64_t primary_key() const { return id; }
        checksum256 secondary_key() const { return (sha256(&type[0], type.length())); }
    };

    typedef multi_index<"sale"_n, sale, indexed_by<"saletype"_n, const_mem_fun<sale, checksum256, &sale::secondary_key>>> sale_index;

    /** table for whitelist accounts **/
    struct [[eosio::table]] whitelist
    {
        uint64_t sale_id;
        std::vector<name> accounts;
        string type;

        uint64_t primary_key() const { return sale_id; }
        checksum256 secondary_key() const { return sha256(&type[0], type.length()); }
    };

    typedef multi_index<"whitelist"_n, whitelist,
                        indexed_by<"saletype"_n, const_mem_fun<whitelist, checksum256, &whitelist::secondary_key>>>
        whitelist_index;

    /** table for whitelist airdrop accounts **/
    struct [[eosio::table]] airdropw
    {
        uint64_t sale_id;
        std::vector<name> accounts;

        uint64_t primary_key() const { return sale_id; }
    };

    typedef eosio::multi_index<"airdropw"_n, airdropw> airdropw_index;

    /** table for whitelist airdrop accounts **/
    struct [[eosio::table]] limits
    {
      name user;
      time_point_sec limit_end;
      uint32_t captcha;
      uint64_t already;

      uint64_t primary_key() const { return user.value; };
    };

    typedef eosio::multi_index<"limits"_n, limits> limits_index;
};

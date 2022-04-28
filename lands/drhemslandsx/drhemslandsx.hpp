#include <math.h>
/*#include <string>*/
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include "resources.hpp"

#include "../atomicassets/atomicdata.hpp"
#include "lands.hpp"
#include "accounts.hpp"
#include "factors.hpp"
#include "types.hpp"
// #include "landtools.hpp"
#include "b64.h"
#include "vigenere.h"

using namespace eosio;
using namespace std;
using namespace atomicdata;

class [[eosio::contract("drhemslandsx")]] drhemslandsx : public contract
{
public:
   drhemslandsx(name receiver, name code, datastream<const char *> ds);

   [[eosio::action("updatetype")]] void updatetype(const uint64_t& type_id,  const int32_t& discount,  const int32_t& total_free,  const time_point_sec& start,  const time_point_sec& end);
   [[eosio::action("updateland")]] void updateland(const uint64_t& template_id, const int64_t& total, const asset& price);
   [[eosio::action("updateacc")]] void updateacc(const name& account, const uint64_t& type);
   [[eosio::action("dbsold")]] void dbsold(const uint64_t& template_id, const int64_t& sold);
   [[eosio::action("dbfree")]] void dbfree(const uint64_t& template_id, const int64_t& free);
   [[eosio::action("rmland")]] void rmland(const uint64_t& template_id);
   [[eosio::action("rmtype")]] void rmtype(const uint64_t& type_id);

   // [[eosio::action("updatels")]] void update_land_seed(const uint64_t& id, const string& data);
   // [[eosio::action("updatelt")]] void update_land_tool(const uint64_t& id, const string& data);


   // [[eosio::action("claim")]] void claim(const name& account, uint32_t amount, uint32_t template_id);
   [[eosio::action("lfactor")]] void landFactor(const name& account, const uint64_t& landId, const uint64_t& avatarId, const std::vector<uint64_t>& tools, const uint64_t& seedId);

   [[eosio::action("buy")]] void buy(const name& user, const uint64_t& template_id, const uint32_t& amount);
   //[[eosio::on_notify("drhemstokenx::transfer")]] void on_transfer(name from, name to, asset quantity, string memo);

private:
  double get_landseed_factor(const uint64_t& land_id);
  // double get_landtool_factor(const uint64_t& land_id, const uint64_t& tool_id);
  std::string decrypt(std::string& encrypted_msg, std::string& key);

  std::vector<std::string> split(const std::string &s, const std::string &delimiter);
  bool isInteger(const std::string& str);
  void send_mintasset(const name& authorized_minter, const name& collection_name, const name& schema_name,
                                    const int32_t& template_id, const name& new_asset_owner, const ATTRIBUTE_MAP& immutable_data,
                                    const ATTRIBUTE_MAP& mutable_data, const std::vector<asset>& tokens_to_back);
};

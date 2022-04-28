#include "drhemslandsx.hpp"

drhemslandsx::drhemslandsx(name receiver, name code, datastream<const char *> ds)
  : contract::contract(receiver, code, ds)
{
}

void drhemslandsx::landFactor(const name& account, const uint64_t& landId, const uint64_t& avatarId, const std::vector<uint64_t>& tools, const uint64_t& seedId){
  check(has_auth(get_self()) || has_auth(COLLECTION_ACCOUNT), "Unauthorized access");
  check(tools.size() > 0 && tools.size() <= 2, "Invalid tools count");

  double resultFactor = get_landseed_factor(landId);/* * get_landtool_factor(landId, tools[0]);*/
  /*if(tools.size() == 2){
    resultFactor *= get_landtool_factor(landId, tools[1]);
  }*/

  action(
        permission_level{get_self(), name("active")},
        COLLECTION_ACCOUNT,
        name("receivelf"),
        std::make_tuple(
            account,
            asset(ceil(resultFactor*10000), PERCENT_SYMBOL)
        )
    ).send();
}

// void drhemslandsx::update_land_seed(const uint64_t& id, const string& data){
// 	require_auth(get_self());
//
// 	landseeds _lands(get_self(), get_self().value);
//   auto it = _lands.find(id);
//
//   if(it == _lands.end())
//   {
//     _lands.emplace(get_self(), [&](auto &a) {
//       a.id = id;
//       a.data = data;
//     });
//   }
//   else
//   {
//     _lands.modify(it, same_payer, [&](auto &a) {
// 			a.data = data;
//     });
//   }
// }
//
//
// void drhemslandsx::update_land_tool(const uint64_t& id, const string& data){
// 	require_auth(get_self());
//
// 	landtools _lands(get_self(), get_self().value);
//   auto it = _lands.find(id);
//
//   if(it == _lands.end())
//   {
//     _lands.emplace(get_self(), [&](auto &a) {
//       a.id = id;
//       a.data = data;
//     });
//   }
//   else
//   {
//     _lands.modify(it, same_payer, [&](auto &a) {
// 			a.data = data;
//     });
//   }
// }

/*
void drhemslandsx::claim(const name& account, uint32_t amount, uint32_t template_id){
  lands _lands(get_self(), get_self().value);
  auto itr = _lands.find(template_id);

  check(itr != _lands.end(), "Invalid template id");
  check(amount > 0 && amount <= 10, "Invalid amount");
  check(itr->sold + itr->free + amount <= itr->total, "Not enough lands for this type");

  accounts _accounts(get_self(), get_self().value);
  auto itrAcc = _accounts.find(account.value);

  check(itrAcc != _accounts.end(), "No permission");

  types _types(get_self(), get_self().value);
  auto itrType = _types.find(itrAcc->type);

  check(itrAcc->free + amount <= itrType->total_free, "Free land limit reached");
  check(itrType->start <= current_time_point(), "Sale is offline");
  check(itrType->end >= current_time_point(), "Sale ended");

  _lands.modify(itr, _self, [&](auto &rec) {
    rec.free += amount;
  });

  _accounts.modify(itrAcc, _self, [&](auto &rec) {
    rec.free += amount;
  });

  for(int i = 0; i < amount; i++){
    send_mintasset(get_self(), COLLECTION_NAME, SCHEMA, template_id, account, ATTRIBUTE_MAP(), ATTRIBUTE_MAP(), std::vector<asset>());
  }
}
*/
/*void drhemslandsx::on_transfer(name from, name to, asset quantity, string memo){
  if (_first_receiver == _self || from == _self || to != _self)
      return;

  std::vector<std::string> memoParts = split(memo,",");
  if(memoParts.size() == 2 && isInteger(memoParts[0]) && isInteger(memoParts[1])){
    uint32_t template_id = stoi(memoParts[0]);
    int32_t amount = stoi(memoParts[1]);

    lands _lands(get_self(), get_self().value);
    auto itr = _lands.find(template_id);

    check(itr != _lands.end(), "Invalid template id");
    check(amount > 0 && amount <= 10, "Invalid amount");
    check(itr->sold + itr->free + amount <= itr->total, "Not enough lands for this type");
    check(itr->price.symbol == HEMS_SYMBOL, "Invalid symbol");

    accounts _accounts(get_self(), get_self().value);
    auto itrAcc = _accounts.find(from.value);

    uint64_t type_id;
    if(itrAcc == _accounts.end()){
      type_id = 1;
    }else{
      type_id = itrAcc->type;
    }

    types _types(get_self(), get_self().value);
    auto itrType = _types.find(type_id);

    check(itrType != _types.end(), "Invalid type id");

    check(itrType->start <= current_time_point(), "Sale is offline");
    check(itrType->end >= current_time_point(), "Sale ended");

    int64_t totalPrice = ceil(((double) itr->price.amount)*(100-itrType->discount)/100)*amount;

    check(totalPrice == quantity.amount,"Not enough HEMS");

    _lands.modify(itr, _self, [&](auto &rec) {
      rec.sold += amount;
    });

    if(itrAcc == _accounts.end()){
      _accounts.emplace(_self, [&](auto &rec) {
        rec.user = from;
        rec.type = 1;
        rec.count = amount;
        rec.free = 0;
      });
      type_id = 1;
    }else{
      _accounts.modify(itrAcc, _self, [&](auto &rec) {
        rec.count += amount;
      });
    }

    for(int i = 0; i < amount; i++){
      send_mintasset(get_self(), COLLECTION_NAME, SCHEMA, template_id, from, ATTRIBUTE_MAP(), ATTRIBUTE_MAP(), std::vector<asset>());
    }
  }
}*/

void drhemslandsx::buy(const name& user, const uint64_t& template_id, const uint32_t& amount){
  lands _lands(get_self(), get_self().value);
  auto itr = _lands.find(template_id);

  check(itr != _lands.end(), "Invalid template id");
  check(amount > 0 && amount <= 10, "Invalid amount");
  check(itr->sold + itr->free + amount <= itr->total, "Not enough lands for this type");
  check(itr->price.symbol == HEMS_SYMBOL, "Invalid symbol");

  accounts _accounts(get_self(), get_self().value);
  auto itrAcc = _accounts.find(user.value);

  uint64_t type_id;
  if(itrAcc == _accounts.end()){
    type_id = 1;
  }else{
    type_id = itrAcc->type;
  }

  types _types(get_self(), get_self().value);
  auto itrType = _types.find(type_id);

  check(itrType != _types.end(), "Invalid type id");

  check(itrType->start <= current_time_point(), "Sale is offline");
  check(itrType->end >= current_time_point(), "Sale ended");

  int64_t totalPrice = ceil(((double) itr->price.amount)*(100-itrType->discount)/100)*amount;
  check(totalPrice > 0, "Invalid amount");

  _lands.modify(itr, _self, [&](auto &rec) {
    rec.sold += amount;
  });

  if(itrAcc == _accounts.end()){
    _accounts.emplace(_self, [&](auto &rec) {
      rec.user = user;
      rec.type = 1;
      rec.count = amount;
      rec.free = 0;
    });
  }else{
    _accounts.modify(itrAcc, _self, [&](auto &rec) {
      rec.count += amount;
    });
  }

  action(
        permission_level{get_self(), name("active")},
        GAME_ACCOUNT,
        name("spend"),
        std::make_tuple(
            user,
            totalPrice
        )
    ).send();

  for(int i = 0; i < amount; i++){
    send_mintasset(get_self(), COLLECTION_NAME, SCHEMA, template_id, user, ATTRIBUTE_MAP(), ATTRIBUTE_MAP(), std::vector<asset>());
  }
}

void drhemslandsx::updatetype(const uint64_t& type_id, const int32_t& discount, const int32_t& total_free, const time_point_sec& start, const time_point_sec& end)
{
  require_auth(get_self());
  types _types(get_self(), get_self().value);
  auto itr = _types.find(type_id);
  if(itr == _types.end()){
    _types.emplace(_self, [&](auto &rec) {
      rec.type_id = type_id;
      rec.discount = discount;
      rec.total_free = total_free;
      rec.start = start;
      rec.end = end;
    });
  }else{
    _types.modify(itr, _self, [&](auto &rec) {
      rec.discount = discount;
      rec.total_free = total_free;
      rec.start = start;
      rec.end = end;
    });
  }
}

void drhemslandsx::updateland(const uint64_t& template_id, const int64_t& total, const asset& price)
{
  require_auth(get_self());
  lands _lands(get_self(), get_self().value);
  auto itr = _lands.find(template_id);
  if(itr == _lands.end()){
    _lands.emplace(_self, [&](auto &rec) {
      rec.template_id = template_id;
      rec.sold = 0;
      rec.free = 0;
      rec.total = total;
      rec.price = price;
    });
  }else{
    _lands.modify(itr, _self, [&](auto &rec) {
      rec.total = total;
      rec.price = price;
    });
  }
}

void drhemslandsx::dbsold(const uint64_t& template_id, const int64_t& sold)
{
  require_auth(get_self());
  lands _lands(get_self(), get_self().value);
  auto itr = _lands.find(template_id);
  check(itr != _lands.end(), "Invalid template id");
  _lands.modify(itr, _self, [&](auto &rec) {
    rec.sold = sold;
  });
}

void drhemslandsx::dbfree(const uint64_t& template_id, const int64_t& free)
{
  require_auth(get_self());
  lands _lands(get_self(), get_self().value);
  auto itr = _lands.find(template_id);
  check(itr != _lands.end(), "Invalid template id");
  _lands.modify(itr, _self, [&](auto &rec) {
    rec.free = free;
  });
}

void drhemslandsx::rmland(const uint64_t& template_id)
{
  require_auth(get_self());
  lands _lands(get_self(), get_self().value);
  auto itr = _lands.find(template_id);
  check(itr != _lands.end(), "Invalid template id");
  _lands.erase(itr);
}

void drhemslandsx::rmtype(const uint64_t& type_id)
{
  require_auth(get_self());
  types _types(get_self(), get_self().value);
  auto itr = _types.find(type_id);
  check(itr != _types.end(), "Invalid type id");
  _types.erase(itr);
}

void drhemslandsx::updateacc(const name& account, const uint64_t& type){
  require_auth(get_self());
  accounts _accounts(get_self(), get_self().value);
  auto itr = _accounts.find(account.value);
  if(itr == _accounts.end()){
    _accounts.emplace(_self, [&](auto &rec) {
      rec.user = account;
      rec.type = type;
      rec.count = 0;
      rec.free = 0;
    });
  }else{
    _accounts.modify(itr, _self, [&](auto &rec) {
      rec.type = type;
    });
  }
}


// PRIVATE


double drhemslandsx::get_landseed_factor(const uint64_t& land_id){
	factors _factors(COLLECTION_ACCOUNT, COLLECTION_ACCOUNT.value);
  auto it = _factors.find(land_id);
  check(it != _factors.end(), "No factor object found for land");
  std::string dataStr = it->data;
  std::string data = decrypt(dataStr, ENC_KEY);
  auto dataParsed = split(data,";");

  uint32_t v = std::stoi(dataParsed[8]);

  return (double)v/100;
}

/*double drhemslandsx::get_landtool_factor(const uint64_t& land_id, const uint64_t& tool_id){
  if(tool_id == 0) return 1.0;
  uint64_t key = stoull(to_string(land_id)+to_string(tool_id));

	landtools _landtools(get_self(), get_self().value);
	auto it = _landtools.find(key);
	check(it != _landtools.end(), "No land - tool object found");

	std::string dataStr = it->data;
  std::string data = decrypt(dataStr, ENC_KEY);

  return (double)stoi(data)/100;
}*/

std::vector<std::string> drhemslandsx::split(const std::string &s, const std::string &delimiter)
{
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
  {
      token = s.substr(pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

bool drhemslandsx::isInteger(const std::string& str)
{
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}

void drhemslandsx::send_mintasset(const name& authorized_minter, const name& collection_name, const name& schema_name,
                                  const int32_t& template_id, const name& new_asset_owner, const ATTRIBUTE_MAP& immutable_data,
                                  const ATTRIBUTE_MAP& mutable_data, const std::vector<asset>& tokens_to_back)
{
  action(
        permission_level{get_self(), name("active")},
        ATOMIC_ASSETS,
        name("mintasset"),
        std::make_tuple(
            authorized_minter,
            collection_name,
            schema_name,
            template_id,
            new_asset_owner,
            immutable_data,
            mutable_data,
            tokens_to_back
        )
    ).send();
}


std::string drhemslandsx::decrypt(std::string& encrypted_msg, std::string& key) {
	std::string newKey = extend_key(encrypted_msg, key);
	std::string b64_encoded_str = decrypt_vigenere(encrypted_msg, newKey);
	std::string b64_decode_str = base64_decode(b64_encoded_str);
	return b64_decode_str;
}

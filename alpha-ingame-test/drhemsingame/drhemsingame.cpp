#include "drhemsingame.hpp"

drhemsingame::drhemsingame(name receiver, name code, datastream<const char *> ds)
  : contract::contract(receiver, code, ds)
{
}


void drhemsingame::earn(const name& user, int64_t quantity){
	check(has_auth(get_self()) || has_auth(GAME_CONTRACT) || has_auth(LANDS_CONTRACT), "Access violation");
  check(quantity > 0, "Invalid quantity");

	players _players(get_self(), get_self().value);
  auto itr = _players.find(user.value);

	if(itr == _players.end()){
		_players.emplace(_self, [&](auto &rec) {
	    rec.user = user;
	    rec.balance = asset(quantity, HEMS_SYMBOL);
      rec.last_wd = time_point_sec(0);
	  });
	}else{
		_players.modify(itr, same_payer, [&](auto &rec) {
	    rec.balance += asset(quantity, HEMS_SYMBOL);
	  });
	}

}

void drhemsingame::spend(const name& user, int64_t quantity){
	check(has_auth(get_self()) || has_auth(GAME_CONTRACT) || has_auth(LANDS_CONTRACT), "Access violation");
  check(quantity > 0, "Invalid quantity");

	players _players(get_self(), get_self().value);
  auto itr = _players.find(user.value);

	check(itr != _players.end(), "Player not found");
  check(itr->balance.amount >= quantity, "overdrawn game balance");

	_players.modify(itr, same_payer, [&](auto &rec) {
    rec.balance -= asset(quantity, HEMS_SYMBOL);
  });

}

/*void drhemsingame::withdrawn(const name& user, int64_t quantity){
  require_auth(user);
  check(quantity > 0, "Invalid quantity");

  players _players(get_self(), get_self().value);
  auto itr = _players.find(user.value);

	check(itr != _players.end(), "Player not found");

  auto cfg = get_config();
  asset currentBalance = itr->balance;

  check(currentBalance.amount * cfg.max_wd / 100 >= quantity, "Invalid quantity");

  int64_t fee;
  time_point_sec now = time_point_sec(current_time_point());

  if(itr->last_wd.utc_seconds == 0){
    fee = ceil((1.0-((double)cfg.wd_min_fee/100)*quantity);
  }else{
    double deltaSec = now.utc_seconds - itr->last_wd.utc_seconds;
    double wdFee = std::max(5.0, std::min(1.0,deltaSec/cfg.wd_cooldown.utc_seconds))*((double)cfg.wd_fee);
    fee = ceil((1.0-wdFee/100)*quantity);
  }

  int64_t finalQuantity = quantity-fee;

  asset hemstokens = asset(finalQuantity, symbol(HEMS_SYMBOL));

  _players.modify(itr, same_payer, [&](auto &rec) {
    rec.balance -= hemstokens;
    rec.last_wd = now;
  });

  action(
      permission_level{_self, "active"_n},
      TOKEN_CONTRACT,
      "transfer"_n,
      std::make_tuple(_self, itr->user, hemstokens, string("")))
      .send();
}*/

void drhemsingame::on_transfer(name from, name to, asset quantity, string memo){
  if (_first_receiver == _self || from == _self || to != _self)
      return;

  players _players(get_self(), get_self().value);
  auto itr = _players.find(from.value);

  if(itr == _players.end()){
    _players.emplace(_self, [&](auto &rec) {
      rec.user = from;
      rec.balance = quantity;
      rec.last_wd = time_point_sec(0);
    });
  }else{
    _players.modify(itr, same_payer, [&](auto &rec) {
      rec.balance += quantity;
    });
  }
}

void drhemsingame::lastwd(const name& user, time_point_sec value){
  check(has_auth(get_self()), "Access violation");

  players _players(get_self(), get_self().value);
  auto itr = _players.find(user.value);

  if(itr == _players.end()){
    _players.emplace(_self, [&](auto &rec) {
      rec.user = user;
      rec.balance = asset(0, HEMS_SYMBOL);
      rec.last_wd = value;
    });
  }else{
    _players.modify(itr, same_payer, [&](auto &rec) {
      rec.last_wd = value;
    });
  }
}

void drhemsingame::balance(const name& user, int64_t quantity){
  check(has_auth(get_self()), "Access violation");
  check(quantity > 0, "Invalid quantity");

  players _players(get_self(), get_self().value);
  auto itr = _players.find(user.value);

  if(itr == _players.end()){
    _players.emplace(_self, [&](auto &rec) {
      rec.user = user;
      rec.balance = asset(quantity, HEMS_SYMBOL);
      rec.last_wd = time_point_sec(0);
    });
  }else{
    _players.modify(itr, same_payer, [&](auto &rec) {
      rec.balance = asset(quantity, HEMS_SYMBOL);
    });
  }
}

// Private Functions

config drhemsingame::get_config()
{
  config_table _config_table(get_self(), get_self().value);
  return _config_table.get();
}

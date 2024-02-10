#include "ups.hpp"


ACTION ups::payup(name upsender) {
    
}

ACTION ups::updatecont(uint64_t content_id) {
    
}

ACTION ups::regdomain(const name& submitter, const string& url) {
    
}

ACTION ups::configdomain(const name& submitter, const string& url, const name& up_token_contract, const symbol& up_token_symbol, const name& reward_token_contract, const symbol& reward_token_symbol, const asset& one_up_amount, const asset& one_reward_amount) {
    
}

ACTION ups::regnftcol(const name& submitter, const name& nft_collection) {
    
}

ACTION ups::addnftcol(const uint64_t provider_id, const name& submitter, const name& nft_collection) {
    
}

ACTION ups::addurl(const uint64_t provider_id, const name& submitter, const string& url) {
    
}

ACTION ups::pauserewards(bool pause) {
    // --- Action must be authorized by the contract itself --- //
    check(require_auth(get_self()), "Only contract owner can pause the rewards."); 

    // --- Access the config singleton --- //
    config_table _config(get_self(), get_self().value); 

    // --- Check if the config exists --- //
    check(_config.exists(), "Call setconfig() then come back.");

    // --- Get the existing config --- //
    auto conf = _config.get();

    // --- Update the paused_rewards status --- //
    conf.paused_rewards = pause;

    // --- Set the updated config back to the singleton --- //
    _config.set(conf, get_self()); 

}

ACTION ups::pauseups(bool pause) {
    // --- Action must be authorized by the contract itself --- //
    check(require_auth(get_self()), "Only contract owner can pause the Ups."); 

    // --- Access the config singleton --- //
    config_table _config(get_self(), get_self().value); 

    // --- Check if the config exists --- //
    check(_config.exists(), "Call setconfig() then come back.");

    // --- Get the existing config --- //
    auto conf = _config.get();

    // --- Update the paused_ups status --- //
    conf.paused_ups = pause;

    // --- Set the updated config back to the singleton --- //
    _config.set(conf, get_self()); 
}

// --- Action to set configuration --- //
ACTION ups::setconfig(name up_token_contract, symbol up_token_symbol, name reward_token_contract, symbol reward_token_symbol, asset one_up_amount, asset one_reward_amount, uint32_t timeunit) {
    // --- Ensure the action is authorized by the contract itself --- //
    check(require_auth(get_self()), "Only contract owner can set the config"); 

    // --- Access the config singleton --- //
    config_table _config(get_self(), get_self().value); 

    // --- Create new config object --- //
    config new_conf;

    // --- Set new configuration values --- //
    new_conf.up_token_contract = up_token_contract;
    new_conf.up_token_symbol = up_token_symbol;
    new_conf.reward_token_contract = reward_token_contract;
    new_conf.reward_token_symbol = reward_token_symbol;
    new_conf.one_up_amount = one_up_amount;
    new_conf.one_reward_amount = one_reward_amount;
    new_conf.timeunit = timeunit;
    new_conf.paused_rewards = false; // Default to not paused
    new_conf.paused_ups = false; // Default to not paused

    // --- Set the new config in the singleton --- //
    _config.set(new_conf, get_self()); 
}


// === Notification Handlers === //

// --- Receive Tokenized Ups --- //
[[eosio::on_notify("*::transfer")]] void ups::_catch( const name from, const name to, const asset quantity, const string memo )
{  
  // --- Check that we're the intended recipient --- // 
  if (to != _self) return; 
    name content_name;

  content_name = name(memo); 
  if(!content_name){ // Not a NFT
    // ---  --- //
  } else {

  }

  require_auth(get_self());

  // --- Token-symbol Check --- //
  check(quantity.symbol.is_valid(), "Invalid token symbol");
  check(quantity.amount > 0, "Quantity must be positive");

  // --- Instantiate Content Table --- //
  _content ups(_self, _self.value);

  // --- Check for content in table --- // 
  auto itr = ups.find(contentid_upped);
  check(itr != ups.end(), "Content ID does not exist");

  // --- Set up Variables --- //
  uint32_t up_quantity = static_cast<uint32_t>(quantity.amount); 
  
  // --- Pass on to updateup() to register in table --- //
  updateup(up_quantity, from, contentid_upped, 0);
  
} // END listen->SOL ups

// Assuming the existence of updateup and up_table definitions elsewhere in your code


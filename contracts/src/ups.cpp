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
    
}

ACTION ups::pauseups(bool pause) {
    
}


// --- Receive Tokenized Ups --- //
[[eosio::on_notify("*::transfer")]] void ups::_catch( const name from, const name to, const asset quantity, const string memo )
{  
  // --- Check that we're the intended recipient --- // 
  if (to != _self) return; 
    name content_name = name(memo); 
  
  require_auth(get_self());

  // --- Token-symbol Check --- //
  check(quantity.symbol.is_valid(), "Invalid token symbol");
  check(quantity.amount > 0, "Quantity must be positive");

  // --- Instantiate Table --- //
  up_table ups(_self, _self.value);

  // --- Check for content in table --- // 

  auto itr = ups.find(contentid_upped);
  check(itr != ups.end(), "Content ID does not exist");

  // --- Set up Variables --- //
  uint32_t up_quantity = static_cast<uint32_t>(quantity.amount); 
  
  // --- Pass on to updateup() to register in table --- //
  updateup(up_quantity, 1, from, contentid_upped, 0); // 1=SOL Ups (uint32_t quantity, uint8_t upstype, uint32_t songid, name upsender)
  
} // END listen->SOL ups

// Assuming the existence of updateup and up_table definitions elsewhere in your code


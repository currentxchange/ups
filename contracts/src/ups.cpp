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

ACTION ups::addnftcol(const name domain, const name& submitter, const name& nft_collection) {
    // --- Check if collection exists + use is authorized  --- //
    


}

ACTION ups::addurl(const name domain, const name& submitter, const string& url) {
    
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
    config old_conf = check_config(true);

    // --- Other checks --- //
    check ((is_account(up_token_contract) && is_account(reward_token_contract)), "Contract account(s) doesn't exist");

    if (old_conf && old_conf.timeunit != timeunit){
        // --- Can't change time unit after ups have been made as it's used for reward calculations --- //
        _ups(get_self(), self().value);
        bool dundidit = (_ups.begin() != _ups.end());
        check (!dundidit, "You cannot adjust the timeunit after Ups have been made. Whoops.")
    }

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
    new_conf.paused_rewards = false; // --- Defaults to not be paused
    new_conf.paused_ups = false;

    // --- Set the new config in the singleton --- //
    _config.set(new_conf, get_self()); 
}


// === Notification Handlers === //

// --- Receive Tokenized Ups --- //
[[eosio::on_notify("*::transfer")]] void ups::_catch( const name from, const name to, const asset quantity, const string memo )
{  

    if (from == _self) return;
    
    // --- Checks + Set up Variables --- //
    config conf = check_config()
    check(!conf.paused_ups, "Ups are currently paused.");
    check(get_first_receiver() == conf.up_token_contract, "This isn't the correct Up token.");
    uint64_t up_quantity 
    check(up_quantity = static_cast<uint32_t>(quantity.amount / conf.one_up_amount.amount), "Please send exact amount, a multiple of "+ quantity.to_string());
    check(up_quantity >= 1, "Your Up was too small. Send at least "+ quantity.to_string());


    name content_name; // --- To parse URL if needed 
    bool force_reg_content = false;

    // --- Check for '|' in memo --- //
    size_t delimiter_pos = memo.find('|');
    if (delimiter_pos != string::npos) {
        // --- Split memo into function name and parameter
        string memo_man = memo.substr(0, delimiter_pos);
        string parameter = memo.substr(delimiter_pos + 1);
        name domain;
        name content_name;
 
        // --- Call the function based on memo_man --- //
        if (memo_man == "up") {
            if(parameter.size() <= 12){// --- Its a name
                upsertup(up_quantity, from, Name.from(parameter), 0);
            } else {// --- It's a URL
                content_name = parse_url_for_domain(parameter)
            }
            return;
        } else if (memo_man == "reg") {
            addcontent(name& submitter, string& url)

            return;
        } else if (memo_man == "url") {
            domain = parse_url_for_domain(parameter);

            // --- Check if content is registered in _content --- //

        } else if (memo_man == "url") {

            return;/
        } else {
            // Handle unknown memo
            check (0, "Unknown memo, send contentid or the url with up| or url| register/upvote or reg| to register");
        }
    }

    // --- If '|' is not found in memo, treat the entire memo as a name

    check(memo.size() <= 12, "Please send Up with an contentid or register this content. reg|url")
  // --- Token-symbol Check --- //
  check(quantity.symbol.is_valid(), "Invalid token symbol");
  check(quantity.amount > 0, "Quantity must be positive");

  // --- Instantiate Content Table --- //
  _content ups(_self, _self.value);

  // --- Check for content in table --- // 
  auto itr = ups.find(content_name);
  check(itr != ups.end(), "Content ID does not exist. Add it first.");

 
  
  // --- Pass on to upsertup() to register in table --- //
  upsertup(up_quantity, from, content_name, 0);
  
} // END listen->SOL ups



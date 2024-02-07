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

name ups::parse_url(const string& url) const {
    
}

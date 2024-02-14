  TABLE config {
      name vote_token_contract = name("bluxbluxblux");
      symbol vote_token_symbol = symbol(symbol_code("BLUX"), 0);
      name reward_token_contract = name("purplepurple");
      symbol reward_token_symbol = symbol(symbol_code("PURPLE"), 8);
      uint32_t timeunit = 300;
      asset one_vote_amount;
      asset one_reward_amount;
  };

  typedef singleton<name("config"), config> config_t;


Decided its fine, anyone can add the collection if they pay for the RAM

    // --- NFT Checks --- //
    if (collection && templateid){
      // --- Only collectionowners can register a collection --- //
        check(isAuthorized(collection, submitter));
    }

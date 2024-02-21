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

Didn't really need this because of improved indexing on content table 

    /*/--- Alternate location if other way isn't efficient
  TABLE location {
    uint64_t    id;            // Unique ID for each location
    uint32_t    int_code;      // Integer code for the location
    string      iso_alpha3;    // ISO Alpha-3 code
    string      iso_alpha2;    // ISO Alpha-2 code
    string      location_name; // Name of the location
    uint32_t    level;         // Level of location (e.g., country, state)

    uint64_t primary_key() const { return id; }
    uint64_t by_int_code() const { return int_code; } // Secondary index for int_code
    uint64_t by_level() const { return level; }

    EOSLIB_SERIALIZE(location, (id)(int_code)(iso_alpha3)(iso_alpha2)(location_name)(level))
  };

  typedef eosio::multi_index<"locations"_n, location,
      indexed_by<"byintcode"_n, const_mem_fun<location, uint64_t, &location::by_int_code>>, // Index for int_code
      indexed_by<"bylevel"_n, const_mem_fun<location, uint64_t, &location::by_level>>
  > locations_t;

/*/

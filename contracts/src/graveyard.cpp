
/*/
// --- Returns Name from Domain or url checksum256 if whole thang --- //
auto ups::parse_url(const string& url, bool hash_whole = false, bool chopped_whole = false, bool chopped_domain = false) { 
    // Find the start position after "://"
    auto start = url.find("://");
    if (start != string::npos) {
        start += 3; // Move past "://"
    } else {
        start = 0; // If "://" not found, start from the beginning
    }

    // Check if "www." is present after "://"
    auto www = url.find("www.", start);
    if (www == start) {
        start += 4; // Move past "www."
    }

    //  --- Extract the domain part after "://" and "www." --- //
    string domain_part;
    string domain_main = url.substr(start);

    // --- Stop at the slash --- // 
    auto end = domain_main.find('/');
    if (end != string::npos) {
        domain_part = domain_main.substr(0, end);
    }

    // --- If hash is needed return hash + done --- //
    if (hash_whole) {
        return eosio::sha256(domain_main.data(), domain_main.size());
    } else if (chopped_whole){
      return domain_main;
    } else if (chopped_domain){
      return domain_part;
    }

    // Replace invalid characters with a deterministic mapping to letters starting with 'a'
    for (auto& c : domain_part) {
        check (static_cast<unsigned char>(c) > 127, "⚡️ Invalid domain name. Must be ASCII characters only.");
            // Make uppercase letters lowercase
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }

        if ((c < 'a' || c > 'z') && (c < '1' || c > '5') && c != '.') {
            if (c >= '6' && c <= '9') {
                // Map '6'-'9' directly to 'a'-'d'
                c = 'a' + (c - '6');
            } else {
                // Map other invalid characters to letters starting with 'e'
                unsigned char illegalCharValue = static_cast<unsigned char>(c) % 20; // Using modulo to spread the mapping
                c = 'e' + (illegalCharValue % (122 - 'e')); // Ensure mapping is within 'e'-'z'
            }
        }
    }

    // --- Return the domain part as a name --- //
    return name(domain_part);
}//END parse_url()

/*/



From the end of upcatcher, old code not needed

    // --- If '|' is not found in memo, treat the entire memo as a name
    check(memo.size() <= 12, "Please send Up with an contentid or register this content. reg|url")

  // --- Instantiate Content Table --- //
  _content ups(_self, _self.value);

  // --- Check for content in table --- // 
  auto itr = ups.find(content_name);
  check(itr != ups.end(), "Content ID does not exist. Add it first.");
  
  // --- Pass on to upsertup() to register in table --- //
  upsertup(up_quantity, from, content_name, 0);


TABLE content_domain {
    uint64_t contentid;
    name domain;
    uint64_t primary_key() const { return contentid; }
};
typedef singleton<name("contdomain"), content_domain> content_domain_t;

"

"


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



  //using internallog_t = multi_index<name("internallog"), internallog>;


    void updateupper(uint32_t upscount, name upsender);
/*/ --- 

This file handles the dispatch of tokens, etc in a standardized way.
This allows us to be more flexible with what constitutes an up:
1) A token transfer with an up| memo
2) A token stansfer with just a name that's a content ID (no special memo)
3) An Up action on a contract that has been registered as a "content id" (no action for this yet)

/*/// ---



// --- DISPATCHER Checks + calls logup() updateiou() and updatetotal() --- //
void ups::upsertup(uint32_t upscount, name upsender, uint64_t content_id, bool negative) {

    // --- Check content Id valid

    if (!negative){
        // --- Log the ups in ups table --- // 
        upsert_logup(upscount, upsender, content_id, negative);
        
        // --- Calls action to update the TOTALS table -- //
        upsert_total(upscount, upsender, content_id, negative);

        //  --- Call action to update IOU table ----- //
        ious::updateiou(upscount, upsender, content_id, false);
    }
}//END upsertup()

// --- Update running log of ups --- // TODO update to this contract
void upsert_logup(uint32_t upscount, name upsender, uint32_t content_id, bool negative){
  //NOTE negative should only be called for deletions (user gets removed from system)

  // --- Add record to _ups --- //
  _ups(get_self(), content_id); //WARN CHECK - is content_id right here? URGENT I really think this is wrong for all upserts
  auto ups_iterator = _ups.find(upid);
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  if( ups_iterator == _ups.end() )
  { // -- Make New Record
    _ups.emplace(upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
      row.upid = _ups.available_primary_key();
      row.totalups = newups;
      row.tuid = momentu;
    });
  } 
  else 
  { // -- Update Record
    if (negative){
      _ups.modify(ups_iterator, upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
          row.totalups -= newups;
      });
    } else {
      _ups.modify(ups_iterator, upsender, [&]( auto& row ) {//URGENT This needs to be changed when we figure out the PK issue
          row.totalups += newups;
      });
    }

    
  }//END if(results _ups) 
}

// --- Upsert _uppers and _totals --- // TODO update to this contract
void upsert_total(uint32_t &upscount, name &upsender, uint32_t &content_id, bool negative) {
  require_auth( upsender );

  // --- Update / Insert _totals record of cumulative song Ups --- //
  _totals(get_self(), content_id);
  auto total_iterator = _totals.find(content_id);
  uint32_t time_of_up = eosio::time_point_sec::sec_since_epoch();
  if( total_iterator == _totals.end() )
  { // -- Make New Record
    _totals.emplace(upsender, [&]( auto& row ) {
      row.key = content_id;
      row.totalups = newups;
      row.updated = time_of_up;
    });
  } 
  else 
  { // -- Update Record 
    if(!negative){
      _totals.modify(total_iterator, upsender, [&]( auto& row ) {
        row.key = content_id;
      row.totalups += newups;
        row.updated = time_of_up;
      });
    } else { // Subtract the value from totals
      _totals.modify(total_iterator, upsender, [&]( auto& row ) {
        row.key = content_id;
      row.totalups -= newups;
        row.updated = time_of_up;
      });
    }
    
    
  }//END if(results _totals)

  // --- Update / Insert _uppers record --- //
  _uppers(get_self(), content_id);
  auto listener_iterator = _uppers.find(content_id);
  if( listener_iterator == _uppers.end() )
  {
    _uppers.emplace(upsender, [&]( auto& row ) {
      row.upsender = upsender;
      row.firstup = time_of_up;
      row.lastup = time_of_up;
      row.totalups = newups;
    });
  } 
  else 
  {
    _uppers.modify(listener_iterator, upsender, [&]( auto& row ) {
      row.lastup = eosio::time_point_sec::sec_since_epoch();
      row.totalups += newups;
    });
  }//END if(results _uppers)
}//END upsert_total()


void removeiou(name sender, name receiver) {
    
}//END removeiou()

void upsertupper(uint32_t upscount, name upsender) {
    
}//END upsertupper()

void removeupper(name upsender) {
    
}//END removeupper()

void removecontent(uint64_t content_id) {
    
}//END removecont()

// --- Handles adding both NFT content and URL content --- //
void addcontent(name& submitter, vector<double> latlng = {0.0,0.0}, const vector<uint32_t>& tetra_locode = {0, 0, 0, 0}, string& url = "", name domain = ""_n, name collection = ""_n, uint32_t templateid = 0) { //CHECK need ""_n instead of false?

    // --- Check if submitter is in providers table --- //
    require_auth(submitter): 

    bool is_nft = (collection != ""_n) && !templateid;

    // --- Handle URL --- //
    if ( !is_nft && url != "" ) {
      name domain = parse_url(url);
      hash url_hash = parse_url(url, 1);
      hash url_chopped = parse_url(url, 0, 1);

      // --- Check if domain is registered --- //
      content_provider_singleton content_prov(get_self(), domain_parsed.value);
      check(content_prov.exists(), "Register the doman to start adding content for upvotes");

      // --- Check if content already exists --- //
        content_table contents(get_self(), get_self().value);
        auto gudhash = contents.get_index<"bygudahash"_n>();
        auto itr = gudhash.find(url_hash);

        check(itr == gudhash.end(), "Content already exists, you can sends ups now");

        // Insert new content
        contents.emplace(submitter, [&](auto& row) {
            row.id = contents.available_primary_key();
            row.domain = domain;
            row.submitter = submitter;
            row.link = url_chopped;
            row.external_id = 0;
            row.gudahash = url_hash;
            row.created = time_point_sec::sec_since_epoch();
            row.latlng = latlng;
            row.tetra_loc = tetra_locode;
        });
    } else if ( is_nft ) {
      // --- Handle NFT --- //
      // --- Check the providers table --- //
      content_provider_singleton content_prov(get_self(), nft_collection.value);

      // --- Ensure the collection is not already registered --- //
      check(content_prov.exists(), "This collection is not registered. Call regnftcol first.");


      // --- Check if templateid is valid --- //
      templates_t templates_table(ATOMICASSETS_ACCOUNT, collection.value);
      auto template_itr = templates_table.find(templateid);
      check(template_itr != templates_table.end(), "Template does not exist");

      // --- Check if NFT already exists in content_table --- //
      content_table contents(get_self(), get_self().value);
      auto by_external_id_idx = contents.get_index<"byexternal"_n>(); // Assuming this is the secondary index for external_id
      auto nft_itr = by_external_id_idx.find(templateid);
      check(nft_itr == by_external_id_idx.end(), "NFT is already registered. Send Ups.");

      // Insert new NFT content
      contents.emplace(submitter, [&](auto& row) {
        row.id = contents.available_primary_key();
        row.domain = collection; // --- Using collection name as domain for NFTs
        row.submitter = submitter;
        row.external_id = templateid; // --- Set external_id to templateid for NFTs
        row.gudahash = checksum256(); 
        row.created = current_time_point();
        row.latlng = latlng;
        row.tetra_loc = tetra_locode;
      });


    } else {
      check(false, "This is not a valid URL or NFT");
    }

    // dont forget : row.id = _ups.available_primary_key();
}

void deepremvcont(uint64_t content_id) {
    
}


// --- Returns the current Time Unit --- //
uint32_t find_tu(uint32_t& momentuin, uint32_t tu_length){
  // 1561139700 is the first Time Unit in Seconds
  uint32_t time_unit = floor((momentuin - 1561139700) / tu_length);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Returns the current Time Unit --- //
uint32_t find_tu(uint32_t tu_length){
  uint32_t momentuin = eosio::time_point_sec::sec_since_epoch();
  uint32_t time_unit = floor((momentuin - 1561139700) / tu_length);  // Divide by the length of a Time Unit in seconds
  return time_unit;
}

// --- Get content_id from iouid (concat of tuid + content_id) --- //
uint32_t iouid_to_contentid(uint32_t& iouid){
  uint32_t content_id = (uint32_t) iouid;
  return content_id;
}

// --- Get Tuid from iouid (concat of tuid + content_id) --- //
uint32_t iouid_to_tuid(uint32_t& iouid){
  uint32_t tuid = (uint32_t iouid>>32);
  return tuid;
}

// --- Returns Name from Domain or url checksum256 if whole thang --- //
auto parse_url(const string& url, bool hash_whole = false, bool chopped_whole = false, bool chopped_domain = false) const { 
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
        return sha256(domain_main, domain_main.size());
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

// --- Gets config object and ensures contract not paused --- //
auto check_config(bool ignore_empty = false) // --- RETURNS false or config type
{
    // --- Get config table --- //
    config_table conf_tbl(get_self(), get_self().value);

    bool existencial = conf_tbl.exists();

    // --- Ensure the rewards are set up --- //
    check(existencial || ignore_empty, "⚡️ An administrator needs to set up this contract before you can use it.");

    // --- Return a blank object or the config object --- //
    if (!existencial){
        const auto& conf = conf_tbl.get();

        // --- If both rewards and ups are paused, no go, shut down everything --- //
        check(!(conf.paused_rewards && !conf.paused_ups), "⚡️ Rewards and Ups are both currently frozen. Check back later.");

        return conf;

    } else return false; // --- Returns only when ignore empty is set to avoid check. 
}

// --- Check if user is authorized on NFT collection --- //
bool isAuthorized(name collection, name user)
    {
        auto itrCollection = atomicassets::collections.require_find(collection.value, "No collection with this name exists.");
        bool authorized = false;
        vector<name> authAccounts = itrCollection->authorized_accounts;
        for (auto it = authAccounts.begin(); it != authAccounts.end() && !authorized; it++)
        {
        if (user == name(*it))
        {
            authorized = true;
            break;
        }
        }
        return authorized;
    }//END isAuthorized()

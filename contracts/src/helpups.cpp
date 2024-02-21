#include "helphelpups.cpp"


/*/ --- 

This file handles the dispatch of tokens, etc in a standardized way.
This allows us to be more flexible with what constitutes an up:
1) A token transfer with an up| memo
2) A token stansfer with just a name that's a content ID (no special memo)
3) An Up action on a contract that has been registered as a "content id" (no action for this yet)

/*/// ---



// --- DISPATCHER Checks + calls logup() updateiou() and updatetotal() --- //
void ups::upsertup(uint32_t upscount, name upsender, uint64_t content_id, bool negative = 0) {
      require_auth( upsender );
    // --- Check content Id valid --- //
    if (!negative){// TODO remove this + test
        // --- Log the ups in ups table --- // 
        ups::upsert_logup(upscount, upsender, content_id, negative);
        
        // --- Calls action to update the TOTALS table -- //
        ups::upsert_total(upscount, upsender, content_id, negative);

        //  --- Call action to update IOU table ----- //
        ups::upsert_ious(upscount, upsender, content_id, false);
    } //TODO else call removal functions
}//END upsertup()

// --- ROUTER prepares and calls upsertup() --- //
void ups::upsertup_url(uint32_t upscount, name upsender, string& url ) {
    // Extract the domain name from the URL to use as scope for the content table
    //inline auto get_domain = parse_url(url, 0, 0, 1); 
    name domain = url_domain_name(url);
    //delete get_domain;
    

    // Use the domain to scope the content table
    content_t contents(get_self(), domain.value);

    // --- Get the hash of the URL --- //
    checksum256 url_hash = ups::url_hash(url);

    //auto parse_url(const string& url) -> checksum256;

    // Search for the content by its hash within the scoped content table
    auto by_gudahash_index = contents.get_index<"bygudahash"_n>();
    auto content_itr = by_gudahash_index.find(url_hash);
    check(content_itr != by_gudahash_index.end(), "⚡️ Linked content not found. Please register the provider + content URL before sending ups.");

    uint64_t content_id = content_itr->id;
    upsertup(upscount, upsender, content_id, false);

}//END upsertup_url()


// --- ROUTER prepares and calls upsertup() --- //
void ups::upsertup_nft(uint32_t upscount, name upsender, int32_t templateid) {

  // --- Search for the domain (NFT collection) in the content_domain table using templateid -- //
  content_domain_t content_domain_singleton(get_self(), templateid);
  eosio::check(content_domain_singleton.exists(), "Template ID not found in content_domain");
  auto get_collection = content_domain_singleton.get();
  name collection = get_collection.domain;

  // --- Ensure collection matches for correct contentid --- //
  content_t content_tbl(get_self(), get_self().value);
  auto by_external_id_idx = content_tbl.get_index<"byextid"_n>();
  auto content_itr = by_external_id_idx.find(templateid);
  eosio::check(content_itr != by_external_id_idx.end() && content_itr->domain == collection, "NFT content not found or collection mismatch");

  // Pass the content_id to upsertup to update the ups
  upsertup(upscount, upsender, content_itr->id, 0);

}//END upsertup_nft()

// --- Update running log of ups --- // TODO update to this contract
void ups::upsert_logup(uint32_t upscount, name upsender, uint32_t content_id, bool negative){
  //NOTE negative should only be called for deletions (user gets removed from system)

  uint32_t now_tu = find_tu(eosio::current_time_point().sec_since_epoch());

  // --- Add record to _ups --- //
  upslog_t _ups(get_self(), get_self().value);
  auto by_content_id_idx = _ups.get_index<"bycontentid"_n>();
  auto ups_itr = by_content_id_idx.lower_bound(content_id);


//TODO let user cancel their up befote timeunit expires, just action that calls this function with negative flag
//TODO double-check we are updating the other totals table
  // Iterate through entries to find a match with upsender and now_tu
  bool found_up = false;
  for (; ups_itr != by_content_id_idx.end(); ++ups_itr) {
      if (ups_itr->upsender == upsender && ups_itr->tuid == now_tu) {
          // Matching entry found, update its totalups
          by_content_id_idx.modify(ups_itr, eosio::same_payer, [&](auto& row) {
              row.totalups = negative ? row.totalups - upscount : row.totalups + upscount;
          });
          found_up = true;
          break; // Exit the function after updating
      }
  }

  if( !found_up )
  { // -- Make New Record
    _ups.emplace(upsender, [&]( auto& row ) {
      row.upid = _ups.available_primary_key();
      row.content_id = content_id;
      row.totalups = upscount;
      row.tuid = now_tu;
    });
  } 

}

// --- Upsert _uppers and _totals --- // TODO update to this contract
void ups::upsert_total(uint32_t upscount, name upsender, uint32_t content_id, bool negative) {


  // --- Update / Insert _totals record of cumulative song Ups --- //
  totals_t _totals(get_self(), content_id);
  auto total_iterator = _totals.find(content_id);
  uint32_t time_of_up = eosio::current_time_point().sec_since_epoch();
  if( total_iterator == _totals.end() )
  { // -- Make New Record
    _totals.emplace(upsender, [&]( auto& row ) {
      row.key = content_id;
      row.totalups = upscount;
      row.updated = time_of_up;
    });
  } 
  else 
  { // -- Update Record 
    if(!negative){
      _totals.modify(total_iterator, upsender, [&]( auto& row ) {
        row.key = content_id;
        row.totalups += upscount;
        row.updated = time_of_up;
      });
    } else { // Subtract the value from totals
      _totals.modify(total_iterator, upsender, [&]( auto& row ) {
        row.key = content_id;
        row.totalups -= upscount;
        row.updated = time_of_up;
      });
    }
    
    
  }//END if(results _totals)

  // --- Update / Insert _uppers record --- //
  uppers_t _uppers(get_self(), content_id);
  auto listener_iterator = _uppers.find(content_id);
  if( listener_iterator == _uppers.end() )
  {
    _uppers.emplace(upsender, [&]( auto& row ) {
      row.upsender = upsender;
      row.firstup = time_of_up;
      row.lastup = time_of_up;
      row.totalups = upscount;
    });
  } 
  else 
  {
    _uppers.modify(listener_iterator, upsender, [&]( auto& row ) {
      row.lastup = eosio::current_time_point().sec_since_epoch();
      row.totalups += upscount;
    });
  }//END if(results _uppers)
}//END upsert_total()

// --- Upsert IOUs --- //
void ups::upsert_ious(uint32_t upscount, name upsender, uint64_t content_id, bool subtract){
  //CHECK Not using any auth, double check we already did that 

  check(has_auth(get_self()), "Only the contract can modify the ious table. ");//CHECK true??

  // --- Add record to _ups --- // 
  _ious(get_self(), get_self().value); 
  auto ious_itr = _ious.find(iouid); 
  uint32_t time_of_up = eosio::current_time_point().sec_since_epoch();
  uint32_t timeunit = find_tu(time_of_up);
  if( ious_itr == _ups.end())
  { // -- Make New Record
    _ious.emplace(upsender, [&]( auto& row ) {
      row.key = _ious.available_primary_key(); 
      row.upsender = upsender;
      row.content_id = content_id;
      row.tuid = timeunit;
      row.upcatcher = artistacc;
      row.upscount = upscount;
      row.initialized = time_of_up;
      row.updated = time_of_up;
    });
  } 
  else 
  { // -- Update Record
    _ious.modify(ious_itr, upsender, [&]( auto& row ) {
      row.upscount += upscount;
      row.upstype = upstype;// Can be from SOL -> BIGSOL
      row.updated = time_of_up;
    });
  }//END if(results _ups)
}//END upsert_ious()

// --- Send the beautiful people their tokens  --- //
void pay_iou(uint32_t maxpay = 0, name& receiver, bool paythem = true){

  check(receiver != ""_n, "We can't pay no one.");

  // --- Check that the contract is the receiver --- //
  config_t _config(get_self(), get_self().value);
  check(_config.exists(), "Configuration must be set first. How did you even get here?");
  auto conf = _config.get();

  // Find the receiver records is in the _ious table

  // --- Get the IOUs --- //
  ious_t _ious(get_self(), receiver.value);
  auto iou_itr = _ious.begin();
  check(iou_itr != _ious.end(), "You are all paid up. Send some Ups and come back");
  
  // --- Calculate Payments --- //
  uint32_t paid = 0;
  std::vector<uint64_t> ious_to_erase;

  // --- Iterate over the IOUs and accumulate payments until reaching maxpay or end of table --- //
  while(iou_itr != _ious.end() && (maxpay == 0 || paid < maxpay) && records_processed <= 12){
    auto& iou = iou_itr;
    paid += iou.upscount; 
    ious_to_erase.push_back(iou.iouid); // Track IOU IDs for deletion
    iou_itr++;
  }

    // Calculate the total amount to be paid
    asset total_payment = conf.one_reward_amount.amount * paid;

  // --- Pay the people --- //
  if(total_payment.amount > 0 && paythem){
    // Use the eosio.token transfer action to send the payment
    action(
        permission_level{get_self(), "active"_n},
        conf.reward_token_contract, 
        "transfer"_n,
        std::make_tuple(get_self(), receiver, total_payment, std::string("Rewards for "+receiver.to_string()))
    ).send();
  }

  // --- Erase paid IOUs from the table --- //
  for(auto& iouid: ious_to_erase){
    auto itr = _ious.find(iouid);
    if(itr != _ious.end()){
        _ious.erase(itr);
    }
  }
}//END pay_iou()


// --- Handles adding both NFT content and URL content --- // TODO add to the new content_domain singleton
void addcontent(name& submitter, double latitude = 0.0, double longitude = 0.0, uint32_t continent_subregion_code = 0, uint32_t country_code = 0, const std::string& continent_subregion_name = "", const std::string& country_iso3 = "", uint32_t subdivision = 0, uint32_t postal_code = 0, string& url = "", name domain = ""_n, name collection = ""_n, uint32_t templateid = 0)
{ 
    // --- Check if submitter is in providers table --- //
    require_auth(submitter);

    bool is_nft = (collection != ""_n) && !templateid;

    uint32_t subcontinent = 0;
    uint32_t country = 0;
    int32_t latitude_int = 0;
    int32_t longitude_int = 0;

    // --- Validate and format Latitude and Longitude --- //
    if (latitude == 0.0 && longitude == 0.0){
        vector<int32_t> formatted_coords = validate_and_format_coords({latitude, longitude});
        latitude_int = formatted_coords[0];
        longitude_int = formatted_coords[1];
    }

    // --- Validate the Continent Subregion as a string or an int --- //
    if (!continent_subregion_name.empty() || continent_subregion_code != 0) {
        subcontinent = is_valid_continent_subregion(continent_subregion_code, continent_subregion_name);
    } 

    // --- Validate the country as a string or an int --- //
    if (!country_name.empty() || country_code != 0) {
        country = is_valid_country(country_code, country_name);
    } 
    
    // --- Handle URL --- //
    if ( !is_nft && url != "" ) {
      name domain = url_domain_name(url);
      hash url_hash = url_hash(url);
      string url_chopped = chopped_url(url);

      // --- Check if domain is registered --- //
      content_provider_singleton content_prov(get_self(), domain_parsed.value);
      check(content_prov.exists(), "Register the domain to start adding content for upvotes");

      // --- Check if content already exists --- //
        content_t contents(get_self(), get_self().value);
        auto gudhash = contents.get_index<"bygudahash"_n>();
        auto itr = gudhash.find(url_hash);

        check(itr == gudhash.end(), "Content already exists, you can sends ups now");

        // --- Insert NFT into content table -- //
        contents.emplace(submitter, [&](auto& row) {
            row.id = contents.available_primary_key();
            row.domain = domain;
            row.submitter = submitter;
            row.link = url_chopped;
            row.external_id = 0;
            row.gudahash = url_hash;
            row.created = eosio::current_time_point().sec_since_epoch();
            row.latitude = latitude_int; // CHANGE and see if it compiles 
            row.longitude = longitude_int;
            row.subcontinent = (subcontinent != 0) ? subcontinent : 1;
            row.country = country;
            row.subdivision = subdivision
            row.postal_code = postal_code;

    } else if ( is_nft ) {
      // --- Handle NFT --- //
      // --- Check the providers table --- //
      content_provider_singleton content_prov(get_self(), nft_collection.value);

      // --- Ensure the collection is not already registered --- //
      check(content_prov.exists(), "This collection is not registered. Use regnftcol first.");


      // --- Check if templateid is valid --- //
      atomicassets::templates_t templates_tbl(ATOMICASSETS_ACCOUNT, collection.value); /// CHECK mangled, should be 
      auto template_itr = templates_tbl.find(templateid);
      check(template_itr != templates_tbl.end(), "Template does not exist");

      // --- Check if NFT already exists in content_t --- //
      content_t contents(get_self(), get_self().value);
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
        row.latitude = latitude_int; // CHANGE and see if it compiles 
        row.longitude = longitude_int;
        row.subcontinent = (subcontinent != 0) ? subcontinent : 1;
        row.country = country;
        row.subdivision = subdivision
        row.postal_code = postal_code;
      });

      // --- Insert NFT into content_domain table (needed later for simple upvotes) -- //
      content_domain_t content_domain_singleton(get_self(), templateid); // Scope by template ID
      eosio::check(!content_domain_singleton.exists(), "Template ID already exists in content_domain.");
      content_domain domain_entry = {collection}; // Assuming content_domain struct has a 'domain' field for collection name
      content_domain_singleton.set(domain_entry, submitter); // Set with the authority of the submitter


    } else {
      check(0, "This is not a valid URL or NFT");
    }
    // dont forget : row.id = _ups.available_primary_key();
}



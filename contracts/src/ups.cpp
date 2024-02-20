#include "ups.hpp"

ACTION ups::payup(name upsender = false) {
    /*/ --- Require only the upsender to be able to claim rewards [Optional] --- //
    Commented out, action allows for anyone to call the action to pay other people. Otherwise a person would be unable to claim if they were out of CPU. 
    /*/
    //check((has_auth(upsender) || has_auth(get_self())) , "Please put your account name.")
    // --- Send to the payment dispatcher --- //
    pay_iou(0, upsender);
    
}

ACTION ups::updatecont(name& submitter, uint64_t content_id, double latitude = 0.0, double longitude = 0.0, uint32_t continent_subregion_code = 0, uint32_t country_code = 0, const std::string& continent_subregion_name = "", const std::string& country_iso3 = "", uint32_t subdivision = 0, uint32_t postal_code = 0){

    check((has_auth(itr->submitter) || has_auth(get_self())) , "⚡️ Only the submitter "+ itr->submitter.to_string() +" can update content.")

    // --- Get the content --- //
    content_t contents(get_self(), get_self().value);
    auto itr = contents.find(content_id);
    check(itr != contents.end(), "⚡️ Content with the specified ID does not exist.");

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

    // --- Update the content record --- //
    contents.modify(itr, get_self(), [&](auto& row) {
        row.latitude = (latitude_int != 0) ? latitude : row.latitude; // CHANGE and see if it compiles 
        row.longitude = (longitude_int != 0) ? longitude : row.longitude;
        row.subcontinent = (subcontinent != 0) ? subcontinent : row.subcontinent;
        row.country = (nation != 0) ? nation : row.nation;
        row.subdivision = (subdivision != 0) ? subdivision : row.subdivision;
        row.postal_code = (postal_code != 0) ? postal_code : row.postal_code;
    });

}//END updatecont()



ACTION ups::regdomain(const name& submitter, const string& url, const vector<uint32_t>& tetra_locode = {0, 0, 0, 0} ) {

    check((has_auth(submitter) || has_auth(get_self())) , "Please put your account name as the submitter.")

    // ---- Get a name from the URL --- //
    name domain_parsed = parse_url(url);
    name domain_chopped = parse_url(url, 0, 0, 1);

    // ---- Check if domain is already registered --- //
    content_provider_singleton content_prov(get_self(), domain_parsed.value);
    eosio::check(!content_prov.exists(), "Content provider already registered for this domain");

    // ---- Register new content provider --- //
    content_provider prov_data;
    prov_data.domain = domain_parsed;    

    prov_data.raw_domain = domain_chopped;

    // Save the content provider information
    content_prov.set(prov_data, get_self());
}

/*/----
  This functionality isn't included, but can be added   
  Optionally, you can scope the config table to the name of a domain (content provider liek a web platform or a NFT collection)
  Allows each content to have their own token for Ups + rewards

ACTION ups::configdomain(const name& submitter, const string& url, const name& up_token_contract, const symbol& up_token_symbol, const name& reward_token_contract, const symbol& reward_token_symbol, const asset& one_up_amount, const asset& one_reward_amount) {

}
/*///---

ACTION ups::regnftcol(const name& submitter, const name& nft_collection, string& country) {
    // --- Check if collection exists + user is authorized  --- //
    check(has_auth(submitter), "The content submitter must sign."); 
    auto itrCollection = atomicassets::collections.require_find(collection.value, "No collection with this name exists.");

    // --- Require collection owners to register collection --- //
    //check(isAuthorized(nft_collection, submitter), "Submitter is not authorized for this collection.");

    check(is_valid_country()

    // --- Check the providers table --- //
    content_provider_singleton content_prov(get_self(), nft_collection.value);

    // --- Ensure the collection is not already registered --- //
    check(!content_prov.exists(), "Content provider already registered for this domain");

    // --- Register the collection --- //
    content_provider new_provider
    new_provider.domain = nft_collection,
    new_provider.raw_domain = nft_collection.to_string(),
    new_provider.country = country

    content_prov.set(new_provider, submitter); // --- Submitter pays to register
}

ACTION ups::addurl(const name& submitter, const string& url, const name& domain, double latitude = 0.0, double longitude = 0.0, uint32_t continent_subregion_code = 0, uint32_t country_code = 0, const string& continent_subregion_name = "", const string& country_iso3 = "", uint32_t subdivision = 0, uint32_t postal_code = 0) { 
    
    name collection = ""_n;
    uint32_t templateid = 0;
    auto conf = check_config();

    // --- Uncomment to allow free registration of content --- //
    check(/*/has_auth(submitter) || /*/has_auth(get_self())), "Add linked content by sending " conf.one_up_amount.to_string() +" " + conf.up_token_symbol.to_string()+" with memo url|<your url>" ); //CHECK If this is the correct memo with the updated upcatcher

    // --- Call dispatcher function --- // 
    addcontent(submitter, latitude, longitude, continent_subregion_code, country_code, continent_subregion_name, country_iso3, subdivision, postal_code, url, domain, collection, templateid);

    return;
}//END addurl

ACTION ups::addnft(const name& submitter, double latitude = 0.0, double longitude = 0.0, uint32_t continent_subregion_code = 0, uint32_t country_code = 0, const string& continent_subregion_name = "", const string& country_iso3 = "", uint32_t subdivision = 0, uint32_t postal_code = 0, const name& collection = ""_n, const uint32_t& templateid = 0) { 
    string url = ""; 
    name domain = ""_n; 
    auto conf = check_config();
    
    // --- Uncomment to allow free registration of content --- //
    check(/*/has_auth(submitter) || /*/has_auth(get_self())), "Add NFTs to be ranked by sending " conf.one_up_amount.to_string() +" " + conf.up_token_symbol.to_string()+" with memo url|<your url>" ); //CHECK If this is the correct memo with the updated upcatcher

    // --- Call dispatcher function --- // 
    addcontent(submitter, latitude, longitude, continent_subregion_code, country_code, continent_subregion_name, country_iso3, subdivision, postal_code, url, domain, collection, templateid);

    return;
}//END addbft

//TODO WARN needs update to remove the up records
ACTION ups::removecontent(uint32_t content_id = 0, name collection = ""_n, uint32_t template_id = 0) {

    content_t contents(get_self(), get_self().value); // Access the content table

    // If content_id is provided, remove by content_id
    if (content_id != 0) {
        auto itr = contents.find(content_id);
        check(itr != contents.end(), "Content with this ID does not exist.");

        check((has_auth(itr->submitter) || has_auth(get_self())) , "Only the submitter or contract can remove the content.")

        contents.erase(itr); // Remove the content from the table
    } 
    // If content_id is not set, use the collection name and template_id
    else if (collection != ""_n && template_id != 0) {
        auto by_template = contents.get_index<"byextid"_n>(); 
        auto temp_itr = by_template.find(((uint64_t) content_id));

        bool found = false;
        for (; temp_itr != by_template.end(); ++temp_itr) {
            if (temp_itr->domain == collection) {
                by_template.erase(temp_itr);
                found = true;
                break; 
            }
        }

        check(found, "Nothing found for the specified collection and template ID.");
    } else {
        check(false, "Either content_id or both collection and template_id must be provided.");
    }
}


// WARN must add checks to payment actions to avoid paying ""_n
ACTION ups::removeupper(name upsender) {
    check(has_auth(upsender) || has_auth(get_self()), "Only "+upsender.to_string()+" can reset their account"); // Ensure only the contract can call this action 

    // --- Get the IOUs --- //
    ious_t ious(get_self(), get_self().value);
    auto upsender_idx = ious.get_index<"byupsender"_n>();
    auto upcatcher_idx = ious.get_index<"byupcatcher"_n>();

    auto upsender_itr = upsender_idx.lower_bound(upsender.value);
    auto upcatcher_itr = upcatcher_idx.lower_bound(upsender.value);
    int count = 0;

    while (upsender_itr != upsender_idx.end() && upsender_itr->upsender == upsender && count < 36) {
        upsender_itr = upsender_idx.erase(upsender_itr);
        count++;
    }

    count = 0; 
    while (upcatcher_itr != upcatcher_idx.end() && upcatcher_itr->upcatcher == upsender && count < 36) {
        upcatcher_itr = upcatcher_idx.erase(upcatcher_itr);
        count++;
    }

    // --- Get content where upsender is the submitter --- //
    content_t contents(get_self(), get_self().value);
    auto submitter_idx = contents.get_index<"bysubmitter"_n>();
    auto submitter_itr = submitter_idx.lower_bound(upsender.value);

    while (submitter_itr != submitter_idx.end() && submitter_itr->submitter == upsender) {
        contents.modify(*submitter_itr, same_payer, [&](auto& row) {
            row.submitter = ""_n; // Set the submitter to an empty name
        });
        submitter_itr++;
    }

    // Check if there are no more records in the ious table related to the upsender
    if (upsender_idx.lower_bound(upsender.value) == upsender_idx.end() && 
        upcatcher_idx.lower_bound(upsender.value) == upcatcher_idx.end()) {
        // Access the uppers table and remove the upsender
        uppers_t uppers(get_self(), get_self().value);
        auto upper_itr = uppers.find(upsender.value);
        if (upper_itr != uppers.end()) {
            uppers.erase(upper_itr);
        } else { // --- Add the user to Purgatory so oracle can remove them TODO add readme explacountry about why this is needed

            internallog_t internal_log(get_self(), get_self().value);
            check(internal_log.exists(), "");

            // Fetch the existing internal log record
            auto log = internal_log.get();

            // Check if the upsender is already in purgatory
            if (std::find(log.purgatory.begin(), log.purgatory.end(), upsender) == log.purgatory.end()) {
                // Add the upsender to purgatory
                log.purgatory.push_back(upsender);

                // Save the updated internal log record
                internal_log.set(log, get_self());
            }
        }
}
//END removeupper()

ACTION ups::pauserewards(bool pause) {
    // --- Action must be authorized by the contract itself --- //
    check(has_auth(get_self()), "Only contract owner can pause the rewards."); 

    // --- Access the config singleton --- //
    config_t _config(get_self(), get_self().value); 

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
    check(has_auth(get_self()), "Only contract owner can pause the Ups."); 

    // --- Access the config singleton --- //
    config_t _config(get_self(), get_self().value); 

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
ACTION ups::setconfig(name up_token_contract, symbol up_token_symbol, name reward_token_contract, symbol reward_token_symbol, asset one_up_amount, asset one_reward_amount, double reward_multiplier, uint32_t timeunit) {
    // --- Ensure the action is authorized by the contract itself --- //
    check(has_auth(get_self()), "Only contract owner can set the config"); 

    // --- Access the config singleton --- //
    config old_conf = check_config(true);

      // --- Token Checks --- //
    check(reward_token_symbol.is_valid() && up_token_symbol.is_valid(), "Invalid token symbol");
    check(one_up_amount.amount > 0 && one_reward_amount.amount > 0, "Quantity must be positive");
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
    new_conf.one_up_amount = one_up_amount; // --- One up and one reward are linked to one unit of time per user
    new_conf.one_reward_amount = one_reward_amount; // --- Keep this in mind if planning rewards + inflatioon
    new.conf.reward_multiplier = 1.0; // --- Increase or decrease rewards per time unit without destabalizing relationship. WARN may mean claims get paif current rate without calc of change tim 
    new_conf.timeunit = timeunit; // This cannot change once  Ups are made. 
    new_conf.paused_rewards = false; // --- Defaults to not be paused
    new_conf.paused_ups = false;

    // --- Set the new config in the singleton --- //
    _config.set(new_conf, get_self()); 

    // --- Set up the internal log --- //
    internallog_t internlog(get_self(), get_self().value);
    internallog internlog_data = internlog.get_or_default(internallog{//CHECK does this need to set a variable?
        .lastpay = current_time_point();
        .lastfullpay = current_time_point();
        .purgatory = vector<name>(); 
        .purg_content = vector<uint64_t>();
    });
}


// --- Receive Tokenized Ups + Put them in Tables --- //
[[eosio::on_notify("*::transfer")]] void ups::_catch( const name from, const name to, const asset quantity, const string memo )
{  

    if (from == _self || to != self) return;
    
    // --- Checks + Set up Variables --- //
    config conf = check_config()
    check(!conf.paused_ups, "Ups are currently paused.");
    check(get_first_receiver() == conf.up_token_contract, "This isn't the correct Up token.");
    uint64_t up_quantity 
    check(up_quantity = static_cast<uint32_t>(quantity.amount / conf.one_up_amount.amount), "Please send exact amount, a multiple of "+ conf.one_up_amount.to_string());
    check(up_quantity >= 1, "Your Up was too small. Send at least "+  conf.one_up_amount.to_string());

    name content_name; // --- To parse URL if needed 
    bool force_reg_content = false;

    // --- Check for '|' in memo --- //
    size_t delimiter_pos = memo.find('|');
    if (delimiter_pos != string::npos) {
        // --- Split memo into function name and parameter --- //
        string memo_man = memo.substr(0, delimiter_pos); // first part of URL
        string parameter = memo.substr(delimiter_pos + 1); // second part of URL
        name domain;
        name content_name;
 
        // --- Call the function based on memo_man --- //
        if (memo_man == "up") {
            if(parameter.size() <= 12){// --- Its a name of a content 
                upsertup(up_quantity, from, Name.from(parameter), 0);
            } else {// --- It's a URL
                content_name = parse_url(parameter)
                //TODO
            }
            return;
        } else if (memo_man == "url" ||memo_man == "link" ) {
            addcontent(from, 0.0, 0.0, 0, 0, "", "", 0, 0, parameter, content_name, ""_n, 0);
            return;
        } else if (memo_man == "nft") { // format nft|collection|tokenid  
            delimiter_pos = parameter.find('|')
            check(delimiter_pos != string::npos, "Please send the memo as: nft|<collection>|<templateid> like ");
            //WARN could use more checks
            // --- Split memo into collection name and template id --- //
            name collection = name(parameter.substr(0, delimiter_pos)); // first part of URL
            uint32_t templateid;
           check(templateid = static_cast<uint32_t>(parameter.substr(delimiter_pos + 1)), "Template ID isn't a number. Please send the memo as: nft|collection|templateid") ; // second part of URL

            addcontent(from, 0.0, 0.0, 0, 0, "", "", 0, 0, "", ""_n, collection, templateid);
            return;
        } else if (memo.size() <= 12) {
            domain = parse_url(parameter);
            // --- Check if content is registered in _content --- //
        } else {
            // Handle unknown memo
            check (0, "Unknown memo, send contentid or the url with up| or url| register/upvote or reg| to register");
        }
    }

    // --- If '|' is not found in memo, treat the entire memo as a name
    check(memo.size() <= 12, "Please send Up with an contentid or register this content. reg|url")

  // --- Instantiate Content Table --- //
  _content ups(_self, _self.value);

  // --- Check for content in table --- // 
  auto itr = ups.find(content_name);
  check(itr != ups.end(), "Content ID does not exist. Add it first.");
  
  // --- Pass on to upsertup() to register in table --- //
  upsertup(up_quantity, from, content_name, 0);
  
} // END token transfer listener



/*/

This file handles the dispatch of tokens, etc in a standardized way.
This allows us to be more flexible with what constitutes an up:
1) A token transfer with an up| memo
2) A token stansfer with just a name that's a content ID (no special memo)
3) An Up action on a contract that has been registered as a "content id"



/*/



// --- DISPATCHER Checks + calls logup() updateiou() and updatetotal() --- //
void ups::upsertup(uint32_t upscount, name upsender, uint64_t content_id, bool negative) {

    // --- Check content ID id valid

    if (!negative){
        // --- Log the ups in ups table --- // 
        upsert_logup(upscount, upsender, content_id, negative);
        
        // --- Calls action to update the TOTALS table -- //
        upsert_total(upscount, upsender, content_id, negative);
    }
}//END upsertup()




// --- Update running log of ups --- //
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

// --- Upsert _uppers and _totals --- //
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
    
}

void upsertupper(uint32_t upscount, name upsender) {
    
}

void removeupper(name upsender) {
    
}

void removecont(uint64_t content_id) {
    
}

void addcontent(name& submitter, string& url) {
    name domain = parse_url_for_domain(url);

    // dont forget :       row.id = _ups.available_primary_key();
}

void deepremvcont(uint64_t content_id) {
    
}

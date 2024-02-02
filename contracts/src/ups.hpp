#include "eosio/eosio.hpp"
#include <eosio/asset.hpp>
#include <atomicassets-interface.hpp>

#include <checkformat.hpp>
#include <atomicdata.hpp>


using namespace eosio;
using namespace std;
using namespace atomicdata;

class [[eosio::contract]] ups : public contract {
  
using contract::contract;
public: 



struct content_provider {
  name provider_hemi;
  string domain_tld;
  uint64_t tetra_loc;

  uint64_t primary_key() const { return provider_hemi.value; }
};

typedef singleton<name("content_provider"), content_provider> content_provider_sing;

// --- SCOPED to name provider_hemi --- //
TABLE content_table {
  name content_hemi;
  string domain_tld;
  name submitter;
  vector<double> geoloc;
  uint64_t tetraloc;

  vector<string> link;
  uint64_t primary_key() const { return content_hemi.value; }
  uint64_t by_submitter() const { return submitter; }
  uint64_t by_index() const { return index; }
  uint64_t by_tetraloc() const { return tetraloc; }
};

using content_table_index = multi_index<name("content"), content_table,
  indexed_by<"byauthor"_n, const_mem_fun<content_table, uint64_t, &content_table::by_author>>,
  indexed_by<"byindex"_n, const_mem_fun<content_table, uint64_t, &content_table::by_index>>,
  indexed_by<"bygeoloc"_n, const_mem_fun<content_table, uint64_t, &content_table::by_geoloc>>,
  indexed_by<"bytetraloc"_n, const_mem_fun<content_table, uint64_t, &content_table::by_tetraloc>>,
  indexed_by<"bylink"_n, const_mem_fun<content_table, uint64_t, &content_table::by_link>>
  >;


private:  
  TABLE upslog { 
    uint64_t upid; 
    uint64_t content_id;
    uint32_t totalups; 
    uint32_t tuid;
  
    uint64_t primary_key() const { return upid; }
    uint64_t by_content_id() const { return content_id; }
    uint64_t by_ups() const { return (uint64_t) totalups; }

    uint64_t by_tuid() const { return (uint64_t) tuid; }
  };
  
  using upslog_table = multi_index<name("upslog"), upslog,
    eosio::indexed_by<"bycontentid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_content_id>>,
    eosio::indexed_by<"byups"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_ups>>,
    eosio::indexed_by<"bytuid"_n, eosio::const_mem_fun<upslog, uint64_t, &upslog::by_tuid>>
  >;
  
  TABLE totals {
    uint64_t content_id;
    uint32_t totalups; 
    uint32_t updated;
    
    uint64_t primary_key() const { return content_id; }
  };
  
  using totals_table = multi_index<name("totals"), totals>;
  
  

  
  // --- Activity stats for uppers (For future awards) --- //
  TABLE uppers {
    name upsender;
    uint32_t firstup;
    uint32_t lastup;
    uint32_t totalups;
    uint64_t primary_key() const { return upsender.value; }
  };
  
    using uppers_table = multi_index<name("uppers"), uppers>;



  TABLE schemas_s {
      name schema_name;
      vector <FORMAT> format;

      uint64_t primary_key() const { return schema_name.value; }
  };

  
  // --- Store record of who to pay --- // 
  // CHECK (in .cpp) that we are paying both the upsender + upcatcher
  TABLE ious {
    uint64_t iouid;
    name upsender;
    name upcatcher;
    uint32_t upscount; // Should be either BIGSOL or sol up or both
    uint8_t upstype;
    uint32_t initiated;
    uint32_t updated; 
    uint64_t primary_key() const { return iouid; }
    uint64_t by_upcatcher() const { return upcatcher.value; }
    uint64_t by_upsender() const { return upsender.value; }
    uint64_t by_upscount() const { return (uint64_t) upscount; }
    uint64_t by_initiated() const { return (uint64_t) initiated; }
    uint64_t by_updated() const { return (uint64_t) updated; }
  };

  using ious_table = multi_index<name("ious"), ious,
    eosio::indexed_by<"byupcatcher"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upcatcher>>,
    eosio::indexed_by<"byupsender"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upsender>>,
    eosio::indexed_by<"byupscount"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_upscount>>,
    eosio::indexed_by<"byinitiated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_initiated>>,
    eosio::indexed_by<"byupdated"_n, eosio::const_mem_fun<ious, uint64_t, &ious::by_updated>>
  >;

  
  
  
  TABLE internallog { // track Macro statistics for each token 
    uint32_t lastpay; // Last time the payment was called for all 
    uint32_t lastfullpay; // All accounts sent BLUX 
    bool remaining; // Did we reach the end of who is owed to pay? 
    
    uint64_t primary_key() const { return (uint64_t) lastpay; } //WARN CHECK if this is singleton (it isn't, fix it)
  };
  
  using cxclog_table = multi_index<name("internallog"), internallog>;

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

  
  void updateup(uint32_t upscount, uint8_t upstype, name upsender, uint64_t content_id); //DISPATCHER
  void logup(uint32_t upscount, uint8_t upstype, name upsender, uint64_t content_id); 
  void removeiou(name sender, name receiver); // Receiver or sender can be set to dummy value to delete all for a user
  void updatelisten(uint32_t upscount, uint8_t upstype, name upsender);
  void removelisten(name upsender);
  void removecont(uint64_t content_id); // Removes all IOUs for nft + nft record (minimal)
  void deepremvcont(uint64_t content_id); // Removes all records of Ups for this content
  //MOVED to upsert in helpups.cpp // void updateartistgroup(string groupname, name intgroupname, vector<string> artists, vector<int8_t> weights);
  
  
  // --- Declare the _tables for later use --- // 
  ious_table _ious;
  upslog_table _upslog;
  uppers_table _uppers;
  totals_table _totals;
  cxclog_table _internallog;

  // --- Declare Config Singleton --- //
  typedef singleton<name("config"), config> config_t;
  
public:
  
  [[eosio::on_notify("*::transfer")]] // CHECK REQUIRES correct contract for SOL/BLUX Listens for any token transfer
  void up_catch( const name from, const name to, const asset quantity, const std::string memo );
  
  ACTION payup(void); // Default call
  
  ACTION payup(name upsender); // User's call to pay themselves
  
  ACTION updatecont(uint64_t content_id, nft danft); 

// === Contract Utilities === //




};

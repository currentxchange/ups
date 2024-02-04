#include <eosio/eosio.hpp>
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
  name domain;
  string raw_domain;
  vector<uint32_t> tetra_loc;

  uint64_t primary_key() const { return domain.value; }
};

typedef singleton<name("content_provider"), content_provider> content_provider_sing;

// --- SCOPED to name domain --- //
TABLE content_table {
  uint64_t id;
  name domain_tld;
  name submitter;
  string link;
  checksum256 gudahash;
  vector<double> latlng(1, [0.0,0.0]);
  vector<uint32_t> tetra_loc(0,0,0,0);

  uint64_t primary_key() const { return id; }
  uint64_t by_domain() const { return domain_tld.value; }
  uint64_t by_tetraloc1() const { return static_cast<uint64_t>(tetra_loc[0]); }
  uint64_t by_tetraloc2() const { return static_cast<uint64_t>(tetra_loc[1]); }
  uint64_t by_tetraloc3() const { return static_cast<uint64_t>(tetra_loc[2]); }
  uint64_t by_tetraloc4() const { return static_cast<uint64_t>(tetra_loc[3]); }
  uint64_t by_tetraloc12() const { return (static_cast<uint64_t>(tetra_loc[0]) << 32) | tetra_loc[1]; } // Combines the first two parts
  uint64_t by_tetraloc34() const { return (static_cast<uint64_t>(tetra_loc[2]) << 32) | tetra_loc[3]; } // Combines the last two parts

};

using content_table_index = multi_index<name("content"), content_table,
  indexed_by<"bydomain"_n, const_mem_fun<content_table, uint64_t, &content_table::by_domain>>,
  indexed_by<"bytetra1"_n, const_mem_fun<content_table, uint64_t, &content_table::by_tetraloc1>>,
  indexed_by<"bytetra2"_n, const_mem_fun<content_table, uint64_t, &content_table::by_tetraloc2>>,
  indexed_by<"bytetra3"_n, const_mem_fun<content_table, uint64_t, &content_table::by_tetraloc3>>,
  indexed_by<"bytetra4"_n, const_mem_fun<content_table, uint64_t, &content_table::by_tetraloc4>>,
  indexed_by<"bytetra12"_n, const_mem_fun<content_table, uint64_t, &content_table::by_tetraloc12>>,
  indexed_by<"bytetra34"_n, const_mem_fun<content_table, uint64_t, &content_table::by_tetraloc34>>
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

name parse_url(const string& url) const {
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

    // Extract the domain part after "://" and "www."
    string domain_part = url.substr(start);

    // Find the first slash after the domain part to ensure only the domain is included
    auto end = domain_part.find('/');
    if (end != string::npos) {
        domain_part = domain_part.substr(0, end);
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

    // Convert the domain part to a name type
    return name(domain_part);
}

};

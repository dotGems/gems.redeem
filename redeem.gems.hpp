#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

using namespace eosio;

class [[eosio::contract("redeem.gems")]] redeem : public eosio::contract {
public:
    using contract::contract;

    [[eosio::action]]
    void setredeem( const uint32_t template_id, const asset quantity, const bool pomelo_grant );
};
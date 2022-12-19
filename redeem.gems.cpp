#include "redeem.gems.hpp"


[[eosio::action]]
void redeem::setredeem( const uint32_t template_id, const asset quantity, const bool pomelo_grant )
{
    require_auth( get_self() );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must redeem positive quantity" );
    check( quantity.symbol == symbol("EOS", 4), "symbol precision mismatch" );

}
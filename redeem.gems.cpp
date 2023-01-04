#include "./redeem.gems.hpp"

[[eosio::action]]
void redeem::delredeem( const uint32_t template_id )
{
    require_auth( get_self() );

    redeem::redeems_table _redeems( get_self(), get_self().value );
    auto & itr = _redeems.get( template_id, "template_id does not exists" );
    _redeems.erase( itr );
}

[[eosio::action]]
void redeem::setredeem( const uint32_t template_id, const asset quantity, const bool redirect_to_pomelo_grant )
{
    require_auth( get_self() );

    check( quantity.is_valid(), "invalid quantity" );
    check( quantity.amount > 0, "must redeem positive quantity" );
    check( quantity.symbol == symbol("EOS", 4), "invalid quantity symbol" );

    redeem::redeems_table _redeems( get_self(), get_self().value );
    auto itr = _redeems.find( template_id );

    // recipe content
    auto insert = [&]( auto & row ) {
        row.template_id = template_id;
        row.quantity = extended_asset{quantity, "eosio.token"_n};
        row.redirect_to_pomelo_grant = redirect_to_pomelo_grant;
    };

    if ( itr == _redeems.end() ) _redeems.emplace( get_self(), insert );
    else  _redeems.modify( itr, get_self(), insert );
}

[[eosio::on_notify("atomicassets::transfer")]]
void redeem::on_nft_transfer( const name from, const name to, const vector<uint64_t> asset_ids, const std::string memo )
{
    redeem::redeems_table _redeems( get_self(), get_self().value );

    for ( const uint64_t asset_id : asset_ids ) {
        auto asset = atomic::get_asset( get_self(), asset_id );
        auto redeem = _redeems.get( asset.template_id, "cannot redeem template_id" );
        
        // NFT name attribute
        atomicdata::ATTRIBUTE_MAP data = atomic::get_template_immutable( asset );
        const string nft_name = atomic::attribute_to_string(data, "name");

        // burn and transfer
        atomic::burnasset( get_self(), asset_id );

        if ( redeem.redirect_to_pomelo_grant ) {
            handle_pomelo_transfer( from, redeem.quantity, memo, nft_name);
        } else {
            const string message = "🪂💰 [" + from.to_string() + "] has redeemed a " + nft_name + " Airdrop!";
            transfer( get_self(), from, redeem.quantity, message.c_str());
        }
    }
}

name redeem::parse_pomelo_grant_name( string memo )
{
    // remove URL pathing
    if (memo.find("https://pomelo.io/grants/") != std::string::npos) {
        const string toReplace = "https://pomelo.io/grants/";
        const size_t pos = memo.find(toReplace);
        memo = memo.replace(pos, toReplace.length(), "");
    }

    // parse name
    const name grant = utils::parse_name(memo);
    check( grant.value, "invalid Pomelo Grant name (ex: hotsauce)" );
    return grant;
}

[[eosio::action]]
void redeem::test( const string memo ) {
    print(parse_pomelo_grant_name(memo));
}

void redeem::handle_pomelo_transfer( const name from, const extended_asset value, const string memo, const string nft_name )
{
    pomelo::globals_table _globals( "app.pomelo"_n, "app.pomelo"_n.value );
    pomelo::grants_table _grants( "app.pomelo"_n, "app.pomelo"_n.value );
    pomelo::rounds_table _rounds( "app.pomelo"_n, "app.pomelo"_n.value );
    pomelo::seasons_table _seasons( "app.pomelo"_n, "app.pomelo"_n.value );

    uint16_t season_id = _globals.get().season_id;
    vector<uint16_t> round_ids = _seasons.get( season_id, "Pomelo Season does not exists" ).round_ids;
    const name grant = parse_pomelo_grant_name(memo);
    
    auto & itr = _grants.get( grant.value, "Pomelo Grant does not exists" );
    check( itr.status == "published"_n, "Pomelo Grant is not published" );

    bool active_grant = false;
    for ( const uint16_t round_id : round_ids ) {
        pomelo::rounds_table _rounds( "app.pomelo"_n, "app.pomelo"_n.value );
        auto & round = _rounds.get( round_id, "Pomelo Grant round_id does not exists" );

        for ( const name grand_id : round.grant_ids ) {
            if ( grand_id == grant ) {
                active_grant = true;
                break;
            }
        }
    }
    check( active_grant, "Pomelo Grant is not active");

    const string message = "🍈💰 [" + from.to_string() + "] has redeemed a " + nft_name + " and donated to your [" + grant.to_string() + "] Pomelo Grant!";
    transfer( get_self(), itr.funding_account, value, message.c_str());
}

void redeem::transfer( const name from, const name to, const extended_asset value, const string memo )
{
    eosio::token::transfer_action transfer( value.contract, { from, "active"_n });
    transfer.send( from, to, value.quantity, memo );
}

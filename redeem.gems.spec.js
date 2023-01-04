import { TimePointSec } from "@greymass/eosio";
import { Blockchain } from "@proton/vert"
import { it, describe, beforeEach } from "node:test";
import assert from 'node:assert';

// Vert EOS VM
const blockchain = new Blockchain()

// contracts
const code = 'redeem.gems';
const contract = blockchain.createContract(code, code, true);

// one-time setup
beforeEach(async () => {
  blockchain.setTime(TimePointSec.from(new Date()));
});

describe(code, () => {
  it("parse_pomelo_grant_name", async () => {
    await contract.actions.test(["https://pomelo.io/grants/hotsauce"]).send();
    await contract.actions.test(["hotsauce"]).send();
    assert.ok(true);
  });

  it("setredeem", async () => {
    await contract.actions.setredeem([123, "1.0000 EOS", false]).send();
    assert.ok(true)
  });

  it("error: check time in future", async () => {
    const action = contract.actions.setredeem([123, "1.0000 FOO", false]).send();
    await expectToThrow(action, /invalid quantity symbol/);
  });
});

/**
 * Expect a promise to throw an error with a specific message.
 * @param promise - The promise to await.
 * @param {string} errorMsg - The error message that we expect to see.
 */
const expectToThrow = async (promise, errorMsg) => {
  try {
    await promise
    assert.fail('Expected promise to throw an error');
  } catch (e) {
    if ( errorMsg ) assert.match(e.message, errorMsg);
    else assert.fail('Expected promise to throw an error');
  }
}
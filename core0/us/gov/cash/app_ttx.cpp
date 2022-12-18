//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the GPLv3 License as published by the Free
//===-    Software Foundation.
//===-
//===-    This program is distributed in the hope that it will be useful,
//===-    but WITHOUT ANY WARRANTY; without even the implied warranty of
//===-    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//===-
//===-    You should have received a copy of the General Public License
//===-    along with this program, see LICENCE file.
//===-    see https://www.gnu.org/licenses
//===-
//===----------------------------------------------------------------------------
//===-
#include <random>
#include <thread>
#include <chrono>

#include <us/gov/engine/protocol.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>

#include "tx_t.h"
#include "ttx.h"
#include "map_tx.h"
#include "file_tx.h"
#include "accounts_t.h"
#include "local_delta.h"
#include "app.h"

#define loglevel "gov/cash"
#define logclass "app_ttx"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::app;
using us::ko;


bool c::unlock(const hash_t& address, const locking_program_t& locking_program, const locking_program_input_t& locking_program_input, const ttx& t) {
    log("unlock");
    if (unlikely(locking_program == 0)) {
        return true;
    }
    if (locking_program == locking_program_id) {
        if (locking_program_input.pubkey.hash() != address) {
            log("Address check failed.");
            return false;
        }
        auto h = t.get_hash();
        if (!crypto::ec::instance.verify(locking_program_input.pubkey, h, locking_program_input.sig)) {
            return false;
        }
        return true;
    }
    return false;
}

bool c::process(const ttx& t) {
    if (t.token != t.address) {
        log("token must match address", "not a mint.");
        return false;
    }
    accounts_t::batch_t batch;
    account_t state;
    if (!account_state(batch, t.address, state)) {
        #if CFG_COUNTERS == 1
            ++counters.tx_rejected_state;
        #endif
        return false;
    }
    if (!unlock(t.address, state.locking_program, t.locking_program_input, t)) {
        #if CFG_COUNTERS == 1
            ++counters.tx_rejected_could_not_unlock_input;
        #endif
        return false;
    }
    cash_t fee = 1;
    if (state.box.value < fee) {
        log("not enough gas to pay fees");
        return false;
    }
    state.box.value -= fee;
    state.box.set_supply(t.token, t.supply);
    batch.add(t.address, state);

    pool->fees += fee;
    pool->accounts.add(batch);
    #if CFG_COUNTERS == 1
        ++counters.tx_processed;
    #endif
    return true;
}


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
#include "app.h"
#include "local_delta.h"

#define loglevel "gov/cash"
#define logclass "app_tx"
#include <us/gov/logs.inc>

using namespace std;
using namespace us::gov;
using namespace us::gov::cash;
using c = us::gov::cash::app;
using us::ko;

bool c::unlock(const hash_t& address, const locking_program_t& locking_program, const locking_program_input_t& locking_program_input, const tx_t& t) {
    log("unlock");
    if (likely(locking_program == locking_program_id)) {
        if (locking_program_input.pubkey.hash() != address) {
            log("Address check failed.");
            return false;
        }
        auto h = t.get_hash(locking_program_input.sigcode);
        if (!crypto::ec::instance.verify(locking_program_input.pubkey, h, locking_program_input.sig)) {
            return false;
        }
        return true;
        return check_input(address, t, locking_program_input);
    }
    if (unlikely(locking_program == 0)) {
        return true;
    }
/*
    ostringstream prog;
    prog << "firejail/apparmor --private <dir>" << locking_program << "/main";
    pid_t pid = fork();
    switch (pid) {
        case -1:
            log("Uh-Oh! fork() failed.");
            return false;
        case 0:
            log("firejail ", prog.str());
            execl("firejail", prog.str().c_str());
            return true;
        default:
            int status;
            while (!WIFEXITED(status)) {
                waitpid(pid, &status, 0);
            }
            //std::cout << "Process exited with " << WEXITSTATUS(status) << "\n";
            return status == 0;
    }
*/
    return false;
}

bool c::process(const tx_t& t, const tx::section_t& s, batch_t& batch, cash_t& fee) {
    log("process inputs", s.inputs.size());
    for (size_t j = 0; j < s.inputs.size(); ++j) {
        auto& i = s.inputs[j];
        account_t state;
        if (!account_state(batch, i.address, state)) {
            #if CFG_COUNTERS == 1
                ++counters.tx_rejected_state;
            #endif
            return false;
        }
        if (!unlock(i.address, state.locking_program, i.locking_program_input, t)) {
            #if CFG_COUNTERS == 1
                ++counters.tx_rejected_could_not_unlock_input;
            #endif
            return false;
        }
        if (!state.box.burn(s.token,i.amount, fee)) {
            return false;
        }
        batch.add(i.address, state);
    }
    log("process outputs", s.outputs.size());
    for (size_t j = 0; j < s.outputs.size(); ++j) {
        auto& i = s.outputs[j];
        account_t state;
        account_state(batch, i.address, state);
        state.locking_program = locking_program_id; //1;
        state.box.add(s.token, i.amount);
        batch.add(i.address, state);
    }
    if (!s.check_amounts()) {
        log("KO 77069 Amounts are not correct.");
        return false;
    }
    return true;
}

bool c::process(const tx_t& t, const tx::sections_t& ss, batch_t& batch, cash_t& fee) {
    for (auto& i: ss) {
        log("process section");
        if (!process(t, i, batch, fee)) return false;
    }
    return true;
}

bool c::process(const tx_t& t) {
    log("process tx", t.ts);
    accounts_t::batch_t batch;
    cash_t fee = 0;
    if (!process(t, t.sections, batch, fee)) {
        return false;
    }
    pool->fees += fee;
    log("fees", fee);
    pool->accounts.add(batch);
    #if CFG_COUNTERS == 1
        ++counters.tx_processed;
    #endif
    report_wallet(t, "mempool");
    return true;
}

bool c::check_input(const hash_t& address, const tx_t& t, const locking_program_input_t& locking_program_input) {
    if (locking_program_input.pubkey.hash() != address) {
        //cout << "Address check failed." << endl;
        return false;
    }
    sigmsg_hash_t h = t.get_hash(locking_program_input.sigcode);
    return us::gov::crypto::ec::instance.verify(locking_program_input.pubkey, h, locking_program_input.sig);
}


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
#include <us/gov/config.h>
#include <us/gov/io/cfg0.h>
#include <us/gov/socket/datagram.h>

#include "daemon_t.h"
#include "protocol.h"
#include "peer_t.h"

#define loglevel "gov/dfs"
#define logclass "peer_t"
#include "logs.inc"
#include "dto.inc"

using namespace us::gov::dfs;
using c = us::gov::dfs::peer_t;

c::peer_t(daemon_t& daemon, int sock): b(daemon, sock) {
}

c::~peer_t() {
}

bool c::process_work(datagram* d) {
    log("process_work");
    assert(d->service < protocol::dfs_end);
    if (d->service < protocol::dfs_begin) {
        return b::process_work(d);
    }
    #ifdef CFG_PERMISSIONED_NETWORK
    switch(d->service) {
        case protocol::dfs_file_request: {
            log("received gov_dfs_file_request");
                if (!static_cast<daemon_t&>(daemon).check_dfs_permission(pubkey.hash())) {
                    delete d;
                    log("query file rejected because lack of permissions");
                    return true;
                }
            log("RBF - skipping check permission", "bug. wont sync");
        }
        break;
    }
    #endif
    log("process_work");
    switch(d->service) {
        #ifdef has_us_gov_dfs_api
            using namespace us::gov::protocol;
            #include <us/api/generated/gov/c++/dfs/hdlr_svc-router>
        #endif
        break;
    }
    return false;
}

#ifdef has_us_gov_dfs_api
#include <us/api/generated/gov/c++/dfs/hdlr_svc_handler-impl>

//------------------apitool - API Spec defined @ us/api/generated/gov/c++/dfs/hdlr_local-impl

ko c::handle_file_request(hash_t&& content_digest, vector<uint8_t>& content) {
    log("file_request");
    ostringstream file;
    file << static_cast<daemon_t&>(daemon).fsroot << '/' << content_digest;
    log("send_file", file.str());
    auto r = io::read_file_(file.str(), content);
    if (is_ko(r)) {
        return r;
    }
    if (unlikely(content.empty())) {
        auto r = "KO 21021 empty content";
        log(r);
        return r;
    }
    return ok;
}

ko c::handle_file_request_response(vector<uint8_t>&& content) {
    log("file_request_response");

    auto h = ::us::gov::crypto::ripemd160::digest(content);
    log("RBF", "file reception is not spam controlled");
    ostringstream file;
    file << static_cast<daemon_t&>(daemon).fsroot << '/' << h;
    if (io::cfg_filesystem::file_exists(file.str())) {
        log("file already exists", file.str(), io::cfg_filesystem::file_size(file.str()), "ignoring");
        return ok;
    }
    log("saving file", file.str());
    ofstream os(file.str(), ios::out | ios::binary);
    os.write((const char*)&content[0], content.size());
    if (unlikely(os.fail())) {
        log("failed writing file", file.str());
        ::remove(file.str().c_str());
    }
    else {
        log("success written file", file.str(), content.size(), "bytes");
    }
    return ok;
}

//-/----------------apitool - End of API implementation.

#include <us/api/generated/gov/c++/dfs/cllr_rpc-impl>

#endif


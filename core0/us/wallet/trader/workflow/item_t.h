//===-                           P L E B B L E
//===-                         https://plebble.us
//===-
//===-              Copyright (C) 2017-2022 root1m3@plebble.us
//===-
//===-                      GNU GENERAL PUBLIC LICENSE
//===-                       Version 3, 29 June 2007
//===-
//===-    This program is free software: you can redistribute it and/or modify
//===-    it under the terms of the AGPLv3 License as published by the Free
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
#pragma once
#include <string>

#include <us/gov/socket/client.h>
#include <us/gov/io/seriable.h>

#include <us/wallet/engine/peer_t.h>
#include <us/wallet/trader/ch_t.h>
#include <us/wallet/trader/params_t.h>

#include "doc0_t.h"

namespace us { namespace wallet { namespace trader { namespace workflow {

    struct workflow_t;

    struct item_t: virtual us::gov::io::seriable {
        using b = us::gov::io::seriable;
        using peer_t = us::wallet::engine::peer_t;
        using magic_t = doc0_t::magic_t;
        using ch_t = trader::ch_t;

        item_t(): parent(nullptr) {}
        item_t(workflow_t* parent, const string& name, const string& long_name);
        virtual ~item_t();

        virtual doc0_t* create_doc() const = 0;
        void set(ch_t&) const;
        void unset(ch_t&) const;
        ko load(const string& filename, ch_t&);
        inline ko load(ch_t& ch) { return load(filename(), ch); }
        ko unload(ch_t&);
        ko save() const;
        ko tamper(const string& w0, const string& w1);
        void help_onoffline(const string& indent, ostream&) const;
        ko exec_offline(const string& home, const string& cmd, ch_t&);
        ko send_to(peer_t&) const;
        ko send_request_to(peer_t&) const;
        inline bool has_doc() const { return doc != nullptr; }
        void replace_doc(doc0_t*, ch_t&);
        bool sig_reset(ostream&);
        bool sig_hard_reset(const string& dir, ostream&);
        bool verify(ostream&) const;
        bool verify() const;
        virtual void api_list(const string& scope, multimap<string, pair<string, string>>& items) const;
        virtual ko api_exec(const string& scope, const string& fn, const string& args, ostream&);

        struct doctypes_t: pair<vector<uint16_t>, vector<uint16_t>> {
            void set(ch_t&) const;
        };

        virtual magic_t my_magic() const = 0;

        void doctypes(doctypes_t&) const; //consumer, producer

        enum mode_t {
            mode_send,
            mode_recv,

            num_modes
        };

        constexpr static array<const char*, num_modes> modestr = {"send", "recv"};

        void set_mode(mode_t, ch_t&); //returns true:changed false:not changed
        string filename() const;
        pair<ko, doc0_t*> doc_from_blob(blob_reader_t&) const;

    public:
        size_t blob_size() const override;
        void to_blob(blob_writer_t&) const override;
        ko from_blob(blob_reader_t&) override;

    public:
        string name;
        string long_name;
        doc0_t* doc{nullptr};
        workflow_t* parent;
        mode_t mode{mode_recv};
    };

}}}}



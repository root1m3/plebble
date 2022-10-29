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
#pragma once
#include <us/gov/engine/rpc_peer_t.h>

namespace us::gov::cli {

    struct rpc_peer_t: engine::rpc_peer_t {
        using b = engine::rpc_peer_t;
        using b::rpc_peer_t;

        void dump(const string& prefix, ostream&) const;
        void dump_all(const string& prefix, ostream&) const override;
//        inline ko authorize(const pub_t&, pin_t) override { return ok; } //Improve this to authenticate our connection against gov daemon
//        void on_peer_disconnected(const reason_t&) override;
//        void on_I_disconnected(const reason_t&) override;

        inline ko authorize(const pub_t&, pin_t) override { return ok; }
        void on_peer_disconnected(const reason_t&) override;
        void on_I_disconnected(const reason_t&) override;
        void verification_completed(pport_t, pin_t) override;
        void upgrade_software() override;

/*
        virtual void verification_completed(bool verif_ok);
        virtual void on_I_disconnected(const string& reason);
        virtual void on_peer_disconnected(const string& reason);
        virtual void on_connect(ko error);
        virtual string rewrite(ko r) const;
*/
    };

}

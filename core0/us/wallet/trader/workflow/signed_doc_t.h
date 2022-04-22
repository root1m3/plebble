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
#include "expiry_doc_t.h"

namespace us { namespace wallet { namespace trader { namespace workflow {

    using namespace std;

    template<typename base_doc_t>
    struct signed_doc: base_doc_t, us::gov::engine::signed_data {
        using b = base_doc_t;
        using b2 = us::gov::engine::signed_data;
        using ripemd160 = b2::ripemd160;
        using sigmsg_hasher_t = b2::sigmsg_hasher_t;
        using body = b;
        using sig = b2;

        signed_doc() {
        }

        ~signed_doc() override {
        }

        struct options: b::options {
            options() {
                sk.zero();
            }
            options(const options&) = delete;
            options(options&&) = delete;
            options& operator = (const options&) = delete;

            ko parse(const string& option, istream& is) override {
                if (option == "-sk") {
                    sk.zero();
                    is >> sk;
                    if (is.fail()) {
                        sk.zero();
                        return "KO 81119 priv key.";
                    }
                    return ok;
                }
                return b::options::parse(option, is);
            }

            static void exec_help(const string& prefix, ostream& os) {
                os << prefix << "-sk  Secret key for signing.\n";
                b::options::exec_help(prefix, os);
            }

            void dump(const string& pfx, ostream& os) const {
                os << pfx << "sk " << sk << '\n';
                b::options::dump(pfx, os);
            }

            us::gov::crypto::ec::keys::priv_t sk;
        };

        ko init(const options& o) {
            auto r = b::init(o);
            if (is_ko(r)) {
                return r;
            }
            us::gov::crypto::ec::keys k(o.sk);
            if (!k.pub.valid) {
                return "KO 20119 Invalid secret key.";
            }
            sign(k);
            return ok;
        }

        void write_pretty(ostream& os) const {
            b::write_pretty(os);
        }

        void write_pretty_en(ostream& os) const override {
            b::write_pretty_en(os);
            b2::write_pretty_en(os);
        }

        void write_pretty_es(ostream& os) const override {
            b::write_pretty_es(os);
            b2::write_pretty_es(os);
        }

        void hash_data_to_sign(sigmsg_hasher_t& h) const override {
            b::hash_data_to_sign(h);
        }

        void hash_data(hasher_t& h) const override {
            b::hash_data(h);
        }

        bool verify(ostream& os) const override {
            if (!b::verify(os)) return false;
            return b2::verify(os);
        }

        bool verify() const override {
            if (!b::verify()) return false;
            return b2::verify();
        }

        size_t blob_size() const override {
            return b::blob_size() + b2::blob_size();
         }

        void to_blob(blob_writer_t& writer) const override {
            b::to_blob(writer);
            b2::to_blob(writer);
        }

        ko from_blob(blob_reader_t& reader) override {
            {
                auto r = b::from_blob(reader);
                if (is_ko(r)) return r;
            }
            {
                auto r = b2::from_blob(reader);
                if (is_ko(r)) return r;
            }
            return ok;
        }

    };

    using signed_doc_t = signed_doc<doc0_t>;
    using signed_expiry_doc_t = signed_doc<expiry_doc_t>;

}}}}


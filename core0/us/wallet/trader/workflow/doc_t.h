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

#include <us/gov/crypto/types.h>

#include <us/wallet/trader/traders_t.h>
#include <us/wallet/wallet/local_api.h>

#include "doc0_t.h"

namespace us::wallet::trader::workflow {

    ///b one of {workflow_signed_expiry_doc_t, workflow_signed_doc_t, workflow_expiry_doc_t, workflow_doc0_t }
    ///traits. Must publish t:: values
    template<typename b, typename t>
    struct doc_t: b, t {
        using traits = t;
        using magic_t = typename b::magic_t;

        string title() const override { return t::long_name; }
        string title_es() const override { return t::long_name_es; }
        magic_t get_magic() const override { return t::magic; }
        const char* get_name() const override { return t::name; }

        struct options: b::options {
            ko parse(const string& option, istream& is) override {
                if (option == "-o") {
                    is >> output_file;
                    if (output_file.empty()) {
                        return "KO 30121 output filename";
                    }
                    return ok;
                }
                return b::options::parse(option, is);
            }

            static void exec_help(const string& prefix, ostream& os) {
                os << prefix << "-h  This help\n";
                os << prefix << "-o  Output file\n";
                b::options::exec_help(prefix, os);
            }

            void dump(const string& pfx, ostream& os) const {
                os << pfx << "output_file " << output_file << '\n';
                b::options::dump(pfx, os);
            }

            string output_file;
        };

        static void exec_help(const string& prefix, ostream& os) {
            os << prefix << "create [options]:\n";
            options::exec_help("        ", os);
            os << prefix << "read <filename>:\n";
        }

        static pair<ko, doc_t*> create(const options& o) {
            auto doc = new doc_t();
            auto r = doc->init(o);
            if (is_ko(r)) {
                delete doc;
                doc = nullptr;
            }
            return make_pair(r, doc);
        }

        static ko exec(istream& is, wallet::local_api& w) {
            string cmd;
            is >> cmd;
            if (cmd == "create") {
                options o;
                {
                    auto r = o.parse_cmdline(is);
                    if (is_ko(r)) {
                        return r;
                    }
                }
                auto r = create(o);
                if (is_ko(r.first)) {
                    return r.first;
                }
                auto doc = r.second;
                assert(doc != nullptr);
                if (!o.output_file.empty()) {
                    us::gov::io::cfg0::ensure_writable(o.output_file);
                    auto r = doc->save(o.output_file);
                    if (is_ko(r)) {
                        return r;
                    }
                }
                ostringstream os;
                doc->write_pretty(os);
                if (!o.output_file.empty()) {
                    os << "file saved as " << o.output_file << '\n';
                }
                delete doc;
                return w.push_OK(hash_t(0), os.str());
            }
            if (cmd == "read") {
                string filename;
                is >> filename;
                if (is.fail()) {
                    auto r = "KO 66985 File name must be given.";
                    return r;
                }
                auto doc = new doc_t();
                auto r = doc->load(filename);
                if (is_ko(r)) {
                    delete doc;
                    return r;
                }
                ostringstream os;
                doc->write_pretty(os);
                delete doc;
                return w.push_OK(hash_t(0), os.str());
            }
            return "KO 66952 Invalid command";
        }
    };

}


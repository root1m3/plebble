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
#include "coder_cpp.h"
#include "api_t.h"
#include <sstream>
#include <cassert>
#include <fstream>

using namespace std;
using namespace us::apitool;
using c = us::apitool::cpp::coder;

unordered_map<string, string> c::types {
    {"s", "string"},
    {"p", "pub_t"},
    {"u1", "uint8_t"},
    {"k", "priv_t"},
    {"h", "hash_t"},
    {"i8", "cash_t"},
    {"u2", "uint16_t"},
    {"u4", "uint32_t"},
    {"u8", "uint64_t"},
    {"v", "blob_t"},
    {"S", "sig_t"},
    {"1", "vector<uint8_t>"},
    {"v{s}", "vector<string>"},
    {"vss", "vector<pair<string, string>>"},
    {"v{hs}", "vector<pair<hash_t, string>>"},
    {"v{h}", "vector<hash_t>"},
    {"H", "sigmsg_hash_t"},
    {"v{hu4u2}", "vector<tuple<hash_t, uint32_t, uint16_t>>"},
};

string c::protocol_prefix{""};

string c::get_type(const string& mne) const {
    auto i = types.find(mne);
    if (i != types.end()) return i->second;
    return mne;
}

void c::gen_gov_protocol_counters_init(const api_t& a, int base, ostream& include) const {
    ostringstream fn;
    write_file_prefix(a, fn);
    fn << "counters_init";
    feedback(fn.str());
    ofstream os(fn.str());
    a.warn_h(os);
    a.info(os);
    assert(!a.v.empty());
    auto f = a.begin();
    int n = 1;
    for (; f != a.end(); ++f) {
        os << "emplace(" << protocol_prefix << "" << a.v[n].first << ", new iodata());\n";
        ++n;
    }
    a.warn_f(os);
    include << "#include \"" << a.name << "/counters_init\"\n";
}

void c::gen_fcgi_index(const apifun& f, ostream& os) const {
    if (f.fcgi == "-") return;
    os << "os << url << \"/?<b>" << f.fcgi << "</b>\" << '\\n';" << '\n';
}

void c::gen_fcgi_index(const api_t& a, ostream& os) const {
    a.info(os);
    for (auto& i: a) {
        gen_fcgi_index(i, os);
    }
}

void c::gen_fcgi_index(const api_t&a) const {
    ostringstream fn;
    write_file_prefix(fn);
    fn << "links_" << a.name << "_fcgi_query_strings";
    string file = fn.str();
    feedback(file);
    ofstream os(file);
    a.warn_h(os);
    gen_fcgi_index(a, os);
    a.warn_f(os);
}

void c::gen_purevir(const apifun& f, bool side_caller, ostream& os) const {
    auto n_in = names_in(f, side_caller);
    auto n_out = names_out(f, side_caller);

    vector<string> args;
    if (!side_caller) {
        if (f.api->delegate && f.in.peerinfo) {
            args.push_back("peer_t&");
        }
        if (f.in.pass_dgram) {
            args.push_back("datagram*");
        }
        if (f.in.peerinfo) {
            args.push_back("seq_t");
        }
    }
    if (!f.in.empty()) {
        if (f.in.size() == 1) {
            if (side_caller) {
                args.push_back("const " + n_in.first + "&");
            }
            else {
                if (f.in.heap) {
                    args.push_back(n_in.first + "*");
                }
                else {
                    args.push_back(n_in.first + "&&");
                }
            }
        }
        else {
            args.push_back(n_in.first + "&&");
        }
    }
    if (f.is_sync()) {
        args.push_back(n_out.first + "&");
    }

    os << "  virtual ko " << fn_name(f, side_caller) << "(";
        for (auto& i: args) {
            os << i;
            if (&i != &*args.rbegin()) os << ", ";
        }
    os << ") = 0;\n";

    //_response for sync functions
    if (f.is_sync() && f.out.async_handler) {
        auto n_out = names_out(f, !side_caller);
        vector<string> args;
        if (side_caller) {
            if (f.api->delegate && f.out.peerinfo) {
                args.push_back("peer_t&");
            }
            if (f.out.pass_dgram) {
                args.push_back("datagram*");
            }
            if (f.out.peerinfo) {
                args.push_back("seq_t");
            }
        }
        if (!f.out.empty()) {
            if (f.out.size() == 1) {
                if (side_caller) {
                    args.push_back("const " + n_out.first + "&");
                }
                else {
                    if (f.out.heap) {
                        args.push_back(n_out.first + "*");
                    }
                    else {
                        args.push_back(n_out.first + "&&");
                    }
                }
            }
            else {
                args.push_back(n_out.first + "&&");
            }
        }
        os << "  virtual ko " << fn_name(f, side_caller) << "_response(";
            for (auto& i: args) {
                os << i;
                if (&i != &*args.rbegin()) os << ", ";
            }
        os << ") = 0;\n";
    }
}

void c::gen_override(const apifun& f, bool side_caller, ostream& os) const {
    auto n_in = names_in(f, side_caller);
    auto n_out = names_out(f, side_caller);
    vector<string> args;
    if (!side_caller) {
        if (f.api->delegate && f.in.peerinfo) {
            args.push_back("peer_t&");
        }
        if (f.in.pass_dgram) {
            args.push_back("datagram*");
        }
        if (f.in.peerinfo) {
            args.push_back("seq_t");
        }
    }
    if (!f.in.empty()) {
        if (f.in.size() == 1) {
            if (side_caller) {
                args.push_back("const " + n_in.first + "&");
            }
            else {
                if (f.in.heap) {
                    args.push_back(n_in.first + "*");
                }
                else {
                    args.push_back(n_in.first + "&&");
                }
            }
        }
        else {
            args.push_back(n_in.first + "&&");
        }
    }
    if (f.is_sync()) {
        args.push_back(n_out.first + "&");
    }
    os << "ko " << fn_name(f, side_caller) << "(";
        for (auto& i: args) {
            os << i;
            if (&i != &*args.rbegin()) os << ", ";
        }
    os << ") override;\n";

    //_response for sync functions
    if (f.is_sync() && f.out.async_handler) {
        auto n_out = names_out(f, !side_caller);
        vector<string> args;
        if (side_caller) {
            if (f.api->delegate && f.out.peerinfo) {
                args.push_back("peer_t&");
            }
            if (f.out.pass_dgram) {
                args.push_back("datagram*");
            }
            if (f.out.peerinfo) {
                args.push_back("seq_t");
            }
        }
        if (!f.out.empty()) {
            if (f.out.size() == 1) {
                if (side_caller) {
                    args.push_back("const " + n_out.first + "&");
                }
                else {
                    if (f.out.heap) {
                        args.push_back(n_out.first + "*");
                    }
                    else {
                        args.push_back(n_out.first + "&&");
                    }
                }
            }
            else {
                args.push_back(n_out.first + "&&");
            }
        }
        os << "ko " << fn_name(f, side_caller) << "_response(";
            for (auto& i: args) {
                os << i;
                if (&i != &*args.rbegin()) os << ", ";
            }
        os << ") override;\n";
    }
}

void c::gen_impl(const apifun& f, bool side_caller, function<void(const apifun& f, const string& pfx, bool side_caller, ostream&)> implementation, ostream& os) const {
    auto n_in = names_in(f, side_caller);
    auto n_out = names_out(f, side_caller);
    vector<string> args;
    if (!side_caller) {
        if (f.api->delegate && f.in.peerinfo) {
            args.push_back("peer_t& peer");
        }
        if (f.in.pass_dgram) {
            args.push_back("datagram* dgram");
        }
        if (f.in.peerinfo) {
            args.push_back("seq_t seq");
        }
    }
    if (!f.in.empty()) {
        if (f.in.size() == 1) {
            if (side_caller) {
                args.push_back("const " + n_in.first + "& " + n_in.second);
            }
            else {
                if (f.in.heap) {
                    args.push_back(n_in.first + "* " + n_in.second);
                }
                else {
                    args.push_back(n_in.first + "&& " + n_in.second);
                }
            }
        }
        else {
            args.push_back(n_in.first + "&& " + n_in.second);
        }
    }
    if (!f.out.empty()) {
        args.push_back(n_out.first + "& " + n_out.second);
    }

    os << "ko c::" << fn_name(f, side_caller) << "(";
        for (auto& i: args) {
            os << i;
            if (&i != &*args.rbegin()) os << ", ";
        }
        os << ") {\n";


    string pfx = "    ";
    string in_attrs;
    if (f.in.size() == 1) {
        //no doc
    }
    else {
            ostringstream os;
            for (auto& i: f.in) {
                os << pfx << "///     " << get_type(i.first) << " " << i.second << ";\n";
            }
            in_attrs = os.str();
    }
    string out_attrs;
    if (f.out.size() == 1) {
        //no doc
    }
    else {
        ostringstream os;
        for (auto& i: f.out) {
            os << pfx << "///    " << get_type(i.first) << " " << i.second << ";\n";
        }
        out_attrs = os.str();
    }
    os << pfx << "log(\"" << f.name << "\"";
    if (!side_caller) {
        if (f.in.pass_dgram) {
            os << ", \"svc\", dgram->service, \"sz\", dgram->size()";
        }
        if (f.in.peerinfo) {
            os << ", seq";
        }
    }
    os << ");\n";

    if (!in_attrs.empty()) {
        os << pfx << "/// in:\n" << in_attrs << '\n';
    }
    if (!out_attrs.empty()) {
        os << pfx << "/// out:\n" << out_attrs << '\n';
    }
    implementation(f, pfx, side_caller, os);
    os << "}\n";
}

void c::gen_impl_response(const apifun& f, bool side_caller, function<void(const apifun& f, const string& pfx, bool side_caller, ostream&)> implementation, ostream& os) const {
    auto n_out = names_out(f, !side_caller);
    vector<string> args;
    if (side_caller) {
        if (f.api->delegate && f.out.peerinfo) {
            args.push_back("peer_t& peer");
        }
        if (f.out.pass_dgram) {
            args.push_back("datagram* dgram");
        }
        if (f.out.peerinfo) {
            args.push_back("seq_t seq");
        }
    }
    if (!f.out.empty()) {
        if (f.out.size() == 1) {
            if (side_caller) {
                args.push_back("const " + n_out.first + "& " + n_out.second);
            }
            else {
                if (f.in.heap) {
                    args.push_back(n_out.first + "* " + n_out.second);
                }
                else {
                    args.push_back(n_out.first + "&& " + n_out.second);
                }
            }
        }
        else {
            args.push_back(n_out.first + "&& " + n_out.second);
        }
    }

    os << "ko c::" << fn_name(f, side_caller) << "_response(";
        for (auto& i: args) {
            os << i;
            if (&i != &*args.rbegin()) os << ", ";
        }
        os << ") {\n";

    string pfx = "    ";

    string out_attrs;
    if (f.out.size() == 1) {
        //no doc
    }
    else {
        ostringstream os;
        for (auto& i: f.out) {
            os << pfx << "///    " << get_type(i.first) << " " << i.second << ";\n";
        }
        out_attrs = os.str();
    }
    os << pfx << "log(\"" << f.name << "_response\"";
    if (f.out.pass_dgram) {
        os << ", \"svc\", dgram->service, \"sz\", dgram->size()";
    }
    if (f.out.peerinfo) {
        os << ", seq";
    }
    os << ");\n";

    if (!out_attrs.empty()) {
        os << pfx << "/// ans:\n" << out_attrs << '\n';
    }
    implementation(f, pfx, side_caller, os);
    os << "}\n";
}

bool c::gen_rpc_impl(const apifun& f, bool side_caller, ostream& os) const {
    gen_impl(f, side_caller, [&](const apifun& f, const string& pfx, bool side_caller, ostream& os) {
        auto n_in = names_in(f, side_caller);
        auto n_out = names_out(f, side_caller);
        if (f.is_sync()) {
            os << pfx << "datagram* d_in;\n";
            os << pfx << "{\n";
            if (f.in.empty()) {
                os << pfx << "    auto r = sendrecv(blob_writer_t::get_datagram(daemon.channel, protocol::" << f.service << ", 0), lasterror);\n";
            }
            else {
                if (f.in.size() == 1) {
                    os << pfx << "    auto r = sendrecv(blob_writer_t::get_datagram(daemon.channel, protocol::" << f.service << ", 0, " << n_in.second << "), lasterror);\n";
                }
                else {
                    os << pfx << "    auto r = sendrecv(" << n_in.second << ".get_datagram(daemon.channel, 0), lasterror);\n";
                }
            }
            os << pfx << "    if (unlikely(is_ko(r.first))) {\n";
            os << pfx << "        assert(r.second == nullptr);\n";
            os << pfx << "        return r.first;\n";
            os << pfx << "    }\n";
            os << pfx << "    assert(r.second != nullptr);\n";
            os << pfx << "    d_in = r.second;\n";
            os << pfx << "}\n";
            os << pfx << "if (unlikely(d_in->service != protocol::" << f.service << "_response)) {\n";
            os << pfx << "    auto r = KO_50143;\n";
            os << pfx << "    log(r, \"expected\", protocol::" << f.service << "_response, \"got\", d_in->service);\n";
            os << pfx << "    delete d_in;\n";
            os << pfx << "    return r;\n";
            os << pfx << "}\n";
            os << pfx << "auto r = blob_reader_t::readD(*d_in, " << n_out.second << ");\n";
            os << pfx << "delete d_in;\n";
            os << pfx << "return r;\n";
        }
        else {
            string seq = "0";
            if (!side_caller && f.in.peerinfo) {
                seq = "seq";
            }
            if (f.in.empty()) {
                os << pfx << "return send1(blob_writer_t::get_datagram(daemon.channel, protocol::" << f.service << ", " << seq << "));\n";
            }
            else {
                if (f.in.size() == 1) {
                    os << pfx << "return send1(blob_writer_t::get_datagram(daemon.channel, protocol::" << f.service << ", " << seq << ", " << n_in.second << "));\n";
                }
                else {
                    os << pfx << "return send1(" << n_in.second << ".get_datagram(daemon.channel, " << seq << "));\n";
                }
            }
        }
    }, os);

    if (!f.out.async_handler) return true;
    if (!f.is_sync()) return true;

    os << '\n';
    gen_impl_response(f, side_caller, [&](const apifun& f, const string& pfx, bool side_caller, ostream& os) {
        auto n_out = names_out(f, side_caller);
        string seq = "0";
        if (side_caller && f.out.peerinfo) {
            seq = "seq";
        }
        if (f.out.empty()) {
            os << pfx << "return send1(blob_writer_t::get_datagram(daemon.channel, protocol::" << f.service << "_response, " << seq << "));\n";
        }
        else {
            if (f.out.size() == 1) {
                os << pfx << "return send1(blob_writer_t::get_datagram(daemon.channel, protocol::" << f.service << "_response, " << seq << ", " << n_out.second << "));\n";
            }
            else {
                os << pfx << "return send1(o_ans.get_datagram(daemon.channel, " << seq << "));\n";
            }
        }
    }, os);

    return true;
}

bool c::gen_local_impl(const apifun& f, ostream& os) const {
    gen_impl(f, false, [&](const apifun& f, const string& pfx, bool side_caller, ostream& os) {
        os << pfx << "// Implement here.\n";
        os << pfx << "\n";
        auto n_in = names_in(f, side_caller);
        if (f.in.heap) {
            os << pfx << "delete " << n_in.second << ";\n";
        }
        os << pfx << "return KO_10210;\n";
    }, os);

    if (!f.out.async_handler) return true;
    if (!f.is_sync()) return true;

    os << '\n';
    gen_impl_response(f, false, [](const apifun& f, const string& pfx, bool side_caller, ostream& os) {
        os << pfx << "\n";
        os << pfx << "return KO_20179; //Ignore\n";
    }, os);
    return true;
}

void c::gen_service_router(const apifun& f, bool side_caller, ostream& os) const {
    os << "    case " << f.service << ": { return process_" << f.sync_type << "_api__" << f.service << "(d); }\n";
    if (f.is_sync() && f.out.async_handler) {
        os << "    case " << f.service << "_response: { return process_" << f.sync_type << "_api__" << f.service << "_response(d); }\n";
    }
}

void c::gen_dto_from_blob(const apifun& f, const apifun::io_types_t& iotypes, ostream& os) const {
    string pfx = "    ";
    string pfxb = "        ";
    os << pfx << "ko from_blob(blob_reader_t& reader) override {\n";
    for (auto& i: iotypes) {
        os << pfxb << "{\n";
        os << pfxb << "    auto r = reader.read(" << i.second << ");\n";
        os << pfxb << "    if (unlikely(is_ko(r))) return r;\n";
        os << pfxb << "}\n";
    }
    os << pfxb << "return ok;\n";
    os << pfx << "}\n";
    os << "\n";
}

void c::gen_dto_in_constructor_args(const apifun& f, bool names, ostream& os) const {
    const apifun::io_types_t& iotypes = f.in;
    for (auto& i: iotypes) {
        os << "const " << get_type(i.first) << "&";
        if (names) {
            os << " " << i.second;
        }
        if (&i != &*iotypes.rbegin()) {
            os << ", ";
        }
    }
}

void c::gen_dto_in_constructor(const apifun& f, ostream& os) const {
    string pfx = "    ";
    string pfxb = "        ";
    const apifun::io_types_t& iotypes = f.in;
    os << pfx << f.name << "_in_t(";
    gen_dto_in_constructor_args(f, true, os);
    os << "): ";
    for (auto& i: iotypes) {
        os << i.second << "(" << i.second << ")";
        if (&i != &*iotypes.rbegin()) {
            os << ", ";
        }
    }
    os << " {\n";
    os << pfx << "}\n";
    os << "\n";
}

void c::gen_dto_blob_size(const apifun& f, const apifun::io_types_t& iotypes, ostream& os) const {
    string pfx = "    ";
    string pfxb = "        ";
    os << pfx << "size_t blob_size() const override {\n";
    os << pfxb << "return ";
    for (auto& i: iotypes) {
        if (&i != &*iotypes.begin()) {
            os << pfxb << "    ";
        }
        os << "blob_writer_t::blob_size(" << i.second << ")";
        if (&i != &*iotypes.rbegin()) {
            os << " +\n";
        }
        else {
            os << ";\n";
        }
    }
    os << pfx << "}\n";
    os << "\n";
}

void c::gen_dto_to_blob(const apifun& f, const apifun::io_types_t& iotypes, ostream& os) const {
    string pfx = "    ";
    string pfxb = "        ";
    os << pfx << "void to_blob(blob_writer_t& writer) const override {\n";
    for (auto& i: iotypes) {
        os << pfxb << "writer.write(" << i.second << ");\n";
    }
    os << pfx << "}\n";
    os << "\n";
}

void c::gen_dto_get_datagram(const apifun& f, const apifun::io_types_t& iotypes, const string& side_sfx, ostream& os) const {
    string pfx = "    ";
    string pfxb = "        ";
    os << pfx << "datagram* get_datagram(channel_t channel, seq_t seq) const {\n";
    os << pfx << "  return b::get_datagram(channel, protocol::" << f.service << (side_sfx == "in" ? "" : "_response") << ", seq);\n";
    os << pfx << "}\n";
    os << "\n";

    os << pfx << "static datagram* get_datagram(channel_t channel, seq_t seq, ";
    for (auto& i: iotypes) {
        os << "const " << get_type(i.first) << "& " << i.second;
        if (&i != &*iotypes.rbegin()) {
            os << ", ";
        }
    }
    os << ") {\n";
    os << pfxb << f.name << "_" << side_sfx << "_t o(";
    for (auto& i: iotypes) {
        os << i.second;
        if (&i != &*iotypes.rbegin()) {
            os << ", ";
        }
    }
    os << ");\n";
    os << pfxb << "return o.get_datagram(channel, seq);\n";
    os << pfx << "}\n";
    os << "\n";
}

void c::gen_dto_in_dst_constructor(const apifun& f, ostream& os) const {
    string pfx = "    ";
    string pfxb = "        ";
    const apifun::io_types_t& iotypes = f.in;
    os << pfx << f.name << "_in_dst_t() {}\n";
    os << "\n";
}

void c::gen_dto_in_hdr(const apifun& f, bool side_caller, ostream& os) const {
    string pfx = "    ";
    const apifun::io_types_t& iotypes = f.in;
    if (iotypes.empty()) return;

    if (iotypes.size() == 1) {
        return;
    }

    if (side_caller) {
        os << "/// " << f.name << " - IN\n";

        os << "struct " << f.name << "_in_t final: blob_writer_t::writable {\n";
        os << pfx << "using b = blob_writer_t::writable;\n";
        os << "\n";

        gen_dto_in_constructor(f, os);
        gen_dto_blob_size(f, iotypes, os);
        gen_dto_to_blob(f, iotypes, os);
        gen_dto_get_datagram(f, iotypes, "in", os);

        //-----------------------------attributes
        for (auto& i: iotypes) {
            os << pfx << "const " << get_type(i.first) << "& " << i.second << ";\n";
        }
        //-----------------------------

        os << "};\n";
        os << "\n";
    }
    else {
        os << "struct " << f.name << "_in_dst_t final: blob_reader_t::readable {\n";
        os << pfx << "using b = blob_reader_t::readable;\n";
        os << "\n";

        gen_dto_in_dst_constructor(f, os);
        os << '\n';
        gen_dto_from_blob(f, iotypes, os);

        //-----------------------------attributes
        for (auto& i: iotypes) {
            os << pfx << get_type(i.first) << " " << i.second << ";\n";
        }
        os << "};\n";
        os << "\n";
    }
}

void c::gen_dto_out_constructor(const apifun& f, ostream& os) const {
    string pfx = "    ";
    string pfxb = "        ";
    const apifun::io_types_t& iotypes = f.out;
    os << pfx << f.name << "_out_t() {}\n";
    os << pfx << f.name << "_out_t(";
    for (auto& i: iotypes) {
        os << "const " << get_type(i.first) << "& " << i.second;
        if (&i != &*iotypes.rbegin()) {
            os << ", ";
        }
    }
    os << "): ";
    for (auto& i: iotypes) {
        os << i.second << "(" << i.second << ")";
        if (&i != &*iotypes.rbegin()) {
            os << ", ";
        }
    }
    os << " {}\n";
    os << "\n";
}

void c::gen_dto_out_dst_constructor(const apifun& f, ostream& os) const {
    string pfx = "    ";
    string pfxb = "        ";
    const apifun::io_types_t& iotypes = f.out;
    os << pfx << f.name << "_out_dst_t() {}\n";
    os << "\n";
}

void c::gen_dto_out_hdr(const apifun& f, bool side_caller, ostream& os) const {
    string pfx = "    ";
    string pfxb = "        ";
    const apifun::io_types_t& iotypes = f.out;
    if (iotypes.empty()) return;

    if (iotypes.size() == 1) {
        return;
    }

    if (!side_caller) {
        os << "/// " << f.name << " - OUT\n";

        os << "struct " << f.name << "_out_t final: blob_writer_t::writable {\n";
        os << pfx << "using b = blob_writer_t::writable;\n";
        os << "\n";

        gen_dto_out_constructor(f, os);
        gen_dto_blob_size(f, iotypes, os);
        gen_dto_to_blob(f, iotypes, os);
        gen_dto_get_datagram(f, iotypes, "out", os);

        //-----------------------------attributes
        for (auto& i: iotypes) {
            os << pfx << get_type(i.first) << ' ' << i.second << ";\n";
        }
        //-----------------------------

        os << "};\n";
        os << "\n";
    }
    else {
        os << "struct " << f.name << "_out_dst_t final: blob_reader_t::readable {\n";
        os << pfx << "using b = blob_reader_t::readable;\n";
        os << "\n";

        gen_dto_out_dst_constructor(f, os);
        gen_dto_from_blob(f, iotypes, os);

        //-----------------------------attributes
        for (auto& i: iotypes) {
            os << pfx << get_type(i.first) << ' ' << i.second << ";\n";
        }
        //-----------------------------
        os << "};\n";
        os << "\n";
    }
}

void c::gen_protocol(const api_t& a, int nbase, ostream& os) const {
    a.warn_h(os);
    a.info(os);
    assert(!a.v.empty());
    int n = 0;
    const string& base = a.v[0].first;
    os << "    static constexpr svc_t " << protocol_prefix << base << "{" << nbase << "};\n";
    ++n;
    auto f = a.begin();
    int s = 0;
    for ( ; f != a.end(); ++n, ++f) {
        os << "    static constexpr svc_t " << protocol_prefix << a.v[n].first << "{" << protocol_prefix << base << " + " << s << "}; /" << "/ svc " << (nbase + s) << '\n';
        ++s;
        if (a.v[n].second) {
            os << "    static constexpr svc_t " << protocol_prefix << a.v[n].first << "_response{" << protocol_prefix << base << " + " << s << "}; /" << "/ svc " << (nbase + s) << '\n';
            ++s;
        }
        os << '\n';
    }
    os << "    static constexpr svc_t " << protocol_prefix << a.v[n].first << "{" << protocol_prefix << base << " + " << s << "}; /" << "/ svc " << (nbase + s) << '\n';
    os << '\n';
    a.warn_f(os);
}

bool c::gen_service_handlers(const apifun& f, const string& scope, bool side_caller, ostream& os) const {
    string dop = "->";
    auto n_in = names_in(f, false);
    auto n_out = names_out(f, false);

    os << "bool c::process_" << f.sync_type << "_api__" << f.service << "(datagram* d) {\n";
    string pfx = "    ";
    os << pfx << "log(\"protocol::" << f.service << "\");\n";
    string seq = "seq";
    string inarg;
    string type_in = n_in.first;
    string name_in = n_in.second;
    if (!f.in.empty()) {
        if (f.in.heap) {
            os << pfx << type_in << "* " << name_in << " = new " << type_in << "();\n";
        }
        else {
            os << pfx << type_in << " " << name_in << ";\n";
        }
        os << pfx << "{\n";
        os << pfx << "    auto r = blob_reader_t::readD(*d, " << (f.in.heap ? "*" : "") << name_in << ");\n";
        os << pfx << "    if (is_ko(r)) {\n";
        os << pfx << "        log(r);\n";
        os << pfx << "        delete d;\n";
        if (f.in.heap) {
            os << pfx << "        delete " << name_in << ";\n";
        }
        os << pfx << "        return true; //processed\n";
        os << pfx << "    }\n";
        os << pfx << "}\n";
        if (f.in.heap) {
            inarg = name_in;
        }
        else {
            inarg = "move(" + name_in + ")";
        }
    }
    string type_out = n_out.first;
    string name_out = n_out.second;
    if (f.is_sync()) {
        os << pfx << "auto seq = d->decode_sequence();\n";
        os << pfx << type_out << " " << name_out << ";\n";
    }
    vector<string> args;
    if (!side_caller) {
        if (f.api->delegate && f.in.peerinfo) {
            args.push_back("*this");
        }
        if (f.in.pass_dgram) {
            args.push_back("d");
        }
        if (f.in.peerinfo) {
            args.push_back("d->decode_sequence()");
        }
    }
    if (!inarg.empty()) {
        args.push_back(inarg);
    }
    if (f.is_sync()) {
        args.push_back(name_out);
    }
    os << pfx << "{\n";
    string obj = "";
    if (f.api->delegate) {
        obj = "delegate.";
    }
    os << pfx << "    auto r = " << obj << fn_name(f, side_caller) << "(";
        for (auto& i: args) {
            os << i;
            if (&i != &*args.rbegin()) os << ", ";
        }
        os << ");\n";
    os << pfx << "    if (unlikely(is_ko(r))) {\n";
    os << pfx << "        process_ko_work(d->decode_channel(), d->decode_sequence(), r);\n";
    os << pfx << "        delete d;\n";
    os << pfx << "        return true; //processed\n";
    os << pfx << "    }\n";
    os << pfx << "}\n";

    if (f.is_sync()) {
        if (f.out.size() == 1) {
            os << pfx << "datagram* dout = blob_writer_t::get_datagram(daemon.channel, protocol::" << f.service << "_response, seq, " << name_out << ");\n";
        }
        else {
            os << pfx << "datagram* dout = " << name_out << ".get_datagram(daemon.channel, seq);\n";
        }
        if (!f.in.pass_dgram) {
            os << pfx << "delete d;\n";
        }
        os << pfx << "log(\"processing response svc = \", dout->service);\n";
        os << pfx << "process_ok_work(dout);\n";
    }
    else {
        if (!f.in.pass_dgram) {
            os << pfx << "delete d;\n";
        }
    }
    os << pfx << "return true; //processed\n";
    os << "}\n";
    return true;
}

bool c::gen_service_handlers_response(const apifun& f, const string& scope, bool side_caller, ostream& os) const {
    if (!f.is_sync()) return false;
    if (f.out.empty()) return false;
    string pfx = "    ";
    string dop = "->";
    auto n_out = names_out(f, true);
    os << "bool c::process_" << f.sync_type << "_api__" << f.service << "_response(datagram* d) {\n";
    string outarg;
    string type_out = n_out.first;
    string name_out = n_out.second;
    if (f.out.heap) {
        os << pfx << type_out << "* " << name_out << " = new " << type_out << "();\n";
    }
    else {
        os << pfx << type_out << " " << name_out << ";\n";
    }
    os << pfx << "{\n";
    os << pfx << "    auto r = blob_reader_t::readD(*d, " << (f.out.heap ? "*" : "") << name_out << ");\n";
    os << pfx << "    if (unlikely(is_ko(r))) {\n";
    os << pfx << "        log(r);\n";
    os << pfx << "        delete d;\n";
    os << pfx << "        return true; //processed\n";
    os << pfx << "    }\n";
    os << pfx << "}\n";
    if (f.out.heap) {
        outarg = name_out;
    }
    else {
        outarg = "move(" + name_out + ")";
    }
    vector<string> args;
    if (side_caller) {
        if (f.api->delegate && f.out.peerinfo) {
            args.push_back("*this");
        }
        if (f.out.pass_dgram) {
            args.push_back("d");
        }
        if (f.out.peerinfo) {
            args.push_back("seq");
        }
    }
    if (!outarg.empty()) {
        args.push_back(outarg);
    }

    os << pfx << "{\n";
    string obj = "";
    if (f.api->delegate) {
        obj = "delegate.";
    }
    os << pfx << "    auto r = " << obj << fn_name(f, side_caller) << "_response(";
        for (auto& i: args) {
            os << i;
            if (&i != &*args.rbegin()) os << ", ";
        }
        os << ");\n";
    os << pfx << "    if (unlikely(is_ko(r))) {\n";
    os << pfx << "        auto seq = d->decode_sequence();\n";
    os << pfx << "        auto chan = d->decode_channel();\n";
    os << pfx << "        delete d;\n";
    os << pfx << "        if (r == KO_20179) { // Ignore\n";
    os << pfx << "            return false;\n";
    os << pfx << "        }\n";
    os << pfx << "        process_ko_work(chan, seq, r);\n";
    os << pfx << "        return true; //processed\n";
    os << pfx << "    }\n";
    os << pfx << "}\n";
    if (!f.out.pass_dgram) {
        os << pfx << "delete d;\n";
    }
    os << pfx << "return true; //processed\n";
    os << "}\n";
    return true;
}

void c::gen_service_handler_headers(const api_t& a, const string& scope, bool side_caller, ostream& os) const {
    a.info(os);
    for (auto& f: a) {
        os << "bool process_" << f.sync_type << "_api__" << f.service << "(datagram*);\n";
        if (f.is_sync() && f.out.async_handler) {
            os << "bool process_" << f.sync_type << "_api__" << f.service << "_response(datagram*);\n";
        }
    }
}

void c::gen_service_handler_headers(const api_t&a, const string& scope) const {
    bool side_caller = false;
    ostringstream fn;
    write_file_prefix(a, fn);
    sides_prefix(side_caller, fn);
    fn << "svc_handler-hdr";
    string file = fn.str();
    feedback(file);
    ofstream os(file);
    a.warn_h(os);
    gen_service_handler_headers(a, scope, side_caller, os);
    a.warn_f(os);
}


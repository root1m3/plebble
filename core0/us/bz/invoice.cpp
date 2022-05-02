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
#include "invoice.h"
#include <us/gov/crypto/base58.h>
#include <us/gov/io/cfg0.h>
#include <fstream>

using c = us::bz::invoice;
using namespace std;
using namespace us::bz;

size_t c::base_id{16670};
size_t c::prev_id{0};
string c::homedir;

bool c::init(const string& homedir0) {
        homedir=homedir0+"/invoice";
        string f=homedir+"/lastiid";
        ifstream is(f);
        is >> prev_id;
        if (is.fail()) {
            cerr << f << " cannot be read" << endl;
            cerr << "suggestion: echo '0' > " << f << endl;
            return false;
        }
        return true;
}

c::invoice() {
}

string c::close() {
        if (closed) return "already closed";
        ostringstream os;
        if (issue_date.empty()) os << "invalid issue_date" << endl;
        if (due_date.empty()) os << "invalid due_date" << endl;
        if (client_ref.empty()) os << "invalid client_ref" << endl;
        if (concept0.empty()) os << "invalid concept" << endl;
        if (subtotal==0) os << "invalid subtotal" << endl;
        if (total_vat==0) os << "invalid total_vat" << endl;
        string err=os.str();
        if (err.empty()) {
            total=subtotal+total_vat;
            id=base_id+(++prev_id);
            {
            string f=homedir+"/lastiid";
            ofstream os(f);
            os << prev_id;
            }
            closed=true;
            return "";
        }
        return err;
    }

    void c::to_stream(ostream&os) const {
        os << id << ' ';
        os << us::gov::crypto::b58::encode(issue_date) << ' ';
        os << us::gov::crypto::b58::encode(due_date) << ' ';
        os << us::gov::crypto::b58::encode(client_ref) << ' ';
        os << us::gov::crypto::b58::encode(concept0) << ' ';
        os << subtotal << ' ';
        os << total_vat << ' ';
        os << closed << ' ';
    }

    string c::from_stream(istream&is) {
        is >> id;
        is >> issue_date;
        is >> due_date;
        is >> client_ref;
        is >> concept0;
        is >> subtotal;
        is >> total_vat;
        is >> closed;
        issue_date=us::gov::crypto::b58::decode_string(issue_date);
        due_date=us::gov::crypto::b58::decode_string(due_date);
        client_ref=us::gov::crypto::b58::decode_string(client_ref);
        concept0=us::gov::crypto::b58::decode_string(concept0);
        if (is.fail()) return "unable to parse";
        return "";
    }

namespace {

    template<typename T>
    string to_string(const T& nd, bool fiat) {
        ostringstream os;
        if (fiat) os << "£ ";
        os << nd;
        return os.str();
    }

}

    void c::replace_token(string& s, const string& token, const string& value) const {
        int i=0;
        while(true) {
            i=s.find(token,i);
            if (i==string::npos) break;
            string r=s.substr(0,i);
            r+=value;
            r+=s.substr(i+token.size());
            s=r;
        }

    }

    bool c::generate_pdf() const {
        if (!generate_svg()) return false;
        ostringstream svgfile;
            ostringstream cmd;
            cmd << "rsvg-convert -f pdf -o " << homedir << "/outbox/invoice_" << bzname << "_" << (closed?id:-1) << "_" << due_date << ".pdf" << " " << homedir << "/outbox/invoice_" << bzname << "_" << (closed?id:-1) << "_" << due_date << ".svg";
          system(cmd.str().c_str());
        return true;
    }

    bool c::generate_svg() const {
        string templfile=homedir+"/template.svg";
            vector<unsigned char> content;
            auto e = us::gov::io::read_file_(templfile, content);
            if (is_ko(e)) {
                cerr << e << ". Cannot read invoice template " << templfile << endl;
                return false;
            }
          string cont(content.begin(), content.end() );

          if (closed)
              replace_token(cont, "<:ID:>",to_string(id));
          else
              replace_token(cont, "<:ID:>","--INVALID--");

          replace_token(cont, "<:ISSUE_DATE:>", issue_date);
          replace_token(cont, "<:DUE_DATE:>", due_date);
          replace_token(cont, "<:CLIENT_REF:>", client_ref);
          replace_token(cont, "<:CLI_ADDR1:>", cliaddr1);
          replace_token(cont, "<:CLI_ADDR2:>", cliaddr2);
          replace_token(cont, "<:CLI_ADDR3:>", cliaddr3);
          replace_token(cont, "<:CONCEPT:>", concept0);
          replace_token(cont, "<:SUBTOTAL:>", to_string(subtotal,true));
          replace_token(cont, "<:TOTAL_VAT:>", to_string(total_vat,true));
          if (closed)
              replace_token(cont, "<:TOTAL:>", to_string(total,true));
          else
              replace_token(cont, "<:TOTAL:>", "--INVALID--");

          {
            ostringstream fn;
            fn << homedir << "/outbox"; //invoice_" << issue_date << "_" << id << ".svg";
            system((string("mkdir -p ")+fn.str()).c_str());
            fn << "/invoice_" << bzname << "_" << (closed?id:-1) << "_" << due_date << ".svg";
           ofstream os(fn.str());
           os << cont;
           if (os.fail()) {
            cerr << "error writing file " << fn.str() << endl;
            return false;
            }
         }
          return true;
    }

    void c::dump(ostream&os) const {
    }

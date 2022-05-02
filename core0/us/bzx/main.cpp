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
#include <string>
#include <chrono>
#include <thread>
#include <us/gov/config.h>
#include <us/gov/vcs.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/io/shell_args.h>
#include <us/gov/io/cfg.h>
#include <us/bz/invoice.h>
#include <us/wallet/protocol.h>


#define loglevel "bz"
#define logclass "main"
#include <us/gov/logs.inc>

using namespace us::bz;
using namespace std;
using us::gov::io::shell_args;

using cash_t = uint64_t;

struct params {
    params() {
    }

    static string get_bzname() {
            const char* env_p = std::getenv("BZNAME");
            if (!env_p) {
                cerr << "No $BZNAME env var defined. source bzdata" << endl;
                exit(1);
            }
            return env_p;
    }

    static string get_cliname() {
            const char* env_p = std::getenv("CLINAME");
            if (!env_p) {
                cerr << "No $CLINAME env var defined. source clidata" << endl;
                exit(1);
            }
            return env_p;
    }

    static string get_cliaddr1() {
            const char* env_p = std::getenv("CLIADDR1");
            if (!env_p) {
                cerr << "No $CLIADDR1 env var defined. source clidata" << endl;
                exit(1);
            }
            return env_p;
    }
    static string get_cliaddr2() {
            const char* env_p = std::getenv("CLIADDR2");
            if (!env_p) return "";
            return env_p;
    }
    static string get_cliaddr3() {
            const char* env_p = std::getenv("CLIADDR3");
            if (!env_p) return "";
            return env_p;
    }

    string get_home() const {
        ostringstream os;
        if (homedir.empty()) {
            const char* env_p = std::getenv("HOME");
            if (!env_p) {
                cerr << "No $HOME env var defined" << endl;
                exit(1);
            }
            os << env_p << "/.us";
//suspicious TODO homedir should be set here?
        }
        else {
            os << homedir;
        }
        if (channel!=0) {
            os << '/' << channel;
        }
        return os.str();
    }

    void dump(ostream& os) const {
        os << "channel: " << channel << endl;
        os << "home: " << get_home() << endl;
        os << "wallet: " << walletd_host << ":" << walletd_port << endl;
    }

    uint16_t channel{CFG_CHANNEL};
    string homedir;
    bool help{false};
    string walletd_host{"localhost"}; uint16_t walletd_port{CFG_WALLET_PORT};
    bool offline{false};
};

void help(const params& p, ostream& os=cout) {
    os << "us-bz. Channel " << p.channel << '\n';

    os << endl;
    os << "Usage: us-bz [options] [command]" << endl;
    os << "options are:" << endl;
        os << " -home <homedir>   homedir. [" << p.get_home() << "]" << endl;

        os << " -c <channel>  System channel. " << p.channel << " (" << (p.channel==0?"public":"private") << ')' << endl;
        os << " RPC to wallet-daemon (us-wallet) parameters: (conflicts with -local)" << endl;
        os << "   -whost <address>  walletd address. [" << p.walletd_host << "]" << endl;
        os << "   -wp <port>  walletd port. [" << p.walletd_port << "]" << endl;
        os << " -version   Print version and licence." << endl;
    os << endl;
    os << "commands are:" << endl;
    os << "  bill <invoices_file>" << endl;
    os << "  vat period" << endl;
/*
    os << "cash:" << endl;
    os << "  balance [detailed=0|1]                                Displays the spendable amount." << endl;
    os << "  transfer <dest account> <receive amount> <token>      Order a cash transfer to <dest account>. Fees are paid by you, the sender." << endl;
        os << "  tx make_p2pkh <dest account> <amount> <fee> <token> <sigcode_inputs=all> <sigcode_outputs=all> [<send>]" << endl;
        os << "    sigcodes are: "; cash::tx::dump_sigcodes(cout); cout << endl;
        os << "  tx decode <tx_b58>" << endl;
        os << "  tx check <tx_b58>" << endl;
        os << "  tx send <tx_b58>" << endl;
        //os << "  tx sign <tx_b58> <sigcode_inputs> <sigcode_outputs>" << endl;  //hidden fuctionality
    os << endl;
    os << "keys:";
    os << endl;
    os << "  address new            Generates a new key-pair, adds the private key to the wallet and prints its asociated address." << endl;
        os << "  address add <privkey>  Imports a given private key in the wallet" << endl;
    os << "  list [show_priv_keys=0|1]   Lists all keys. Default value for show_priv_keys is 0." << endl;
        os << "  gen_keys                  Generates a key pair without adding them to the wallet. (requires -local)" << endl;
        os << "  mine_public_key  <string> Finds a private key where its corrersponding public key contains the specified string. It will take from long to ages. (requires -local)" << endl;
        os << "  priv_key <private key>    Gives information about the given private key. (requires -local)" << endl;
        os << "  pub_key <public key>    Gives information about the given public key. (requires -local)" << endl;
        os << "  digest <filename>         Computes RIPEMD160+base58 to the content of the file. (requires -local)" << endl;
    os << endl;
      os << "device pairing:" << endl;
      os << "  device_id              Show this device Id. (Only RPC mode)" << endl;
      os << "  pair <pubkey> <name>   Authorize the device identified by its public key to operate the wallet. Give it a name." << endl;
      os << "  unpair <pubkey>        Revoke authorization to the specified device." << endl;
      os << "  list_devices           Prints the list of recognized devices." << endl;
      os << endl;
      os << "Storage:" << endl;
      os << "  key-value:" << endl;
      os << "    store <address> -kv <key> <value> " << endl;
      os << "        Store key-value pair in a funded account. cost: 1 per cycle." << endl;
      os << "    rm <address> -kv <key>" << endl;
      os << "        Removes the key-value pair." << endl;
      os << "    search <address> <word1|word2...>" << endl;
      os << "        Search kv table" << endl;
      os << "  file:" << endl;
      os << "    store <address> -f <file> " << endl;
      os << "        Store file in a funded account. cost: 1 per kibibyte per cycle." << endl;
      os << "    rm <address> -f <hash>" << endl;
      os << "        Removes the stored file." << endl;
      os << "    file <hash>" << endl;
      os << "        Retrieve a file." << endl;
      os << endl;
      os << "system control:" << endl;
      os << "  patch_os <script file> <priv key>       System upgrade/maintenance. (requires: -local, grid-sysop key)" << endl;
      os << "  touch" << endl;
      os << endl;
      os << "net:" << endl;
      os << "  ping                   endpoint check" << endl;
      os << endl;
*/
    os << endl;
}

string parse_options(shell_args& args, params& p) {
    string cmd;
    while(true) {
        cmd=args.next<string>();
        if (cmd=="-wp") {
            p.walletd_port=args.next<int>();
        }
        else if (cmd=="-whost") {
            p.walletd_host=args.next<string>();
        }
        else if (cmd=="-home") {
            p.homedir=args.next<string>();
        }
        else if (cmd=="-c") {
            p.channel=args.next<uint16_t>();
        }
        else if (cmd=="-h") {
            p.help=true;
        }
        else if (cmd=="-version") {
            us::vcs::version(cout);
            cout << endl;
//            cout << "You can use this software under the terms of the Affero General Public License (AGPL)." << endl;
//            cout << "Obtain a copy of the licence here: https://www.gnu.org/licenses/agpl.txt" << endl;
            exit(0);
        }
        else {
            break;
        }
    }
    return cmd;
}

using us::gov::io::cfg;
using us::gov::io::cfg0;
using us::gov::io::cfg1;
using us::gov::io::cfg_id;

void bill(shell_args& args, const params& p, const string& concept0, const string& issue_date, const string& due_date, ostream& os);

void bill(shell_args& args, const params& p, const string& invoices, ostream& os) {
    ifstream is(invoices);
    while(is.good()) {
        string line;
        getline(is,line);
        if (line.empty()) continue;
        if (line[0]=='#') continue;
        istringstream isl(line);
        string issue_date;
        string due_date;
        issue_date=line;
        getline(is,line);
        due_date=line;
        getline(is,line);
        string content=line;
        bill(args,p,content, issue_date, due_date, os);

    }
}

void bill(shell_args& args, const params& p, const string& concept0, const string& issue_date, const string& due_date, ostream& os) {
    us::bz::invoice i;
    i.bzname=p.get_bzname();
    i.client_ref="N/A";
    i.cliaddr1=p.get_cliaddr1();
    i.cliaddr2=p.get_cliaddr2();
    i.cliaddr3=p.get_cliaddr3();
    i.issue_date=issue_date;
    i.due_date=due_date;
    i.subtotal=2500;
    i.total_vat=i.subtotal*0.20;
    i.concept0=concept0;
    i.close();
    i.generate_pdf();

}


struct vatline {
    int id;
    cash_t value;
    cash_t vat;
};

struct vat_info: vector<vatline> {
vat_info(const string& filename) {
    ifstream is(filename);
    while(is.good()) {
        string line;
        getline(is,line);
        if (line.empty()) continue;
        istringstream ln(line);
cout << line << endl;
        vatline vl;

        ln >> vl.id;

        double d;
        ln >> d;
        if (ln.fail()) {
            cerr << "fail 1" << endl;
            abort;
        }
        vl.value=(cash_t)(d*100.0);
        ln >> d;
        if (ln.fail()) {
            vl.vat=(cash_t)(((double)vl.value)*0.20);
            vl.value-=vl.vat;
            cout << "interpreted as << " << ((double)vl.value/100.00) << ' ' << ((double)vl.vat/100.00) << endl;
        }
        else {
            vl.vat=(cash_t)(d*100.0);
        }
        emplace_back(vl);
    }
}

    void submit_return(const vat_info& outputs, ostream& os) const {
        os << "---------------------------HMRC-----------------------" << endl;
        cash_t vat_due_outputs=0;
        for (auto&i:outputs) {
            vat_due_outputs+=i.vat;
        }
        os << "[Box 1] vat_due_outputs " << ((double)vat_due_outputs/100.0) << endl;
        cash_t vat_due_outputs_other_EC_states=0;
        os << "[Box 2] vat_due_outputs_other_EC_states " << ((double)vat_due_outputs_other_EC_states/100.0) << endl;

        cash_t total_due=vat_due_outputs+vat_due_outputs_other_EC_states;
        os << "total_due " << ((double)total_due/100.0) << endl;

        cash_t vat_reclaimed_inputs=0;
        for (auto&i:*this) {
            vat_reclaimed_inputs+=i.vat;
        }
        os << "[Box 4] vat_reclaimed_inputs " << ((double)vat_reclaimed_inputs/100.0) << endl;
        cash_t total_reclaimed=vat_reclaimed_inputs;
        os << "total_reclaimed " << ((double)total_reclaimed/100.0) << endl;
        cash_t net_vat=total_due-total_reclaimed;
        os << "[Box 5] net_vat " << ((double) net_vat/100.0) << endl;

        cash_t total_value_outputs=0; // (whole pounds)
        for (auto&i:outputs) {
            total_value_outputs+=i.value;
        }
        total_value_outputs/=100;

        os << "[Box 6] total_value_outputs " << total_value_outputs << endl;
        int total_value_inputs=0;
        for (auto&i:*this) {
            total_value_inputs+=i.value;
        }
        total_value_inputs/=100;
        os << "[Box 7] total_value_inputs " << total_value_inputs << endl;
        os << "[Box 8] 0 " << endl;
        os << "[Box 9] 0 " << endl;
        os << "-------------------------/-HMRC-----------------------" << endl;
    }
};

void vat(const string& homedir, shell_args& args, const params& p) {
    string period=args.next<string>();
    if (period.empty()) {
        ostringstream cmd;
        cmd << "ls -1 " << homedir;
        cout << "Periods at " << homedir << endl;
        system(cmd.str().c_str());
        return;
    }

    ostringstream file1;
    file1 << homedir << "/" << period << "/outputs/index";
    vat_info outs(file1.str());

    ostringstream file2;
    file2 << homedir << "/" << period << "/expenses";
    vat_info vatlines(file2.str());

    vatlines.submit_return(outs,cout);

}


bool run_local(string command, shell_args& args, const params& p) {
    bool ret=true;

    auto homedir=p.get_home()+"/bz";

    cfg0::load(homedir);
    if (!invoice::init(homedir)) {
        return false;
    }
    ostringstream os;
    if (command=="bill") {
        string invoices_file=args.next<string>();
        bill(args,p,invoices_file,os);
    }
    else if (command=="vat") {
        vat(homedir+"/vat",args,p);
    }
    else {
        cerr << "Invalid command " << command << endl;
        help(p);
        ret=false;
    }
    cout << os.str() << endl;
    return ret;
}

int main(int argc, char** argv) {
    shell_args args(argc,argv);
    params p;
    string command=parse_options(args,p);
    if (p.help) {
        help(p);
        return 0;
    }
    log_pstart(argv[0]);
    log_start("","main");

    if (!run_local(command, args, p)) return 1;
    return 0;
}


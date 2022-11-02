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
package us.wallet.cli;
import us.CFG;                                                                                 // CFG
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.id.types.*;                                                               // *
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import us.stdint.*;                                                                            // *
import java.io.PrintStream;                                                                    // PrintStream
import us.gov.io.shell_args;                                                                   // shell_args

public class params {

    public static void log(final String line) {     //--strip
        CFG.log_wallet_cli("params: " + line);      //--strip
    }                                               //--strip

    public enum output_mode {
        om_human,
        om_text,
        om_xml,
        om_json,

        num_modes;

        public short as_short() {
            return (short)ordinal();
        }

        public static output_mode from_short(short i) {
            return output_mode.values()[i];
        }
    }

    void constructor() {
        int hc = Runtime.getRuntime().availableProcessors();
        log("hardware concurrency " + hc);  //--strip
        workers = new uint16_t(2 * hc);
        downloads_dir = "/var/www/" + us.CFG.PLATFORM + "_html/downloads";
    }

    public params() {
        log("def constructor"); //--strip
        constructor();
        args = new shell_args("");
    }

    public params(shell_args args_) {
        log("args constructor"); //--strip
        constructor();
        args = args_;
        while (true) {
            String command = args.next_string();
            log("command: " + command); //--strip
            if (command.equals("-wp")) {
                walletd_port.value = args.next_uint16().value;
            }
            else if (command.equals("-lp")) {
                listening_port.value = args.next_uint16().value;
            }
            else if (command.equals("-pp")) {
                published_port.value = args.next_uint16().value;
            }
            else if (command.equals("-pin")) {
                pin.value = args.next_uint16().value;
            }
            else if (command.equals("-whost")) {
                walletd_host.value = args.next_string();
            }
            else if (command.equals("-bp")) {
                backend_port.value = args.next_uint16().value;
            }
            else if (command.equals("-bhost")) {
                backend_host.value = args.next_string();
            }
            else if (command.equals("-home")) {
                homedir = args.next_string();
            }
            else if (command.equals("-d")) {
                daemon = true;
            }
            else if (command.equals("-b")) {
                advanced = false;
            }
            else if (command.equals("-n")) {
                names = true;
            }
            else if (command.equals("-log")) { //--strip
                verbose = true; //--strip
                CFG.logs.set(verbose); //--strip
                log("enabled logs"); //--strip
            } //--strip
            else if (command.equals("-c")) {
                channel.value = args.next_uint16().value;
            }
            else if (command.equals("-v")) {
                max_devices = args.next_uint16();
            }
            else if (command.equals("-e")) {
                max_trade_edges = args.next_uint16();
            }
            else if (command.equals("-w")) {
                workers = args.next_uint16();
            }
            else if (command.equals("-om")) {
                uint16_t m=args.next_uint16();
                if (m.value<output_mode.num_modes.as_short()) om = output_mode.from_short((short)m.value);
            }
            else if (command.equals("-dd")) {
                downloads_dir = args.next_string();
            }
            else if (command.equals("--custodial_wallet_id")) {
                subhome = args.next_string();
            }
            else if (!command.isEmpty()) {
                cmd = command;
                break;
            }
            else {
                break;
            }
        }
    }

    public String get_home_wallet() {
        return get_home() + "/wallet";
    }

    public String get_home_wallet_rpc_client() {
        return get_home_wallet() + "/rpc_client";
    }

    public String get_home() {
        String os = new String();
        if (homedir.isEmpty()) {
            String env = System.getenv("HOME");
            if (env.isEmpty()) {
                System.err.println("No $HOME env var defined");
                System.exit(1);
            }
            os += env + "/" + CFG.DATADIR;
        }
        else {
            os += homedir;
        }
        if (channel.value != 0) {
            os += "/" + channel.value;
        }
        return os;
    }

    public void dump(final String pfx, PrintStream os) {
        os.println(pfx + "channel: " + channel.value);
        os.println(pfx + "home: " + get_home());
        if (CFG.DEBUG == 1) {
            os.println(pfx + "warning: this is a debug build.");
        }
        else {
            os.println(pfx + "this is an optimized build.");
        }

        String strip = "instrumentation code has been stripped.";
        strip = "warning: instrumentation code hasn't been stripped"; //--strip
        os.println(pfx + strip);
        os.println(pfx + "run rpc shell");
        os.println(pfx + "  wallet daemon at: " + walletd_host.value + ":" + walletd_port.value);
        os.println(pfx + "  connect_for_recv  " + rpc__connect_for_recv);
        os.println(pfx + "  stop_on_disconnection  " + rpc__stop_on_disconnection);
    }

    public void connecting_to(PrintStream os) {
        os.print(CFG.PLATFORM);
        os.print("-wallet daemon at " + walletd_host.value + ':' + walletd_port.value);
    }

    public channel_t channel = CFG.CHANNEL;
    public String homedir = new String("");
    public boolean daemon = false;
    public boolean advanced = true;
    public boolean verbose = false;
    public uint16_t max_trade_edges = new uint16_t(4);
    public uint16_t max_devices = new uint16_t(2);
    public uint16_t workers = new uint16_t(4);
    public String subhome = new String("");
    public String cmd = new String("");
    public shell_args args = null;
    public pin_t pin = new pin_t(0);
    public port_t listening_port = CFG.WALLET_PORT;
    public pport_t published_port = CFG.WALLET_PPORT;
    public shost_t walletd_host = new shost_t("localhost");
    public port_t walletd_port = CFG.WALLET_PORT;
    public shost_t backend_host = new shost_t("localhost");
    public port_t backend_port = CFG.PORT;
    public boolean rpc__connect_for_recv = true;
    public boolean rpc__stop_on_disconnection = true;
    public output_mode om = output_mode.om_human;
    public boolean names = false;
    public String downloads_dir = "";
    public boolean banner = true;
}


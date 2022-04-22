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
package us.gov;
import us.stdint.*;                                                                            // *

public class protocol {

    //#include <us/api/generated/java/gov/socket/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/socket/svc>
    // socket - master file: us/api/data/gov/socket

        public static final int socket_begin = 1;
        public static final int socket_error = socket_begin + 0;  // svc 1

        public static final int socket_ping = socket_begin + 1;  // svc 2
        public static final int socket_ping_response = socket_begin + 2;  // svc 3

        public static final int socket_finished = socket_begin + 3;  // svc 4


        public static final int socket_end = 5; // svc 5
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/id/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/id/svc>
    // id - master file: us/api/data/gov/id

        public static final int id_begin = 5;
        public static final int id_request = id_begin + 0;  // svc 5

        public static final int id_peer_challenge = id_begin + 1;  // svc 6

        public static final int id_challenge_response = id_begin + 2;  // svc 7


        public static final int id_end = 8; // svc 8
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/auth/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/auth/svc>
    // auth - master file: us/api/data/gov/auth

        public static final int auth_begin = 8;
        public static final int auth_f1 = auth_begin + 0;  // svc 8


        public static final int auth_end = 9; // svc 9
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/peer/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/peer/svc>
    // peer - master file: us/api/data/gov/peer

        public static final int peer_begin = 9;
        public static final int peer_f1 = peer_begin + 0;  // svc 9


        public static final int peer_end = 10; // svc 10
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/relay/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/relay/svc>
    // relay - master file: us/api/data/gov/relay

        public static final int relay_begin = 10;
        public static final int relay_push = relay_begin + 0;  // svc 10


        public static final int relay_end = 11; // svc 11
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/dfs/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/dfs/svc>
    // dfs - master file: us/api/data/gov/dfs

        public static final int dfs_begin = 11;
        public static final int dfs_file_request = dfs_begin + 0;  // svc 11
        public static final int dfs_file_request_response = dfs_begin + 1;  // svc 12


        public static final int dfs_end = 13; // svc 13
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/engine/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/engine/svc>
    // engine - master file: us/api/data/gov/engine

        public static final int engine_begin = 13;
        public static final int engine_local_deltas = engine_begin + 0;  // svc 13

        public static final int engine_vote_tip = engine_begin + 1;  // svc 14

        public static final int engine_query_block = engine_begin + 2;  // svc 15
        public static final int engine_query_block_response = engine_begin + 3;  // svc 16

        public static final int engine_sysop = engine_begin + 4;  // svc 17
        public static final int engine_sysop_response = engine_begin + 5;  // svc 18

        public static final int engine_track = engine_begin + 6;  // svc 19
        public static final int engine_track_response = engine_begin + 7;  // svc 20

        public static final int engine_ev = engine_begin + 8;  // svc 21

        public static final int engine_ev_track = engine_begin + 9;  // svc 22
        public static final int engine_ev_track_response = engine_begin + 10;  // svc 23

        public static final int engine_harvest = engine_begin + 11;  // svc 24

        public static final int engine_harvest_rollback = engine_begin + 12;  // svc 25


        public static final int engine_end = 26; // svc 26
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/cash/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/cash/svc>
    // cash - master file: us/api/data/gov/cash

        public static final int cash_begin = 26;
        public static final int cash_fetch_accounts = cash_begin + 0;  // svc 26
        public static final int cash_fetch_accounts_response = cash_begin + 1;  // svc 27

        public static final int cash_share_file = cash_begin + 2;  // svc 28
        public static final int cash_share_file_response = cash_begin + 3;  // svc 29

        public static final int cash_search_kv = cash_begin + 4;  // svc 30
        public static final int cash_search_kv_response = cash_begin + 5;  // svc 31

        public static final int cash_file_hash = cash_begin + 6;  // svc 32
        public static final int cash_file_hash_response = cash_begin + 7;  // svc 33

        public static final int cash_file_content = cash_begin + 8;  // svc 34
        public static final int cash_file_content_response = cash_begin + 9;  // svc 35


        public static final int cash_end = 36; // svc 36
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/engine_auth/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/engine_auth/svc>
    // engine_auth - master file: us/api/data/gov/engine_auth

        public static final int engine_auth_begin = 36;
        public static final int engine_auth_nodes = engine_auth_begin + 0;  // svc 36
        public static final int engine_auth_nodes_response = engine_auth_begin + 1;  // svc 37

        public static final int engine_auth_lookup_node = engine_auth_begin + 2;  // svc 38
        public static final int engine_auth_lookup_node_response = engine_auth_begin + 3;  // svc 39


        public static final int engine_auth_end = 40; // svc 40
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/traders/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/traders/svc>
    // traders - master file: us/api/data/gov/traders

        public static final int traders_begin = 40;
        public static final int traders_wallets = traders_begin + 0;  // svc 40
        public static final int traders_wallets_response = traders_begin + 1;  // svc 41

        public static final int traders_lookup_wallet = traders_begin + 2;  // svc 42
        public static final int traders_lookup_wallet_response = traders_begin + 3;  // svc 43

        public static final int traders_random_wallet = traders_begin + 4;  // svc 44
        public static final int traders_random_wallet_response = traders_begin + 5;  // svc 45


        public static final int traders_end = 46; // svc 46
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/gov/sys/svc>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/gov/sys/svc>
    // sys - master file: us/api/data/gov/sys

        public static final int sys_begin = 46;
        public static final int sys_f1 = sys_begin + 0;  // svc 46


        public static final int sys_end = 47; // svc 47
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

    public static final int net_daemon_end = dfs_end;

}


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
package us.wallet.engine;
import java.util.ArrayList;                                                                    // ArrayList
import us.wallet.trader.bookmark_info_t;                                                       // bookmark_info_t
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import us.wallet.trader.endpoint_t;                                                            // endpoint_t
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.crypto.types.*;                                                           // *
import static us.gov.socket.types.*;                                                           // *
import static us.stdint.*;                                                                     // *
import us.gov.io.types.*;                                                                      // *
import us.ko;                                                                                  // ko
import us.wallet.trader.protocol_selection_t;                                                  // protocol_selection_t
import us.wallet.trader.qr_t;                                                                  // qr_t
import us.gov.crypto.sha256;                                                                   // sha256
import us.string;                                                                              // string

public interface caller_api extends dto {

    //#include <us/api/generated/java/wallet/engine/cllr_purevir>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/wallet/engine/cllr_purevir>
    // engine - master file: us/api/data/wallet/engine

      ko call_print_grid(string data);
      ko call_print_socket_connections(string data);
      ko call_allw(string data);
      ko call_sync(string data);
      ko call_reload_file(final string filename, string ans);
      ko call_get_component_update(final get_component_update_in_t o_in, get_component_update_out_dst_t o_out);
      ko call_get_component_hash(final get_component_hash_in_t o_in, string curver);
      ko call_harvest(final harvest_in_t o_in, string ans);
      ko call_world(vector_hash wallets);
      ko call_lookup_wallet(final hash_t addr, lookup_wallet_out_dst_t o_out);
      ko call_random_wallet(random_wallet_out_dst_t o_out);
      ko call_register_w(final uint32_t net_addr, string ans);
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/wallet/pairing/cllr_purevir>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/wallet/pairing/cllr_purevir>
    // pairing - master file: us/api/data/wallet/pairing

      ko call_pair_device(final pair_device_in_t o_in, string ans);
      ko call_unpair_device(final pub_t pub, string ans);
      ko call_list_devices(string data);
      ko call_attempts(vector_string data);
      ko call_prepair_device(final prepair_device_in_t o_in, prepair_device_out_dst_t o_out);
      ko call_unprepair_device(final uint16_t pin, string ans);
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/wallet/r2r/cllr_purevir>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/wallet/r2r/cllr_purevir>
    // r2r - master file: us/api/data/wallet/r2r

      ko call_trading_msg(final trading_msg_in_t o_in);
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.
    //#include <us/api/generated/java/wallet/wallet/cllr_purevir>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/java/wallet/wallet/cllr_purevir>
    // wallet - master file: us/api/data/wallet/wallet

      ko call_balance(final uint16_t detail, string data);
      ko call_list(final uint16_t detail, string data);
      ko call_recv_address(hash_t addr);
      ko call_new_address(hash_t addr);
      ko call_add_address(final priv_t priv, hash_t addr);
      ko call_transfer(final transfer_in_t o_in, blob_t blob_ev);
      ko call_transfer_from(final transfer_from_in_t o_in, blob_t blob_ev);
      ko call_tx_charge_pay(final tx_charge_pay_in_t o_in, blob_t blob_ev);
      ko call_invoice_verify(final invoice_verify_in_t o_in, string err);
      ko call_payment_verify(final payment_verify_in_t o_in, string err);
      ko call_tx_pay(final tx_pay_in_t o_in, blob_t blob_ev2);
      ko call_tx_new(blob_t blob_ev);
      ko call_tx_add_section(final tx_add_section_in_t o_in, blob_t blob_ev);
      ko call_tx_add_input(final tx_add_input_in_t o_in, blob_t blob_ev);
      ko call_tx_add_output(final tx_add_output_in_t o_in, blob_t blob_ev);
      ko call_tx_sign(final tx_sign_in_t o_in, blob_t blob_ev);
      ko call_tx_send(final blob_t blob_ev, string info);
      ko call_tx_decode(final blob_t blob_ev, string tx_pretty);
      ko call_tx_check(final blob_t blob_ev, string data);
      ko call_set_supply(final set_supply_in_t o_in, blob_t blob_ev);
      ko call_store_kv(final store_kv_in_t o_in, blob_t blob_ev);
      ko call_rm_kv(final rm_kv_in_t o_in, blob_t blob_ev);
      ko call_store_file(final store_file_in_t o_in, store_file_out_dst_t o_out);
      ko call_rm_file(final rm_file_in_t o_in, blob_t blob_ev);
      ko call_search(final search_in_t o_in, string result);
      ko call_file(final hash_t digest, bin_t content);
      ko call_exec(final string cmd);
      ko call_get_subhome(string subhome);
      ko call_net_info(net_info_out_dst_t o_out);
      ko call_compilance_report(final compilance_report_in_t o_in, string data);
      ko call_priv_key(final priv_t priv, priv_key_out_dst_t o_out);
      ko call_pub_key(final pub_t pub, pub_key_out_dst_t o_out);
      ko call_gen_keys(gen_keys_out_dst_t o_out);
      ko call_sign(final sign_in_t o_in, sign_out_dst_t o_out);
      ko call_verify(final verify_in_t o_in, uint8_t result);
      ko call_encrypt(final encrypt_in_t o_in, bin_t msg_scrambled);
      ko call_decrypt(final decrypt_in_t o_in, bin_t msg);
      ko call_patch_os(final patch_os_in_t o_in, patch_os_out_dst_t o_out);
      ko call_list_protocols(string data);
      ko call_trade(final trade_in_t o_in, hash_t tid);
      ko call_list_trades(string data);
      ko call_kill_trade(final hash_t tid, string data);
      ko call_exec_trade(final exec_trade_in_t o_in);
      ko call_qr(bookmarks_t bookmarks);
      ko call_bookmark_add(final bookmark_add_in_t o_in, string ans);
      ko call_bookmark_delete(final string key, string ans);
      ko call_bookmark_list(bookmarks_t bookmarks);
      ko call_timeseries_list(string ans);
      ko call_timeseries_show(final timeseries_show_in_t o_in, string ans);
      ko call_timeseries_new(final timeseries_new_in_t o_in, string ans);
      ko call_timeseries_add(final timeseries_add_in_t o_in, string ans);
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

}


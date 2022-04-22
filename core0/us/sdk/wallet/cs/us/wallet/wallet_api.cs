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
using System;
using System.IO;
using static us.gov.cash.tx;

namespace us.wallet
{
    interface wallet_api
    {
//---------------------------------------------------------------------generated by make, do not edit
//content of file: ../../../api/apitool_generated__functions_wallet_cs_purevir
//------------------generated by apitool- do not edit
// wallet - master file: us/apitool/data/wallet
  void ping(Stream os);
  void balance(bool arg0, Stream os);
  void list(bool arg0, Stream os);
  void new_address(Stream os);
  void add_address(byte[] arg0, Stream os);
  void transfer(byte[] arg0, long arg1, byte[] arg2, Stream os);
  void transfer_from(byte[] arg0, byte[] arg1, long arg2, byte[] arg3, Stream os);
  void tx_charge_pay(byte[] arg0, long arg1, byte[] arg2, long arg3, byte[] arg4, Stream os);
  void tx_pay(string arg0, Stream os);
  void tx_new(Stream os);
  void tx_add_section(string arg0, byte[] arg1, Stream os);
  void tx_add_input(string arg0, int arg1, byte[] arg2, long arg3, Stream os);
  void tx_add_output(string arg0, int arg1, byte[] arg2, long arg3, Stream os);
  void tx_make_p2pkh(tx_make_p2pkh_input arg0, Stream os);
  void tx_sign(string arg0, string arg1, Stream os);
  void tx_send(string arg0, Stream os);
  void tx_decode(string arg0, Stream os);
  void tx_check(string arg0, Stream os);
  void set_supply(byte[] arg0, long arg1, Stream os);
  void store_kv(byte[] arg0, string arg1, string arg2, Stream os);
  void rm_kv(byte[] arg0, string arg1, Stream os);
  void store_file(byte[] arg0, byte[] arg1, Stream os);
  void rm_file(byte[] arg0, byte[] arg1, Stream os);
  void search(byte[] arg0, string arg1, Stream os);
  void file(byte[] arg0, Stream os);
//-/----------------generated by apitool- do not edit

//-/-------------------------------------------------------------------generated by make, do not edit
    }
}

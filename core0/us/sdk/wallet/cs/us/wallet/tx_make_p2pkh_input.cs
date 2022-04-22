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
using static us.gov.cash.tx;

namespace us.wallet
{
    public class tx_make_p2pkh_input
    {
        public tx_make_p2pkh_input(String rcpt_addr0, long amount0, /*String token0, sigcode_t sigcode_inputs0, sigcode_t sigcode_outputs0, */ bool sendover0)
        {
            src_addr = "";
            rcpt_addr = rcpt_addr0;
            amount = amount0;
            sendover = sendover0;
            if (amount < 1) amount = 0;
            //if (fee < 1) fee = 0;
            //sigcode_inputs = sigcode_inputs0;
            //sigcode_outputs = sigcode_outputs0;            
        }

        String src_addr;
        String rcpt_addr;
        long amount;
        //String token;
        //sigcode_t sigcode_inputs;
        //sigcode_t sigcode_outputs;
        bool sendover;

        public bool check()
        {
            //return fee > 0 && amount > 0 && !rcpt_addr.Equals(String.Empty);
            return amount > 0 && !rcpt_addr.Equals(String.Empty);
        }

        public String to_string()
        {
            return "11111111111111111111 " + rcpt_addr + " " + amount + " "/* + token + " " + (short)sigcode_inputs + " " + (short)sigcode_outputs + " "*/ + (sendover ? "1" : "0");
        }
    }
}

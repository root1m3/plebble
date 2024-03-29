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
package us.gov.id;
import static us.gov.socket.types.*;                                                           // *
import us.ko;                                                                                  // ko
import static us.gov.id.types.request_data_t;                                                  // request_data_t
import us.gov.crypto.sha256;                                                                   // sha256
import us.string;                                                                              // string

public interface api extends caller_api {

    //#include <us/api/generated/gov/java/id/hdlr_purevir>
    //------------------------------------------------------------__begin__------generated by configure, do not edit.
    //content of file: <us/api/generated/gov/java/id/hdlr_purevir>
    // id - master file: us/api/data/gov/id

      ko handle_request(final seq_t seq, final sha256.hash_t msg);
      ko handle_peer_challenge(final seq_t seq, final peer_challenge_in_dst_t o_in);
      ko handle_challenge2_response(final seq_t seq, final challenge2_response_in_dst_t o_in);
      ko handle_verification_result(final seq_t seq, final request_data_t request_data);
    //-/----------------------------------------------------------___end___------generated by configure, do not edit.

}


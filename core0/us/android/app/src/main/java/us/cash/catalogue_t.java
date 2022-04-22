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
package us.cash;
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import us.gov.io.blob_writer_t;                                                                // blob_writer_t
import us.CFG;                                                                                 // CFG
import us.wallet.trader.workflow.doc0_t;                                                       // doc0_t
import us.wallet.trader.workflow.expiry_doc_t;                                                 // expiry_doc_t
import java.util.HashMap;                                                                      // HashMap
import static us.gov.io.types.*;                                                               // *
import static us.gov.socket.types.*;                                                           // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.wallet.trader.workflow.doc0_t.*;                                              // *
import us.ko;                                                                                  // ko
import us.pair;                                                                                // pair
import static us.gov.io.types.blob_t.serid_t;                                                  // serid_t
import us.string;                                                                              // string

class catalogue_t extends expiry_doc_t {

    @Override public String title() { return "Catalogue"; }
    @Override public String title_es() { return "Catalogo"; }
    @Override public magic_t get_magic() { return new magic_t(288); }
    @Override public String get_name() { return "cat"; }

    public static class item_t extends us.wallet.trader.workflow.item_t {
        @Override public doc0_t create_doc() {
            return new catalogue_t();
        }
    }

}


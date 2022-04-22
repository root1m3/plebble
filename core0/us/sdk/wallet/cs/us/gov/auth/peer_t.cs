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
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace us.gov.auth
{
    public class peer_t : us.gov.id.peer_t
    {
        public peer_t(Socket sock) : base(sock)
        {            
        }

        public enum Stage_t
        {
            denied = 0,
            authorized = 1,
            num_stages = 2
        };
    
        public override void verification_completed()
        {
            if (!verification_is_fine())
            {
                return;
            }
            if (!authorize(pubkey))
            {
                return;
            }
            stage = Stage_t.authorized;
        }

        public virtual bool authorize(byte[] pub)
        {
            return false;
        }

        public Stage_t stage = Stage_t.denied;
    }
}

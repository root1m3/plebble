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
using System.Net.Sockets;

namespace us.gov.socket
{
    public class peer_t : client
    {
        public peer_t() : base()
        {
        }

        public peer_t(Socket sock): base(sock)
        {            
        }

        public bool is_slow()
        {
            return false;
        }

        public bool ping()
        {
            return send3(new datagram((ushort)protocol.gov_socket_ping, "ping")) == null;
        }

        public void process_pong()
        {
        }

        public virtual bool process_work(datagram d)
        {
            if (d.service.Equals(protocol.gov_socket_ping))
            {
                send3(new datagram((ushort)protocol.gov_socket_pong, "pong"));
            }
            else if (d.service.Equals(protocol.gov_socket_pong))
            {
                process_pong();                
            }
            else {
                return false;
            }
            return true;
        }

    }
}

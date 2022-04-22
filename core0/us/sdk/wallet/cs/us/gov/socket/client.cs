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
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace us.gov.socket
{
    public class client
    {
        public client()
        {
        }

        public client(Socket sock_)
        {
            sock = sock_;
            if (connected()) addr = address();
        }

        public class IPEndPoint : System.Net.IPEndPoint
        {
            public IPEndPoint(long address, int port) : base(address, port) { }
            public IPEndPoint(IPAddress address, int port) : base(address, port) { }

            public static bool TryParse(string value, out IPEndPoint result)
            {
                if (!Uri.TryCreate($"tcp://{value}", UriKind.Absolute, out Uri uri) ||
                    !IPAddress.TryParse(uri.Host, out IPAddress ipAddress) ||
                    uri.Port < 0 || uri.Port > 65535)
                {
                    result = default(IPEndPoint);
                    return false;
                }

                result = new IPEndPoint(ipAddress, uri.Port);
                return true;
            }
        }

        public String connect0(String host, int port)
        {
            return connect0(host, port, false);
        }

        public String connect0(String host, int port, bool block)
        {
            IPEndPoint.TryParse(host + ":" + port, out remoteEndPoint);            
            called_disconnect = false;
            addr = host;
            try
            {
                sock.Blocking = block;
                //ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls12;
                sock = new Socket(remoteEndPoint.Address.AddressFamily, SocketType.Stream, ProtocolType.Tcp);                
                sock.Connect(remoteEndPoint);
            }
            catch (SocketException e)
            {
                return "KO "+ e.Message + e.ErrorCode;                
            }
            addr = host;
            return null;
        }

        public void disconnect()
        {
            called_disconnect = true;
            if (sock == null) return;
            //if (!is_connected) return;
            if (!sock.Connected) return;
            try
            {
                //sock.Disconnect(true);
                //sock.Shutdown(SocketShutdown.Both);
                sock.Close();                
            }
            catch (SocketException)
            {
            }
        }

        public bool connected() {            
            return sock != null && sock.Connected && !called_disconnect; //&& IsConnected()
        }

        public bool IsConnected() //Sin uso por ahora
        {
            try
            {
                //if (sock.GetSocketOption(SocketOptionLevel.Soc ket, SocketOptionName.KeepAlive, 1)[0].Equals(1))
                // return sock.Connected;
                if (!sock.Connected) return false;
                //checkSocket.BeginSend(new byte[0], 0, 0, SocketFlags.None, null, null);
                bool bSelectRead = sock.Poll(1, SelectMode.SelectRead);
                bool bSelectWrite = sock.Poll(1, SelectMode.SelectWrite);
                int available = sock.Available;
                //if (bSelectWrite && bSelectRead && available 0)
                if (bSelectWrite && bSelectRead)
                {
                    //return true;
                    //checkSocket.BeginReceive(new byte[1], 0, 1, SocketFlags.Peek, null, null);
                    sock.Receive(new byte[0], 0, 0, SocketFlags.Peek);
                    sock.Send(new byte[0], 0, 0, SocketFlags.None);
                    return sock.Connected;
                }
                else
                    return false;
            }
            catch (SocketException)
            {
                return false;
            }
            catch (ObjectDisposedException)
            {
                return false;
            }
        }

        public String address()
        {
            return remoteEndPoint.Address.ToString();
        }

        public void ready() { }

        public class pair<f, s>
        {
            public f first;
            public s second;
            public pair(f fst, s snd)
            {
                this.first = fst;
                this.second = snd;
            }
        }

        public pair<String, datagram> send_recv3(datagram d)
        {
            String r = send3(d);
            if (r != null)
            {
                return new pair<String, datagram>(r, null);
            }
            return recv6();
        }

        public pair<String, datagram> recv6()
        {
            pair<String, datagram> ans = new pair<String, datagram>(null, null);
            int prevdend = 0;
            while (true)
            {
                datagram d = ans.second;
                //System.out.println("1");
                ans = recv4(d);
                //System.out.println("2");
                if (ans.first != null)
                {
                    //System.out.println("3 "+ans.first);
                    return ans;
                }
                //System.out.println("dend "+ans.second.dend+" prev "+prevdend);
                if (ans.second.dend == prevdend)
                {
                    ans.first = "Nothing received.";
                    ans.second = null;
                    break;
                }
                if (ans.second.completed())
                {
                    break;
                }
                prevdend = ans.second.dend;
            }
            return ans;

        }

        public pair<String, datagram> recv3()
        {            
            pair<String, datagram> r = new pair<String, datagram>(null, null);
            r.second = new datagram();
            String ans = r.second.recvfrom(sock);
            if (ans != null)
            {
                r.first = ans;
                r.second = null;
                disconnect();
            }
            return r;
        }

        public pair<String, datagram> recv4(datagram d)
        {            
            pair<String, datagram> r = new pair<String, datagram>(null, null);
            if (d != null)
            {
                //System.out.println("recv4 - 1");
                r.second = d;
            }
            else
            {
                r.second = new datagram();
            }
            String ans = r.second.recvfrom(sock);
            if (ans != null)
            { 
                //System.out.println("err - "+r.first);
                r.first = ans;
                r.second = null;
                disconnect();
            }
            return r;
        }

        void dump_d(String prefix, datagram d, String addr)
        {
            //        System.out.println("SOCKET: " + prefix + " datagram " + d.service + " " + d.service_str() + " of size " + d.size() + " bytes. to " + addr);
            //        System.out.println("      : " + d.parse_string());
        }

        public String send3(datagram d)
        {
            if (!connected())
            {
                return "KO - Sending datagram before connecting.";
            }
            String r = d.sendto(sock);

            if (r != null)
            {
                disconnect();
            }
            //    else {
            //        if (DEBUG) dump_d("sent",d,addr);
            //    }
            return r;
        }

        public Socket sock;
        String addr;
        IPEndPoint remoteEndPoint;
        bool called_disconnect = false;
    }
}

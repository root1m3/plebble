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
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using us.gov.crypto;
using us.gov.socket;

namespace us.wallet
{
    public class rpc_api : us.gov.auth.peer_t
    {
        String walletd_host;
        short walletd_port;
        KeyPair id;        

        public rpc_api(KeyPair k, String walletd_host_, short walletd_port_) : base(new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp))
        {
            walletd_host = walletd_host_;
            walletd_port = walletd_port_;
            id = k;
        }
        
        public override KeyPair get_keys()
        {
            return id;
        }

        public override bool authorize(byte[] p)
        {
            return true;
        }

        public bool connect_walletd(Stream os)
        {
            try
            {
                if (connected()) return true;
                String r = connect(walletd_host, walletd_port, '2', true);
                if (r != null)
                {
                    os.Write(Encoding.UTF8.GetBytes(r), 0, Encoding.UTF8.GetBytes(r).Length);
                    return false;
                }
                r = run_auth_responder();
                if (r != null)
                {
                    os.Write(Encoding.UTF8.GetBytes(r), 0, Encoding.UTF8.GetBytes(r).Length);
                    return false;
                }
                if (!connected())
                {
                    os.Write(Encoding.UTF8.GetBytes("KO - Auth failed"), 0, Encoding.UTF8.GetBytes("KO - Auth failed").Length);
                    return false;
                }
                return true;
            }
            catch (Exception e)
            {
                byte[] msg = Encoding.UTF8.GetBytes(e.Message + ". Remote host has disconnected the socket.");
                os.Write(msg, 0, msg.Length);
                return false;
            }
        }

        public void ask_string(short service, Stream os)
        {
            ask_string(service, "", os);
        }

        public pair<String, datagram> sendrecv(datagram d)
        {
            Stream os = Console.OpenStandardOutput();
            if (!connect_walletd(os))
            {
                try
                {
                    return new pair<String, datagram>(os.ToString(), null);
                }
                catch (Exception e)
                {
                    return new pair<String, datagram>(e.Message, null);
                }
            }
            pair<String, datagram> r = send_recv3(d);
            if (r.first != null)
            {
                disconnect();
                try
                {
                    sock.Close();
                }catch(Exception)
                {

                }
                if (!connect_walletd(os))
                {
                    try
                    {
                        return new pair<String, datagram>(os.ToString(), null);
                    }
                    catch (Exception e)
                    {
                        return new pair<String, datagram>(e.Message, null);
                    }
                }
                r = send_recv3(d);
                if (r.first != null)
                {
                    return r;
                }
            }
            return r;
        }

        public void ask_string(short service, String args, Stream os)
        {
            datagram d = new datagram((ushort)service, args);
            pair<String, datagram> r = sendrecv(d);
            if (r.first != null)
            {
                try
                {                    
                    os.Write(Encoding.UTF8.GetBytes(r.first), 0, Encoding.UTF8.GetBytes(r.first).Length);
                }
                catch (IOException e)
                {
                    try
                    {
                        os.Write(Encoding.UTF8.GetBytes(e.Message), 0, Encoding.UTF8.GetBytes(e.Message).Length);
                    }
                    catch (Exception)
                    {
                        System.Diagnostics.Debug.Fail("Unable to output Stream.");
                    }
                }
            }
            else
            {
                try
                {
                    os.Write(Encoding.UTF8.GetBytes(r.second.parse_string()),0, Encoding.UTF8.GetBytes(r.second.parse_string()).Length);   
                }
                catch (IOException e)
                {
                    try
                    {
                        os.Write(Encoding.UTF8.GetBytes(e.Message), 0, Encoding.UTF8.GetBytes(e.Message).Length);
                    }
                    catch (Exception)
                    {
                        System.Diagnostics.Debug.Fail("Unable to output Stream.");
                    }
                }
            }
        }
    }
}

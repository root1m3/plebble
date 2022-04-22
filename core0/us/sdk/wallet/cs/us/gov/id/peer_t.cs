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
using System.Net.Sockets;
using System.Security;
using us.gov.crypto;
using us.gov.socket;

namespace us.gov.id
{
    public class peer_t : us.gov.socket.peer_t
    {        

        public stage_t stage_peer = stage_t.anonymous;
        String msg;
        public byte[] pubkey;

        public enum stage_t
        {
            anonymous = 0,
            verified = 1,
            verified_fail = 2,
            num_stages = 3
        };

        public peer_t(Socket sock) : base(sock)
        {            
        }
        
        public String connect(String host, int port, char role, bool block)
        {
            String r = base.connect0(host, port, block);
            if (r == null)
            {
                initiate_dialogue(role);
            }
            return r;
        }
        
        public override bool process_work(datagram d)
        {

            //System.out.println("US_CASH "+"process_work 1 "+d.service);
            if (d.service.Equals(protocol.gov_id_request)) process_request(d, get_keys());
            else if (d.service.Equals(protocol.gov_id_peer_challenge)) process_peer_challenge(d, get_keys());
            else if (d.service.Equals(protocol.gov_id_challenge_response)) process_challenge_response(d);
            else return false;            
            return true;
        }

        public void process_request(datagram d,KeyPair mykeys)
        {
            String hello_msg = d.parse_string();
            try
            {
                String signature = ec.Instance.sign_encode(mykeys.PrivateKey, hello_msg);
                msg = get_random_message();
                String os = ec.Instance.to_b58(mykeys.PublicKey) + ' ' + signature + ' ' + msg;
                send3(new datagram((ushort)protocol.gov_id_peer_challenge, os));
            }
            catch (SecurityException)
            {
            }
        }

        public void process_peer_challenge(datagram d,KeyPair mykeys)
        {
            //System.out.println("US_CASH "+"process_peer_challenge 1 "+(mykeys==null?"NO":"ok"));
            String data = d.parse_string();
            //System.out.println("US_CASH "+"data "+data);
            String[] parts = data.Split(' ');
            //System.out.println("US_CASH "+" 2");
            if (parts.Length != 3)
            {
                //System.out.println("US_CASH "+" 3");
                stage_peer = stage_t.verified_fail;
                return;
            }
            //System.out.println("US_CASH "+" 4");

            byte[] peer_pubk;
            try
            {
                //System.out.println("US_CASH "+" 5");
                peer_pubk = base58.decode(parts[0]);
                try
                {
                    if (ec.Instance.verify(peer_pubk, msg, parts[1]))
                    {
                        //System.out.println("US_CASH "+" 6");
                        stage_peer = stage_t.verified;
                    }
                    else
                    {
                        //System.out.println("US_CASH "+" 7");
                        stage_peer = stage_t.verified_fail;                                               
                    }
                    pubkey = peer_pubk;
                }
                catch (Exception)
                {
                    //System.out.println("US_CASH "+" 8");
                    stage_peer = stage_t.verified_fail;
                }
            }
            catch (Exception)
            {
                //System.out.println("US_CASH "+" 9");
                stage_peer = stage_t.verified_fail;
            }
            //System.out.println("US_CASH "+" 10");
            msg = null;
            try
            {
                //System.out.println("US_CASH "+" 11");
                String signature_der_b58 = ec.Instance.sign_encode(mykeys.PrivateKey, parts[2]);
                //System.out.println("US_CASH "+" 12");
                String os = ec.Instance.to_b58(mykeys.PublicKey) + ' ' + signature_der_b58;
                //System.out.println("US_CASH "+" 14");
                send3(new datagram((ushort)protocol.gov_id_challenge_response, os));
                //System.out.println("US_CASH "+" 15");
            }
            catch (SecurityException)
            {
                //System.out.println("US_CASH "+" 16");
            }
            //System.out.println("US_CASH "+" 17");
            verification_completed();
            //System.out.println("US_CASH "+" 18");
        }

        public void process_challenge_response(datagram d)
        {
            String data = d.parse_string();
            String[] parts = data.Split(' ');
            if (parts.Length != 2)
            {
                stage_peer = stage_t.verified_fail;
                return;
            }

            byte[] peer_pubk;
            try
            {
                peer_pubk = base58.decode(parts[0]);
                try
                {
                    if (ec.Instance.verify(peer_pubk, msg, parts[1]))
                    {
                        stage_peer = stage_t.verified;
                        pubkey = peer_pubk;
                    }
                    else
                    {
                        stage_peer = stage_t.verified_fail;
                    }
                }
                catch (Exception)
                {
                    stage_peer = stage_t.verified_fail;
                }
            }
            catch (Exception)
            {
                stage_peer = stage_t.verified_fail;
            }
            msg = null;
            verification_completed();
        }

        public void initiate_dialogue(char role)
        {
            if (msg == null)
            {
                msg = get_random_message();
                char[] chars = msg.ToCharArray();
                chars[0] = role;
                msg = new String(chars);
                send3(new datagram((ushort)protocol.gov_id_request, msg));
            }
        }

        public static String get_random_message()
        {
            try { 
                var rnd = System.Security.Cryptography.RandomNumberGenerator.Create();
                byte[] v = new Byte[20];
                rnd.GetBytes(v);
                return base58.encode(v);
            }
            catch (Exception)
            {
                return null;
            }
        }

        public bool verification_is_fine() { return stage_peer.Equals(stage_t.verified); }

        public virtual void verification_completed() { }

        public virtual KeyPair get_keys()
        {            
            System.Environment.Exit(1);
            return new KeyPair();
        }

        public String run_auth_responder()
        {
            //System.out.println("US_CASH "+"auth_responder 1 ");
            stage_peer = stage_t.anonymous;
            while (true)
            {
                //System.out.println("US_CASH "+"auth_responder L 1 ");
                if (stage_peer == stage_t.verified || stage_peer == stage_t.verified_fail)
                {
                    break;
                }
                //System.out.println("US_CASH "+"auth_responder L 2 ");
                pair<String, datagram> r = recv6();
                //System.out.println("US_CASH "+"auth_responder L 3 ");
                if (r.first != null)
                {
                    //System.out.println("US_CASH "+"auth_responder L 3.1 "+ r.first);
                    return r.first;
                }
                //System.out.println("US_CASH "+"auth_responder L 4 ");
                if (process_work(r.second))
                {
                    //System.out.println("US_CASH "+"auth_responder L 5 ");
                    continue;
                }
                //System.out.println("US_CASH "+"auth_responder L 6 ");
                return "KO - Unrecognized datagram arrived. declsize=" + r.second.decode_size() + " svc=" + r.second.decode_service() + " chan=" + r.second.decode_channel();
            }
            //System.out.println("US_CASH "+"auth_responder L 7 ");
            return stage_peer == stage_t.verified ? null : "KO - Peer failed to demonstrate identity";
        }
    }
}

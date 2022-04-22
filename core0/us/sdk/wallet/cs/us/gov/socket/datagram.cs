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
using System.Diagnostics;
using System.IO;
using System.Net.Sockets;
using System.Text;

namespace us.gov.socket
{
    public class datagram
    {
        static readonly int offset_channel = 0;
        static readonly int size_channel = 2;
        static readonly int offset_size = offset_channel + size_channel;
        static readonly int size_size = 4;
        static readonly int offset_service = offset_size + size_size;
        static readonly int size_service = 2;
        static readonly int h = size_channel + size_size + size_service;
        static readonly int maxsize = 20000000;
        public static short system_channel = 0;

        byte[] bytes;
        public int dend;
        public short service;

        public datagram()
        {
            bytes = new byte[h];
            dend = 0;
        }

        public datagram(UInt16 service)
        {
            bytes = new byte[h];
            encode_channel();
            encode_size(h);
            encode_service(service);
            dend = bytes.Length;
        }

        public datagram(UInt16 service, String msg)
        {
            int size = h + msg.Length;
            bytes = new byte[size];
            encode_channel();
            encode_size(size);
            encode_service(service);
            //System.arraycopy(msg.getBytes(), 0, bytes, h, msg.Length);
            Buffer.BlockCopy(Encoding.ASCII.GetBytes(msg), 0, bytes, h, msg.Length);
            dend = bytes.Length;
        }

        public datagram(UInt16 service, short payload)
        {
            int size = h + 2;
            bytes = new byte[size];
            encode_channel();
            encode_size(size);
            encode_service(service);
            bytes[h] = (byte)(payload & 0xff);
            bytes[h + 1] = (byte)(payload >> 8);
            dend = bytes.Length;    
        }

        /*
        public static byte[] hexStringToByteArray(String s)
        {
            int len = s.Length;
            byte[] data = new byte[len / 2];
            for (int i = 0; i < len; i += 2)
            {
                data[i / 2] = (byte)((Char.digit(s[i], 16) << 4) + Char.digit(s[i + 1], 16));
            }
            return data;
        }
        */

        public datagram(UInt16 service, byte[] addr, byte[] content)
        {
            if (addr.Length != 20) return;
            int payload_sz = 20 + content.Length;
            int size = h + payload_sz;
            bytes = new byte[size];
            encode_channel();
            encode_size(size);
            encode_service(service);     
            Buffer.BlockCopy(addr, 0, bytes, h, 20);            
            Buffer.BlockCopy(content, 0, bytes, h + 20, content.Length);
            dend = bytes.Length;
        }

        public datagram(UInt16 service, byte[] addr)
        {
            if (addr.Length != 20) return;
            int payload_sz = 20;
            int size = h + payload_sz;
            bytes = new byte[size];
            encode_channel();
            encode_size(size);
            encode_service(service);
            //System.arraycopy(addr, 0, bytes, h, 20);
            Buffer.BlockCopy(addr, 0, bytes, h, 20);            
            dend = bytes.Length;
        }

        public datagram(UInt16 service, byte[] addr, String s1)
        {
            if (addr.Length != 20) return;
            int payload_sz = 20 + s1.Length;
            int size = h + payload_sz;
            bytes = new byte[size];
            encode_channel();
            encode_size(size);
            encode_service(service);
            Buffer.BlockCopy(addr, 0, bytes, h, 20);            
            Buffer.BlockCopy(Encoding.ASCII.GetBytes(s1), 0, bytes, h + 20, s1.Length);
            dend = bytes.Length;
        }

        public void encode_channel()
        {
            Debug.Assert(bytes.Length >= offset_channel + 2);
            byte[] t = BitConverter.GetBytes(system_channel);
            bytes[offset_channel] = t[0];
            bytes[offset_channel + 1] = t[1];
        }

        public short decode_channel()
        {
            Debug.Assert(bytes.Length > 2);
            byte[] t = new byte[2];
            t[0] = bytes[offset_channel];
            t[1] = bytes[offset_channel + 1];
            /*
            MemoryStream bb = new MemoryStream();
            using (BinaryWriter writer = new BinaryWriter(bb))
            {
                writer.Write(t);
            }
            */
            return BitConverter.ToInt16(t, 0);
        }

        public void encode_size(int sz)
        {
            Debug.Assert(bytes.Length >= offset_size + 3);
            bytes[offset_size] = (byte)(sz & 0xff);
            bytes[offset_size + 1] = (byte)(sz >> 8 & 0xff);
            bytes[offset_size + 2] = (byte)(sz >> 16 & 0xff);
            bytes[offset_size + 3] = (byte)(sz >> 24 & 0xff);
        }

        public UInt32 decode_size()
        {
            Debug.Assert(bytes.Length > 3);
            UInt32 sz = bytes[offset_size];
            sz |= (uint)(bytes[offset_size + 1] << 8);
            sz |= (uint)(bytes[offset_size + 2] << 16);
            sz |= (uint)(bytes[offset_size + 3] << 24);
            return sz;
        }

        public long size()
        {
            return bytes.Length;
        }

        public void encode_service(UInt16 svc)
        {
            Debug.Assert(bytes.Length >= offset_service + 2);
            bytes[offset_service] = (byte)(svc & 0xff);
            bytes[offset_service + 1] = (byte)(svc >> 8 & 0xff);
        }

        public UInt16 decode_service()
        {
            Debug.Assert(bytes.Length >= h);
            UInt16 svc = bytes[offset_service];
            svc |= (ushort)(bytes[offset_service + 1] << 8);
            return svc;
        }

        public bool completed()
        {
            return dend == bytes.Length && bytes.Length > 0;
        }

        public String sendto(Socket sock)
        {
            if (bytes.Length >= maxsize)
            {
                return "KO - Datagram is too big.";
            }
            try
            {                
                sock.Send(bytes);
                //int bytesRec = sock.Receive(bytes);
                //return Encoding.UTF8.GetString(bytes, 0, bytes.Length);
            }
            catch (Exception e)
            {
                return e.Message;
            }
            return null;
        }

        public static int timeout = 3000;

        public String recvfrom(Socket s)
        {
            
            //System.out.println("dgram::recvfrom dend="+dend);
            try
            {                
                //DataInputStream input = new DataInputStream(s.getInputStream());
                if (dend < h)
                {
                    //int nread = input.read(bytes, dend, h - dend);
                    int nread = s.Receive(bytes, dend, h - dend, SocketFlags.None);
                    //System.out.println("dgram::recvfrom nread="+nread);
                    if (nread <= 0)
                    {
                        return null;
                    }
                    dend += nread;
                    if (dend < h) return null;
                    short ch = decode_channel();
                    if (ch != system_channel)
                    {
                        return "evil. channel mismatch " + ch + "!=" + system_channel + "(system channel)";
                    }
                    int sz = (int)decode_size();
                    if (sz > maxsize)
                    {
                        return "evil. too big";
                    }
                    if (sz < h)
                    {
                        return "evil. too small";
                    }
                    byte[] m = new byte[sz];
                    Buffer.BlockCopy(bytes, 0, m, 0, h);
                    bytes = m;
                    service = (short)decode_service();
                    if (dend == sz)
                    {
                        return null;
                    }
                }
                //int nread = input.read(bytes, dend, bytes.Length - dend);
                int nread2 = s.Receive(bytes, dend, bytes.Length - dend, SocketFlags.None);
                if (nread2 <= 0)
                {
                    return "KO - Timeout waiting for data from peer. or Incoming datagram is too big";
                }
                dend += nread2;
                return null;
            }
            catch (Exception e)
            {
                return "KO - " + e.Message;
            }
        }

        public String parse_string()
        {
            byte[] b = new byte[dend - h];
            //System.arraycopy(bytes, h, b, 0, dend - h);
            Buffer.BlockCopy(bytes, h, b, 0, dend - h);
            return Encoding.UTF8.GetString(b, 0, b.Length); ;
        }

        /*
        public short parse_uint16()
        {
            if (bytes.Length != h + 2) return 0;
            byte[] t = new byte[2];
            t[0] = bytes[h];
            t[1] = bytes[h + 1];
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            return bb.getShort();
        }
        */
        public byte[] parse_bin()
        {
            byte[] t = new byte[dend - h];
            Buffer.BlockCopy(bytes, h, t, 0, dend - h);
            return t;
        }        

        static char[] x = { 'P', 'E', 'Q', '_' };

        public String service_str()
        {
            return service_str(service);
        }

        public String service_str(short svc)
        {
            String os = null;
            if (svc >= 100)
            {
                os = x[svc & 3] + ".";
                int m = svc >> 2;
                int l = m / 100;
                os += l + "." + m % 100;
            }
            else
            {
                os = "wallet";
            }
            return os;
        }
    }
}

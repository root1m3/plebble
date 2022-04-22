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
package us;
import java.math.BigInteger;                                                                   // BigInteger
import java.nio.ByteBuffer;                                                                    // ByteBuffer
import java.nio.ByteOrder;                                                                     // ByteOrder
import java.lang.Comparable;                                                                   // Comparable
import java.lang.Long;                                                                         // Long

public class stdint {

    public static class int8_t {

        public static int size() { return 1; }

        public int8_t() {
        }

        public int8_t(byte v) {
            value = v;
        }

        public int write(byte[] dest, int pos) {
            dest[pos++] = value;
            return pos;
        }

        public int read(byte[] src, int pos) {
            value = src[pos++];
            return pos;
        }

        public static int8_t from(byte[] src, int pos) {
            int8_t o = new int8_t();
            o.read(src, pos);
            return o;
        }

        public byte value = 0;
    }

    public static class uint8_t {

        public static int size() { return 1; }

        public uint8_t() {
        }

        public uint8_t(short v) {
            value = (short)(v & 0x00FF);
        }

        public uint8_t(uint8_t v) {
            value = v.value;
        }

        public int write(byte[] dest, int pos) {
            assert value < 0x0100;
            dest[pos++] = (byte)(value);
            return pos;
        }

        public int read(byte[] src, int pos) {
            value = (short)(src[pos++] & 0x00FF);
            return pos;
        }

        public static uint8_t from(byte[] src, int pos) {
            uint8_t o = new uint8_t();
            o.read(src, pos);
            return o;
        }

        public short value = 0;
    }

    public static class uint16_t {

        public static int size() { return 2; }

        public uint16_t() {
        }

        public uint16_t(int v) {
            value = v & 0x0000FFFF;
        }

        public uint16_t(uint16_t v) {
            value = v.value;
        }

        public int write(byte[] dest, int pos) {
            assert value < 0x00010000;
            byte[] t = new byte[4];
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            bb.asIntBuffer().put(value);
            dest[pos++] = t[0];
            dest[pos++] = t[1];
            return pos;
        }

        public int read(byte[] src, int pos) {
            byte[] t = new byte[4];
            t[0] = src[pos++];
            t[1] = src[pos++];
            t[2] = t[3] = 0;
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            value = bb.getInt();
            return pos;
        }

        public static uint16_t from(byte[] src, int pos) {
            uint16_t o = new uint16_t();
            o.read(src, pos);
            return o;
        }

        public int value = 0;
    }

    public static class uint32_t {

        public static int size() { return 4; }

        public uint32_t() {
        }

        public uint32_t(long v) {
            value = v & 0x00000000FFFFFFFFL;
        }

        public uint32_t(String v) {
            value = Long.parseUnsignedLong(v);
        }

        public uint32_t(uint32_t v) {
            value = v.value;
        }

        public int write(byte[] dest, int pos) {
            assert value < 0x0000000100000000L;
            byte[] t = new byte[8];
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            bb.asLongBuffer().put(value);
            dest[pos++] = t[0];
            dest[pos++] = t[1];
            dest[pos++] = t[2];
            dest[pos++] = t[3];
            return pos;
        }

        public int read(byte[] src, int pos) {
            byte[] t = new byte[8];
            t[0] = src[pos++];
            t[1] = src[pos++];
            t[2] = src[pos++];
            t[3] = src[pos++];
            t[4] = t[5] = t[6] = t[7] = 0;
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            value = bb.getLong();
            assert value < 0x0000000100000000L;
            return pos;
        }

        public static uint32_t from(byte[] src, int pos) {
            uint32_t o = new uint32_t();
            o.read(src, pos);
            return o;
        }

        public long value = 0;
    }

    public static class int32_t {

        public static int size() { return 4; }

        public int32_t() {
        }

        public int32_t(long v) {
            value = v & 0x00000000FFFFFFFFL;
        }

        public int32_t(String v) {
            value = Long.parseLong(v);
        }

        public int32_t(int32_t v) {
            value = v.value;
        }

        public int write(byte[] dest, int pos) {
            assert value < 0x0000000100000000L;
            byte[] t = new byte[8];
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            bb.asLongBuffer().put(value);
            dest[pos++] = t[0];
            dest[pos++] = t[1];
            dest[pos++] = t[2];
            dest[pos++] = t[3];
            return pos;
        }

        public int read(byte[] src, int pos) {
            byte[] t = new byte[8];
            t[0] = src[pos++];
            t[1] = src[pos++];
            t[2] = src[pos++];
            t[3] = src[pos++];
            t[4] = t[5] = t[6] = t[7] = 0;
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            value = bb.getLong();
            assert value < 0x0000000100000000L;
            return pos;
        }

        public static int32_t from(byte[] src, int pos) {
            int32_t o = new int32_t();
            o.read(src, pos);
            return o;
        }

        public long value = 0;
    }

    public static class uint64_t implements Comparable<uint64_t> {

        public static int size() { return 8; }

        public uint64_t() {
        }

        public uint64_t(String v) {
            value = Long.parseUnsignedLong(v);
        }

        public uint64_t(long v) {
            value = v >= 0L ? v : 0L;
        }

        public uint64_t(uint64_t v) {
            value = v.value;
        }

        public int write(byte[] dest, int pos) {
            byte[] t = new byte[8];
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            bb.asLongBuffer().put(value);
            for (int i = 0; i < 8; ++i) {
                dest[pos++] = t[i];
            }
            return pos;
        }

        public int read(byte[] src, int pos) {
            byte[] t = new byte[8];
            for (int i = 0; i < 8; ++i) {
                t[i] = src[pos++];
            }
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            value = bb.getLong();
            return pos;
        }

        public static uint64_t from(byte[] src, int pos) {
            uint64_t o = new uint64_t();
            o.read(src, pos);
            return o;
        }

        public String to_string() {
            return Long.toUnsignedString(value);
        }

        @Override public int compareTo(uint64_t other) {
            if (value == other.value) return 0;
            return value < other.value ? -1 : 1;
        }

        public long value = 0;
   }

    public static class int64_t {

        public static int size() { return 8; }

        public int64_t() {
        }

        public int64_t(long v) {
            value = v;
        }

        public int64_t(int64_t v) {
            value = v.value;
        }

        public int write(byte[] dest, int pos) {
            byte[] t = new byte[8];
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            bb.asLongBuffer().put(value);
            for (int i = 0; i < 8; ++i) {
                dest[pos++] = t[i];
            }
            return pos;
        }

        public int read(byte[] src, int pos) {
            byte[] t = new byte[8];
            for (int i = 0; i < 8; ++i) {
                t[i] = src[pos++];
            }
            ByteBuffer bb = ByteBuffer.wrap(t);
            bb.order(ByteOrder.LITTLE_ENDIAN);
            value = bb.getLong();
            return pos;
        }

        public static int64_t from(byte[] src, int pos) {
            int64_t o = new int64_t();
            o.read(src, pos);
            return o;
        }

        public long value = 0;
   }
}


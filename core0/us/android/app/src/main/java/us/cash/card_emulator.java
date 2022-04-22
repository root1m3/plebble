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
import java.util.Arrays;                                                                       // Arrays
import android.os.Binder;                                                                      // Binder
import android.os.Bundle;                                                                      // Bundle
import android.nfc.cardemulation.HostApduService;                                              // HostApduService
import android.os.IBinder;                                                                     // IBinder
import android.content.Intent;                                                                 // Intent
import android.util.Log;                                                                       // Log

public class card_emulator extends HostApduService {

    public card_emulator() {
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if(intent.getExtras() != null){
            message = intent.getExtras().getString("message");
            Log.d("INTENT", message);
        }
        return START_NOT_STICKY;
    }

    public static byte[] hex_2_byte_array(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i+1), 16));
        }
        return data;
    }

    public static String byte_array_2_hex(byte[] bytes) {
        final char[] hex_array = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
        char[] hex = new char[bytes.length * 2]; // Each byte has two hex characters (nibbles)
        int v;
        for (int j = 0; j < bytes.length; j++) {
            v = bytes[j] & 0xFF; // Cast bytes[j] to int, treating as unsigned value
            hex[j * 2] = hex_array[v >>> 4]; // Select hex character from upper nibble
            hex[j * 2 + 1] = hex_array[v & 0x0F]; // Select hex character from lower nibble
        }
        return new String(hex);
    }

    @Override public void onDeactivated(int reason) {
        Log.d(TAG, "Deactivated: " + reason);
    }

    boolean checkAID(byte[] command_apdu) {
        for (int i=0; i<AID.length; ++i) {
            if (command_apdu[5+i] != AID[i]) return false;
        }
        return true;
    }

    @Override public byte[] processCommandApdu(byte[] command_apdu, Bundle extras) {
        if (command_apdu == null) {
            return STATUS_FAILED;
        }
        if (command_apdu.length < MIN_APDU_LENGTH) {
            return STATUS_FAILED;
        }
        if (command_apdu[0] != CLA) {
            return CLA_NOT_SUPPORTED;
        }
        if (command_apdu[1] != INS) {
            return CLA_NOT_SUPPORTED;
        }
        if (!checkAID(command_apdu)) return STATUS_FAILED;
        return concat_arrays(message.getBytes(),STATUS_SUCCESS);
    }

    public static byte[] concat_arrays(byte[] first, byte[]... rest) {
        int totalLength = first.length;
        for (byte[] array : rest) {
            totalLength += array.length;
        }
        byte[] result = Arrays.copyOf(first, totalLength);
        int offset = first.length;
        for (byte[] array : rest) {
            System.arraycopy(array, 0, result, offset, array.length);
            offset += array.length;
        }
        return result;
    }

    public static String TAG = "Host Card Emulator";
    public static byte STATUS_SUCCESS[] = hex_2_byte_array("9000");
    public static byte STATUS_FAILED[] = hex_2_byte_array("6F00");
    public static byte CLA_NOT_SUPPORTED[] = hex_2_byte_array("6E00");
    public static byte INS_NOT_SUPPORTED[] = hex_2_byte_array("6D00");
    public static byte AID[] = hex_2_byte_array("F000016672"); //https://stackoverflow.com/questions/27877373/how-to-get-aid-for-reader-host-based-card-emulation
    public static byte INS = (byte)0xA4;
    public static byte CLA = (byte)0x00;
    public static byte LE = (byte)0x00;
    public static int MIN_APDU_LENGTH = 10;
    String message="";
}



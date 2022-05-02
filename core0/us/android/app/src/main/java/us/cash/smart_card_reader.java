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
package us.cash;
import android.app.Activity;                                                                   // Activity
import java.util.Arrays;                                                                       // Arrays
import android.content.Context;                                                                // Context
import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
import java.io.IOException;                                                                    // IOException
import android.nfc.tech.IsoDep;                                                                // IsoDep
import android.util.Log;                                                                       // Log
import android.nfc.NfcAdapter;                                                                 // NfcAdapter
import android.nfc.Tag;                                                                        // Tag
import android.widget.Toast;                                                                   // Toast

public class smart_card_reader implements NfcAdapter.ReaderCallback {
    main_activity a;
    NfcAdapter nfc_adapter;
    boolean reading=false;

    boolean is_reading() {
        return reading;
    }

    public smart_card_reader(main_activity a_) {
        a=a_;
        nfc_adapter = NfcAdapter.getDefaultAdapter(a);
        if (nfc_adapter == null) {
            return;
        }
    }

    boolean nfc_available() {
        return nfc_adapter != null;
    }

    byte[] apdu_command() {
        int sz=4+1+card_emulator.AID.length+1;
        byte[] r=new byte[sz];
        r[0]=card_emulator.CLA;
        r[1]=card_emulator.INS;
        r[2]=(byte)0x04;
        r[3]=(byte)0x00;
        r[4]=(byte)(card_emulator.AID.length);
        for (int i=0; i<card_emulator.AID.length; ++i) {
            r[5+i]=card_emulator.AID[i];
        }
        r[sz-1]=card_emulator.LE;
        return r;
    }

    @Override
    public void onTagDiscovered(Tag tag) {
//        Log.d("us.cash", "onTagDiscovered "+tag.toString());
        String addr="";
//        Log.d("us.cash", "APDUCommand "+HostCardEmulatorService.ByteArrayToHexString(APDUCommand()));
        try {
            IsoDep isoDep = IsoDep.get(tag);
            isoDep.connect();
            //00A4040007A0000002471001
            //isoDep.getMaxTransceiveLength()
            byte[] response = isoDep.transceive(apdu_command());
            if (response.length>2) {
//                Log.d("SmartCardReader", String.valueOf(response.length));
//                Log.d("SmartCardReader", String.valueOf(response[response.length-2]));
//                Log.d("SmartCardReader", String.valueOf(response[response.length-1]));
                if (response[response.length-2]==card_emulator.STATUS_SUCCESS[0] && response[response.length-1]==card_emulator.STATUS_SUCCESS[1]) {
//                    Log.d("SmartCardReader","B ");
                    response = Arrays.copyOf(response, response.length-2);
                    final String r=new String(response, "UTF-8");
//                    Log.d("SmartCardReader",r);
                    a.runOnUiThread(new Thread(new Runnable() {
                        public void run() {
                            //a.deliver_address_from_NFC(r);
                        }
                    }));
                }
            }
//            Log.d("SmartCardReader","CardResponse: " + HostCardEmulatorService.ByteArrayToHexString(response));
            isoDep.close();
        }
        catch(IOException e) {
            Log.d("us.cash", "onTagDiscovered "+tag.toString()+ "crashed "+e.getMessage());
            error_manager.manage(e, e.getMessage() + "    " + e.toString());
        }
    }

    public void disable() {
        if (!nfc_available()) return;
        if (reading) {
            nfc_adapter.disableReaderMode(a);
            reading = false;
        }
    }

    public void enable() {
        if (!nfc_available()) return;
        if (!reading) {
            nfc_adapter.enableReaderMode(a, this, NfcAdapter.FLAG_READER_NFC_A | NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK, null);
            reading = true;
        }
    }
}


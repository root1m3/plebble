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
package us.wearable_JClife.ble;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.text.TextUtils;

public class BleManager {

    private BleManager(Context context) {
        this.context = context;
        final BleManager i = this;
        serviceConnection = new ServiceConnection() {
            @Override public void onServiceDisconnected(ComponentName name) {// TODO Auto-generated method stub
                bleService = null;
            }

            @Override public void onServiceConnected(ComponentName name, IBinder service) {
                bleService = ((BleService.LocalBinder) service).getService();
                if (!TextUtils.isEmpty(address)) {
                    bleService.initBluetoothDevice(address, i.context);
                }
            }
        };

        if (serviceIntent == null) {
            serviceIntent = new Intent(context, BleService.class);
            context.bindService(serviceIntent, serviceConnection, Service.BIND_AUTO_CREATE);
        }
        BluetoothManager bluetoothManager = (BluetoothManager)context.getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = bluetoothManager.getAdapter();
    }

    public static void init(Context context) {
        if (ourInstance == null) {
            synchronized (BleManager.class) {
                if (ourInstance == null) {
                    ourInstance = new BleManager(context);
                }
            }
        }
    }

    public  boolean isBleEnable(){
        return bluetoothAdapter.enable();
    }

    public static BleManager getInstance() {
        return ourInstance;
    }

    public void connectDevice(String address) {
        if (!bluetoothAdapter.isEnabled() || TextUtils.isEmpty(address) || isConnected()) return;
        if (bleService == null) {
            this.address = address;
        }
        else {
            bleService.initBluetoothDevice(address, this.context);
        }
    }

    public void enableNotifaction() {
        if (bleService == null) return;
        bleService.setCharacteristicNotification(true);
    }

    public void writeValue(byte[]value) {
        if (bleService == null || ourInstance == null || !isConnected()) return;
        bleService.writeValue(value);
    }

    public void offerValue(byte[]data) {
        if (bleService == null) return;
        bleService.offerValue(data);
    }

    public void writeValue(){
        if (bleService == null) return;
        bleService.nextQueue();
    }
    public void disconnectDevice(){
        if (bleService == null) return;
        bleService.disconnect();
    }

    public boolean isConnected(){
        if (bleService == null) return false;
        return bleService.isConnected();
    }

    private static BleManager ourInstance;
    private String address;
    private BleService bleService;
    private Intent serviceIntent;
    BluetoothAdapter bluetoothAdapter;
    Context context;
    private ServiceConnection serviceConnection;
}


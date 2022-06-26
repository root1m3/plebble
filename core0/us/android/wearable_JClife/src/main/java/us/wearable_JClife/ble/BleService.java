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
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;
import com.jstyle.blesdk2025.Util.BleSDK;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.UUID;
import us.wearable_JClife.util.BleData;
import us.wearable_JClife.util.ResolveData;
import us.wearable_JClife.util.RxBus;

public final class BleService extends Service {

    private static final String TAG = "BleService";
    private static final UUID NOTIY = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    private static final UUID SERVICE_DATA = UUID.fromString("0000fff0-0000-1000-8000-00805f9b34fb");
    private static final UUID DATA_Characteristic = UUID.fromString("0000fff6-0000-1000-8000-00805f9b34fb");
    private static final UUID NOTIY_Characteristic = UUID.fromString("0000fff7-0000-1000-8000-00805f9b34fb");
    public final static String ACTION_GATT_onDescriptorWrite = "com.jstylelife.ble.service.onDescriptorWrite";
    public final static String ACTION_GATT_CONNECTED = "com.jstylelife.ble.service.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED = "com.jstylelife.ble.service.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_DATA_AVAILABLE = "com.jstylelife.ble.service.ACTION_DATA_AVAILABLE";
    public static BluetoothGattCharacteristic colorCharacteristic;
    private static ArrayList<BluetoothGatt> arrayGatts = new ArrayList<BluetoothGatt>(); // 存放BluetoothGatt的集�? / Store the collection of BluetoothGatt?

    @Override public IBinder onBind(Intent intent) {
        initAdapter();
        return kBinder;
    }

    @Override public boolean onUnbind(Intent intent) {
        return super.onUnbind(intent);
    }

    public void initBluetoothDevice(final String address, final Context context) {
        this.address = address;
        final BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        if (isConnected()) return;
        if (null != mGatt) {
            refreshDeviceCache(mGatt);
            mGatt = null;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            mGatt = device.connectGatt(context, false, bleGattCallback, BluetoothDevice.TRANSPORT_LE);
        }
        else {
            mGatt = device.connectGatt(context, false, bleGattCallback);
        }

        if (mGatt == null) {
            System.out.println(device.getAddress() + "gatt is null");
        }

        mLeScanCallback = new BluetoothAdapter.LeScanCallback() {

            @Override public void onLeScan(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {
                if (device.getAddress().equals(address) && Math.abs(rssi) < 90) {
    //				//String name = Tools.decodeDeviceName(scanRecord);
    //				if (!TextUtils.isEmpty(name) && name.equals("DfuTarg"))
    //					return;
    //				if (mGatt != null)
    //					return;
    //				handler.post(new Runnable() {
    //
    //					@Override
    //					public void run() {
    //						// TODO Auto-generated method stub
    //						mGatt = (mBluetoothAdapter.getRemoteDevice(device
    //								.getAddress())).connectGatt(mContext, false,
    //								bleGattCallback);
    //						if (mGatt == null) {
    //							System.out.println("gatt is null ");
    //						}
    //					}
    //				});

                }
            }
        };

    }

    public String getDeviceAddress() {
        return this.address;
    }

    private void initAdapter() {
        if (bluetoothManager == null) {
            bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            if (bluetoothManager == null) {
                return;
            }
        }
        mBluetoothAdapter = bluetoothManager.getAdapter();
    }

    public void disconnect() {
        NeedReconnect = false;
        if (mGatt == null) return;
        mGatt.disconnect();
    }

    public void disconnect(String address) {
        ArrayList<BluetoothGatt> gatts = new ArrayList<BluetoothGatt>();
        for (BluetoothGatt gatt: arrayGatts) {
            if (gatt != null && gatt.getDevice().getAddress().equals(address)) {
                gatts.add(gatt);
                gatt.close();
            }
        }
        arrayGatts.removeAll(gatts);
    }

    public class LocalBinder extends Binder {
        public BleService getService() {
            return BleService.this;
        }
    }

    private int discoverCount;
    private Object ob = new Object();
    private BluetoothGattCallback bleGattCallback = new BluetoothGattCallback() {
        // @see android.bluetooth.BluetoothGattCallback#onConnectionStateChange(android.bluetooth.BluetoothGatt, int, int)
        @Override public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            String action = null;
            Log.i(TAG, "onConnectionStateChange:  status" + status + " newstate " + newState);
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                if (status == 133) {
                    mGatt.close();
                    mGatt = null;
                    return;
                }
                action = ACTION_GATT_CONNECTED;
                try {
                    gatt.discoverServices();

                }
                catch (Exception e) {
                    e.printStackTrace(); // TODO: handle exception
                }

            }
            else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                isConnected = false;
                Log.i(TAG, "onConnectionStateChange: " + ACTION_GATT_DISCONNECTED);
                if (mGatt != null) {
                    mGatt.close();
                    mGatt = null;
                }
                queues.clear();
                if (!NeedReconnect) {
                    action = ACTION_GATT_DISCONNECTED;
                    broadcastUpdate(action);
                }

            }
        }

        // @see android.bluetooth.BluetoothGattCallback#onServicesDiscovered(android.bluetooth.BluetoothGatt, int)
        @Override public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                String address = gatt.getDevice().getAddress();
                String name = mBluetoothAdapter.getRemoteDevice(address).getName();
                setCharacteristicNotification(true);
                discoverCount = 0;
            }
            else {
                Log.w("servieDiscovered", "onServicesDiscovered received: " + status);
            }
        }

        @Override public void onMtuChanged(BluetoothGatt gatt, int mtu, int status) {
            super.onMtuChanged(gatt, mtu, status);
            if (BluetoothGatt.GATT_SUCCESS == status) {
                setCharacteristicNotification(true);
            }
            else {
                gatt.requestMtu(153);
            }
        }

        // @see android.bluetooth.BluetoothGattCallback#onCharacteristicRead(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic, int)
        public void onCharacteristicRead(BluetoothGatt gatt, android.bluetooth.BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic, gatt.getDevice().getAddress());
            }
            else {

            }
        }

        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                offerValue(BleSDK.disableAncs());
                nextQueue();
                isConnected = true;
                broadcastUpdate(ACTION_GATT_onDescriptorWrite);
            }
            else{
                Log.i(TAG, "onDescriptorWrite: failed");
            }
        }

        // @see android.bluetooth.BluetoothGattCallback#onCharacteristicChanged(android.bluetooth.BluetoothGatt, android.bluetooth.BluetoothGattCharacteristic)         */
        public void onCharacteristicChanged(BluetoothGatt gatt, android.bluetooth.BluetoothGattCharacteristic characteristic) {
            if (mGatt == null) return;
            Log.i(TAG, "onCharacteristicChanged: " + ResolveData.byte2Hex(characteristic.getValue()));
            broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic, gatt.getDevice().getAddress());
        }

        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                nextQueue();
            }
        }

    };

    public boolean refreshDeviceCache(BluetoothGatt gatt) {
        try {
            BluetoothGatt localBluetoothGatt = gatt;
            Method localMethod = localBluetoothGatt.getClass().getMethod("refresh", new Class[0]);
            if (localMethod != null) {
                boolean bool = ((Boolean) localMethod.invoke(localBluetoothGatt, new Object[0])).booleanValue();
                return bool;
            }
        }
        catch (Exception localException) {
            Log.e("s", "An exception occured while refreshing device");
        }
        return false;
    }

    private void broadcastUpdate(String action) {
        BleData bleData = new BleData();
        bleData.setAction(action);
        RxBus.getInstance().post(bleData);
    }

    private void broadcastUpdate(String action, BluetoothGattCharacteristic characteristic, String mac) {
        byte[] data = characteristic.getValue();
        BleData bleData = new BleData();
        bleData.setAction(action);
        bleData.setValue(data);
        RxBus.getInstance().post(bleData);
    }

    public void readValue(BluetoothGattCharacteristic characteristic) {
        if (mGatt == null) return;
        mGatt.readCharacteristic(characteristic);
    }

    public void writeValue(byte[] value) {
        if (mGatt == null || value == null) return;
        BluetoothGattService service = mGatt.getService(SERVICE_DATA);
        if (service == null) return;
        BluetoothGattCharacteristic characteristic = service.getCharacteristic(DATA_Characteristic);
        if (characteristic == null) return;
        if (value[0] == (byte) 0x47) {
            NeedReconnect = false;
        }
        characteristic.setValue(value);
        Log.i(TAG, "writeValue: "+ ResolveData.byte2Hex(value));
        mGatt.writeCharacteristic(characteristic);
    }

    public void setCharacteristicNotification(boolean enable) {
        if (mGatt == null) return;
        BluetoothGattService service = mGatt.getService(SERVICE_DATA);
        if (service == null) return;
        BluetoothGattCharacteristic characteristic = service.getCharacteristic(NOTIY_Characteristic);
        if (characteristic == null) return;
        mGatt.setCharacteristicNotification(characteristic, enable);
        try {
            Thread.sleep(20);
        }
        catch (InterruptedException e) {
            e.printStackTrace();
        }
        BluetoothGattDescriptor descriptor = characteristic.getDescriptor(NOTIY);
        if (descriptor == null) {
            return;
        }
        descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
        if (mGatt == null) return;
        mGatt.writeDescriptor(descriptor);
    }

    public List<BluetoothGattService> getSupportedGattServices() {
        if (mGatt == null) {
            return null;
        }
        return mGatt.getServices();
    }

    private BluetoothGatt getBluetoothGatt(BluetoothDevice device) {
        return mGatt;
    }

    public void readRssi(BluetoothDevice device) {
        mGatt.readRemoteRssi();
    }

    @Override public void onDestroy() {
        super.onDestroy();
    }

    Queue<byte[]> queues = new LinkedList<>();

    public void offerValue(byte[]value) {
        queues.offer(value);
    }

    public void nextQueue(){
        final Queue<byte[]> requests = queues;
        byte[] data = requests != null ? requests.poll() : null;
        writeValue(data);
    }

    public boolean isConnected(){
        return this.isConnected;
    }

    private boolean NeedReconnect = false;
    public HashMap<BluetoothDevice, BluetoothGatt> hasp = new HashMap<BluetoothDevice, BluetoothGatt>();
    private final IBinder kBinder = new LocalBinder();
    private HashMap<String, BluetoothGatt> gattHash = new HashMap<String, BluetoothGatt>();
    private BluetoothManager bluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothGatt mGatt;
    private boolean isConnected;
    private String address;
    private BluetoothAdapter.LeScanCallback mLeScanCallback;
}


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
package us.wearable_JClife;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import com.jstyle.blesdk2025.model.ExtendedBluetoothDevice;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
//import butterknife.ButterKnife;
//import butterknife.BindView;
import us.wearable_JClife.util.PermissionsUtil;
import us.wearable_JClife.util.ResolveData;
import us.wearable_JClife.R;

public class DeviceScanActivity extends AppCompatActivity implements PermissionsUtil.PermissionListener {

    @Override public void onCreate(Bundle savedInstanceState) {
        setTheme(R.style.AppBarTheme);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_scan);
//        ButterKnife.bind(this);
        listView = findViewById(R.id.list_view);
        mHandler = new Handler();
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }
        final BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            finish();
            return;
        }
        PermissionsUtil.requestPermissions(this, this, Manifest.permission.ACCESS_FINE_LOCATION);
        Toolbar tb = findViewById(R.id.ble_toolbar);
        setSupportActionBar(tb);
    }

    @Override public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        if (!mScanning) {
            menu.findItem(R.id.menu_stop).setVisible(false);
            menu.findItem(R.id.menu_scan).setVisible(true);
            menu.findItem(R.id.menu_refresh).setActionView(null);
        }
        else {
            menu.findItem(R.id.menu_stop).setVisible(true);
            menu.findItem(R.id.menu_scan).setVisible(false);
            menu.findItem(R.id.menu_refresh).setActionView(R.layout.actionbar_indeterminate_progress);
        }
        return true;
    }

    public static String assetsPath = "J1668_06_V011_3_20180326.zip";

    private void copyFileFromAssets(String filePath) {
        File file = new File(getFilesDir().getAbsolutePath());
        try {
            if (!file.exists()) {
                file.mkdirs();
            }
            File fileOTA = new File(file, "ota");
            if (!fileOTA.exists()) {
                fileOTA.mkdir();
            }
            File fileMusic = new File(fileOTA, assetsPath);
            if (fileMusic.exists()) {
                return;
            } 
            else {
                fileMusic.createNewFile();
            }
            AssetManager assetManager = getAssets();
            InputStream inputStream = assetManager.open(assetsPath);
            OutputStream outputStream = new FileOutputStream(fileMusic);
            byte[] buffer = new byte[1024];
            while (inputStream.read(buffer) > 0) {
                outputStream.write(buffer);
            }
            outputStream.flush();
            inputStream.close();
            outputStream.close();
        }
        catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override public boolean onOptionsItemSelected(MenuItem item) {
        final int i = item.getItemId();
        if (i == R.id.menu_scan) {
            PermissionsUtil.requestPermissions(this, this, Manifest.permission.ACCESS_FINE_LOCATION);
        }
        else if (i == R.id.menu_stop) {
            scanLeDevice(false);
        }
        else if (i == R.id.menu_filter) {
        }
        return true;
    }

    private void changeFilter() {
    }

    @Override protected void onResume() {
        super.onResume();
        if (!mBluetoothAdapter.isEnabled()) { // Ensures Bluetooth is enabled on the device. If Bluetooth is not currently enabled, fire an intent to display a dialog asking the user to grant permission to enable it.
            if (!mBluetoothAdapter.isEnabled()) {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            }
        }
        mLeDeviceListAdapter = new LeDeviceListAdapter();
        //listView = findViewById(R2.id.list_view);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int position, long l) {
                mBluetoothAdapter.stopLeScan(mLeScanCallback);
                final BluetoothDevice device = mLeDeviceListAdapter.getDevice(position);
                if (device == null)
                    return;
                String name = mLeDeviceListAdapter.getName(position);
                if (mScanning) {
                    scanLeDevice(false);
                }
                final Intent intent = new Intent(DeviceScanActivity.this, BleActivity.class);
                intent.putExtra("address", device.getAddress());
                intent.putExtra("name", name);
                startActivity(intent);
            }
        });
        setListAdapter(mLeDeviceListAdapter);
        Set<BluetoothDevice> devices = mBluetoothAdapter.getBondedDevices();
        List<ExtendedBluetoothDevice> list = new ArrayList<>();
        for (BluetoothDevice device : devices) {
            list.add(new ExtendedBluetoothDevice(device));
        }
        mLeDeviceListAdapter.addBondDevice(list);
        scanLeDevice(true);
    }

    private void setListAdapter(LeDeviceListAdapter mLeDeviceListAdapter) {
        listView.setAdapter(mLeDeviceListAdapter);
    }

    @Override protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_ENABLE_BT && resultCode == Activity.RESULT_CANCELED) { // User chose not to enable Bluetooth.
            finish();
            return;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override protected void onPause() {
        super.onPause();
        scanLeDevice(false);
        extendedBluetoothDevices.clear();
        mLeDeviceListAdapter.clear();
    }

    private void scanLeDevice(final boolean enable) {
        if (enable) {
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mBluetoothAdapter.stopLeScan(mLeScanCallback);
                    mScanning = false;
                    invalidateOptionsMenu();
                }
            }, 12000);
            mBluetoothAdapter.startLeScan(mLeScanCallback);
            mScanning = true;
        }
        else {
            if (!mScanning) return;
            mBluetoothAdapter.stopLeScan(mLeScanCallback);
            mHandler.removeCallbacksAndMessages(null);
            mScanning = false;
        }
        invalidateOptionsMenu();
    }

    int filterRssi = -100;
    private List<ExtendedBluetoothDevice> extendedBluetoothDevices = new ArrayList<>();

    private ExtendedBluetoothDevice findDevice(final BluetoothDevice device) {
        for (final ExtendedBluetoothDevice mDevice : extendedBluetoothDevices) {
            if (mDevice.matches(device)) return mDevice;
        }
        return null;
    }

    public void addDevice(BluetoothDevice device, String name, int rssi) {
        ExtendedBluetoothDevice bluetoothDevice = findDevice(device);
        if (bluetoothDevice == null) {
            extendedBluetoothDevices.add(new ExtendedBluetoothDevice(device, name, rssi));
        }
        else {
            bluetoothDevice.rssi = rssi;
        }
    }

    @Override public void granted(String name) {
        if (Manifest.permission.WRITE_EXTERNAL_STORAGE.equals(name)) {
        }
        else if (Manifest.permission.ACCESS_FINE_LOCATION.equals(name)) {
            if (mLeDeviceListAdapter != null) mLeDeviceListAdapter.clear();
            if (extendedBluetoothDevices != null) extendedBluetoothDevices.clear();
            scanLeDevice(true);
        }
    }

    @Override public void NeverAskAgain() {
    }

    @Override public void disallow(String name) {
    }

    private class LeDeviceListAdapter extends BaseAdapter {
        private List<ExtendedBluetoothDevice> deviceList;
        private LayoutInflater mInflator;
        int filterRssi;

        public void setDeviceList(List<ExtendedBluetoothDevice> deviceList) {
            this.deviceList = deviceList;
            notifyDataSetChanged();
        }

        public void setFilterRssi(int rssi) {
            this.filterRssi = rssi;
        }

        public LeDeviceListAdapter() {
            super();
            deviceList = new ArrayList<>();
            mInflator = DeviceScanActivity.this.getLayoutInflater();
        }

        public void addBondDevice(List<ExtendedBluetoothDevice> list) {
            deviceList.addAll(list);
            notifyDataSetChanged();
        }

        public void addDevice(BluetoothDevice device, String name, int rssi) {
            ExtendedBluetoothDevice bluetoothDevice = findDevice(device);
            if (bluetoothDevice == null) {
                deviceList.add(new ExtendedBluetoothDevice(device, name, rssi));
            } else {
                bluetoothDevice.rssi = rssi;
            }
        }

        private ExtendedBluetoothDevice findDevice(final BluetoothDevice device) {
            for (final ExtendedBluetoothDevice mDevice : deviceList) {
                if (mDevice.matches(device)) return mDevice;
            }
            return null;
        }

        public BluetoothDevice getDevice(int position) {
            return deviceList.get(position).device;
        }

        public String getName(int position) {
            return deviceList.get(position).name;
        }

        public void clear() {
            deviceList.clear();
        }

        @Override public int getCount() {
            return deviceList == null ? 0 : deviceList.size();
        }

        @Override public Object getItem(int i) {
            return deviceList.get(i);
        }

        @Override public long getItemId(int i) {
            return i;
        }

        @Override public View getView(int i, View view, ViewGroup viewGroup) {
            ViewHolder viewHolder;
            if (view == null) { // General ListView optimization code.
                view = mInflator.inflate(R.layout.listitem_device, null);
                viewHolder = new ViewHolder();
                viewHolder.deviceAddress = view.findViewById(R.id.device_address);
                viewHolder.deviceName = view.findViewById(R.id.device_name);
                viewHolder.deviceRssi = view.findViewById(R.id.device_rssi);
                view.setTag(viewHolder);
            }
            else {
                viewHolder = (ViewHolder) view.getTag();
            }
            ExtendedBluetoothDevice extendedBluetoothDevice = deviceList.get(i);
            BluetoothDevice device = extendedBluetoothDevice.device;
            final String deviceName = extendedBluetoothDevice.name;
            if (deviceName != null && deviceName.length() > 0) {
                viewHolder.deviceName.setText(deviceName);
            }
            else {
                viewHolder.deviceName.setText(R.string.unknown_device);
            }
            viewHolder.deviceAddress.setText(device.getAddress());
            if (extendedBluetoothDevice.rssi == ExtendedBluetoothDevice.NO_RSSI) {
                viewHolder.deviceRssi.setText("Bonded");
            }
            else {
                viewHolder.deviceRssi.setText("Rssi:" + extendedBluetoothDevice.rssi);
            }
            return view;
        }
    }

    // Device scan callback.
    private BluetoothAdapter.LeScanCallback mLeScanCallback = new BluetoothAdapter.LeScanCallback() {
        @Override
        public void onLeScan(final BluetoothDevice device, final int rssi, final byte[] scanRecord) {
            runOnUiThread(new Runnable() {
                @Override public void run() {
                    String deviceName = device.getName();
                    if (TextUtils.isEmpty(deviceName)) {
                        deviceName = ResolveData.decodeDeviceName(scanRecord);
                    }
                    if (TextUtils.isEmpty(deviceName)) {
                        deviceName = getString(R.string.unknown_device);
                    }
                    addDevice(device, deviceName, rssi);
                    if (rssi > filterRssi) {
                        mLeDeviceListAdapter.addDevice(device, deviceName, rssi);
                        mLeDeviceListAdapter.notifyDataSetChanged();
                    }
                }
            });
        }
    };

    static class ViewHolder {
        TextView deviceName;
        TextView deviceAddress;
        TextView deviceRssi;
    }

    ListView listView;
//    @BindView(R2.id.list_view) ListView listView;
    private LeDeviceListAdapter mLeDeviceListAdapter;
    private BluetoothAdapter mBluetoothAdapter;
    private boolean mScanning;
    private Handler mHandler;
    private static final int REQUEST_ENABLE_BT = 1;

}


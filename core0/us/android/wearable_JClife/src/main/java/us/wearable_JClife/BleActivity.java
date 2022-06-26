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
import android.app.ProgressDialog;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.TextView;
import androidx.appcompat.widget.SwitchCompat;
import com.jstyle.blesdk2025.Util.BleSDK;
import com.jstyle.blesdk2025.constant.BleConst;
import com.jstyle.blesdk2025.constant.DeviceKey;
import java.util.Map;
//import butterknife.BindArray;
//import butterknife.BindView;
//import butterknife.ButterKnife;
//import butterknife.OnClick;
import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.Disposable;
import io.reactivex.functions.Consumer;
import io.reactivex.schedulers.Schedulers;
import us.wearable_JClife.activityBle.BaseActivity;
import us.wearable_JClife.ble.BleManager;
import us.wearable_JClife.ble.BleService;
import us.wearable_JClife.util.BleData;
import us.wearable_JClife.util.RxBus;
import us.wearable_JClife.R;

public class BleActivity extends BaseActivity {
    private static final String TAG = "MainActivity";

/*
    @BindArray(R2.array.item_options)    String[] options;
    @BindView(R2.id.BT_CONNECT)    Button btConnect;
    @BindView(R2.id.button_startreal)    Button buttonStartreal;
    @BindView(R2.id.textView_step)    TextView textViewStep;
    @BindView(R2.id.textView_cal)    TextView textViewCal;
    @BindView(R2.id.textView_distance)    TextView textViewDistance;
    @BindView(R2.id.textView_time)    TextView textViewTime;
    @BindView(R2.id.textView1)    TextView textView1;
    @BindView(R2.id.textView_heartValue)    TextView textViewHeartValue;
    @BindView(R2.id.textView_tempValue)    TextView textViewTempValue;
    @BindView(R2.id.textView_ActiveTime)    TextView textViewActiveTime;
    @BindView(R2.id.SwitchCompat_temp)    SwitchCompat SwitchCompatTemp;
*/
    String[] options;
    Button btConnect;
    Button buttonStartreal;
    TextView textViewStep;
    TextView textViewCal;
    TextView textViewDistance;
    TextView textViewTime;
    TextView textView1;
    TextView textViewHeartValue;
    TextView textViewTempValue;
    TextView textViewActiveTime;
    SwitchCompat SwitchCompatTemp;

    private ProgressDialog progressDialog;
    private Disposable subscription;
    private String address;
    boolean isStartReal;
    public static int phoneDataLength = 200; //手机一个包能发送的最多数据 / The maximum amount of data a mobile phone can send in one packet

    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ble);
//        ButterKnife.bind(this);
        options = getResources().getStringArray(R.array.item_options);
        btConnect = findViewById(R.id.BT_CONNECT);
        buttonStartreal = findViewById(R.id.button_startreal);
        textViewStep = findViewById(R.id.textView_step);
        textViewCal= findViewById(R.id.textView_cal);
        textViewDistance = findViewById(R.id.textView_distance);
        textViewTime = findViewById(R.id.textView_time);
        textView1 = findViewById(R.id.textView1);
        textViewHeartValue = findViewById(R.id.textView_heartValue);
        textViewTempValue = findViewById(R.id.textView_tempValue);
        textViewActiveTime = findViewById(R.id.textView_ActiveTime);
        SwitchCompatTemp = findViewById(R.id.SwitchCompat_temp);

        btConnect.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                connectDevice();
            }
        });

        buttonStartreal.setOnClickListener(new Button.OnClickListener() {
            public void onClick(View v) {
                on_startreal_click();
            }
        });

        init();
        connectDevice();
    }

    void on_startreal_click() {
        isStartReal = !isStartReal;
        buttonStartreal.setText(isStartReal ? "Stop" : "Start");
        sendValue(BleSDK.RealTimeStep(isStartReal,SwitchCompatTemp.isChecked()));
    }

    private void connectDevice() {
        address = getIntent().getStringExtra("address");
        if (TextUtils.isEmpty(address)) {
            Log.i(TAG, "onCreate: address null ");
            return;
        }
        Log.i(TAG, "onCreate: ");
        BleManager.getInstance().connectDevice(address);
        showConnectDialog();
    }

    private void showConnectDialog() {
        progressDialog = new ProgressDialog(this);
        progressDialog.setMessage(getString(R.string.connectting));
        if (!progressDialog.isShowing()) progressDialog.show();

    }

    private void dissMissDialog() {
        if (progressDialog != null && progressDialog.isShowing()) progressDialog.dismiss();
    }

    private void init() {
        BleManager.init(this);
        subscription = RxBus.getInstance().toObservable(BleData.class).subscribeOn(Schedulers.io()).observeOn(AndroidSchedulers.mainThread()).subscribe(new Consumer<BleData>() {
            @Override
            public void accept(BleData bleData) throws Exception {
                String action = bleData.getAction();
                if (action.equals(BleService.ACTION_GATT_onDescriptorWrite)) {
                    btConnect.setEnabled(false);
                    buttonStartreal.setEnabled(true);
                    SwitchCompatTemp.setEnabled(true);
                    dissMissDialog();
                } else if (action.equals(BleService.ACTION_GATT_DISCONNECTED)) {
                    btConnect.setEnabled(true);
                    buttonStartreal.setEnabled(false);
                    SwitchCompatTemp.setEnabled(false);
                    isStartReal = false;
                    dissMissDialog();
                }
            }
        });
        SwitchCompatTemp.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                sendValue(BleSDK.RealTimeStep(isStartReal,isChecked));
            }
        });
    }

    @Override protected void onDestroy() {
        super.onDestroy();
        unsubscribe();
        if (BleManager.getInstance().isConnected()) BleManager.getInstance().disconnectDevice();
    }

    private void unsubscribe() {
        if (subscription != null && !subscription.isDisposed()) {
            subscription.dispose();
            Log.i(TAG, "unSubscribe: ");
        }
    }

    @Override public void dataCallback(Map<String, Object> map) {
        super.dataCallback(map);
        String dataType = getDataType(map);
        Log.e("info",map.toString());
        switch (dataType) {
            case BleConst.ReadSerialNumber:
                showDialogInfo(map.toString());
                break;
            case BleConst.RealTimeStep:
                Map<String, String> maps = getData(map);
                String step = maps.get(DeviceKey.Step);
                String cal = maps.get(DeviceKey.Calories);
                String distance = maps.get(DeviceKey.Distance);
                String time = maps.get(DeviceKey.ExerciseMinutes);
                String ActiveTime = maps.get(DeviceKey.ActiveMinutes);
                String heart = maps.get(DeviceKey.HeartRate);
                String TEMP= maps.get(DeviceKey.TempData);
                textViewCal.setText(cal);
                textViewStep.setText(step);
                textViewDistance.setText(distance);
                textViewTime.setText(time);
                textViewHeartValue.setText(heart);
                textViewActiveTime.setText(ActiveTime);
                textViewTempValue.setText(TEMP);
                break;
            case BleConst.DeviceSendDataToAPP:
                //showDialogInfo(BleConst.DeviceSendDataToAPP);
                break;
            case BleConst.FindMobilePhoneMode:
                //showDialogInfo(BleConst.FindMobilePhoneMode);
                break;
            case BleConst.RejectTelMode:
                //showDialogInfo(BleConst.RejectTelMode);
                break;
            case BleConst.TelMode:
                //showDialogInfo(BleConst.TelMode);
                break;
            case BleConst.BackHomeView:
                showToast(map.toString());
                break;
            case BleConst.Sos:
                showToast(map.toString());
                break;
        }
    }
}


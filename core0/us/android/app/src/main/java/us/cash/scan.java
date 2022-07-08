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
import androidx.appcompat.app.ActionBarDrawerToggle;                                           // ActionBarDrawerToggle
import androidx.core.app.ActivityCompat;                                                       // ActivityCompat
import android.os.Bundle;                                                                      // Bundle
import androidx.core.content.ContextCompat;                                                    // ContextCompat
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import us.ko;                                                                                  // ko
import android.widget.LinearLayout;                                                            // LinearLayout
import android.Manifest;                                                                       // Manifest
import com.google.android.material.button.MaterialButton;                                      // MaterialButton
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import static us.ko.ok;                                                                        // ok
import android.content.pm.PackageManager;                                                      // PackageManager
import us.pair;                                                                                // pair
import android.widget.RelativeLayout;                                                          // RelativeLayout
import com.google.zxing.Result;                                                                // Result
import com.google.android.material.textfield.TextInputEditText;                                // TextInputEditText
import android.widget.Toast;                                                                   // Toast
import android.media.ToneGenerator;                                                            // ToneGenerator
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View
import me.dm7.barcodescanner.zxing.ZXingScannerView;                                           // ZXingScannerView

public class scan extends activity implements ZXingScannerView.ResultHandler {

    static final int MY_PERMISSIONS_REQUEST_CAMERA = 1873;

    static void log(final String s) {       //--strip
        System.out.println("scan: " + s);   //--strip
    }                                       //--strip

    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.activity_scan);
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[] {Manifest.permission.CAMERA}, 50);
        }
//        Toolbar toolbar = findViewById(R.id.toolbar);
//        setSupportActionBar(findViewById(R.id.toolbar));
//        progressbarcontainer = findViewById(R.id.progressbarcontainer);
        toolbar_button refresh = findViewById(R.id.refresh);
        refresh.setVisibility(View.GONE);
        ViewGroup contentFrame = findViewById(R.id.content_frame);
        mScannerView = new ZXingScannerView(this);
        contentFrame.addView(mScannerView);
        mScannerView.setResultHandler(this); // Register ourselves as a handler for scan results.
        Bundle bundle = getIntent().getExtras();
        if (getIntent().hasExtra("continuous")) {
            cont = bundle.getBoolean("continuous", false);
            tid = new hash_t(bundle.getByteArray("tid"));
            Toast.makeText(getApplicationContext(), "trade " + tid.encode(), 6000).show();
        }
        else {
            cont = false;
        }
        if (getIntent().hasExtra("what")) {
            what = bundle.getInt("what", 0);
        }
        else {
            what = 0;
        }
        if (what == 0) {
            MaterialButton use_endpoint_btn = findViewById(R.id.use_endpoint_btn);
            TextInputEditText ep_text = findViewById(R.id.ep_text);
            use_endpoint_btn.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    String ep = ep_text.getText().toString();
                    if (ep.trim().isEmpty()) {
                        Toast.makeText(scan.this, "type or paste an endpoint.", 6000).show();
                    }
                    else {
                        submit(ep);
                    }
                }
            });
            toolbar.setTitle(R.string.scanendpoint);

        }
        else if (what == 1) {
            LinearLayout endpoint_paste = findViewById(R.id.endpoint_paste);
            endpoint_paste.setVisibility(View.GONE);
            toolbar.setTitle("Product scanner");
        }
        //progressbarcontainer.setVisibility(View.GONE);
    }

    @Override public void onResume() {
        super.onResume();
        log("onResume"); //--strip
        log("Starting camera"); //--strip
        mScannerView.setResultHandler(this); // Register ourselves as a handler for scan results.
        mScannerView.startCamera(); // Start camera on resume
    }

    @Override public void onPause() {
        super.onPause();
        log("onPause"); //--strip
        log("Stopping camera"); //--strip
        mScannerView.stopCamera(); // Stop camera on pause
    }

    @Override public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        switch (requestCode) {
            case MY_PERMISSIONS_REQUEST_CAMERA: { // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // permission was granted, yay!
                }
                else {
                    setResult(RESULT_CANCELED);
                    finish();
                }
                return;
            }
        }
    }

    void submit(final String text) {
        log("submit " + text); //--strip
        a.tone.startTone(ToneGenerator.TONE_CDMA_PIP, 150);
        Intent data = new Intent();
        if (what == 0) {
            data.putExtra("go_qr", text);
        }
        else {
            data.putExtra("selection", text);
        }
        setResult(RESULT_OK, data);
        finish();
    }

    @Override public void handleResult(Result rawResult) {
        final String text = rawResult.getText();
        log("Result: " + text); //--strip
        submit(text);
    }

    private ZXingScannerView mScannerView;
    boolean cont = false;
    int what = 0;
    hash_t tid;
//    RelativeLayout progressbarcontainer;

}

/*
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode != main_activity.AC_RESULT) return;
        if (resultCode != RESULT_OK) return;
//        if (data.hasExtra("nodeEndPoint")){
//            Bundle extras = data.getExtras();
//            Intent intent = new Intent(); //this,MainActivity.class);
            //intent.setData(Uri.parse("NEWTRADEFROMSCAN"));
//            String nodeEndPoint = extras.getString("nodeEndPoint");
            //intent.putExtra("nodeEndPoint", nodeEndPoint);
//            intent.putExtra("go_endpoint", nodeEndPoint);
        setResult(RESULT_OK, data);
        finish();
//        }
    }
*/

/*
    boolean additem(final String product) {
        app.assert_ui_thread(); //--strip
        a.hmi.command_trade(tid, "select " + product + " 1");
        return true;
    }
*/

/*
    public void submit_c1(final String c1) {
        tone.startTone(ToneGenerator.TONE_CDMA_PIP, 150);
        Intent data = new Intent();
        data.putExtra("go_conf", c1);
        setResult(RESULT_OK, data);
        finish();
    }
*/
/*
    public void submit(String endpoint) {
        if (cont) {
            final int prev = bserial;
            additem(endpoint);
            final ZXingScannerView.ResultHandler rh = this;
//            update_basket_serial();
            new java.util.Timer().schedule(
                new java.util.TimerTask() {
                    @Override
                    public void run() {
                        if (bserial > prev) {
                            tone.startTone(ToneGenerator.TONE_CDMA_PIP, 150);
                        } else {
                            tone.startTone(ToneGenerator.TONE_CDMA_NETWORK_BUSY, 200);

                        }
                    }
                },
                1000
            );
            mScannerView.resumeCameraPreview(this);
            return;
        }
        tone.startTone(ToneGenerator.TONE_CDMA_PIP, 150);

        // Do something with the result here
        //        System.out.println("-------------------------scan.handleresult");
        //        Log.v(TAG, rawResult.getText()); // Prints scan results
        //        Log.v(TAG, rawResult.getBarcodeFormat().toString()); // Prints the scan format (qrcode, pdf417 etc.)
        //mScannerView.stopCamera();
        Intent data = new Intent(); //this,MainActivity.class);
        data.putExtra("go_endpoint", endpoint);

        //---set the data to pass back---
//        data.setData(Uri.parse(endpoint));
        //      data.putExtra("endpoint",Uri.parse(ttext));
        setResult(RESULT_OK, data);

        //---close the activity---
        //Log.d("AAA","A4");
        //startActivity(data);
        finish();
    }
*/


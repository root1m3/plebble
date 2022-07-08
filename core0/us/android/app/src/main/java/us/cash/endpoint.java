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
import com.journeyapps.barcodescanner.BarcodeEncoder;                                          // BarcodeEncoder
import com.google.zxing.BarcodeFormat;                                                         // BarcodeFormat
import android.graphics.Bitmap;                                                                // Bitmap
import com.google.zxing.common.BitMatrix;                                                      // BitMatrix
import us.wallet.trader.bookmarks_t;                                                           // bookmarks_t
import us.wallet.trader.bookmark_t;                                                            // bookmark_t
import android.os.Bundle;                                                                      // Bundle
import android.content.ClipboardManager;                                                       // ClipboardManager
import android.content.ClipData;                                                               // ClipData
import android.content.Context;                                                                // Context
import android.widget.EditText;                                                                // EditText
import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
import android.view.GestureDetector;                                                           // GestureDetector
import android.widget.ImageView;                                                               // ImageView
import android.text.InputType;                                                                 // InputType
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import android.view.MotionEvent;                                                               // MotionEvent
import com.google.zxing.MultiFormatWriter;                                                     // MultiFormatWriter
import com.google.android.material.navigation.NavigationView;                                  // NavigationView
import android.view.View.OnTouchListener;                                                      // OnTouchListener
import us.pair;                                                                                // pair
import android.widget.RelativeLayout;                                                          // RelativeLayout
import android.view.GestureDetector.SimpleOnGestureListener;                                   // SimpleOnGestureListener
import android.widget.Toast;                                                                   // Toast
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.view.View;                                                                      // View
import com.google.zxing.WriterException;                                                       // WriterException

public class endpoint extends activity {

    static void log(final String line) {          //--strip
        CFG.log_android("endpoint: " + line);     //--strip
    }                                             //--strip

    public class OnSwipeTouchListener implements OnTouchListener {

        private final GestureDetector gestureDetector;

        public OnSwipeTouchListener(Context context) {
            gestureDetector = new GestureDetector(context, new GestureListener());
        }

        public void onSwipeLeft() {
        }

        public void onSwipeRight() {
        }

        public boolean onTouch(View v, MotionEvent event) {
            return gestureDetector.onTouchEvent(event);
        }

        private final class GestureListener extends SimpleOnGestureListener {

            private static final int SWIPE_DISTANCE_THRESHOLD = 100;
            private static final int SWIPE_VELOCITY_THRESHOLD = 100;

            @Override public boolean onDown(MotionEvent e) {
                return true;
            }

            @Override public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
                float distanceX = e2.getX() - e1.getX();
                float distanceY = e2.getY() - e1.getY();
                if (Math.abs(distanceX) > Math.abs(distanceY) && Math.abs(distanceX) > SWIPE_DISTANCE_THRESHOLD && Math.abs(velocityX) > SWIPE_VELOCITY_THRESHOLD) {
                    if (distanceX > 0)
                        onSwipeRight();
                    else
                        onSwipeLeft();
                    return true;
                }
                return false;
            }
        }
    }

    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.activity_endpoint);
//        Toolbar toolbar = findViewById(R.id.toolbar);
//        setSupportActionBar(toolbar);
        toolbar.setTitle(R.string.myendpoint);
//        progressbarcontainer = findViewById(R.id.progressbarcontainer);
//        progressbarcontainer.setVisibility(View.VISIBLE);
        refresh = findViewById(R.id.refresh);
        refresh.setVisibility(View.GONE);
        qrcode = findViewById(R.id.qrcode);
        qrtext = findViewById(R.id.qrtext);
        qrlabel = findViewById(R.id.qrlabel);
        page = findViewById(R.id.page);
        qrtext.setInputType(InputType.TYPE_NULL);
        qrtext.setTextIsSelectable(true);
        qrtext.setKeyListener(null);
        qrlabel.setInputType(InputType.TYPE_NULL);
        qrlabel.setKeyListener(null);
        page.setInputType(InputType.TYPE_NULL);
        page.setKeyListener(null);
        qrtext.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                copy_clip();
            }
        });
        qrcode.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                copy_clip();
            }
        });
        qrcode.setOnTouchListener(new OnSwipeTouchListener(this) {
            @Override public void onSwipeRight() {
                --index;
                if (index < 0) index = bookmarks.size() - 1;
                paintQR();
            }
            @Override public void onSwipeLeft() {
                ++index;
                if (index >= bookmarks.size()) index = 0;
                paintQR();
            }
        });
        pair<ko, bookmarks_t> r = a.hmi.bookmarks_me();
        if (is_ko(r.first)) {
            finish();
            return;
        }
        bookmarks = r.second;
        if (bookmarks.isEmpty()) {
            log("KO 54094"); //--strip
            finish();
            return;
        }
        index = 0;
        paintQR();
        //progressbarcontainer.setVisibility(View.GONE);
    }

    void copy_clip() {
        ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
        String qr = bookmarks.get(bookmarks.keySet().toArray()[index]).qr.to_string();
        ClipData clip = ClipData.newPlainText("endpoint", qr);
        clipboard.setPrimaryClip(clip);
        Toast.makeText(endpoint.this, getResources().getString(R.string.endpointcopied) + "\n" + qr, 600).show();
    }

    public void paintQR() {
        MultiFormatWriter multiFormatWriter = new MultiFormatWriter();
        try {
            bookmark_t b = bookmarks.get("" + bookmarks.keySet().toArray()[index]);
            String txt = b.qr.to_string();
            log("my-bookmark=" + txt); //--strip
            if (txt.isEmpty()) {
                qrtext.setText("Invalid");
                qrcode.setVisibility(View.INVISIBLE);
                qrtext.setVisibility(View.INVISIBLE);
            }
            else {
                BitMatrix bitMatrix = multiFormatWriter.encode(txt, BarcodeFormat.QR_CODE, 200, 200);
                BarcodeEncoder barcodeEncoder = new BarcodeEncoder();
                Bitmap bitmap = barcodeEncoder.createBitmap(bitMatrix);
                qrcode.setImageBitmap(bitmap);
                qrtext.setText(txt);
                String pg = "" + (index + 1) + "/" + bookmarks.size();
                page.setText(pg);
                qrlabel.setText(b.label);
                qrcode.setVisibility(View.VISIBLE);
                qrtext.setVisibility(View.VISIBLE);
            }
        }
        catch (WriterException e) {
            error_manager.manage(e, e.getMessage() + "    " + e.toString());
            e.printStackTrace();
        }
    }

    bookmarks_t bookmarks = null;
    int index = -1;
    private ImageView qrcode;
    private EditText qrtext;
    private EditText qrlabel;
    private EditText page;
//    RelativeLayout progressbarcontainer;
    private toolbar_button refresh;
}


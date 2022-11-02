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
import java.util.ArrayList;                                                                    // ArrayList
import java.util.concurrent.atomic.AtomicInteger;                                              // AtomicInteger
import us.gov.crypto.base58;                                                                   // base58
import us.gov.io.blob_reader_t;                                                                // blob_reader_t
import android.os.Build;                                                                       // Build
import android.os.Bundle;                                                                      // Bundle
import android.content.Context;                                                                // Context
import us.gov.socket.datagram;                                                                 // datagram
import static android.graphics.BitmapFactory.decodeResource;                                   // decodeResource
import android.os.Environment;                                                                 // Environment
import java.io.File;                                                                           // File
import java.io.FileNotFoundException;                                                          // FileNotFoundException
import java.io.FileOutputStream;                                                               // FileOutputStream
import static us.gov.crypto.ripemd160.hash_t;                                                  // hash_t
import static us.gov.io.types.*;                                                               // *
import static us.ko.*;                                                                         // *
import static us.stdint.*;                                                                     // *
import static us.tuple.*;                                                                      // *
import android.content.Intent;                                                                 // Intent
import java.io.IOException;                                                                    // IOException
import us.ko;                                                                                  // ko
import android.view.Menu;                                                                      // Menu
import android.view.MenuItem;                                                                  // MenuItem
import androidx.core.app.NotificationCompat;                                                   // NotificationCompat
import androidx.core.app.NotificationManagerCompat;                                            // NotificationManagerCompat
import us.pair;                                                                                // pair
import android.app.PendingIntent;                                                              // PendingIntent
import us.wallet.protocol;                                                                     // protocol
import android.widget.RelativeLayout;                                                          // RelativeLayout
import androidx.annotation.RequiresApi;                                                        // RequiresApi
import us.string;                                                                              // string
import androidx.core.app.TaskStackBuilder;                                                     // TaskStackBuilder
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import androidx.appcompat.widget.Toolbar;                                                      // Toolbar
import android.net.Uri;                                                                        // Uri
import android.view.View;                                                                      // View

//import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics

public final class doc_viewer extends activity implements datagram_dispatcher_t.handler_t {

    static void log(final String line) {          //--strip
        CFG.log_android("doc_viewer: " + line);   //--strip
    }                                             //--strip

    static void log_paranoid(final String s) {  //--strip
        //System.out.println("tx: "+s);         //--strip
    }                                           //--strip

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        set_content_layout(R.layout.activity_tx);
        content = findViewById(R.id.content2);
        action = findViewById(R.id.action);
        refresh = findViewById(R.id.refresh);
        save_btn = findViewById(R.id.save_btn);
        Bundle bundle = getIntent().getExtras();
        if (!getIntent().hasExtra("tid")) {
            finish();
            return;
        }
        tid = (hash_t) getIntent().getSerializableExtra("tid");
        toolbar.setTitle(bundle.getString("title"));
        actioncommand = bundle.getString("action_cmd");
        contentcommand = bundle.getString("fetch_content_cmd");
        fname = bundle.getString("fname");
        int icon = bundle.getInt("icon");
        if (actioncommand == null || actioncommand.isEmpty()) {
            action.setVisibility(View.GONE);
        }
        else {
            action.setVisibility(View.VISIBLE);
        }
        save_btn.setVisibility(View.VISIBLE);
        action.set_stock_image(icon);
        action.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                doaction();
            }
        });
        refresh.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                fetch_content();
            }
        });
        save_btn.setOnClickListener(new View.OnClickListener() {
            @Override public void onClick(View view) {
                save(view);
            }
        });
        log("connect network-datagram hose");//--strip
        dispatchid = a.hmi().dispatcher.connect_sink(this);
        fetch_content();
    }

    @Override public void onDestroy() {
        super.onDestroy();
        log("onDestroy"); //--strip
        final app a = (app)getApplication();
        log("disconnect network-datagram hose");//--strip
        a.hmi().dispatcher.disconnect_sink(dispatchid);
    }

    @Override public void onPause() {
        super.onPause();
        log("onPause"); //--strip
    }

    @Override public void onResume() {
        super.onResume();
        log("onResume"); //--strip
    }

    @Override public void onBackPressed() {
        log("Back pressed"); //--strip
        finish();
    }

    @Override public void on_push(final hash_t target_tid, final uint16_t code, final byte[] payload) {
        if(!tid.equals(target_tid)) return;
        switch(code.value) {
            case us.wallet.trader.workflow.trader_protocol.push_doc: {
                string s = new string();
                ko r = blob_reader_t.parse(new blob_t(payload), s);
                if (is_ko(r)) {
                    log(r.msg); //--strip
                    return;
                }
                final String content = s.value;
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        on_doc(content);
                    }
                });
            }
        }
    }

    void on_workflow_item(String payload) {
        log("on_workflow_item" + payload); //--strip
    }

    void on_doc(String payload) {
        log("on_doc" + payload); //--strip
        set_content(payload);
    }

    void set_content(final String txt) {
        log("set_content "); //--strip
        log_paranoid(txt); //--strip
        content.setText(txt);
        //progressbarcontainer.setVisibility(View.GONE);
    }

    void doaction() {
        app.assert_ui_thread(); //--strip
        log("doaction"); //--strip
        final app a = (app) getApplication();
        Intent data = new Intent();
        data.putExtra("actioncommand", actioncommand);
        setResult(RESULT_OK, data);
        finish();
    }

    void fetch_content() {
        app.assert_ui_thread(); //--strip
        //progressbarcontainer.setVisibility(View.VISIBLE);
        final app a = (app) getApplication();
        log("contentcommand="+contentcommand); //--strip
        a.hmi().command_trade(tid, contentcommand);
    }

    public static class actions_helper {

        public static enum attachment_type { IMAGE, AUDIO, VIDEO, WORD, EXCEL, POWERPOINT, TXT}

        public static Intent open_file(Context context, File file) {
            String fileName = file.getName();
            String extension = fileName.substring(fileName.lastIndexOf(".") + 1);
            String type = "application/" + extension;
            attachment_type atype = get_attachment_type(fileName);
            if (atype == attachment_type.IMAGE) {
                type = "image/*";
            }
            else if (atype == attachment_type.AUDIO) {
                type = "audio/*";
            }
            else if (atype == attachment_type.VIDEO) {
                type = "video/*";
            }
            else if (atype == attachment_type.WORD) {
                type = "application/msword";
            }
            else if (atype == attachment_type.EXCEL) {
                type = "application/vnd.ms-excel";
            }
            else if (atype == attachment_type.POWERPOINT) {
                type = "application/vnd.ms-powerpoint";
            }
            else if (atype == attachment_type.TXT) {
                type = "text/plain";
            }
            Uri path = Uri.fromFile(file);
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            Intent chooser = Intent.createChooser(intent, "Open with");
            intent.setDataAndType(path, type);
            return chooser;
        }

        public static attachment_type get_attachment_type(String filename) {
            String ext =  filename.substring(filename.lastIndexOf("."));
            switch (ext){
                case "jpg":
                case "png":
                case "gif":
                    return attachment_type.IMAGE;
                case "mp4":
                case "mov":
                case "avi":
                    return attachment_type.VIDEO;
                case "wav":
                case "mp3":
                    return attachment_type.AUDIO;
                case "doc":
                case "docx":
                    return attachment_type.WORD;
                case "xls":
                    return attachment_type.EXCEL;
                case "txt":
                    return attachment_type.TXT;
            }
            return attachment_type.TXT;
        }
    }

    private void show_notification(File file) {
        Intent resultIntent = actions_helper.open_file(this, file);
        if (resultIntent != null) {
            PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, resultIntent, 0);
        }
        String notifTitle = getResources().getString(R.string.downloadedfilefrom)+" " + this.getResources().getString(R.string.app_name);
        String notifBody = getResources().getString(R.string.youhavenewdownloadedfile)+" " + file.getAbsolutePath();
        TaskStackBuilder stackBuilder = TaskStackBuilder.create(this);
        stackBuilder.addNextIntentWithParentStack(resultIntent);
        PendingIntent resultPendingIntent = stackBuilder.getPendingIntent(0, PendingIntent.FLAG_UPDATE_CURRENT);
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setSmallIcon(R.drawable.ic_download_24dp)
                .setContentTitle(notifTitle)
                .setContentText(notifTitle)
                .setStyle(new NotificationCompat.BigTextStyle().bigText(notifBody))
                .setContentIntent(resultPendingIntent)
                .setPriority(NotificationCompat.PRIORITY_DEFAULT);
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(this);
        AtomicInteger c = new AtomicInteger(0);
        int NOTIFICATION_ID = c.incrementAndGet();
        notificationManager.notify(NOTIFICATION_ID, builder.build());
    }

    void save(View v) {
        String text =  content.getText().toString();
        FileOutputStream fos = null;
        String filename = System.currentTimeMillis() + "_" +  this.getResources().getString(R.string.app_name).replace(" ","") + "_" + fname + ".txt";
        File downpath = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
        try {
            File f = new File(downpath, filename);
            fos = new FileOutputStream(f);
            fos.write(text.getBytes());
            Toast.makeText(this, "Saved to " + downpath + "/" + filename, Toast.LENGTH_LONG).show();
            show_notification(f);
        }
        catch (FileNotFoundException e){
            error_manager.manage(e, e.getMessage() + ". " + e.toString());
            e.printStackTrace();
        }
        catch (IOException e) {
            error_manager.manage(e, e.getMessage() + ". " + e.toString());
            e.printStackTrace();
        }
        finally {
            if (fos!=null) {
                try{
                    fos.close();
                }
                catch (IOException e) {
                    error_manager.manage(e, e.getMessage() + "    " + e.toString());
                    e.printStackTrace();
                }
            }
        }
    }

    private final String CHANNEL_ID = "app_notify_channel0";

    hash_t tid;
    String actioncommand;
    String contentcommand;
    String fname;
    private TextView content;
    private toolbar_button action;
    private toolbar_button refresh;
    private toolbar_button save_btn;
    //RelativeLayout progressbarcontainer;
    String command;
    uint16_t doccode = new uint16_t(0);
    int dispatchid;
}


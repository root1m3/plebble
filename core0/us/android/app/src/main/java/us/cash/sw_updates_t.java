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
import androidx.core.app.ActivityCompat;                                                       // ActivityCompat
import android.app.Activity;                                                                   // Activity
import android.app.AlertDialog;                                                                // AlertDialog
import java.util.Arrays;                                                                       // Arrays
import android.os.Build;                                                                       // Build
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import android.content.Context;                                                                // Context
import android.content.DialogInterface;                                                        // DialogInterface
import java.io.File;                                                                           // File
import java.io.FileOutputStream;                                                               // FileOutputStream
import androidx.core.content.FileProvider;                                                     // FileProvider
import android.content.Intent;                                                                 // Intent
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import android.Manifest;                                                                       // Manifest
import static android.Manifest.permission.*;                                                                       // Manifest
import static us.ko.ok;                                                                        // ok
import android.content.pm.PackageManager;                                                      // PackageManager
import us.pair;                                                                                // pair
import android.provider.Settings;                                                              // Settings
import us.string;                                                                              // string
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import android.net.Uri;                                                                        // Uri

public class sw_updates_t {

    static void log(final String line) {         //--strip
       CFG.log_android("sw_updates: " + line);   //--strip
    }                                            //--strip

    void toast(String m) {

        activity.main.runOnUiThread(new Runnable() {
            @Override public void run() {
                Toast.makeText(activity.main, m, 6000).show();
            }
        });

    }

    public sw_updates_t(app a_) {
        a = a_;
        setup_curapk();

        log("swversion " + us.vcs.version_name); //--strip
        log("curapk " + curapk + " curapk0 " + curapk0); //--strip
        log("is_updateavailable " + is_updateavailable); //--strip
    }

    public String get_curapk() {
        return curapk;
    }

    public void forget_curapk() {
        curapk = curapk0;
    }

    void setup_curapk() {
        String r = hmi_t.cfg_android.read_private_file_string(a.getApplicationContext(), "curapk");
        if (r == null) {
            log("cannot read curapk"); //--strip
            return;
        }
        curapk = r;
        if (hmi_t.cfg_android.private_file_exists(a.getApplicationContext(), "newapk_by")) { //I must be its first launch
            String newapk_by = hmi_t.cfg_android.read_private_file_string(a.getApplicationContext(), "newapk_by");
            if (newapk_by != curapk) { //failed installation
                log("Writing curapk: " + newapk_by + " filename curapk"); //--strip
                curapk = newapk_by;
                {
                    ko ret = hmi_t.cfg_android.write_private_file(a.getApplicationContext(), "curapk", curapk);
                    if (is_ko(ret)) {
                        toast(ret.msg);
                        is_updateavailable = false;
                        return;
                    }
                }
                hmi_t.cfg_android.delete_private_file(a.getApplicationContext(), "newapk");
                hmi_t.cfg_android.delete_private_file(a.getApplicationContext(), "newapk_content");
                hmi_t.cfg_android.delete_private_file(a.getApplicationContext(), "newapk_by");
                toast("New version from apk " + curapk + " " + us.vcs.version_name + ". Release notes:");
                is_updateavailable = false;
            }
            else {
                toast("Unexpected version, did the apk fail to install? curapk " + curapk + " name " + us.vcs.version_name + " newapk_by " + newapk_by);
                is_updateavailable = true;
            }
        }
    }

    private static String[] PERMISSIONS_STORAGE = {Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE};
    private static final int REQUEST_EXTERNAL_STORAGE = 1;

//    private static String[] PERMISSIONS_ISTORAGE = {Manifest.permission.READ_INTERNAL_STORAGE, Manifest.permission.WRITE_INTERNAL_STORAGE};
//    private static final int REQUEST_INNTERNAL_STORAGE = 1;

    public static boolean ask_permission(Activity ac) {
        String[] options = {"Install now.", "Remind me later."};
        //final sw_updates_t i = this;
        new AlertDialog.Builder(ac).setTitle("Info about automatic updates.")
                .setMessage("Please Grant permissions to access external storage to be able to install software updates.")
                .setIcon(android.R.drawable.ic_dialog_info).show();

        log("request Permission"); //--strip
        ActivityCompat.requestPermissions(ac, PERMISSIONS_STORAGE, REQUEST_EXTERNAL_STORAGE);
        int permission = ActivityCompat.checkSelfPermission(ac, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        return permission == PackageManager.PERMISSION_GRANTED;
    }

    public static boolean verify_storage_permissions(Activity ac) {        // Check if we have write permission
        log("verify_storage_permissions"); //--strip
        int permission = ActivityCompat.checkSelfPermission(ac, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        log("A"); //--strip
        if (permission != PackageManager.PERMISSION_GRANTED) {
            return ask_permission(ac);
        }
        else {
            log("Permission already granted"); //--strip
        }
        log("C"); //--strip
/*
        log("verify_storage_permissions internal"); //--strip
        int ipermission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_INTERNAL_STORAGE);
        log("A"); //--strip
        if (ipermission != PackageManager.PERMISSION_GRANTED) {
            log("B"); //--strip
            ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE, REQUEST_INTERNAL_STORAGE);
        }
        log("C"); //--strip
*/
        return true;
    }

    ko download_apk() {
        log("download_apk"); //--strip
        a.assert_worker_thread(); //--strip
        if (!verify_storage_permissions(activity.main)) {
            ko r = new ko("KO 60599 Permissions problem");
            log(r.msg); //--strip
            return r;
        }
        if (!hmi_t.cfg_android.is_external_storage_writable()) {
            ko r = new ko("KO 78695 External storage is not writable");
            log(r.msg); //--strip
            return r;
        }

        log(Arrays.toString(Thread.currentThread().getStackTrace())); //--strip
        log("Checking for updates."); //--strip
        log("get_updat. curapk " + curapk); //--strip
        is_updateavailable = false;
        if (a.hmi == null || a.hmi.rpc_peer == null) {
            ko r0 = new ko("KO 60069 HMI is not active. Aborting download sw-updates.");
            log(r0.msg); //--strip
            return r0;
        }
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        log("get_component_update android " + curapk + " blob_id: " + CFG.blob_id); //--strip
        string blob_id = new string(CFG.blob_id);
        us.wallet.cli.rpc_peer_t.get_component_update_in_t o_in = new us.wallet.cli.rpc_peer_t.get_component_update_in_t(blob_id, new string("android"), new string(curapk));
        us.wallet.cli.rpc_peer_t.get_component_update_out_dst_t o_out = new us.wallet.cli.rpc_peer_t.get_component_update_out_dst_t();
        {
            ko r = a.hmi.rpc_peer.call_get_component_update(o_in, o_out);
            if (is_ko(r)) {
                log(a.hmi.rewrite(r)); //--strip
                return r;
            }
        }
        log("Comparing versions apk in disc: " + curapk + "; from api: " + o_out.file.value); //--strip
        if (o_out.file.value.equals(curapk)) {
            log("Same version."); //--strip
            is_updateavailable = !curapk.equals(curapk0);
            return ok;
        }
        if (o_out.file.value.isEmpty()) {
            ko r = new ko("KO 10992 Unexpected empty filename.");
            log(r.msg); //--strip
            return r;
        }
        if (o_out.bin_pkg.value.length < 1000000) {
            ko r = new ko("KO 10994 Suspicious length suggests truncated file." + o_out.bin_pkg.value.length);
            log(r.msg); //--strip
            return r;
        }
        log("Saving content to newapk_content. size=" + o_out.bin_pkg.value.length + " bytes."); //--strip
        {
            ko r = hmi_t.cfg_android.write_public_file(activity.main, "newapk_content", o_out.bin_pkg.value);
            if (is_ko(r)) {
                return r;
            }
        }
        log("Writing newapk: " + o_out.file.value + " filename newapk"); //--strip
        {
            ko r = hmi_t.cfg_android.write_private_file(a.getApplicationContext(), "newapk", o_out.file.value);
            if (is_ko(r)) {
                return r;
            }
        }
        log("Writing newapk_by: " + curapk + " filename newapk_by"); //--strip
        {
            ko r = hmi_t.cfg_android.write_private_file(a.getApplicationContext(), "newapk_by", curapk);
            if (is_ko(r)) {
                return r;
            }
        }
        log("updating var"); //--strip
        curapk = o_out.file.value;
        is_updateavailable = true; //!curapk.equals(curapk0);
        return ok;
    }

    static class NextTask extends TimerTask {
        NextTask(sw_updates_t i_) {
            i = i_;
        }
        sw_updates_t i;

        @Override
        public void run() {
            i.check(false);
        }
    }

    void check(boolean informok) {
        a.assert_worker_thread(); //--strip
        if (activity.main == null) {
            return;
        }
        final ko r = download_apk();
        log("Timer: download_apk? " + is_updateavailable); //--strip
        activity.main.runOnUiThread(new Runnable() {
            @Override public void run() {
                if (r != ok) {
                    Toast.makeText(activity.main, r.msg, 6000).show();
                }
                else {
                    if (is_updateavailable) {
                        ask_install();
                    }
                    else {
                        if (informok) {
                            Toast.makeText(activity.main, a.getResources().getString(R.string.systemisuptodate), 6000).show();
                        }
                    }
                }
            }
        });

    }

    Timer timer;
    public void start() {
        a.assert_ui_thread(); //--strip
        log("Start automatic updates"); //--strip
        timer = new Timer();
        timer.schedule(new NextTask(this), 2 * 1000, 8 * 60 * 60 * 1000); //check on start app and every 8 hours
    }

    public void stop() {
        a.assert_ui_thread(); //--strip
        if (timer != null) {
            timer.cancel();
            timer = null;
        }
    }

    public boolean need_install_apk() {
        log("wallet::need_install_apk()?"); //--strip
        hmi_t.cfg_android.logfiles(a.getApplicationContext());  //--strip
        if (!hmi_t.cfg_android.public_file_exists(a.getApplicationContext(), "newapk_content")) {
            log("newapk_content doesn't exist."); //--strip
            return false;
        }
        log("found newapk_content. Yes, we need to install it."); //--strip
        return true;
    }

    public void ask_install() {
        String[] options = {"Install now.", "Remind me later."};
        final sw_updates_t i = this;
        new AlertDialog.Builder(a.active_ac).setTitle("An update of the app " + CFG.app_name + " is ready to install...")
                .setItems(options, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        switch(which) {
                            case 0:
                                i.do_inst_ask_permission(a.active_ac);
                                break;
                            case 1:
                                break;
                        }
                    }
                })
                .setIcon(android.R.drawable.ic_dialog_alert).show();
    }

    public void show_ui(final activity ac) {
        String[] options = {a.getResources().getString(R.string.checkforupdates), a.getResources().getString(R.string.forgetapknash) + " " + (curapk.equals(curapk0) ? "[0] " : "[+] ") + get_curapk(), a.getResources().getString(R.string.installupdates), a.getResources().getString(R.string.cancel)};
        final sw_updates_t i = this;
        new AlertDialog.Builder(ac).setTitle(a.getResources().getString(R.string.appupdates))
                .setItems(options, new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        switch(which) {
                            case 0:
                                i.check4updates(ac);
                                break;
                            case 1:
                                i.forget_curapk();
                                break;
                            case 2:
                                i.do_inst_ask_permission(ac);
                                break;
                            case 3:
                        }
                    }
                })
                .setIcon(android.R.drawable.ic_dialog_alert).show();
    }

    public void do_inst(activity ac) {
        log("do_inst"); //--strip
        a.assert_ui_thread(); //--strip
        if (!need_install_apk()) {
            Toast.makeText(ac, a.getResources().getString(R.string.noupgradepackages), 6000).show();
            return;
        }
        File f = hmi_t.cfg_android.public_file(a.getApplicationContext(), "newapk_content");
        if (f == null) {
            log("KO 30029 newapk_content doesn't open. " + curapk); //--strip
            return;
        }
        log("Installing file length=" + f.length() + " bytes. " + f.getAbsolutePath()); //--strip
        try {
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setDataAndType(FileProvider.getUriForFile(a.getApplicationContext(), a.getApplicationContext().getPackageName()+".provider", f), "application/vnd.android.package-archive");
            intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_GRANT_READ_URI_PERMISSION);
            ac.startActivity(intent);
        }
        catch (Exception e) {
            log("KO 60508 Ex. " + e.getMessage()); //--strip
            error_manager.manage(e,"KO 60508");
        }
        log("Intent started"); //--strip
    }

    public static final int INST_RESULT = 1;

    public void do_inst_ask_permission(activity ac) {
        log("do_inst_ask_permission");  //--strip
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            log("A");  //--strip
            if (!a.getPackageManager().canRequestPackageInstalls()) {
                log("B");  //--strip
                ac.startActivityForResult(new Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES).setData(Uri.parse(String.format("package:%s", a.getPackageName()))), INST_RESULT);
                return;
            }
        }
        do_inst(ac);
    }

    void check4updates(activity ac) {
        a.assert_ui_thread(); //--strip
        Toast.makeText(ac, "Checking for updates...", 6000).show();
        Thread thread = new Thread(new Runnable() {
            @Override public void run () {
                check(true);
            }
        });
        thread.start();
    }

    static final String curapk0 = CFG.apkfilename;
    String curapk = curapk0;
    public boolean is_updateavailable = false;
    String homedir;
    app a;
}


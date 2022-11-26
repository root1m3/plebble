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
import androidx.core.app.ActivityCompat;                                                       // ActivityCompat
import android.app.Activity;                                                                   // Activity
import android.app.AlertDialog;                                                                // AlertDialog
import java.util.Arrays;                                                                       // Arrays
import android.os.Build;                                                                       // Build
import java.io.ByteArrayOutputStream;                                                          // ByteArrayOutputStream
import androidx.core.content.ContextCompat;                                                    // ContextCompat
import android.content.Context;                                                                // Context
import android.content.DialogInterface;                                                        // DialogInterface
import java.io.File;                                                                           // File
import java.io.FileOutputStream;                                                               // FileOutputStream
import androidx.core.content.FileProvider;                                                     // FileProvider
import static android.Manifest.permission.*;                                                   // *
import android.content.Intent;                                                                 // Intent
import static us.ko.is_ko;                                                                     // is_ko
import us.ko;                                                                                  // ko
import android.Manifest;                                                                       // Manifest
import androidx.annotation.NonNull;                                                            // NonNull
import static us.ko.ok;                                                                        // ok
import android.content.pm.PackageManager;                                                      // PackageManager
import us.pair;                                                                                // pair
import android.provider.Settings;                                                              // Settings
import us.string;                                                                              // string
import java.util.Timer;                                                                        // Timer
import java.util.TimerTask;                                                                    // TimerTask
import android.widget.Toast;                                                                   // Toast
import android.net.Uri;                                                                        // Uri
import java.time.Instant;
import java.time.Duration;

public class sw_updates_t {

    static ko KO_60599 = new ko("KO 60599 Permissions problem.");


    private static void log(final String line) {  //--strip
        CFG.log_android("sw_updates: " + line);   //--strip
    }                                             //--strip

    void toast(String m) {
        if (activity.a.main == null) {
            log("KO 88079 cannot toast. main is null. " + m); //--strip
            return;
        }
        activity.a.main.runOnUiThread(new Runnable() {
            @Override public void run() {
                Toast.makeText(activity.a.main, m, 6000).show();
            }
        });
    }

    public sw_updates_t(app a_, hmi_t hmi_, cfg_android_private_t cfg_private_, cfg_android_public_t cfg_public_) {
        a = a_;
        hmi = hmi_;
        cfg_private = cfg_private_;
        cfg_public = cfg_public_;

        clean_update();
//        query_newapk_hash(); //setup_curapk();
        log("swversion " + us.vcs.version_name); //--strip
        //log("curapk '" + curapk + "'"); //--strip
        //log("is_updateavailable " + is_updateavailable); //--strip
    }

    //public String get_curapk() {
    //    return curapk;
    //}

    public void forget_curapk() {
        //curapk = "";
        clean_update();
    }

    void clean_update() {
//        cfg_private.delete_file("newapk_name");
        //is_updateavailable = false;
        cfg_private.delete_file("newapk_content");
//        cfg_private.delete_file("newapk_by");
//        log("deleted files newapk, newapk_by, newapk_content"); //--strip
        log("deleted file newapk_content"); //--strip
    }

/*
    void boot_first_time() {
        log("curapk '" + curapk + "'"); //--strip
        if (!cfg_private.file_exists2("newapk_by")) { //I must be its first launch
            log("newapk_by doesn't exist, we're running latest version"); //--strip
            return;
        }
        log("newapk_by exists!"); //--strip
        String newapk_by = cfg_private.read_file_string("newapk_by");
        if (newapk_by == null || newapk_by.isEmpty()) {
            log("KO 88574 problem with newapk_by file. cleaning upgrade files."); //--strip
            clean_update();
            return;
        }
        if (newapk_by.equals(us.vcs.apkfilename())) {
            log("We are running the same version used to download the new apk"); //--strip
            toast("Unexpected version, did the apk fail to install? curapk " + curapk + " name " + us.vcs.version_name + " newapk_by " + newapk_by);
            is_updateavailable = true;
            return;
        }
        curapk = cfg_private.read_file_string("newapk");
        if (curapk == null || curapk.isEmpty()) {
            log("KO 88574 problem with newapk file. cleaning upgrade files."); //--strip
            clean_update();
            return;
        }
        log("Writing curapk: " + newapk_by + " filename curapk"); //--strip
        {
            ko ret = cfg_private.write_file("curapk", curapk);
            if (is_ko(ret)) {
                toast(ret.msg);
                clean_update();
                return;
            }
        }
        clean_update();
        toast("New version from apk " + curapk + " " + us.vcs.version_name + ". Release notes:");
        is_updateavailable = false;
    }
*/

//    ko setup_curapk() {
/*
    pair<ko, string> query_apk_hash() {
//        log("setup_curapk"); //--strip
//        curapk = cfg_private.read_file_string("curapk");
//        if (curapk == null || curapk.isEmpty()) {
//            log("cannot read curapk"); //--strip
        log("querying node for my apk hash " + us.vcs.apkfilename()); //--strip
        us.wallet.cli.rpc_peer_t.get_component_hash_in_t o_in = new us.wallet.cli.rpc_peer_t.get_component_hash_in_t(new string("android"), new string(us.vcs.apkfilename()));
        pair<ko, string> ans = new pair<ko, string>(ok, new string());
        ans.first = hmi.rpc_peer.call_get_component_hash(o_in, ans.second);
        return ans;
*/
/*
        if (is_ko(r)) {
            log(hmi.rewrite(r)); //--strip
//            curapk = "";
            ans.first = r;
            return ans;
        }
        return ans;
*/
//        curapk = ans.value;
//        log("curapk from api = " + curapk); //--strip
//        return cfg_private.write_file("curapk", curapk);
//        }
        //boot_first_time();
//    }

    private static String[] PERMISSIONS_STORAGE = {Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE};
    private static final int REQUEST_EXTERNAL_STORAGE = 1832;

    public boolean verify_permissions(String[] permissions) {        // Check if we have write permission
        for (String str : permissions) {
            if (ContextCompat.checkSelfPermission(a.getApplicationContext(), str) != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    public void ask_permission(int req_code, String[] permissions, String rationale) {
        if (a.main == null) {
            return;
        }
        if (ActivityCompat.shouldShowRequestPermissionRationale(a.main, permissions[0])) {
            new AlertDialog.Builder(a.main).setTitle("Info about automatic updates.")
                .setMessage(rationale)
                .setIcon(android.R.drawable.ic_dialog_info).show();
        }
        log("request Permission"); //--strip
        ActivityCompat.requestPermissions(a.main, permissions, req_code);
    }

    ko download_apk() {
        log("download_apk"); //--strip
        a.assert_worker_thread(); //--strip
        //is_updateavailable = false;
        if (!verify_permissions(PERMISSIONS_STORAGE)) {
            if (a.main != null) {
                a.main.runOnUiThread(new Runnable() {
                    @Override public void run() {
                        ask_permission(REQUEST_EXTERNAL_STORAGE, PERMISSIONS_STORAGE, "Please Grant access to external storage. This is needed for temporary storage of apk files (software updates).");
                    }
                });
            }
            log(KO_60599.msg); //--strip
            return KO_60599;
        }
        if (!cfg_public.is_external_storage_writable()) {
            ko r = new ko("KO 78695 External storage is not writable");
            log(r.msg); //--strip
            return r;
        }
        //log(Arrays.toString(Thread.currentThread().getStackTrace())); //--strip
        log("Checking for updates."); //--strip
        log("get_updat. curapk " + curapk); //--strip
        if (hmi == null || hmi.rpc_peer == null) {
            ko r0 = new ko("KO 60069 HMI is not active. Aborting download sw-updates.");
            log(r0.msg); //--strip
            return r0;
        }
        //if (is_updateavailable) {
        //    log("apk is locally available"); //--strip
        //    return ok;
        //}
        //if (cfg_private.file_exists2("newapk_by")) {
        //    is_updateavailable = true;
        //    return ok;
        //}
/*
        log("querying node for updates"); //--strip
        pair<ko, string> apkhash = query_apk_hash()
        if (is_ko(apkhash.first)) {
            return apkhash.first;
        }
        if (apkhash.second.value == null) {
            ko r0 = new ko("KO 60302 .");
            log(r0.msg); //--strip
            return r0;
        }
*/
        string apkfilename = new string(us.vcs.apkfilename());
//        ByteArrayOutputStream os = new ByteArrayOutputStream();
        log("get_component_update android " + curapk + " blob_id: " + CFG.blob_id); //--strip
        us.wallet.cli.rpc_peer_t.get_component_update_in_t o_in = new us.wallet.cli.rpc_peer_t.get_component_update_in_t(new string(CFG.blob_id), new string("android"), apkfilename);
        us.wallet.cli.rpc_peer_t.get_component_update_out_dst_t o_out = new us.wallet.cli.rpc_peer_t.get_component_update_out_dst_t();
        {
            ko r = hmi.rpc_peer.call_get_component_update(o_in, o_out);
            if (is_ko(r)) {
                log(hmi.rewrite(r)); //--strip
                return r;
            }
        }
        if (o_out.vcsname.value.isEmpty()) {
            ko r = new ko("KO 10992 Unexpected empty filename.");
            log(r.msg); //--strip
            return r;
        }
        if (o_out.vcsname.value.equals(apkfilename.value)) {
            log("Same version."); //--strip
            //is_updateavailable = false;
            return new ko("KO 78868 No newer update blob.");
        }

        if (o_out.bin_pkg.value == null) {
            ko r = new ko("KO 10993 file is null.");
            log(r.msg); //--strip
            return r;
        }
        if (o_out.bin_pkg.value.length < 1000000) {
            ko r = new ko("KO 10994 Suspicious length suggests truncated file." + o_out.bin_pkg.value.length);
            log(r.msg); //--strip
            return r;
        }
        synchronized (cfg_android_public_t.downloads_dir_lock_t) {
            log("Saving content to newapk_content. size=" + o_out.bin_pkg.value.length + " bytes."); //--strip
            ko r = cfg_public.write_public_file("newapk_content", o_out.bin_pkg.value);
            if (is_ko(r)) {
                return r;
            }
        }
/*
        log("Writing newapk_name: " + o_out.file.value + " filename newapk"); //--strip
        {
            ko r = cfg_private.write_file("newapk_name", o_out.file.value);
            if (is_ko(r)) {
                return r;
            }
        }
*/
/*
        log("Writing newapk_by: " + us.vcs.apkfilename()); //--strip
        {
            ko r = cfg_private.write_file("newapk_by", us.vcs.apkfilename());
            if (is_ko(r)) {
                return r;
            }
        }
        */
//        log("updating var"); //--strip
//        is_updateavailable = true;
        return ok;
    }

    void check(boolean informok) {
        a.assert_worker_thread(); //--strip
        log("check"); //--strip
        a.main.runOnUiThread(new Runnable() {
            @Override public void run() {
                ask_install();
/*
                if (is_updateavailable) {
                }
                else {
                    if (informok) {
                        toast(a.getResources().getString(R.string.systemisuptodate));
                    }
                }
*/
            }
        });

//        if (a.main == null) return;

    }

/*
    Timer timer;

    public ko start() {
        log("Start automatic updates"); //--strip
        timer = new Timer();
        timer.schedule(new TimerTask() {
                @Override public void run() {
                    check(false);
                }
            }, 2 * 1000, 8 * 60 * 60 * 1000); //check on start app and every 8 hours
        return ok;
    }

    public void stop() {
        if (timer == null) return;
        timer.cancel();
        timer = null;
    }

    public void join() {
    }
*/

    public boolean need_install_local_apk() {
        log("wallet::need_install_local_apk()?"); //--strip
        if (CFG.appstore_edition == 1) {
            log("CFG.appstore_edition"); //--strip
            return false;
        }
        if (!cfg_public.public_file_exists("newapk_content")) {
            log("newapk_content doesn't exist."); //--strip
            return false;
        }
        log("found newapk_content. Yes, we need to install it."); //--strip
        return true;
    }

    public void ask_install() {
        log("ask_install"); //--strip
        a.assert_ui_thread(); //--strip
        if (a.active_ac == null) {
            log("active_ac is null"); //--strip
            return;
        }
        String[] options = {"Install now.", "Remind me later."};
        final sw_updates_t i = this;
        new AlertDialog.Builder(a.active_ac).setTitle("An update of the app " + CFG.app_name + " is ready to install...")
            .setItems(options, new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    switch(which) {
                        case 0:
                            i.do_inst_ask_permission(a.active_ac);
                            break;
                        case 1:
                            user_selected_remind_me_later = Instant.now();
                            break;
                    }
                }
            })
            .setIcon(android.R.drawable.ic_dialog_alert).show();
    }

    public ko do_inst_local(activity ac) {
        log("do_inst_local"); //--strip
        a.assert_ui_thread(); //--strip

        {
            ko r = download_apk();
            if (is_ko(r)) { //retry in 
                return r;
            }
        }

        if (!need_install_local_apk()) {
            ac.toast(a.getResources().getString(R.string.noupgradepackages));
            //Toast.makeText(ac, a.getResources().getString(R.string.noupgradepackages), 6000).show();
            return new ko("KO 59588 apk not in disk");
        }
        File f = cfg_public.public_file("newapk_content");
        if (f == null) {
            ko r = new ko("KO 30029 newapk_content doesn't open.");
            log(r.msg + " " + curapk); //--strip
            return r;
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
            error_manager.manage(e, "KO 60508");
        }
        log("Intent started"); //--strip
        return ok;
    }

    public void do_inst_ask_permission(activity ac) {
        log("do_inst_ask_permission");  //--strip
        a.assert_ui_thread(); //--strip
        if (CFG.appstore_edition == 1) {
            final String appPackageName = ac.getPackageName(); // getPackageName() from Context or Activity object
            try {
                ac.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=" + appPackageName)));
            } catch (android.content.ActivityNotFoundException anfe) {
                ac.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse("https://play.google.com/store/apps/details?id=" + appPackageName)));
            }
            return;
        }
        int n = 5;
        while (true) {
            if (--n == 0) break;
            if (a.getPackageManager().canRequestPackageInstalls()) {
                ko r = do_inst_local(ac);
                if (r != KO_60599) { //retry in 
                    break;
                }
            }
            else {
                log("B");  //--strip
                Intent i = new Intent(Settings.ACTION_MANAGE_UNKNOWN_APP_SOURCES).setData(Uri.parse(String.format("package:%s", a.getPackageName())));
                ac.startActivityForResult(i, activity.INSTALL_PACKAGES_REQUESTCODE);
                return;
            }

            try {
                Thread.sleep(5000);
            }
            catch(InterruptedException e) {
            }
            log("Probl with permissions. Trying again in 5 secs"); //--strip
            continue;                
        }
    }

    void check4updates() {
        a.assert_ui_thread(); //--strip
        toast("Checking for updates...");
        Thread thread = new Thread(new Runnable() {
            @Override public void run() {
                check(true);
            }
        });
        thread.start();
    }
    void check4updates__worker() {
        log("check4updates__worker"); //--strip
        new Exception().printStackTrace();
        a.assert_worker_thread(); //--strip
        //keep the calling thread (work dispatcher) fast
        Thread thread = new Thread(new Runnable() {
            @Override public void run() {
                check(true);
            }
        });
        thread.start();
    }

    public void show_ui(final activity ac) {
        a.assert_ui_thread(); //--strip
//        String[] options = {a.getResources().getString(R.string.checkforupdates), a.getResources().getString(R.string.forgetapknash) + " " + get_curapk(), a.getResources().getString(R.string.installupdates), a.getResources().getString(R.string.cancel)};
        String[] options;

//        boolean blob_available = need_install_local_apk()
/*
        if (is_updateavailable) {
            String[] opt = {a.getResources().getString(R.string.checkforupdates), a.getResources().getString(R.string.installupdates), a.getResources().getString(R.string.cancel)};
            options = opt;
        }
        else {
*/
        String[] opt = {a.getResources().getString(R.string.checkforupdates), a.getResources().getString(R.string.cancel)};
        options = opt;
//        }
        final sw_updates_t i = this;
        new AlertDialog.Builder(ac).setTitle(a.getResources().getString(R.string.appupdates))
            .setItems(options, new DialogInterface.OnClickListener() {
                @Override public void onClick(DialogInterface dialog, int which) {
                    switch(which) {
                        case 0:
                            i.check4updates();
                            break;
/*
                        case 1:
                            i.forget_curapk();
                            break;
                        case 1:
                            if (is_updateavailable) {
                                ko r = i.do_inst_ask_permission(ac);
                                if (is_ko(r)) {
                                    if (a.main != null) {
                                        a.main.toast(r.msg);
                                    }
                                }
                                break;
                            }
                        case 2:
*/  
                    }
                }
            })
            .setIcon(android.R.drawable.ic_dialog_alert).show();
    }

/*
    private void sw_updates_show_ui(activity ac) {
    }

    private void sw_updates_do_inst(activity ac) {
    }
    private boolean sw_updates_is_updateavailable = false;
*/

//    public boolean is_updateavailable = false;


    String curapk = "";
    app a;
    hmi_t hmi;
    cfg_android_private_t cfg_private;
    cfg_android_public_t cfg_public;
    Instant user_selected_remind_me_later = null;
}


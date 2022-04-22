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
import android.app.Activity;                                                                   // Activity
import android.content.ActivityNotFoundException;                                              // ActivityNotFoundException
import android.app.AlertDialog;                                                                // AlertDialog
import java.util.Arrays;                                                                       // Arrays
import android.os.Bundle;                                                                      // Bundle
import java.util.Collection;                                                                   // Collection
import java.util.Collections;                                                                  // Collections
import android.content.DialogInterface;                                                        // DialogInterface
import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
import android.app.Fragment;                                                                   // Fragment
import java.util.HashMap;                                                                      // HashMap
import android.content.Intent;                                                                 // Intent
import java.util.List;                                                                         // List
import android.util.Log;                                                                       // Log
import java.util.Map;                                                                          // Map
import android.content.pm.PackageManager;                                                      // PackageManager
import android.content.pm.ResolveInfo;                                                         // ResolveInfo
import android.net.Uri;                                                                        // Uri

/**
 * 19/05/18.
 */

/*
 * Copyright 2009 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



/**
 * <p>A utility class which helps ease integration with Barcode Scanner via {@link Intent}s. This is a simple
 * way to invoke barcode scanning and receive the result, without any need to integrate, modify, or learn the
 * project's source code.</p>
 *
 * <h2>Initiating a barcode scan</h2>
 *
 * <p>To integrate, create an instance of {@code IntentIntegrator} and call {@link #initiateScan()} and wait
 * for the result in your app.</p>
 *
 * <p>It does require that the Barcode Scanner (or work-alike) application is installed. The
 * {@link #initiateScan()} method will prompt the user to download the application, if needed.</p>
 *
 * <p>There are a few steps to using this integration. First, your {@link Activity} must implement
 * the method {@link Activity#onActivityResult(int, int, Intent)} and include a line of code like this:</p>
 *
 * <pre>{@code
 * public void onActivityResult(int requestCode, int resultCode, Intent intent) {
 *   IntentResult scanResult = IntentIntegrator.parseActivityResult(requestCode, resultCode, intent);
 *   if (scanResult != null) {
 *     // handle scan result
 *   }
 *   // else continue with any other code you need in the method
 *   ...
 * }
 * }</pre>
 *
 * <p>This is where you will handle a scan result.</p>
 *
 * <p>Second, just call this in response to a user action somewhere to begin the scan process:</p>
 *
 * <pre>{@code
 * IntentIntegrator integrator = new IntentIntegrator(yourActivity);
 * integrator.initiateScan();
 * }</pre>
 *
 * <p>Note that {@link #initiateScan()} returns an {@link AlertDialog} which is non-null if the
 * user was prompted to download the application. This lets the calling app potentially manage the dialog.
 * In particular, ideally, the app dismisses the dialog if it's still active in its {@link Activity#onPause()}
 * method.</p>
 *
 * <p>You can use {@link #setTitle(String)} to customize the title of this download prompt dialog (or, use
 * {@link #setTitleByID(int)} to set the title by string resource ID.) Likewise, the prompt message, and
 * yes/no button labels can be changed.</p>
 *
 * <p>Finally, you can use {@link #addExtra(String, Object)} to add more parameters to the Intent used
 * to invoke the scanner. This can be used to set additional options not directly exposed by this
 * simplified API.</p>
 *
 * <p>By default, this will only allow applications that are known to respond to this intent correctly
 * do so. The apps that are allowed to response can be set with {@link #setTargetApplications(List)}.
 * For example, set to {@link #TARGET_BARCODE_SCANNER_ONLY} to only target the Barcode Scanner app itself.</p>
 *
 * <h2>Sharing text via barcode</h2>
 *
 * <p>To share text, encoded as a QR Code on-screen, similarly, see {@link #shareText(CharSequence)}.</p>
 *
 * <p>Some code, particularly download integration, was contributed from the Anobiit application.</p>
 *
 * <h2>Enabling experimental barcode formats</h2>
 *
 * <p>Some formats are not enabled by default even when scanning with {@link #ALL_CODE_TYPES}, such as
 * PDF417. Use {@link #initiateScan(Collection)} with
 * a collection containing the names of formats to scan for explicitly, like "PDF_417", to use such
 * formats.</p>
 *
 * @author Sean Owen
 * @author Fred Lin
 * @author Isaac Potoczny-Jones
 * @author Brad Drehmer
 * @author gcstang
 */
public class scan_intent_integrator {

    public static final int REQUEST_CODE = 0x0000c0de; // Only use bottom 16 bits
    private static final String TAG = scan_intent_integrator.class.getSimpleName();

    public static final String DEFAULT_TITLE = "Install Barcode Scanner?";
    public static final String DEFAULT_MESSAGE = "This application requires Barcode Scanner. Would you like to install it?";
    public static final String DEFAULT_YES = "Yes";
    public static final String DEFAULT_NO = "No";

    private static final String BS_PACKAGE = "com.google.zxing.client.android";
    private static final String BSPLUS_PACKAGE = "com.srowen.bs.android";

    // supported barcode formats
    public static final Collection<String> PRODUCT_CODE_TYPES = list("UPC_A", "UPC_E", "EAN_8", "EAN_13", "RSS_14");
    public static final Collection<String> ONE_D_CODE_TYPES =
            list("UPC_A", "UPC_E", "EAN_8", "EAN_13", "CODE_39", "CODE_93", "CODE_128",
                    "ITF", "RSS_14", "RSS_EXPANDED");
    public static final Collection<String> QR_CODE_TYPES = Collections.singleton("QR_CODE");
    public static final Collection<String> DATA_MATRIX_TYPES = Collections.singleton("DATA_MATRIX");

    public static final Collection<String> ALL_CODE_TYPES = null;

    public static final List<String> TARGET_BARCODE_SCANNER_ONLY = Collections.singletonList(BS_PACKAGE);
    public static final List<String> TARGET_ALL_KNOWN = list(
            BSPLUS_PACKAGE,             // Barcode Scanner+
            BSPLUS_PACKAGE + ".simple", // Barcode Scanner+ Simple
            BS_PACKAGE                  // Barcode Scanner
            // What else supports this intent?
    );

    // Should be FLAG_ACTIVITY_NEW_DOCUMENT in API 21+.
    // Defined once here because the current value is deprecated, so generates just one warning
    private static final int FLAG_NEW_DOC = Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET;

    private final Activity activity;
    private final Fragment fragment;

    private String title;
    private String message;
    private String button_yes;
    private String button_no;
    private List<String> target_applications;
    private final Map<String,Object> more_extras = new HashMap<String,Object>(3);

    /**
     * @param activity {@link Activity} invoking the integration
     */
    public scan_intent_integrator(Activity activity) {
        this.activity = activity;
        this.fragment = null;
        initialize_configuration();
    }

    /**
     * @param fragment {@link Fragment} invoking the integration.
     *  {@link #startActivityForResult(Intent, int)} will be called on the {@link Fragment} instead
     *  of an {@link Activity}
     */
    public scan_intent_integrator(Fragment fragment) {
        this.activity = fragment.getActivity();
        this.fragment = fragment;
        initialize_configuration();
    }

    private void initialize_configuration() {
        title = DEFAULT_TITLE;
        message = DEFAULT_MESSAGE;
        button_yes = DEFAULT_YES;
        button_no = DEFAULT_NO;
        target_applications = TARGET_ALL_KNOWN;
    }

    public String get_title() {
        return title;
    }

    public void set_title(String title) {
        this.title = title;
    }

    public void set_title_by_id(int i) {
        title = activity.getString(i);
    }

    public String get_message() {
        return message;
    }

    public void set_message(String message) {
        this.message = message;
    }

    public void set_message_by_id(int i) {
        message = activity.getString(i);
    }

    public String get_button_yes() {
        return button_yes;
    }

    public void set_button_yes(String button_yes) {
        this.button_yes = button_yes;
    }

    public void set_button_yes_by_id(int i) {
        button_yes = activity.getString(i);
    }

    public String get_button_no() {
        return button_no;
    }

    public void set_button_bo(String button_no) {
        this.button_no = button_no;
    }

    public void set_button_no_by_id(int i) {
        button_no = activity.getString(i);
    }

    public Collection<String> get_target_applications() {
        return target_applications;
    }

    public final void set_target_applications(List<String> target_applications) {
        if (target_applications.isEmpty()) {
            throw new IllegalArgumentException("No target applications");
        }
        this.target_applications = target_applications;
    }

    public void set_single_target_application(String target_application) {
        this.target_applications = Collections.singletonList(target_application);
    }

    public Map<String,?> get_more_extras() {
        return more_extras;
    }

    public final void add_extra(String key, Object value) {
        more_extras.put(key, value);
    }

    /**
     * Initiates a scan for all known barcode types with the default camera.
     *
     * @return the {@link AlertDialog} that was shown to the user prompting them to download the app
     *   if a prompt was needed, or null otherwise.
     */
    public final AlertDialog initiate_scan() {
        return initiate_scan(ALL_CODE_TYPES, -1);
    }

    /**
     * Initiates a scan for all known barcode types with the specified camera.
     *
     * @param cameraId camera ID of the camera to use. A negative value means "no preference".
     * @return the {@link AlertDialog} that was shown to the user prompting them to download the app
     *   if a prompt was needed, or null otherwise.
     */
    public final AlertDialog initiate_scan(int cameraId) {
        return initiate_scan(ALL_CODE_TYPES, cameraId);
    }

    /**
     * Initiates a scan, using the default camera, only for a certain set of barcode types, given as strings corresponding
     * to their names in ZXing's {@code BarcodeFormat} class like "UPC_A". You can supply constants
     * like {@link #PRODUCT_CODE_TYPES} for example.
     *
     * @param desiredBarcodeFormats names of {@code BarcodeFormat}s to scan for
     * @return the {@link AlertDialog} that was shown to the user prompting them to download the app
     *   if a prompt was needed, or null otherwise.
     */
    public final AlertDialog initiate_scan(Collection<String> desired_barcode_formats) {
        return initiate_scan(desired_barcode_formats, -1);
    }

    /**
     * Initiates a scan, using the specified camera, only for a certain set of barcode types, given as strings corresponding
     * to their names in ZXing's {@code BarcodeFormat} class like "UPC_A". You can supply constants
     * like {@link #PRODUCT_CODE_TYPES} for example.
     *
     * @param desiredBarcodeFormats names of {@code BarcodeFormat}s to scan for
     * @param cameraId camera ID of the camera to use. A negative value means "no preference".
     * @return the {@link AlertDialog} that was shown to the user prompting them to download the app
     *   if a prompt was needed, or null otherwise
     */
    public final AlertDialog initiate_scan(Collection<String> desired_barcode_formats, int camera_id) {
        Intent intent_scan = new Intent(BS_PACKAGE + ".SCAN");
        intent_scan.addCategory(Intent.CATEGORY_DEFAULT);

        // check which types of codes to scan for
        if (desired_barcode_formats != null) {
            // set the desired barcode types
            StringBuilder joined_by_comma = new StringBuilder();
            for (String format : desired_barcode_formats) {
                if (joined_by_comma.length() > 0) {
                    joined_by_comma.append(',');
                }
                joined_by_comma.append(format);
            }
            intent_scan.putExtra("SCAN_FORMATS", joined_by_comma.toString());
        }

        // check requested camera ID
        if (camera_id >= 0) {
            intent_scan.putExtra("SCAN_CAMERA_ID", camera_id);
        }

        String target_app_package = find_target_app_package(intent_scan);
        if (target_app_package == null) {
            return show_download_dialog();
        }
        intent_scan.setPackage(target_app_package);
        intent_scan.addFlags(FLAG_NEW_DOC);
        attach_more_extras(intent_scan);
        start_activity_for_result(intent_scan, REQUEST_CODE);
        return null;
    }

    /**
     * Start an activity. This method is defined to allow different methods of activity starting for
     * newer versions of Android and for compatibility library.
     *
     * @param intent Intent to start.
     * @param code Request code for the activity
     * @see Activity#startActivityForResult(Intent, int)
     * @see Fragment#startActivityForResult(Intent, int)
     */
    protected void start_activity_for_result(Intent intent, int code) {
        if (fragment == null) {
            activity.startActivityForResult(intent, code);
        }
        else {
            fragment.startActivityForResult(intent, code);
        }
    }

    private String find_target_app_package(Intent intent) {
        PackageManager pm = activity.getPackageManager();
        List<ResolveInfo> available_apps = pm.queryIntentActivities(intent, PackageManager.MATCH_DEFAULT_ONLY);
        if (available_apps != null) {
            for (String target_app : target_applications) {
                if (contains(available_apps, target_app)) {
                    return target_app;
                }
            }
        }
        return null;
    }

    private static boolean contains(Iterable<ResolveInfo> available_apps, String target_app) {
        for (ResolveInfo available_app : available_apps) {
            String package_name = available_app.activityInfo.packageName;
            if (target_app.equals(package_name)) {
                return true;
            }
        }
        return false;
    }

    private AlertDialog show_download_dialog() {
        AlertDialog.Builder download_dialog = new AlertDialog.Builder(activity);
        download_dialog.setTitle(title);
        download_dialog.setMessage(message);
        download_dialog.setPositiveButton(button_yes, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialogInterface, int i) {
                String package_name;
                if (target_applications.contains(BS_PACKAGE)) {
                    // Prefer to suggest download of BS if it's anywhere in the list
                    package_name = BS_PACKAGE;
                }
                else {
                    // Otherwise, first option:
                    package_name = target_applications.get(0);
                }
                Uri uri = Uri.parse("market://details?id=" + package_name);
                Intent intent = new Intent(Intent.ACTION_VIEW, uri);
                try {
                    if (fragment == null) {
                        activity.startActivity(intent);
                    }
                    else {
                        fragment.startActivity(intent);
                    }
                }
                catch (ActivityNotFoundException anfe) {
                    // Hmm, market is not installed
                    FirebaseCrashlytics.getInstance().recordException(anfe); //--strip
                    Log.w(TAG, "Google Play is not installed; cannot install " + package_name);
                }
            }
        });
        download_dialog.setNegativeButton(button_no, null);
        download_dialog.setCancelable(true);
        return download_dialog.show();
    }


    /**
     * <p>Call this from your {@link Activity}'s
     * {@link Activity#onActivityResult(int, int, Intent)} method.</p>
     *
     * @param requestCode request code from {@code onActivityResult()}
     * @param resultCode result code from {@code onActivityResult()}
     * @param intent {@link Intent} from {@code onActivityResult()}
     * @return null if the event handled here was not related to this class, or
     *  else an {@link IntentResult} containing the result of the scan. If the user cancelled scanning,
     *  the fields will be null.
     */
    public static scan_intent_result parse_activity_result(int request_code, int result_code, Intent intent) {
        if (request_code == REQUEST_CODE) {
            if (result_code == Activity.RESULT_OK) {
                String contents = intent.getStringExtra("SCAN_RESULT");
                String format_name = intent.getStringExtra("SCAN_RESULT_FORMAT");
                byte[] raw_bytes = intent.getByteArrayExtra("SCAN_RESULT_BYTES");
                int intent_orientation = intent.getIntExtra("SCAN_RESULT_ORIENTATION", Integer.MIN_VALUE);
                Integer orientation = intent_orientation == Integer.MIN_VALUE ? null : intent_orientation;
                String error_correction_level = intent.getStringExtra("SCAN_RESULT_ERROR_CORRECTION_LEVEL");
                return new scan_intent_result(contents,
                        format_name,
                        raw_bytes,
                        orientation,
                        error_correction_level);
            }
            return new scan_intent_result();
        }
        return null;
    }


    /**
     * Defaults to type "TEXT_TYPE".
     *
     * @param text the text string to encode as a barcode
     * @return the {@link AlertDialog} that was shown to the user prompting them to download the app
     *   if a prompt was needed, or null otherwise
     * @see #shareText(CharSequence, CharSequence)
     */
    public final AlertDialog share_text(CharSequence text) {
        return share_text(text, "TEXT_TYPE");
    }

    /**
     * Shares the given text by encoding it as a barcode, such that another user can
     * scan the text off the screen of the device.
     *
     * @param text the text string to encode as a barcode
     * @param type type of data to encode. See {@code com.google.zxing.client.android.Contents.Type} constants.
     * @return the {@link AlertDialog} that was shown to the user prompting them to download the app
     *   if a prompt was needed, or null otherwise
     */
    public final AlertDialog share_text(CharSequence text, CharSequence type) {
        Intent intent = new Intent();
        intent.addCategory(Intent.CATEGORY_DEFAULT);
        intent.setAction(BS_PACKAGE + ".ENCODE");
        intent.putExtra("ENCODE_TYPE", type);
        intent.putExtra("ENCODE_DATA", text);
        String target_app_package = find_target_app_package(intent);
        if (target_app_package == null) {
            return show_download_dialog();
        }
        intent.setPackage(target_app_package);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        intent.addFlags(FLAG_NEW_DOC);
        attach_more_extras(intent);
        if (fragment == null) {
            activity.startActivity(intent);
        } else {
            fragment.startActivity(intent);
        }
        return null;
    }

    private static List<String> list(String... values) {
        return Collections.unmodifiableList(Arrays.asList(values));
    }

    private void attach_more_extras(Intent intent) {
        for (Map.Entry<String,Object> entry : more_extras.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();
            // Kind of hacky
            if (value instanceof Integer) {
                intent.putExtra(key, (Integer) value);
            } else if (value instanceof Long) {
                intent.putExtra(key, (Long) value);
            } else if (value instanceof Boolean) {
                intent.putExtra(key, (Boolean) value);
            } else if (value instanceof Double) {
                intent.putExtra(key, (Double) value);
            } else if (value instanceof Float) {
                intent.putExtra(key, (Float) value);
            } else if (value instanceof Bundle) {
                intent.putExtra(key, (Bundle) value);
            } else {
                intent.putExtra(key, value.toString());
            }
        }
    }

}

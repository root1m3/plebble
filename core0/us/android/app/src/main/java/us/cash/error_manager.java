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
import android.app.Application;                                                                // Application
import android.content.Context;                                                                // Context

//import org.acra.annotation.AcraCore;                                                           // AcraCore
//import org.acra.ACRA;                                                                          // ACRA
//import org.acra.BuildConfig;                                                                   // BuildConfig
//import org.acra.config.CoreConfigurationBuilder;                                               // CoreConfigurationBuilder
//import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
//import org.acra.config.HttpSenderConfigurationBuilder;                                         // HttpSenderConfigurationBuilder
//import org.acra.sender.HttpSender;                                                             // HttpSender
//import org.acra.data.StringFormat;                                                             // StringFormat

//@AcraCore(reportFormat = StringFormat.JSON)
public class error_manager {

    static private void log(final String line) {        //--strip
       CFG.log_android("error_manager: " + line);       //--strip
    }                                                   //--strip

    public static void manage(final Exception e, final String error) {
        log(error); //--strip
        if (!CFG.acra_connection_string.isEmpty()) {
            //FirebaseCrashlytics.getInstance().recordException(e); //--strip
        }
        else {
            log("acra is not configured"); //--strip
            e.printStackTrace();
            log(error + " " + e.getMessage() + " - " + e.toString()); //--strip
            System.exit(1);
        }
        e.printStackTrace();
        log(error + " " + e.getMessage() + " - " + e.toString()); //--strip
    }

    public static void setup(Context ctx, Application ap) {
/*
        if (!CFG.acra_connection_string.isEmpty()) {
            String[] s = CFG.acra_connection_string.split("-");
            if (s.length!=3) {
                log("ACRA connection string ust have 3 fields."); //--strip
                return;
            }
            CoreConfigurationBuilder builder = new CoreConfigurationBuilder(ctx);
            builder.setBuildConfigClass(BuildConfig.class).setReportFormat(StringFormat.JSON);
            builder.getPluginConfigurationBuilder(HttpSenderConfigurationBuilder.class).setUri(s[0]).setBasicAuthLogin(s[1]).setBasicAuthPassword(s[2]).setHttpMethod(HttpSender.Method.POST).setEnabled(true);  //--strip
            if (BuildConfig.DEBUG) {
                ACRA.DEV_LOGGING = true;
            }
            ACRA.init(ap, builder);
        }
*/
    }
}


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
import android.app.Application;                                                                // Application
import android.os.Build;                                                                       // Build
import android.content.res.Configuration;                                                      // Configuration
import android.content.Context;                                                                // Context
import android.view.ContextThemeWrapper;                                                       // ContextThemeWrapper
import java.util.Locale;                                                                       // Locale
import android.preference.PreferenceManager;                                                   // PreferenceManager
import android.content.res.Resources;                                                          // Resources
import android.content.SharedPreferences;                                                      // SharedPreferences

public class locale {

    private static Locale instance;

    public static void set(Locale locale){
        instance = locale;
        if(instance != null){
            Locale.setDefault(instance);
        }
    }

    public static void update(ContextThemeWrapper wrapper){
        if(instance != null && Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1){
            Configuration configuration = new Configuration();
            configuration.setLocale(instance);
            wrapper.applyOverrideConfiguration(configuration);
        }
    }

    public static void update(Application application, Configuration configuration){
        if (instance != null && Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN_MR1) {
            Configuration config = new Configuration(configuration);
            config.locale = instance;
            Resources res = application.getBaseContext().getResources();
            res.updateConfiguration(configuration, res.getDisplayMetrics());
        }
    }

    public static void update(Context context, String language, String country){
        Locale locale = new Locale(language, country);
        set(locale);
        if (instance != null) {
            Resources res = context.getResources();
            Configuration configuration = res.getConfiguration();
            configuration.locale = instance;
            res.updateConfiguration(configuration,res.getDisplayMetrics());
        }
    }

    public static String get_lang(Context context) {
        return PreferenceManager.getDefaultSharedPreferences(context).getString("lang_code", "en");
    }

    public static void set_lang(Context context, String code) {
        SharedPreferences.Editor editor = PreferenceManager.getDefaultSharedPreferences(context).edit();
        editor.putString("lang_code", code);
        editor.commit();
    }

    public static String get_country(Context context) {
        return PreferenceManager.getDefaultSharedPreferences(context).getString("country_code", "US");
    }

    public static void set_country(Context context, String code) {
        SharedPreferences.Editor editor = PreferenceManager.getDefaultSharedPreferences(context).edit();
        editor.putString("country_code", code);
        editor.commit();
    }

}


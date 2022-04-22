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
import android.util.AttributeSet;                                                              // AttributeSet
import android.content.Context;                                                                // Context
import com.google.firebase.crashlytics.FirebaseCrashlytics;                                    // FirebaseCrashlytics
import android.widget.ImageButton;                                                             // ImageButton
import android.view.KeyEvent;                                                                  // KeyEvent
import android.widget.LinearLayout;                                                            // LinearLayout
import android.view.MotionEvent;                                                               // MotionEvent
import androidx.annotation.StyleableRes;                                                       // StyleableRes
import android.widget.TextView;                                                                // TextView
import android.widget.Toast;                                                                   // Toast
import android.content.res.TypedArray;                                                         // TypedArray
import android.view.View;                                                                      // View

public class toolbar_button extends LinearLayout implements View.OnClickListener {

    private OnClickListener onClickListener;

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        if(event.getAction() == KeyEvent.ACTION_UP &&
                (event.getKeyCode() == KeyEvent.KEYCODE_DPAD_CENTER || event.getKeyCode() == KeyEvent.KEYCODE_ENTER)) {
            if(onClickListener != null) onClickListener.onClick(this);
        }
        return super.dispatchKeyEvent(event);
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            setPressed(true);
        }
        else if (event.getAction() == MotionEvent.ACTION_UP) {
            if (onClickListener != null) onClickListener.onClick(this);
            setPressed(false);
        }
        else {
            setPressed(false);
        }
        return super.dispatchTouchEvent(event);
    }

    @Override
    public void setOnClickListener(OnClickListener listener) {
        this.onClickListener = listener;
    }

    @Override
    public void onClick(View v) {
        if (onClickListener != null && onClickListener != this)
            onClickListener.onClick(v);
    }

    @StyleableRes
    int index0 = 0;
    @StyleableRes
    int index1 = 1;
    @StyleableRes
    int index2 = 2;

    ImageButton button_image;
    TextView button_text;
    Context context;

    public toolbar_button(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context, attrs);
    }

    private void init(Context context, AttributeSet attrs) {
        this.context = context;
        inflate(context, R.layout.toolbar_button, this);
        setOnClickListener(this);
        int[] sets = {R.attr.custom_src, R.attr.custom_text, R.attr.custom_id};
        TypedArray typedArray = context.obtainStyledAttributes(attrs, sets);
        CharSequence cussrc = typedArray.getText(index0);
        CharSequence custext = typedArray.getText(index1);
        CharSequence custid = typedArray.getText(index2);
        typedArray.recycle();
        initComponents();
        set_custom_src(cussrc);
        set_custom_text(custext);
    }

    private void initComponents() {
        button_image = findViewById(R.id.button_image);
        button_text = findViewById(R.id.button_text);
    }

    public ImageButton get_button_image() {
        return button_image;
    }

    public TextView get_button_text() {
        return button_text;
    }

    public void set_custom_src(CharSequence custom_src) {
        try {
            button_image.setImageDrawable(this.context.getResources().getDrawable(this.context.getResources().getIdentifier(custom_src.toString(), "drawable", this.context.getPackageName())));
        }
        catch (Exception ex) {
            Toast.makeText(context, getResources().getString(R.string.error) + ex.getMessage(), 12000);
            error_manager.manage(ex, ex.getMessage() + "    " + ex.toString());
        }
    }

    public void set_custom_text(String custom_text) {
        try{
            button_text.setText(custom_text);
        }
        catch (Exception ex) {
            Toast.makeText(context, getResources().getString(R.string.error)+ ex.getMessage(), 12000);
            error_manager.manage(ex, ex.getMessage() + "    " + ex.toString());
        }
    }

    public void set_custom_text(CharSequence custom_text) {
        try{
            button_text.setText(custom_text);
        }
        catch (Exception ex) {
            Toast.makeText(context, getResources().getString(R.string.error)+ ex.getMessage(), 12000);
            error_manager.manage(ex, ex.getMessage() + "    " + ex.toString());
        }
    }

    public void set_custom_id(CharSequence custom_id) {
        try{
            this.setId(this.context.getResources().getIdentifier(custom_id.toString(), "id", this.context.getPackageName()));
        }
        catch (Exception ex) {
            Toast.makeText(context, getResources().getString(R.string.error) + ex.getMessage(), 12000);
            error_manager.manage(ex, ex.getMessage() + "    " + ex.toString());
        }
    }

    public void set_stock_image(int i) {
        switch(i) {
            case 2:
                button_image.setImageResource(R.drawable.ic_pay_48dp);
            break;
            case 3:
                button_image.setImageResource(R.raw.create);
            break;
            default:
                button_image.setImageResource(R.drawable.ic_send_48dp);
        }
    }
}


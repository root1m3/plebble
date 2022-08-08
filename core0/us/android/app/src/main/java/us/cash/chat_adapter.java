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
import android.view.animation.Animation;                                                       // Animation
import android.view.animation.AnimationUtils;                                                  // AnimationUtils
import java.util.ArrayList;                                                                    // ArrayList
import android.content.ClipboardManager;                                                       // ClipboardManager
import android.content.ClipData;                                                               // ClipData
import android.content.Context;                                                                // Context
import android.os.Environment;                                                                 // Environment
import java.io.File;                                                                           // File
import com.bumptech.glide.Glide;                                                               // Glide
import android.widget.ImageView;                                                               // ImageView
import android.view.LayoutInflater;                                                            // LayoutInflater
import android.util.Log;                                                                       // Log
import android.media.MediaPlayer;                                                              // MediaPlayer
import androidx.annotation.NonNull;                                                            // NonNull
import androidx.recyclerview.widget.RecyclerView;                                              // RecyclerView
import java.text.SimpleDateFormat;                                                             // SimpleDateFormat
import android.widget.TextView;                                                                // TextView
import android.net.Uri;                                                                        // Uri
import android.view.ViewGroup;                                                                 // ViewGroup
import android.view.View;                                                                      // View

public class chat_adapter extends RecyclerView.Adapter {

    private static final int VIEW_TYPE_MESSAGE_SENT = 1;
    private static final int VIEW_TYPE_MESSAGE_RECEIVED = 2;
    private static final int LATEST_TYPE_MESSAGE_SENT = 3;
    private static final int LATEST_TYPE_MESSAGE_RECEIVED = 4;

    private Context context;
    private ArrayList<message_t> array_list;
    private static MediaPlayer media_player = new MediaPlayer(); //TODO review static

    static private void log(final String line) {            //--strip
       CFG.log_android("chat_adapter: " + line);            //--strip
    }                                                       //--strip

    public chat_adapter(Context context, ArrayList<message_t> array_list) {
        this.context = context;
        this.array_list = array_list;
    }

    @Override public int getItemViewType(int position) {
        message_t message = array_list.get(position);
        if (message.is_sent() && position != array_list.size() - 1)
            return VIEW_TYPE_MESSAGE_SENT;
        else if (!message.is_sent() && position != array_list.size() - 1)
            return VIEW_TYPE_MESSAGE_RECEIVED;
        else if (message.is_sent() && position == array_list.size() - 1)
            return LATEST_TYPE_MESSAGE_SENT;
        else if (!message.is_sent() && position == array_list.size() - 1)
            return LATEST_TYPE_MESSAGE_RECEIVED;
        return 0;
    }

    @NonNull @Override public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        log("onCreateViewHolder" + viewType); //--strip
        View view;
        Animation animation = AnimationUtils.loadAnimation(context, R.anim.slide_from_bottom);

        if (viewType == VIEW_TYPE_MESSAGE_SENT) {
            log("VIEW_TYPE_MESSAGE_SENT"); //--strip
            view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_message_sent, parent, false);
            return new sent_message_holder(view);
        }
        else if (viewType == VIEW_TYPE_MESSAGE_RECEIVED) {
            log("VIEW_TYPE_MESSAGE_RECEIVED"); //--strip
            view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_message_received, parent, false);
            return new received_message_holder(view);
        }
        else if (viewType == LATEST_TYPE_MESSAGE_SENT) {
            log("LATEST_TYPE_MESSAGE_SENT"); //--strip
            view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_message_sent, parent, false);
            view.startAnimation(animation);
            return new sent_message_holder(view);
        }
        else if (viewType == LATEST_TYPE_MESSAGE_RECEIVED) {
            log("LATEST_TYPE_MESSAGE_RECEIVED"); //--strip
            view = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_message_received, parent, false);
            view.startAnimation(animation);
            return new received_message_holder(view);
        }
        return null;
    }

    @Override public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        try {
            log("onBindViewHolder" + position + ' ' + array_list.size());  //--strip
            message_t message = array_list.get(position);
            switch (holder.getItemViewType()) {
                case VIEW_TYPE_MESSAGE_SENT:
                    ((sent_message_holder) holder).bind(message);
                    break;
                case VIEW_TYPE_MESSAGE_RECEIVED:
                    ((received_message_holder) holder).bind(message);
                    break;
                case LATEST_TYPE_MESSAGE_SENT:
                    ((sent_message_holder) holder).bind(message);
                    break;
                case LATEST_TYPE_MESSAGE_RECEIVED:
                    ((received_message_holder) holder).bind(message);
                    break;
            }
        }
        catch (Exception e) {
            log("KO 92328 error in chat: " + e.getLocalizedMessage()); //--strip
        }
    }

    @Override public int getItemCount() {
        return array_list.size();
    }

    private class message_holder extends RecyclerView.ViewHolder {
        TextView message_text, time_text;
        ImageView message_image, play_button, pause_button;
        ClipboardManager clipboard = (ClipboardManager) context.getSystemService(Context.CLIPBOARD_SERVICE);

        public message_holder(View item_view) {
            super(item_view);
            log("message_holder constructor"); //--strip
            message_text = item_view.findViewById(R.id.text_message_body);
            time_text = item_view.findViewById(R.id.text_message_time);
            message_image = item_view.findViewById(R.id.received_image);
            play_button = item_view.findViewById(R.id.play_button);
            pause_button = item_view.findViewById(R.id.pause_button);
            item_view.setOnLongClickListener(this::on_long_click);
        }

        public void media_player_controls(boolean playing) {
            if (playing) {
                play_button.setVisibility(View.INVISIBLE);
                pause_button.setVisibility(View.VISIBLE);
            }
            else {
                play_button.setVisibility(View.VISIBLE);
                pause_button.setVisibility(View.INVISIBLE);
            }
        }

        boolean on_long_click(View view) {
            ClipData clip = ClipData.newPlainText("Copied Text", message_text.getText());
            clipboard.setPrimaryClip(clip);
            return true;
        }

    }

    private class received_message_holder extends message_holder {

        received_message_holder(View item_view) {
            super(item_view);
            item_view.setOnClickListener(this::on_click);
        }

        void bind(message_t message) {
            SimpleDateFormat sdf = new SimpleDateFormat("hh:mm a");
            String current_ts = sdf.format(message.get_time());
            if (message_text == null) {
                log("message_text is null");  //--strip
            }
            assert message_text != null;
            message_text.setText(message.get());
            time_text.setText(current_ts);
            log("bind: " + message.get()); //--strip
            if (message.get().contains("New File Received: ") &&
                    (message.get().contains("png") ||
                            message.get().contains("jpg") ||
                            message.get().contains("jpeg"))) {
                String[] fileName = message.get().split(":");
                log("bind: Received Message" + fileName[1]); //--strip
                StringBuilder stringBuilder = new StringBuilder(fileName[1]);
                stringBuilder.deleteCharAt(0);
                String path = stringBuilder.toString();
                String directory = Environment.getExternalStorageDirectory() + "/Download/" + path;
                log("bind: Print Directory:" + directory);  //--strip
                File imgFile = new File(directory);
                if (imgFile.exists()) {
                    log("bind: +Yeah Exists"); //--strip
                    Glide.with(context)
                            .load(directory)
                            .override(500, 500)
                            .into(message_image);
                }
            }
            else if (message.get().contains("New File Received: ") && (message.get().contains("mp3"))) {
                media_player_controls(media_player.isPlaying());
            }
        }

        void on_click(View view) {
            if (message_text.getText().toString().contains(".mp3") && message_text.getText().toString().contains("New File Received: ")) {
                String[] message = message_text.getText().toString().split(":");
                log("onClick: " + message[1]); //--strip
                String filename = message[1];
                filename = filename.trim();
                String path = Environment.getExternalStorageDirectory() + "/Download/";
                log("path and filename => " + path+filename);  //--strip
                Uri uri = Uri.parse(path + filename);
                if (media_player.isPlaying()) {
                    media_player.stop();
                }
                else {
                    media_player.start();
                }
                media_player_controls(media_player.isPlaying());
            }
        }

    }

    private class sent_message_holder extends message_holder {

        sent_message_holder(View item_view) {
            super(item_view);
            item_view.setOnClickListener(this::on_click);
        }

        void bind(message_t message) {
            String new_message = message.get();
            SimpleDateFormat sdf = new SimpleDateFormat("hh:mm a");
            String current_ts = sdf.format(message.get_time());

            if (message.get().contains("New File Sent: ") && (message.get().contains("png") || message.get().contains("jpg") || message.get().contains("jpeg"))) {
                String[] fileName = message.get().split(":");
                log("bind: Received Message" + fileName[2]); //--strip
                String[] newStringArr = new_message.split(":");
                new_message = newStringArr[0] + newStringArr[1];
                log("bind: Directory:" + new_message);  //--strip
                String directory = newStringArr[2];
                log("bind: Print Directory:" + directory);  //--strip
                File imgFile = new File(directory);
                if (imgFile.exists()) {
                    log("bind: +Yeah Exists");  //--strip
                    Glide.with(context).load(directory).override(500, 500).into(message_image);
                }
            }
            else if (message.get().contains("New File Sent: ") && (message.get().contains("mp3"))) {
                media_player_controls(media_player.isPlaying());
            }
            if (message_text == null) {
                log("message_text is null.");  //--strip
            }
            assert message_text != null;
            message_text.setText(new_message);
            time_text.setText(current_ts);
        }

        void on_click(View view) {
            if (message_text.getText().toString().contains(".mp3") && message_text.getText().toString().contains("New File Sent: ")) {
                String[] message = message_text.getText().toString().split(":");
                log("onClick: " + message[2]); //--strip
                String path = message[2];
                path = path.trim();
                Uri uri = Uri.parse(path);
                if (media_player.isPlaying()) {
                    media_player.stop();
                }
                else {
                    media_player = MediaPlayer.create(context, uri);
                    media_player.start();
                }
                media_player_controls(media_player.isPlaying());
            }
        }
    }
}


/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;

public class RoomStatusArrayAdapter extends ArrayAdapter<Room> {
    private final Context context;
    private final ArrayList<Room> rooms;
    private MainActivity mainActivity = null;
    private int touchPositionX = 0;

    class UserHolder {
        TextView txtName;
        TextView txtTemp;
        TextView txtPercent;
        TextView txtMsgCnt;
        ImageView imgStatus;
        SeekBar seekBar;
        LinearLayout layoutCmd;
    }

    public RoomStatusArrayAdapter(Context context, ArrayList<Room> rooms) {
        super(context, -1, rooms);
        this.context = context;
        this.rooms = rooms;
        this.mainActivity = (MainActivity) context;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        UserHolder holder = null;
        View rowView = convertView;
        if(rowView == null) {
            LayoutInflater inflater = (LayoutInflater) context
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            rowView = inflater.inflate(R.layout.roomstatus_view, parent, false);

            holder = new UserHolder();
            holder.txtName = rowView.findViewById(R.id.txtRoomStatusName);
            holder.txtTemp = rowView.findViewById(R.id.txtRoomStatusTemp);
            holder.txtPercent = rowView.findViewById(R.id.txtRoomStatusPercent);
            holder.txtMsgCnt = rowView.findViewById(R.id.txtRoomStatusMsgCnt);
            holder.imgStatus = rowView.findViewById(R.id.imageStatus);
            holder.seekBar =  rowView.findViewById(R.id.seekBar);
            holder.layoutCmd = rowView.findViewById(R.id.layoutCmd);
            rowView.setTag(holder);
        } else {
            holder = (UserHolder) rowView.getTag();
        }

        Room room = rooms.get(position);
        holder.layoutCmd.setVisibility(room.showControls ? View.VISIBLE : View.GONE);
        holder.txtName.setText( room.name + " : ");
        String mode = "";
        String temp = "";
        String percent = "(?)";

        if(room.valid) {
            temp = Format.tempToString(room.temp);
            if(room.lowBattery)
                mode += "!";
            if(room.autoActive)
                mode = "A";
            if(room.boostActive)
                mode += "B";
            if(mode.length()>0)
                mode += " ";

            percent = "("+mode+room.percent+"%)";
        }

        int icon = R.drawable.ic_offline;
        switch(room.connectionState) {
            case CONNECTED:
                icon = R.drawable.ic_online;
                break;
            case CONNECTING:
                icon = R.drawable.ic_connecting;
                break;
            case QUEUED:
                break;
            case DISCONNECTED:
                icon = R.drawable.ic_offline;
                break;
            case FAILED:
                icon = R.drawable.ic_failed;
                break;
        }
        holder.txtTemp.setText((room.isSensorTemp?"":">") + temp);
        holder.txtPercent.setText(percent);
        holder.imgStatus.setImageResource(icon);
        holder.txtMsgCnt.setText(""+room.msgCount);
        final int roomId = room.id;

        holder.seekBar.setMax(10);
        holder.seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
//               Log.d("temp", Integer.toString(i) +  " : " + Boolean.toString(b));
                int temp = 0;
                if (i<=0) {
                    temp = Const.TEMP_MIN;
                }
                else if (i>=10) {
                    temp = Const.TEMP_MAX;
                } else {
                    temp = 175 + (i*5);
                }
                mainActivity.requestSetTemp(roomId, temp);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        holder.txtName.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mainActivity.showRoomSettingsActivity(roomId);
            }
        });

        holder.txtPercent.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mainActivity.requestStatusOfRoom(roomId);
            }
        });
        //        mainActivity.requestSetTemp(roomId, Const.TEMP_MIN);
/*
        rowView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (touchPositionX < view.getWidth() * 2 / 3) {
                    mainActivity.showRoomSettingsActivity(roomId);
                } else {
                    mainActivity.requestStatusOfRoom(roomId);
                }

            }
        });

        rowView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_DOWN){
                    touchPositionX = (int)event.getX();
                }
                return false;
            }
        });
*/

        rowView.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View view) {
                mainActivity.closeConnection(roomId);
                return false;
            }
        });

        return rowView;
    }


}
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
        Button btnOff;
        Button btnOn;
        Button btnPreset0;
        Button btnPreset1;
        Button btnPreset2;
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
            holder.btnOff = rowView.findViewById(R.id.btnOff);
            holder.btnOn = rowView.findViewById(R.id.btnOn);
            holder.btnPreset0 = rowView.findViewById(R.id.btnPreset0);
            holder.btnPreset1 = rowView.findViewById(R.id.btnPreset1);
            holder.btnPreset2 = rowView.findViewById(R.id.btnPreset2);
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
        String percent = "";

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
        holder.txtTemp.setText(temp);
        holder.txtPercent.setText(percent);
        holder.imgStatus.setImageResource(icon);
        holder.txtMsgCnt.setText(""+room.msgCount);
        final int roomId = room.id;
        final int roomPresetTemp0 = room.presetTemp.get(0);
        final int roomPresetTemp1 = room.presetTemp.get(1);
        final int roomPresetTemp2 = room.presetTemp.get(2);

        holder.btnPreset0.setText(Format.tempToString(roomPresetTemp0));
        holder.btnPreset1.setText(Format.tempToString(roomPresetTemp1));
        holder.btnPreset2.setText(Format.tempToString(roomPresetTemp2));

        holder.btnOff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mainActivity.requestSetTemp(roomId, Const.TEMP_MIN);
            }
        });

        holder.btnOn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mainActivity.requestSetTemp(roomId, Const.TEMP_MAX);
            }
        });

        holder.btnPreset0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mainActivity.requestSetTemp(roomId, roomPresetTemp0);
            }
        });

        holder.btnPreset1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mainActivity.requestSetTemp(roomId, roomPresetTemp1);
            }
        });

        holder.btnPreset2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mainActivity.requestSetTemp(roomId, roomPresetTemp2);
            }
        });

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
/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.util.ArrayList;

public class RoomStatusArrayAdapter extends ArrayAdapter<Room> {
    private final Context context;
    private final ArrayList<Room> rooms;

    public RoomStatusArrayAdapter(Context context, ArrayList<Room> rooms) {
        super(context, -1, rooms);
        this.context = context;
        this.rooms = rooms;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View rowView = inflater.inflate(R.layout.roomstatus_view, parent, false);

        TextView txtName = rowView.findViewById(R.id.txtRoomStatusName);
        TextView txtTemp = rowView.findViewById(R.id.txtRoomStatusTemp);
        TextView txtPercent = rowView.findViewById(R.id.txtRoomStatusPercent);
        TextView txtMsgCnt = rowView.findViewById(R.id.txtRoomStatusMsgCnt);
        ImageView imgStatus = rowView.findViewById(R.id.imageStatus);

        Room room = rooms.get(position);

        txtName.setText( room.name + " : ");
        String mode = "";
        String temp = "";
        String percent = "";

        if(room.valid) {
            temp = Format.tempToString(room.temp);
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
        txtTemp.setText(temp);
        txtPercent.setText(percent);
        imgStatus.setImageResource(icon);
        txtMsgCnt.setText(""+room.msgCount);
        return rowView;
    }
}
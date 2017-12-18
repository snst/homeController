/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
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

        TextView txtName = (TextView) rowView.findViewById(R.id.txtRoomStatusName);
        TextView txtTemp = (TextView) rowView.findViewById(R.id.txtRoomStatusTemp);
        TextView txtPercent = (TextView) rowView.findViewById(R.id.txtRoomStatusPercent);
        TextView txtMsgCnt = (TextView) rowView.findViewById(R.id.txtRoomStatusMsgCnt);

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
                mode += ",";

            percent = "("+mode+room.percent+"%)";
        }
        txtTemp.setText(temp);
        txtPercent.setText(percent);
        String status = " ";
        switch(room.connectionState) {
            case CONNECTED:
                status = "C";
                break;
            case CONNECTING:
                status = "c";
                break;
            case DISCONNECTED:
                status = "d";
                break;
            case NORESPONSE:
                status = "r";
                break;
        }
        txtMsgCnt.setText(status + "#"+room.msgCount);

        return rowView;
    }
}
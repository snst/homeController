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

import com.github.czy1121.view.SegmentedView;

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
        SegmentedView sv;
    }

    public RoomStatusArrayAdapter(Context context, ArrayList<Room> rooms) {
        super(context, -1, rooms);
        this.context = context;
        this.rooms = rooms;
        this.mainActivity = (MainActivity) context;
    }

    private void updateConnectionStatusIcon(UserHolder holder, eConnectionState state) {
        int icon = R.drawable.ic_offline;
        switch(state) {
            case CONNECTED:
                icon = R.drawable.ic_online;
                break;
            case CONNECTING:
                icon = R.drawable.ic_connecting;
                break;
            case QUEUED:
                break;
            case FAILED:
                icon = R.drawable.ic_failed;
                break;
            case DISCONNECTED:
            default:
                icon = R.drawable.ic_offline;
                break;
        }
        holder.imgStatus.setImageResource(icon);
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
            holder.sv = rowView.findViewById(R.id.sv1);
            rowView.setTag(holder);
        } else {
            holder = (UserHolder) rowView.getTag();
        }

        Room room = rooms.get(position);
        holder.sv.setVisibility(room.showControls ? View.VISIBLE : View.GONE);
        holder.txtName.setText( room.name + " : ");
        String mode = "";
        String temp = "";
        String percent = "";

        float alphaTxt = room.refreshing ? 0.3f : 1.0f;
//        float alphaIcon = room.refreshing ? 0.3f : 0.7f;

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

            percent +=  mode + room.percent + "%";
        }

        updateConnectionStatusIcon(holder, room.connectionState);

//        holder.txtTemp.setText((room.isSensorTemp ? "" : ">") + temp);
        holder.txtTemp.setText(temp);
        holder.txtPercent.setText(percent);
        holder.txtMsgCnt.setText(""+room.msgCount);

        holder.txtTemp.setAlpha(alphaTxt);
        holder.txtPercent.setAlpha(alphaTxt);
//        holder.imgStatus.setAlpha(alphaIcon);

        final int roomId = room.id;
        holder.sv.setOnItemSelectedListener(new SegmentedView.OnItemSelectedListener() {
            @Override
            public void onSelected(int index, String text) {
               // Toast.makeText(this,  index + " : " + text, Toast.LENGTH_SHORT).show();
                int val = 0;
                if(text.equals("on"))
                    val = Const.TEMP_MAX;
                else if(text.equals("off"))
                    val = Const.TEMP_MIN;
                else val = Integer.parseInt(text) * 10;

                mainActivity.requestSetTemp(roomId, val);
            }
        });

        holder.txtName.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mainActivity.showRoomSettingsActivity(roomId);
            }
        });

        final UserHolder h = holder;
        View.OnClickListener l = new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mainActivity.requestStatusOfRoom(roomId);
                mainActivity.repaintRooms();
            }
        };
        holder.txtPercent.setOnClickListener(l);
        holder.imgStatus.setOnClickListener(l);

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
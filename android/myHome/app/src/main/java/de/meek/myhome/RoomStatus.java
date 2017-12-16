/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

public class RoomStatus extends LinearLayout implements IRoom {

    public interface IOnClickListener {
        void onClick(int roomId);
    }

    private TextView txtName;
    private TextView txtTemp;
    private TextView txtPercent;
    private TextView txtMsgCnt;
    private int roomId;

    public RoomStatus(Context context, int _roomId) {
        super(context);
        roomId = _roomId;
        initializeViews(context);
    }

    /**
     * Inflates the views in the layout.
     *
     * @param context
     *           the current context for the view.
     */
    private void initializeViews(Context context) {
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.roomstatus_view, this);

        txtName = (TextView)this.findViewById(R.id.txtRoomStatusName);
        txtTemp = (TextView)this.findViewById(R.id.txtRoomStatusTemp);
        txtPercent = (TextView)this.findViewById(R.id.txtRoomStatusPercent);
        txtMsgCnt = (TextView)this.findViewById(R.id.txtRoomStatusMsgCnt);

        View.OnClickListener ol = new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                onRoomClickShowSettings(roomId);
            }
        };

        View.OnClickListener ol2 = new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                onRoomClickRefresh(roomId);
            }
        };
        txtName.setOnClickListener(ol);
        txtTemp.setOnClickListener(ol);
        txtPercent.setOnClickListener(ol2);
    }

    public void onRoomClickShowSettings(int roomId) {}
    public void onRoomClickRefresh(int roomId) {}

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();
    }

    @Override
    public void update(Room room) {
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
        txtMsgCnt.setText("#"+room.msgCount);
    }
}
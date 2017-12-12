/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Intent;
import android.content.SharedPreferences;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class RoomSettingsActivity extends AppCompatActivity {

    String roomName;
    String btAddress;
    TextView txtBtAddress;
    TextView txtRoomName;
    Room room;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_room_settings);
        setTitle("Room Settings");

        txtRoomName = (TextView) findViewById(R.id.editRoomName);
        txtBtAddress = (TextView) findViewById(R.id.editBTAddress);

        Intent intent = getIntent();
        int id = intent.getIntExtra(Const.INTENT_X_ROOM_ID, 0);
        room = House.getRoom(id);

        txtRoomName.setText( room.name);
        txtBtAddress.setText( room.btAddress);
    }

    public void onSaveSettings(View view) {

        room.name = txtRoomName.getText().toString();
        room.btAddress = txtBtAddress.getText().toString();

//        long bt = Format.getBTAddress(room.btAddress);

        SharedPreferences settings = getSharedPreferences(room.getId(), 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(Const.INTENT_X_ROOM_NAME, room.name);
        editor.putString(Const.INTENT_X_BT_ADDRESS, room.btAddress);
        editor.commit();

        Intent data = new Intent();
        setResult(RESULT_OK,data);
        finish();
    }
}

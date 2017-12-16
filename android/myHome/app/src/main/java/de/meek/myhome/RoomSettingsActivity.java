/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Intent;
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
        int id = intent.getIntExtra(Const.INTENT_ROOM_ID, 0);
        room = ((MyApplication)getApplicationContext()).getHouse().getRoom(id);

        txtRoomName.setText( room.name);
        txtBtAddress.setText( room.btAddress.toString());
    }

    public void onSaveSettings(View view) {

        room.name = txtRoomName.getText().toString();
        if(!room.btAddress.convertFromString(txtBtAddress.getText().toString())) {
            Toast.makeText(this, "Invalid BT address!", Toast.LENGTH_LONG).show();
        }

        Settings settings = new Settings(this);
        settings.saveRoom(room);

        Intent data = new Intent();
        setResult(RESULT_OK,data);
        finish();
    }
}

/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class MainSettingsActivity extends AppCompatActivity {

    private String getEditText(int id) {
        return ((TextView) findViewById(id)).getText().toString();
    }

    private void setEditText(int id, String text) {
        ((TextView) findViewById(id)).setText(text);
    }

    public void onBtnSave(View view) {
        AccountConfig.MQTT_SERVER_URI = getEditText(R.id.editServer);
        AccountConfig.MQTT_CLIENT_ID = getEditText(R.id.editClientId);
        AccountConfig.MQTT_USERNAME = getEditText(R.id.editUsername);
        AccountConfig.MQTT_PASSWORD = getEditText(R.id.editPassword);
        AccountConfig.MQTT_TOPIC_DEFAULT = getEditText(R.id.editDefMqttTopic);
        if(!AccountConfig.BT_ADDRESS_DEFAULT.convertFromString(getEditText(R.id.editDefBtAddr))) {
            Toast.makeText(this, "Invalid BT address!", Toast.LENGTH_LONG).show();
        }
        int n = Integer.parseInt(getEditText(R.id.editNumberOfRooms));
        n = Math.min(20, n);
        n = Math.max(1, n);
        AccountConfig.NUMBER_OF_ROOMS = n;

        AccountConfig.save(this);
        setResult(RESULT_OK);
        finish();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_settings);
        setTitle("RoomSettings");

        setEditText(R.id.editServer, AccountConfig.MQTT_SERVER_URI);
        setEditText(R.id.editClientId, AccountConfig.MQTT_CLIENT_ID);
        setEditText(R.id.editUsername, AccountConfig.MQTT_USERNAME);
        setEditText(R.id.editPassword, AccountConfig.MQTT_PASSWORD);
        setEditText(R.id.editDefMqttTopic, AccountConfig.MQTT_TOPIC_DEFAULT);
        setEditText(R.id.editDefBtAddr, AccountConfig.BT_ADDRESS_DEFAULT.toString());
        setEditText(R.id.editNumberOfRooms, ""+AccountConfig.NUMBER_OF_ROOMS);
    }
}

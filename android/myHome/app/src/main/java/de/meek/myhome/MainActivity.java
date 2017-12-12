/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Intent;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import helpers.MqttHelper;

// https://wildanmsyah.wordpress.com/2017/05/11/mqtt-android-client-tutorial/
public class MainActivity extends AppCompatActivity {

    MqttHelper mqttHelper;
    TextView txtViewMsg;
    Handler handler = new Handler();

    int msgCntStatus = 0;
    int msgCntMode = 0;

    public Room getRoom(int i) {
        return House.getRoom(i);
    }

    public void sendCmd(String cmd) {
        mqttHelper.sendCmd(cmd);
    }

    public void requestStatus() {
        DateFormat dateFormat = new SimpleDateFormat("yyMMddHHmmss");
        Date date = new Date();
        String txt = "s" + dateFormat.format(date);
        mqttHelper.sendCmd(txt);
    }

    public void requestSetTemp(int temp) {
        sendCmd("t" + Integer.toString(temp));
        /*
        final int val = temp;
        handler.removeCallbacksAndMessages(null);
        handler.postDelayed(new Runnable() {

            @Override
            public void run() {
                mqttHelper.sendCmd("t" + Integer.toString(val));
            }

        }, 1000); // 5000ms delay
        */
    }

    public void refreshAllRooms() {
        for(int i=0; i<House.getSize(); i++) {
            updateRoom(getRoom(i));
        }
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {

        refreshAllRooms();

        if (resultCode == RESULT_OK) {
            if (requestCode == Const.ACTIVITY_TEMP) {

                String cmd = data.getStringExtra(Const.INTENT_NEW_CMD);
                if(cmd!=null) {
                    mqttHelper.sendCmd(cmd);
                }

                int newTemp = data.getIntExtra(Const.INTENT_X_NEW_TEMP, -1);
                if(newTemp != -1) {
                    requestSetTemp(newTemp);
                }
            } else if( requestCode == Const.ACTIVITY_SETTINGS) {
                restartMqtt();
            }
        }
    }

    public void onBtnRefresh(View view) {
        requestStatus();
    }


    public void onBtnRoom(View view) {

        String roomTag = (String)view.getTag();
        int roomId = Integer.parseInt(roomTag);
        Intent intent = new Intent(this, TempActivity.class);
        intent.putExtra(Const.INTENT_X_ROOM_ID, roomId);
        startActivityForResult(intent,Const.ACTIVITY_TEMP);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch(id) {
            case R.id.action_ping:
                sendCmd(Cmd.PING);
                break;
            case R.id.action_reboot:
                sendCmd(Cmd.REBOOT);
                break;
            case R.id.action_refresh:
                requestStatus();
                break;
            case R.id.action_main_settings:
                showSettingsActivity();
                break;
        }
        return super.onOptionsItemSelected(item);
    }


    void showSettingsActivity() {
        Intent intent = new Intent(this, SettingsActivity.class);
        startActivityForResult(intent,Const.ACTIVITY_SETTINGS);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        txtViewMsg = (TextView) findViewById(R.id.txtViewMsg);
        AccountConfig.load(this);

        House.addRoom(new Room(this, 0, R.id.btnRoom1));
        House.addRoom(new Room(this, 1, R.id.btnRoom2));
        House.addRoom(new Room(this, 2, R.id.btnRoom3));
        House.addRoom(new Room(this, 3, R.id.btnRoom4));

        refreshAllRooms();

        startMqtt();
    }

    void updateRoom(Room room) {
        Button btn = (Button) findViewById(room.buttonId);
        if(btn != null) {
            String statusStr = room.name;
            if(room.valid) {
                statusStr += "  [";
                statusStr += room.autoActive ? "A" : "M";
                if (room.boostActive)
                    statusStr += "B";
                statusStr += "]    ";
                statusStr += Format.tempAndPercentToString(room.temp, room.percent);
            }
            btn.setText(statusStr);
        }
    }

    private void restartMqtt() {

        mqttHelper.disconnect();;
        mqttHelper = null;
        startMqtt();
    }

    public void showStatusMqttConnected() {
        Toast.makeText(this, "MQTT connected", Toast.LENGTH_SHORT).show();
    }
    public void showStatusMqttDisconnected() {
        Toast.makeText(this, "MQTT disconnected", Toast.LENGTH_SHORT).show();
    }

    private void startMqtt(){
        mqttHelper = new MqttHelper(getApplicationContext());
        mqttHelper.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {
                showStatusMqttConnected();
                requestStatus();
            }

            @Override
            public void connectionLost(Throwable throwable) {
                showStatusMqttDisconnected();
            }

            @Override
            public void messageArrived(String topic, MqttMessage mqttMessage) throws Exception {
                //Log.w("Debug",mqttMessage.toString());

                Room room = getRoom(0);

                if(topic.endsWith("/status")) {

                    msgCntStatus++;

                    byte[] b = mqttMessage.getPayload();
                    if(b.length==5 && b[0]=='*') {


                        int status = b[1];
                        room.percent =  b[2];
                        // b[3];
                        room.temp = 5 *  b[4];

                        room.autoActive = (status & 1)==0;
                        room.boostActive = (status & 4)>0;

                        room.valid = true;
                        updateRoom(room);
                    }
                } else {
                    msgCntMode++;
                }
                txtViewMsg.setText("[" + msgCntMode + "/" + msgCntStatus + "]");
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
            }
        });
    }
}

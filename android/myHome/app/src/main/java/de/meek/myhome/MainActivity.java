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

    MqttHelper mqttHelper = null;
    TextView txtViewMsg = null;
    Handler handler = new Handler();
    RoomStatus room0 = null;
    RoomStatus room1 = null;
    RoomStatus room2 = null;
    RoomStatus room3 = null;
    Button btnRefresh = null;


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
            getRoom(i).update();
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

    public void showRoomSettingsActivity(int roomId) {
        Intent intent = new Intent(this, TempActivity.class);
        intent.putExtra(Const.INTENT_X_ROOM_ID, roomId);
        startActivityForResult(intent,Const.ACTIVITY_TEMP);
    }
/*
    public void onBtnRoom(View view) {

        String roomTag = (String)view.getTag();
        int roomId = Integer.parseInt(roomTag);
        showRoomSettingsActivity(roomId);
    }*/

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
    public void onResume() {
        super.onResume();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        txtViewMsg = (TextView) findViewById(R.id.txtViewMsg);
        btnRefresh = (Button) findViewById(R.id.btnRefresh);
        AccountConfig.load(this);
        room0 = (RoomStatus)findViewById(R.id.roomstatus0);
        room1 = (RoomStatus)findViewById(R.id.roomstatus1);
        room2 = (RoomStatus)findViewById(R.id.roomstatus2);
        room3 = (RoomStatus)findViewById(R.id.roomstatus3);

        View.OnClickListener l = new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String s =  (String)view.getTag();
                if(s!=null) {
                    int id = Integer.parseInt(s);
                    showRoomSettingsActivity(id);
                }
            }
        };

        room0.setOnClickListener(l);
        room1.setOnClickListener(l);
        room2.setOnClickListener(l);
        room3.setOnClickListener(l);

        House.addRoom(new Room(this, 0, room0));
        House.addRoom(new Room(this, 1, room1));
        House.addRoom(new Room(this, 2, room2));
        House.addRoom(new Room(this, 3, room3));

        refreshAllRooms();


        startMqtt();
    }

    public void redrawView() {
        /*room0.invalidate();
        room0.requestLayout();
        room0.getParent().requestLayout();
        room0.getParent().getParent().requestLayout();*/
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
                        room.msgCount++;
                       // updateRoom(room);
                        room.update();
                        redrawView();
                    }
                } else {
                    msgCntMode++;
                }
//                txtViewMsg.setText("[" + msgCntMode + "/" + msgCntStatus + "]");
                String status = ""+msgCntMode;
           //     txtViewMsg.setText(status);
                btnRefresh.setText(status);

            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
            }
        });
    }
}

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
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import helpers.MqttHelper;

// https://wildanmsyah.wordpress.com/2017/05/11/mqtt-android-client-tutorial/
public class MainActivity extends AppCompatActivity {

    MqttHelper mqttHelper = null;
    TextView txtViewMsg = null;
    Handler handler = new Handler();
    Button btnRefresh = null;
    ArrayList<RoomStatus> roomStatusList = new ArrayList<RoomStatus>();
    int msgCntStatus = 0;
    int msgCntMode = 0;

    public House getHouse() {
        return ((MyApplication)getApplicationContext()).getHouse();
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
//        sendCmd("t" + Integer.toString(temp));

        final int val = temp;
        handler.removeCallbacksAndMessages(null);
        handler.postDelayed(new Runnable() {

            @Override
            public void run() {
                mqttHelper.sendCmd("t" + Integer.toString(val));
            }

        }, 200); // 5000ms delay
    }

    public void refreshAllRooms() {
        for(int i=0; i<getHouse().getSize(); i++) {
            getHouse().getRoom(i).update();
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
        requestStatus();
    }

    /*
    @Override
    public void onDestroy() {
        super.onDestroy();
        Runtime.getRuntime().gc();
    }
*/

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        AccountConfig.load(this);
        btnRefresh = (Button) findViewById(R.id.btnRefresh);

        final LinearLayout lm = (LinearLayout) findViewById(R.id.idLayoutRoomList);

        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT, LinearLayout.LayoutParams.WRAP_CONTENT);

        //Create four
        for(int j=0;j<4;j++) {
            RoomStatus r = new RoomStatus(this, j) {
                @Override
                public void onRoomClick(int roomId) {
                    showRoomSettingsActivity(roomId);
                }
            };
            roomStatusList.add(r);
            lm.addView(r,j);
            getHouse().addRoom(new Room(this, j, r));
        }

        refreshAllRooms();

        startMqtt();
    }



    @Override
    public void onBackPressed() {
        //Execute your code here
        this.finish();
        System.exit(0);
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

                Room room = getHouse().getRoom(0);

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
                        room.update();
                    }
                } else {
                    msgCntMode++;
                }
                String status = ""+msgCntMode;
                btnRefresh.setText(status);
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
            }
        });
    }
}

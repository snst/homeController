package de.meek.myhome;

import android.content.Intent;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

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
    boolean modeAuto = false;
    boolean boostIsActive = false;
    int temp;
    int percent;
    int msgCntStatus = 0;
    int msgCntMode = 0;

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

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == Const.ACTIVITY_TEMP) {
            if (resultCode == RESULT_OK) {

                String cmd = data.getStringExtra("cmd");
                if(cmd!=null) {
                    mqttHelper.sendCmd(cmd);
                }

                int newTemp = data.getIntExtra("newTemp", -1);
                if(newTemp != -1) {
                    requestSetTemp(newTemp);
                }
            }
        }
    }

    public void onBtnRefresh(View view) {
        requestStatus();
    }

    public void onBtnRoom(View view) {
        Intent intent = new Intent(this, TempActivity.class);
        intent.putExtra(Const.INTENT_X_TEMP, temp);
        intent.putExtra(Const.INTENT_X_PERCENT, percent);
        String roomName = (String)view.getTag();
        intent.putExtra(Const.INTENT_X_ROOM, roomName!=null?roomName:"??");
        intent.putExtra(Const.INTENT_X_AUTO, modeAuto);
        intent.putExtra(Const.INTENT_X_BOOST, boostIsActive);
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
        }
        return super.onOptionsItemSelected(item);
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        txtViewMsg = (TextView) findViewById(R.id.txtViewMsg);
        updateRoom(R.id.btnRoom1, false);
        startMqtt();
    }

    void updateRoom(int btnId, boolean valid) {
        Button btn = (Button) findViewById(btnId);
        if(btn != null) {
            String statusStr = (String) btn.getTag();
            if(statusStr != null) {
                if(valid) {
                    statusStr += "  [";
                    statusStr += modeAuto ? "A" : "M";
                    if (boostIsActive)
                        statusStr += "B";
                    statusStr += "]    ";
                    statusStr += Format.tempAndPercentToString(temp, percent);
                }
                btn.setText(statusStr);
            }
        }
    }

    private void startMqtt(){
        mqttHelper = new MqttHelper(getApplicationContext());
        mqttHelper.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {
                requestStatus();
            }

            @Override
            public void connectionLost(Throwable throwable) {

            }

            @Override
            public void messageArrived(String topic, MqttMessage mqttMessage) throws Exception {
                Log.w("Debug",mqttMessage.toString());

                if(topic.endsWith("/status")) {

                    String s = mqttMessage.toString();
                    if(s.length() == 9 && s.charAt(0)=='*') { // status from eq3

                        msgCntStatus++;

                        String sub0 = s.substring(1, 3);
                        String sub1 = s.substring(3, 5);
                        String sub2 = s.substring(5, 7);
                        String sub3 = s.substring(7, 9);

                        int status = Integer.parseInt(sub0, 16 );
                        percent = Integer.parseInt(sub1, 16 );
                       // int i2 = Integer.parseInt(sub2, 16 );
                        temp = 5 * Integer.parseInt(sub3, 16 );

                        modeAuto = (status & 1)==0;
                        boostIsActive = (status & 4)>0;

                        updateRoom(R.id.btnRoom1, true);
                    } else if(s.equals("pong")) {
                        msgCntStatus++;
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

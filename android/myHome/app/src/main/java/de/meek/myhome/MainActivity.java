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
import android.view.MotionEvent;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.util.Date;

// https://wildanmsyah.wordpress.com/2017/05/11/mqtt-android-client-tutorial/
public class MainActivity extends AppCompatActivity {

    MqttHelper mqttHelper = null;
//    TextView txtViewMsg = null;
    Handler handler = new Handler();
    Button btnRefresh = null;
    ListView listView = null;
//    ArrayList<RoomStatusWidget> roomStatusList = new ArrayList<RoomStatusWidget>();
    int msgCntStatus = 0;
    int msgCntMode = 0;
    int touchPositionX = 0;
    RoomSettings roomSettings = null;
    RoomStatusArrayAdapter adapter = null;

    public House getHouse() {
        return ((MyApplication)getApplicationContext()).getHouse();
    }

    public void sendCmd(Cmd cmd) {

        if(cmd.roomId == -1) {
            mqttHelper.sendCmd(cmd);
        } else {
            Room r = getHouse().getRoom(cmd.roomId);
            if(r != null) {
                if(r.btAddress.toLong() != 0) {
                    cmd.setBtAddress(r.btAddress);
                    mqttHelper.sendCmd(cmd);
                }
            }
        }
    }

    public void requestStatusOfAllRooms() {
        for(int i=0; i<getHouse().getSize(); i++) {
            requestStatusOfRoom(i);
        }
    }

    public void requestStatusOfRoom(int roomId) {
        sendCmd(new CmdGetStatus(roomId));
    }

    public void requestSetTemp(int roomId, int temp) {
        final int finalTemp = temp;
        final int finalRoom = roomId;
        handler.removeCallbacksAndMessages(null);
        handler.postDelayed(new Runnable() {

            @Override
            public void run() {
                sendCmd(new CmdSetTemp(finalRoom, finalTemp));
            }
        }, 200); //delay
    }

    public void updateAllRooms() {
        adapter.notifyDataSetChanged();
        /*
        for (Room room : getHouse().getRoomList()) {
            room.update();
        }*/
//        for(int i=0; i<getHouse().getSize(); i++) {
  //          getHouse().getRoom(i).update();
    //    }
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {

        updateAllRooms();

        if (resultCode == RESULT_OK) {
            if (requestCode == Const.ACTIVITY_TEMP) {

                int cmd = data.getIntExtra(Const.INTENT_NEW_CMD, 0);
                int roomId = data.getIntExtra(Const.INTENT_ROOM_ID, 0);
                if(cmd!=0) {
                    sendCmd(new Cmd(roomId, eCmd.values()[cmd]));
                } else {
                    int newTemp = data.getIntExtra(Const.INTENT_NEW_TEMP, -1);
                    if(newTemp != -1) {
                        requestSetTemp(roomId, newTemp);
                        return;
                    }
                }
            } else if( requestCode == Const.ACTIVITY_SETTINGS) {
                restartMqtt();
            }
        }
    }

    public void onBtnRefresh(View view) {
        requestStatusOfAllRooms();
    }

    public void showRoomSettingsActivity(int roomId) {
        Intent intent = new Intent(this, RoomActivity.class);
        intent.putExtra(Const.INTENT_ROOM_ID, roomId);
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
                sendCmd(new Cmd(-1,eCmd.PING));
                break;
            case R.id.action_reboot:
                sendCmd(new Cmd(-1,eCmd.REBOOT));
                break;
            /*case R.id.action_refresh:
                requestStatusOfAllRooms();
                break;*/
            case R.id.action_main_settings:
                showSettingsActivity();
                break;
        }
        return super.onOptionsItemSelected(item);
    }


    void showSettingsActivity() {
        Intent intent = new Intent(this, MainSettingsActivity.class);
        startActivityForResult(intent,Const.ACTIVITY_SETTINGS);
    }

   @Override
    public void onResume() {
        super.onResume();
  //      requestStatus();
    }

    void updateRoom(Room room) {
     //   listView.deferNotifyDataSetChanged();
        adapter.notifyDataSetChanged();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        AccountConfig.load(this);
        roomSettings = new RoomSettings(this);
        btnRefresh = (Button) findViewById(R.id.btnRefresh);
        listView = (ListView) findViewById(R.id.listViewRooms);

        for(int j=0; j<AccountConfig.NUMBER_OF_ROOMS; j++) {
//            RoomStatusWidget r = new RoomStatusWidget(this, j);
//            roomStatusList.add(r);
            Room room = new Room(j);
            roomSettings.loadRoom(room);
            getHouse().addRoom(room);
        }
        adapter = new RoomStatusArrayAdapter(this, getHouse().getRoomList());
        listView.setAdapter(adapter);
        listView.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            @Override
            public void onItemClick(AdapterView<?> adapter, View v, int position, long arg3) {

                if(touchPositionX < v.getWidth()*2/3 ) {
                    showRoomSettingsActivity(position);
                } else {
                    requestStatusOfRoom(position);
                }
            }
        });

        listView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> adapter, View v, int position, long id) {
                showRoomSettingsActivity(position);
                return true;
            }
        });

        listView.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View view, MotionEvent event) {
                touchPositionX = (int)event.getX();
                return false;
            }
        });

        updateAllRooms();
        startMqtt();
    }

    @Override
    public void onBackPressed() {
        this.finish();
        System.exit(0);
    }

    private void restartMqtt() {

        mqttHelper.disconnect();;
        mqttHelper = null;
        startMqtt();
    }

    public void showShortToast( String str) {
        Toast.makeText(this, str, Toast.LENGTH_SHORT).show();
    }

    private void startMqtt(){
        mqttHelper = new MqttHelper(getApplicationContext());
        mqttHelper.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {
                showShortToast("Mqtt connected");
                requestStatusOfAllRooms();
            }

            @Override
            public void connectionLost(Throwable throwable) {
                showShortToast("Mqtt disconnected");
            }

            @Override
            public void messageArrived(String topic, MqttMessage mqttMessage) throws Exception {

                if(topic.compareTo(AccountConfig.MQTT_TOPIC_STATUS)==0) {

                    msgCntStatus++;

                    int i=0;
                    byte[] b = mqttMessage.getPayload();
                    // CMD, LEN, BTADDR, PARAM
                    if(b != null && b.length>=2 && b[1]==b.length) {
                        i += 2;
                        eResponse r = eResponse.values()[b[0]];
                        switch(r) {
                            case STATE: {
                                if(b.length==(2 + BTAddr.LENGTH + 4)) {
                                    BTAddr addr = new BTAddr();
                                    addr.convertFromBytes(b,i);
                                    i += BTAddr.LENGTH;

                                    Room room = getHouse().findRoom(addr);
                                    if(room != null) {

                                        int status = b[i++];
                                        room.percent =  b[i++];
                                        i++; // b[3];
                                        room.temp = 5 *  b[i++];
                                        room.autoActive = (status & 1)==0;
                                        room.boostActive = (status & 4)>0;
                                        room.valid = true;
                                        room.msgCount++;
                                        room.lastUpdate = new Date();
                                        room.connectionState = eConnectionState.CONNECTED;
                                        //room.update();
                                        updateRoom(room);
                                    } else {
                                        showShortToast("Invalid roomId: " + addr.toString());
                                    }
                                }
                            } break;
                            case PONG: {
                                showShortToast("Pong!");
                            } break;
                            case CONNECTION: {
                                if(b.length==(2 + BTAddr.LENGTH + 1)) {
                                    BTAddr addr = new BTAddr();
                                    addr.convertFromBytes(b, i);
                                    i += BTAddr.LENGTH;

                                    Room room = getHouse().findRoom(addr);
                                    if (room != null) {
                                        room.connectionState = eConnectionState.values()[b[i]];
                                        //room.update();
                                        updateRoom(room);
                                    }
                                }
                            } break;
                        }

                    }
                    else {
                        showShortToast("Invalid status packet");
                    }

                } else {
                    msgCntMode++;
                }
                btnRefresh.setText("" + msgCntMode);
                setTitle("myHome #" + msgCntMode);
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
            }
        });
    }
}

/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Intent;
import android.os.Handler;
import android.support.v4.widget.SwipeRefreshLayout;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ListView;
import android.widget.Toast;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.util.Date;

// https://wildanmsyah.wordpress.com/2017/05/11/mqtt-android-client-tutorial/
public class MainActivity extends AppCompatActivity {

    final String APP_VERSION = "0.3.1";

    MqttHelper mqttHelper = null;
    Handler handler = new Handler();
    ListView listView = null;
    int msgCntStatus = 0;
    int msgCntMode = 0;
    RoomSettings roomSettings = null;
    RoomStatusArrayAdapter adapter = null;
    SwipeRefreshLayout refreshLayout = null;
    boolean mqttConnected = false;
    DwdWeatherProvider dwd = new DwdWeatherProvider() {
        @Override
        public void onNewData(SensorData data) {
            super.onNewData(data);
            showSensorData(data);
        }
    };

    protected MyApplication getMyApp() {
        return ((MyApplication)getApplicationContext());
    }

    public Logger getLogger() {
        return getMyApp().getLogger();
    }

    public House getHouse() {
        return getMyApp().getHouse();
    }

    public void sendCmd(Cmd cmd) {

        if(cmd.roomId == -1) {
            for (String mqttTopic: getHouse().getMqttTopicList()) {
                cmd.setMQTTTopic(mqttTopic);
            }
            mqttHelper.sendCmd(cmd);
        } else {
            Room r = getHouse().getRoom(cmd.roomId);
            if(r != null) {
                if(r.btAddress.toLong() != 0) {
                    cmd.setBtAddress(r.btAddress);
                    cmd.setMQTTTopic(r.mqttTopic);
                    mqttHelper.sendCmd(cmd);
                }
            }
        }
    }

    public void requestStatusOfAllRooms(boolean forceAll) {

        sendCmd(new Cmd(-1,eCmd.GETTEMP));

        for(int i=0; i<getHouse().getSize(); i++) {
            Room r = getHouse().getRoom(i);
            if(forceAll || r.updateOnStart) {
                requestStatusOfRoom(i);
            }
        }
        repaintRooms();
        dwd.update();
    }

    public void requestStatusOfRoom(int roomId) {
        Room r = getHouse().getRoom(roomId);
        if (r != null) {
//            r.connectionState = eConnectionState.DISCONNECTED;
            r.refreshing = true;
            sendCmd(new CmdGetStatus(roomId));
        }
    }

    public void closeConnection(int roomId) {
        sendCmd(new Cmd(roomId, eCmd.CLOSE_CONNCTION));
    }

    public void requestSetTemp(int roomId, int temp) {
        final int finalTemp = temp;
        final int finalRoom = roomId;
        if(temp < Const.TEMP_MIN) {
            temp = Const.TEMP_MIN;
        }
        else if(temp > Const.TEMP_MAX) {
            temp = Const.TEMP_MAX;
        }
        getLogger().add("setTemp(roomId=" + roomId + ", temp=" + temp + ")");

//        Room r = getHouse().getRoom(roomId);
//        r.isSensorTemp = false;
//        r.temp = temp;
//        updateRoom(r);

        handler.removeCallbacksAndMessages(null);
        handler.postDelayed(new Runnable() {

            @Override
            public void run() {
                sendCmd(new CmdSetTemp(finalRoom, finalTemp));
            }
        }, 200); //delay
    }

    public void repaintRooms() {
        adapter.notifyDataSetChanged();
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {

        if (resultCode == RESULT_OK) {
            if (requestCode == Const.ACTIVITY_TEMP) {

                int cmd = data.getIntExtra(Const.INTENT_NEW_CMD, 0);
                int roomId = data.getIntExtra(Const.INTENT_ROOM_ID, 0);
                if(cmd != 0) {
                    sendCmd(new Cmd(roomId, eCmd.values()[cmd]));
                } else {
                    int newTemp = data.getIntExtra(Const.INTENT_NEW_TEMP, -1);
                    if(newTemp != -1) {
                        requestSetTemp(roomId, newTemp);
    //                    return;
                    }
                }
            } else if( requestCode == Const.ACTIVITY_SETTINGS) {
                restartMqtt();
            }
        }

        repaintRooms();
    }

    public void log(String str, boolean toast) {
        getLogger().add(str);
        if(toast) {
            showShortToast(str);
        }
    }
    public void log(String str) {
        log(str, false);
    }

    public void log(String str, BTAddr addr) {
        getLogger().add(str, addr);
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
            case R.id.action_abort:
                sendCmd(new Cmd(-1,eCmd.ABORT));
                break;
            case R.id.action_refresh_all:
                requestStatusOfAllRooms(true);
                break;
            case R.id.action_log:
                showLogActivity();
                break;
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

    void showLogActivity() {
        Intent intent = new Intent(this, LogActivity.class);
        startActivityForResult(intent,Const.ACTIVITY_LOG);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getHouse().clear();
        setContentView(R.layout.activity_main);
        log("App version: " + APP_VERSION);
        updateTitle();

        AccountConfig.load(this);
        roomSettings = new RoomSettings(this);
        listView = findViewById(R.id.listViewRooms);

        for (int j = 0; j < AccountConfig.NUMBER_OF_ROOMS; j++) {
            Room room = new Room(j);
            roomSettings.loadRoom(room);
            getHouse().addRoom(room);
        }
        adapter = new RoomStatusArrayAdapter(this, getHouse().getRoomList());
        listView.setAdapter(adapter);


        refreshLayout = findViewById(R.id.swiperefresh);
        refreshLayout.setOnRefreshListener(
                new SwipeRefreshLayout.OnRefreshListener() {
                    @Override
                    public void onRefresh() {
                        requestStatusOfAllRooms(true);
                        refreshLayout.setRefreshing(false);
                    }
                }
        );

        repaintRooms();
        startMqtt();
    }

    @Override
    public void onBackPressed() {
        this.finish();
        System.exit(0);
    }

    private void restartMqtt() {

        mqttHelper.disconnect();
        mqttHelper = null;
        startMqtt();
    }

    public void showShortToast( String str) {
        Toast.makeText(this, str, Toast.LENGTH_SHORT).show();
    }

    void updateTitle() {
        setTitle("myHome - " + (mqttConnected ? "connected" : "disconnected") + " #" + msgCntMode);
    }

    void showSensorData(SensorData data) {
        int id = 0;
        switch(data.source) {
            case 1: {
                id = R.id.tempViewInside;
                break;
            }
            case 2: {
                id = R.id.tempViewOutside;
                break;
            }
            case 3: {
                id = R.id.tempViewDWD;
                break;
            }
            default:
                return;
        }
        TempSensorView view = findViewById(id);

        if (view != null) {
            view.setData(data);
        }
    }

    void setMqttConnectedStatus(boolean connected) {
        mqttConnected = connected;
        updateTitle();
    }

    private void startMqtt(){
        mqttHelper = new MqttHelper(getApplicationContext());
        mqttHelper.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {
                setMqttConnectedStatus(true);
                log("Mqtt connected");
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        requestStatusOfAllRooms(false);
                    }
                }, 1000); //delay
            }

            @Override
            public void connectionLost(Throwable throwable) {
                setMqttConnectedStatus(false);
                log("Mqtt disconnected");
            }

            @Override
            public void messageArrived(String topicStatus, MqttMessage mqttMessage) throws Exception {
                String topic = topicStatus.replace(AccountConfig.MQTT_TOPIC_STATUS_SUFFIX, "");
                if(getHouse().getMqttTopicList().contains(topic)) {
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
                                    log(">" + topic + ": tempState", addr);

                                    Room room = getHouse().findRoom(addr);
                                    if(room != null) {
                                        int status = b[i++];
                                        room.percent =  b[i++];
                                        i++; // data[2];
                                        room.temp = 5 *  b[i++];
                                        //room.isSensorTemp = true;
                                        room.lastRequestedTemp = room.temp;
                                        room.autoActive = (status & 1)==0;
                                        room.boostActive = (status & 4)>0;
                                        room.lowBattery = (status & 80)>0;
                                        room.valid = true;
                                        room.refreshing = false;
                                        room.msgCount++;
                                        room.lastUpdate = new Date();
                                        room.connectionState = eConnectionState.CONNECTED;
                                        repaintRooms();
                                    } else {
                                        log("Invalid roomId", addr);
                                    }
                                }
                            } break;
                            case PONG: {
                                if(b.length==(2 + 3)) {
                                    String version = "ESP v" + b[i++] + "." + b[i++] + "." + b[i++];
                                    log(">" + topic + ": pong! " + version);
                                    showShortToast("Pong!\n" + version);
                                }
                            } break;
                            case CONNECTION: {
                                if(b.length==(2 + BTAddr.LENGTH + 1)) {
                                    BTAddr addr = new BTAddr();
                                    addr.convertFromBytes(b, i);
                                    i += BTAddr.LENGTH;

                                    Room room = getHouse().findRoom(addr);
                                    if (room != null) {
                                        room.connectionState = eConnectionState.values()[b[i]];
                                        log(">" + topic + ": connState: " + room.connectionState.toString(), addr);
                                    } else {
                                        log(">" + topic + ": connState: no matching room", addr);
                                    }
                                    repaintRooms();
                                }
                            } break;
                            case SENSOR: {
                                if(b.length==(2 + 4)) {

                                    int sensorId = b[i++];
                                    int t1 = b[i++];
                                    int t2 = b[i++];
                                    int humidity = b[i++];
                                    float temperature = t1 + (t2/100.f);

                                    String str = "" + sensorId + ", temp=" + temperature + ", humidity=" + humidity;
                                    log(">" + topic + ": sensor:" + str);

                                    SensorData data = new SensorData(sensorId);
                                    data.setTemperature(temperature);
                                    data.setHumidity(humidity);
                                    showSensorData(data);
                                }
                            } break;
                            default: {
                                log(">" + topic + ": unkown state packet");
                            }
                        }
                    }
                    else {
                        log(">mqtt: " + topic + ": Invalid state packet");
                    }
                } else {
                    msgCntMode++;
                }
                updateTitle();
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
            }
        });
    }
}

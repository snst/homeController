package de.meek.myhome;

import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.TextView;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import helpers.MqttHelper;

// https://wildanmsyah.wordpress.com/2017/05/11/mqtt-android-client-tutorial/
public class MainActivity extends AppCompatActivity {


    interface ICmd {
        public String toString();
        public String getCmd();
        public void exec();
    }

    public class CmdTemp implements ICmd {

        int temp;
        public CmdTemp(int _temp)
        {
            temp = _temp;
        }
        public String getCmd() { return null; }
        public String toString() {
            if(temp==45)
                return "Off";
            else if(temp==300)
                return "On";
            else
                return Float.toString(temp/10f) +  "°";
        }
        public void exec() { requestSetTemp(temp);}

    }

    public class CmdItem implements ICmd {
        public String name;
        public String cmd;

        public CmdItem(String _name, String _cmd) {
            name = _name;
            cmd = _cmd;
        }
        public CmdItem(String _name) {
            name = _name;
            cmd = null;
        }

        public String getCmd() {
            return cmd;
        }

        public void exec() {};

        public String toString() {
            return name;
        }
    }

    public class CmdItem2 implements ICmd {
        public String name;
        public String cmd;
        public String name2;
        public String cmd2;
        public boolean state2;

        public CmdItem2(String _name, String _cmd, String _name2, String _cmd2, boolean _state2) {
            name = _name;
            name2 = _name2;
            cmd = _cmd;
            cmd2 = _cmd2;
            state2 = _state2;
        }

        public String getCmd() {
            return state2 ? cmd2 : cmd;
        }

        public String toString() {
            return state2 ? name2 : name;
        }

        public void setState2(boolean _state2) {
            state2 = _state2;
        }

        public void exec() {}
    }

    MqttHelper mqttHelper;
    TextView txtViewStatus;
    TextView txtViewMsg;
    TextView txtViewTemp;
    NoDefaultSpinner spinner;
    NoDefaultSpinner spinnerTemp;
    Handler handler = new Handler();
    Handler handlerRefresh = new Handler();


    CmdItem2 cmdBoost = new CmdItem2("Boost on", "booston", "Boost off", "boostoff", false);

    int msgCntStatus = 0;
    int msgCntMode = 0;

    int temp;
    int percent;

    public void requestStatus() {
        DateFormat dateFormat = new SimpleDateFormat("yyMMddHHmmss");
        Date date = new Date();
        String txt = "s" + dateFormat.format(date);
        mqttHelper.sendCmd(txt);
    }

    /*
    public void requestUpdatePeriodic() {
        handlerRefresh.removeCallbacksAndMessages(null);
        handlerRefresh.postDelayed(new Runnable() {

            @Override
            public void run() {
                requestStatus();
            }

        }, 10000); // 5000ms delay
    }
*/

    public void requestSetTemp(int temp) {
        final int val = temp;
        handler.removeCallbacksAndMessages(null);
        handler.postDelayed(new Runnable() {

            @Override
            public void run() {
                mqttHelper.sendCmd("t" + Integer.toString(val));
            }

        }, 1000); // 5000ms delay
    }

    public void updateTempSpinner(int temp) {

        List<CmdTemp> spinnerTempArray = new ArrayList<CmdTemp>();
        spinnerTempArray.add(new CmdTemp(45));
        spinnerTempArray.add(new CmdTemp(75));
        spinnerTempArray.add(new CmdTemp(100));
        spinnerTempArray.add(new CmdTemp(125));
        spinnerTempArray.add(new CmdTemp(150));
        spinnerTempArray.add(new CmdTemp(170));
        spinnerTempArray.add(new CmdTemp(180));
        spinnerTempArray.add(new CmdTemp(190));
        spinnerTempArray.add(new CmdTemp(200));
        spinnerTempArray.add(new CmdTemp(210));
        spinnerTempArray.add(new CmdTemp(220));
        spinnerTempArray.add(new CmdTemp(230));
        spinnerTempArray.add(new CmdTemp(240));
        spinnerTempArray.add(new CmdTemp(250));
        spinnerTempArray.add(new CmdTemp(260));
        spinnerTempArray.add(new CmdTemp(280));
        spinnerTempArray.add(new CmdTemp(300));

        /*
        CmdTemp c = (CmdTemp) spinnerTemp.getSelectedItem();
        if(c != null && c.temp == temp)
            return;


        List<CmdTemp> spinnerTempArray = new ArrayList<CmdTemp>();

        spinnerTempArray.add(new CmdTemp(45));
        for(int t=50; t<300; t+=10)
        {
            if(t==temp && temp>150) {
                spinnerTempArray.add(new CmdTemp(t-5));
            }
            spinnerTempArray.add(new CmdTemp(t));
            if(t==temp && temp<240) {
                spinnerTempArray.add(new CmdTemp(t+5));
            }
        }*/

        ArrayAdapter<CmdTemp> adapterTemp = new ArrayAdapter<CmdTemp>(
                this, android.R.layout.simple_spinner_item, spinnerTempArray);

        adapterTemp.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinnerTemp.setAdapter(adapterTemp);
        for(int i=0; i<spinnerTempArray.size(); i++) {
            if(spinnerTempArray.get(i).temp == temp) {
                spinnerTemp.setSelection(i);
            }
        }

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        spinner = (NoDefaultSpinner) findViewById(R.id.spinner);
        spinnerTemp = (NoDefaultSpinner) findViewById(R.id.spinnerTemp);
        txtViewStatus = (TextView) findViewById(R.id.txtViewStatus);
        txtViewMsg = (TextView) findViewById(R.id.txtViewMsg);
        txtViewTemp = (TextView) findViewById(R.id.txtViewTemp);




        List<ICmd> spinnerArray =  new ArrayList<ICmd>();
        spinnerArray.add(new CmdItem("Status") {
            @Override
            public void exec() {
                requestStatus();
            }
        });
        spinnerArray.add(cmdBoost);
        spinnerArray.add(new CmdItem("Off", "off"));
        spinnerArray.add(new CmdItem("On", "on"));
        spinnerArray.add(new CmdItem("Eco", "eco"));
        spinnerArray.add(new CmdItem("Comfort", "comfort"));
        spinnerArray.add(new CmdItem("Manual", "manual"));
        spinnerArray.add(new CmdItem("Auto", "auto"));
        spinnerArray.add(new CmdItem("Ping", "ping"));
        spinnerArray.add(new CmdItem("Reset", "reset"));

        ArrayAdapter<ICmd> adapter = new ArrayAdapter<ICmd>(
                this, android.R.layout.simple_spinner_item, spinnerArray);

        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(adapter);

        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {

            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                ICmd c = (ICmd) spinner.getSelectedItem();
                String cmd = c.getCmd();
                if(cmd != null) {
                    mqttHelper.sendCmd(cmd);
                }
                else {
                    c.exec();
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });

        updateTempSpinner(200);



        spinnerTemp.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {

            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                ICmd c = (ICmd) spinnerTemp.getSelectedItem();
                c.exec();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });















         startMqtt();

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

                        boolean modeAuto = (status & 1)==0;
                        boolean boostIsActive = (status & 4)>0;

                        String statusStr = "";
                        statusStr += modeAuto ? "auto" : "manual";
                        if(boostIsActive)
                            statusStr += " - boost";

                        if(temp==45)
                            statusStr += " - off";

                        if(temp==300)
                            statusStr += " - on";


                        String tempStr = (Float.toString(temp/10f) +  "°  (" + percent + "%)");
                        txtViewStatus.setText(statusStr);
                        txtViewTemp.setText(tempStr);

                        cmdBoost.setState2(boostIsActive);

                        //updateTempSpinner(temp);

                        ((BaseAdapter) spinner.getAdapter()).notifyDataSetChanged();
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

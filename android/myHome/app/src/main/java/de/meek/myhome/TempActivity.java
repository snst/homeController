package de.meek.myhome;

import android.content.Intent;
import android.content.SharedPreferences;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

public class TempActivity extends AppCompatActivity {

    TextView txtViewTempNew;
    TextView txtViewTempCurrent;
    SeekBar seekBarTemp;
    SeekBar seekBarTempComfort;
    String roomName;
    int temp = 0;
    int percent = 0;
    boolean boostIsActive = false;
    boolean autoMode = false;

    public void runCmd(String cmd) {

        Intent data = new Intent();
        data.putExtra("cmd",cmd);
        setResult(RESULT_OK,data);
        finish();
    }

    public void onBtnAuto(View view) {
        runCmd(Cmd.AUTO);
    }

    public void onBtnManual(View view) {
        runCmd(Cmd.MANUAL);
    }

    public void onBtnOff(View view) {
        runCmd(Cmd.OFF);
    }

    public void onBtnOn(View view) {
        runCmd(Cmd.ON);
    }

    public void onBtnBoostOn(View view) {
        runCmd(Cmd.BOOST_ON);
    }

    public void onBtnBoostOff(View view) {
        runCmd(Cmd.BOOST_OFF);
    }

    public void onBtnEco(View view) {
        runCmd(Cmd.ECO);
    }

    public void onBtnComfort(View view) {
        runCmd(Cmd.COMFORT);
    }

    public void onBtnTemp(View view) {
        Button btn = (Button) view;
        temp = (int) btn.getTag(R.id.id_tag_temp);
        requestNewTempAndClose(temp);
    }


    public void onBtnDec(View view) {
        if(temp>Const.TEMP_MIN) {
            temp -= Const.TEMP_STEP;
        }
        showTemp();
    }

    public void onBtnInc(View view) {
        if(temp<Const.TEMP_MAX) {
            temp += Const.TEMP_STEP;
        }
        showTemp();
    }

    public void requestNewTempAndClose(int t)
    {
        Intent data = new Intent();
        data.putExtra("newTemp",temp);
        setResult(RESULT_OK,data);
        finish();
    }

    public void onBtnSetTemp(View view) {
        requestNewTempAndClose(temp);
    }

    void showTemp() {
        String str = Format.tempToString(temp);
        txtViewTempNew.setText(str);
        seekBarTemp.setProgress((temp-Const.TEMP_MIN)/Const.TEMP_STEP);

        int tempComfort = Math.min(Math.max(temp, Const.TEMP_MIN_COMFORT), Const.TEMP_MAX_COMFORT);
        tempComfort = (tempComfort-Const.TEMP_MIN_COMFORT) / Const.TEMP_STEP;
        seekBarTempComfort.setProgress(tempComfort);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_temp, menu);
        return true;
    }

    void setButtonTemp(Button btn, int value) {
        btn.setText(Format.tempToString(value));
        btn.setTag(R.id.id_tag_temp, value);
    }

    void saveButtonTemp(int btnId, int value) {
        Button btn = (Button) findViewById(btnId);
        setButtonTemp(btn, value);
        SharedPreferences settings = getSharedPreferences(roomName, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putInt((String)btn.getTag(), value);
        editor.commit();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch(id) {
            case R.id.action_save1:
                saveButtonTemp(R.id.btnTemp1, temp);
                break;
            case R.id.action_save2:
                saveButtonTemp(R.id.btnTemp2, temp);
                break;
            case R.id.action_save3:
                saveButtonTemp(R.id.btnTemp3, temp);
                break;
            case R.id.action_save4:
                saveButtonTemp(R.id.btnTemp4, temp);
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    void initButtonTemp(SharedPreferences settings, int btnId, int defaultTemp) {
        Button btn = (Button) findViewById(btnId);
        String tag = (String) btn.getTag();
        setButtonTemp(btn, settings.getInt(tag, defaultTemp));
    }

    void showCurrentState() {
        String str = autoMode ? "Auto" : "Manual";
        if(boostIsActive) str += " + Boost";
        str += "    " + Format.tempAndPercentToString(temp, percent);
        txtViewTempCurrent.setText(str);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_temp);
        txtViewTempNew = (TextView) findViewById(R.id.txtViewTempNew);
        txtViewTempCurrent = (TextView) findViewById(R.id.txtViewTempCurrent);
        seekBarTemp = (SeekBar) findViewById(R.id.seekBarTemp);
        seekBarTempComfort = (SeekBar) findViewById(R.id.seekBarTempComfort);
        seekBarTemp.setMax((Const.TEMP_MAX - Const.TEMP_MIN) / Const.TEMP_STEP);
        seekBarTempComfort.setMax((Const.TEMP_MAX_COMFORT - Const.TEMP_MIN_COMFORT) / Const.TEMP_STEP);

        Intent intent = getIntent();
        roomName = intent.getStringExtra(Const.INTENT_X_ROOM);
        temp = intent.getIntExtra(Const.INTENT_X_TEMP, 0);
        percent = intent.getIntExtra(Const.INTENT_X_PERCENT, 0);
        boostIsActive  = intent.getBooleanExtra(Const.INTENT_X_BOOST, false);
        autoMode  = intent.getBooleanExtra(Const.INTENT_X_AUTO, false);
        setTitle(roomName);

        if(temp<Const.TEMP_MIN) {
            temp = Const.TEMP_DEFAULT;
        }

        showCurrentState();

        SharedPreferences settings = getSharedPreferences(roomName, 0);
        initButtonTemp(settings, R.id.btnTemp1, 180);
        initButtonTemp(settings, R.id.btnTemp2, 190);
        initButtonTemp(settings, R.id.btnTemp3, 200);
        initButtonTemp(settings, R.id.btnTemp4, 210);

        seekBarTemp.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if(fromUser) {
                    temp = (progress * Const.TEMP_STEP) + Const.TEMP_MIN;
                    showTemp();
                }
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        seekBarTempComfort.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if(fromUser) {
                    temp = (progress * Const.TEMP_STEP) + Const.TEMP_MIN_COMFORT;
                    showTemp();
                }
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        showTemp();
    }
}

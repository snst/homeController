/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.text.DateFormat;
import java.text.SimpleDateFormat;

public class RoomActivity extends AppCompatActivity {

    TextView txtViewTempNew;
    TextView txtViewTempCurrent;
    TextView txtLastUpdate;
    SeekBar seekBarTemp;
    SeekBar seekBarTempComfort;
    Button btnBoost;
    Room room = null;
    int temp;

    public void runCmd(eCmd cmd) {

        Intent data = new Intent();
        data.putExtra(Const.INTENT_NEW_CMD, (int) cmd.getValue());
        data.putExtra(Const.INTENT_ROOM_ID, room.id);
        setResult(RESULT_OK,data);
        finish();
    }

    public void onBtnOff(View view) {
        runCmd(eCmd.OFF);
    }

    public void onBtnOn(View view) {
        runCmd(eCmd.ON);
    }

    public void onBtnBoost(View view) {
        runCmd(room.boostActive ? eCmd.BOOST_OFF : eCmd.BOOST_ON);
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

    public void requestNewTempAndClose(int t) {

        Intent data = new Intent();
        data.putExtra(Const.INTENT_NEW_TEMP, temp);
        data.putExtra(Const.INTENT_ROOM_ID, room.id);
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

    void updateButtonPresetTemp(int btnId, int value) {
        Button btn = findViewById(btnId);
        btn.setText(Format.tempToString(value));
    }

    void updateAllButtonPresetTemps() {

        updateButtonPresetTemp(R.id.btnTemp0, room.presetTemp.get(0));
        updateButtonPresetTemp(R.id.btnTemp1, room.presetTemp.get(1));
        updateButtonPresetTemp(R.id.btnTemp2, room.presetTemp.get(2));
        updateButtonPresetTemp(R.id.btnTemp3, room.presetTemp.get(3));
    }

    void setNewButtonPresetTemp(int presetId, int value) {
        room.presetTemp.set(presetId, value);
        updateAllButtonPresetTemps();
        RoomSettings roomSettings = new RoomSettings(this);
        roomSettings.saveRoom(room);
        Toast.makeText(this, "Saved preset temperature.", Toast.LENGTH_SHORT).show();
    }

    void showRoomSettingsActivity()
    {
        Intent intent = new Intent(this, RoomSettingsActivity.class);
        intent.putExtra(Const.INTENT_ROOM_ID, room.id);
        startActivityForResult(intent,Const.ACTIVITY_ROOM_SETTINGS);
    }

    void registerButtonPresetLongClick(int btnId, int presetIndex) {
        Button btn = findViewById(btnId);
        btn.setTag(presetIndex);

        btn.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                setNewButtonPresetTemp((int)v.getTag(), temp);
                return true;
            }
        });

        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Button btn = (Button) view;
                int presetIndex = (int)btn.getTag();
                temp = room.presetTemp.get(presetIndex);
                requestNewTempAndClose(temp);
            }
        });
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch(id) {
            case R.id.action_room_auto:
                runCmd(eCmd.AUTO);
                break;
            case R.id.action_room_manual:
                runCmd(eCmd.MANUAL);
                break;
            case R.id.action_room_setting:
                showRoomSettingsActivity();
                break;
            case R.id.action_eco:
                runCmd(eCmd.ECO);
                break;
            case R.id.action_comfort:
                runCmd(eCmd.COMFORT);
                break;
        }
        return super.onOptionsItemSelected(item);
    }


    void showCurrentState() {
        String lastUpdate = "#";
        String str = "?";
        if(room != null) {
            str = room.autoActive ? "Auto" : "Manual";
            if (room.boostActive) str += " + Boost";
            str += "    " + Format.tempAndPercentToString(temp, room.percent);
            lastUpdate += room.msgCount;
            lastUpdate += ": ";
            if(room.lastUpdate != null) {
                DateFormat dateFormat = new SimpleDateFormat("HH:mm:ss  dd.MM.yyyy");
                lastUpdate += dateFormat.format(room.lastUpdate);
            } else {
                lastUpdate += "-";
            }
        }
        txtViewTempCurrent.setText(str);
        txtViewTempNew.setText(Format.tempToString(temp));
        txtLastUpdate.setText(lastUpdate);
    }


    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == Const.ACTIVITY_ROOM_SETTINGS) {
            if (resultCode == RESULT_OK) {
                setTitle(room.name);
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_room);
        txtViewTempNew = findViewById(R.id.txtViewTempNew);
        txtViewTempCurrent = findViewById(R.id.txtViewTempCurrent);
        txtLastUpdate = findViewById(R.id.txtLastUpdate);
        btnBoost = findViewById(R.id.btnBoost);
        seekBarTemp = findViewById(R.id.seekBarTemp);
        seekBarTempComfort = findViewById(R.id.seekBarTempComfort);
        seekBarTemp.setMax((Const.TEMP_MAX - Const.TEMP_MIN) / Const.TEMP_STEP);
        seekBarTempComfort.setMax((Const.TEMP_MAX_COMFORT - Const.TEMP_MIN_COMFORT) / Const.TEMP_STEP);

        Intent intent = getIntent();

        int id = intent.getIntExtra(Const.INTENT_ROOM_ID, 0);
        room = ((MyApplication)getApplicationContext()).getHouse().getRoom(id);
        setTitle(room.name);
        temp = room.temp;

        if(temp<Const.TEMP_MIN) {
            temp = Const.TEMP_DEFAULT;
        }

        btnBoost.setText(room.boostActive ? "Boost off" : "Boost on");

        registerButtonPresetLongClick(R.id.btnTemp0, 0);
        registerButtonPresetLongClick(R.id.btnTemp1, 1);
        registerButtonPresetLongClick(R.id.btnTemp2, 2);
        registerButtonPresetLongClick(R.id.btnTemp3, 3);

        showCurrentState();
        updateAllButtonPresetTemps();

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

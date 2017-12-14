package de.meek.myhome;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Created by stefan on 13.12.2017.
 */

public class RoomStatus extends LinearLayout implements IRoom {

  //  private Button mPreviousButton;
  //  private Button mNextButton;
    private TextView txtName;
    private TextView txtTemp;
    private TextView txtPercent;
    private TextView txtMsgCnt;

    public RoomStatus(Context context) {
        super(context);
        initializeViews(context);
    }

    public RoomStatus(Context context, AttributeSet attrs) {
        super(context, attrs);
        initializeViews(context);
    }

    public RoomStatus(Context context,
                       AttributeSet attrs,
                       int defStyle) {
        super(context, attrs, defStyle);
        initializeViews(context);
    }

    /**
     * Inflates the views in the layout.
     *
     * @param context
     *           the current context for the view.
     */
    private void initializeViews(Context context) {
        LayoutInflater inflater = (LayoutInflater) context
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        inflater.inflate(R.layout.roomstatus_view, this);
        txtName = (TextView)this.findViewById(R.id.txtRoomStatusName);
        txtTemp = (TextView)this.findViewById(R.id.txtRoomStatusTemp);
        txtPercent = (TextView)this.findViewById(R.id.txtRoomStatusPercent);
        txtMsgCnt = (TextView)this.findViewById(R.id.txtRoomStatusMsgCnt);
    }



    public void setOnClickListener(View.OnClickListener l) {

        final View v = this;
        final View.OnClickListener ll = l;
        View.OnClickListener ol = new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                ll.onClick(v);
            }
        };
        txtName.setOnClickListener(ol);
        txtTemp.setOnClickListener(ol);
        txtPercent.setOnClickListener(ol);
    }

    @Override
    protected void onFinishInflate() {

        // When the controls in the layout are doing being inflated, set
        // the callbacks for the side arrows.
        super.onFinishInflate();

  /*
        txtTemp.setOnClickListener(new OnClickListener() {
            public void onClick(View view) {
                //showSettings();
                if(onShowSettingsListener!=null)
                    onShowSettingsListener.onClick(view);
            }
        });
        txtPercent.setOnClickListener(new OnClickListener() {
            public void onClick(View view) {
                //showSettings();
                if(onShowSettingsListener!=null)
                    onShowSettingsListener.onClick(view);
            }
        });
*/
        /*
        // When the previous button is pressed, select the previous value
        // in the list.
        mPreviousButton = (Button) this
                .findViewById(R.id.sidespinner_view_previous);
        mPreviousButton
                .setBackgroundResource(android.R.drawable.ic_dialog_info);

        mPreviousButton.setOnClickListener(new OnClickListener() {
            public void onClick(View view) {
            }
        });


        // When the next button is pressed, select the next item in the
        // list.
        mNextButton = (Button)this
                .findViewById(R.id.sidespinner_view_next);
        mNextButton
                .setBackgroundResource(android.R.drawable.ic_popup_sync);
        mNextButton.setOnClickListener(new OnClickListener() {
            public void onClick(View view) {
            }
        });
*/
    }

    @Override
    public void update(Room room) {
//        txtName.setText( room.name + "   " + Format.tempAndPercentToString(room.temp, room.percent) );
        txtName.setText( room.name + " : ");
        String mode = "";
        String temp = "";
        String percent = "";

        if(room.valid) {
            temp = Format.tempToString(room.temp);
            if(room.autoActive)
                mode = "A";
            if(room.boostActive)
                mode += "B";
            if(mode.length()>0)
                mode += ",";

            percent = "("+mode+room.percent+"%)";
        }
        txtTemp.setText(temp);
        txtPercent.setText(percent);
        txtMsgCnt.setText("["+room.msgCount+"]");
        /*
        txtName.invalidate();
        txtTemp.invalidate();
        txtPercent.invalidate();
        txtMsgCnt.invalidate();
        this.invalidate();*/
    }
}
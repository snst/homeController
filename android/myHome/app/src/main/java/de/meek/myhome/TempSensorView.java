/**
 * Copyright (c) 2017 by Stefan Schmidt
 */
package de.meek.myhome;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.View;

public class TempSensorView extends View {

    private Paint paintShape;
    private int txtColor;
    private int backgroundColor;
    Drawable icon = null;
    SensorData data = null;

    public TempSensorView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setupAttributes(attrs);
        setupPaint();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        paintShape.setColor(backgroundColor);
        //canvas.drawRoundRect(0,0,this.getWidth(), this.getHeight(), 5, 5, paintShape);
        canvas.drawRect(0,0,this.getWidth(), this.getHeight(), paintShape);

        paintShape.setAntiAlias(true);
        paintShape.setColor(txtColor);
        paintShape.setTextSize(35);
        paintShape.setTypeface(Typeface.create(Typeface.DEFAULT, Typeface.BOLD));
        int x = 75;

        if (data != null) {
            if (data.temperatureValid) {
                canvas.drawText(String.format("%.1f°", data.temperature), x, 40, paintShape);
            }

            paintShape.setTextSize(25);
            paintShape.setTypeface(Typeface.create(Typeface.DEFAULT, Typeface.NORMAL));

            if (data.humidityValid) {
                canvas.drawText(Integer.toString((int) data.humidity) + "%", x, 70, paintShape);
            }

            if (data.dewPointValid) {
                canvas.drawText(String.format("%.1f°", data.dewPoint), x + 55, 70, paintShape);
            }
        }

        if(icon!=null) {
            icon.draw(canvas);
        }
    }

    private void setupPaint() {
        paintShape = new Paint();
        paintShape.setStyle(Paint.Style.FILL);
   }

    private void setupAttributes(AttributeSet attrs) {
        TypedArray a = getContext().getTheme().obtainStyledAttributes(attrs, R.styleable.TempSensorView, 0, 0);
        try {
            txtColor = a.getColor(R.styleable.TempSensorView_textColor, Color.WHITE);
            backgroundColor = a.getColor(R.styleable.TempSensorView_backgroundColor, Color.BLACK);
            icon = a.getDrawable(R.styleable.TempSensorView_tempIcon);
            icon.setBounds(15, 20, 60, 60);
            icon.setAlpha(200);
        } finally {
            a.recycle();
        }
    }

    public void setData(SensorData data) {
        this.data = data;
        invalidate();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        int width = getMeasuredWidth();
        int height = getMeasuredHeight();
        int widthWithoutPadding = width - getPaddingLeft() - getPaddingRight();
        int heigthWithoutPadding = height - getPaddingTop() - getPaddingBottom();
        int maxWidth = (int) (widthWithoutPadding);
        int maxHeight = (int) (heigthWithoutPadding);

        if (widthWithoutPadding  > maxWidth) {
            width = maxWidth + getPaddingLeft() + getPaddingRight();
        } else {
            height = maxHeight + getPaddingTop() + getPaddingBottom();
        }

        setMeasuredDimension(width, height);
    }
/*
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        boolean result = super.onTouchEvent(event);
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            currentShapeIndex =  (currentShapeIndex ++) % shapeValues.length;
            postInvalidate();
            return true;
        }
        return result;
    }
    @Override
  public Parcelable onSaveInstanceState() {
    // Construct bundle
    Bundle bundle = new Bundle();
    // Store base view state
    bundle.putParcelable("instanceState", super.onSaveInstanceState());
    // Save our custom view state to bundle
    bundle.putInt("currentShapeIndex", this.currentShapeIndex);
    // ... store any other custom state here ...
    // Return the bundle
    return bundle;
  }

  @Override
  public void onRestoreInstanceState(Parcelable state) {
    // Checks if the state is the bundle we saved
    if (state instanceof Bundle) {
      Bundle bundle = (Bundle) state;
      // Load back our custom view state
      this.currentShapeIndex = bundle.getInt("currentShapeIndex");
      // ... load any other custom state here ...
      // Load base view state back
      state = bundle.getParcelable("instanceState");
    }
    // Pass base view state on to super
    super.onRestoreInstanceState(state);
  }
*/
}

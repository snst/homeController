package de.meek.myhome;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.graphics.drawable.Drawable;
import android.util.AttributeSet;
import android.view.View;

/**
 * Created by stefan on 17.02.2018.
 */

public class TempSensorView extends View {

    private int shapeWidth = 180;
    private int shapeHeight = 90;
    private Paint paintShape;
    private int txtColor;
    private int backgroundColor;
    Drawable icon = null;
    private float temp = 0.0f;
    private float humidity = 0.0f;


    public TempSensorView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setupAttributes(attrs);
        setupPaint();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        paintShape.setColor(backgroundColor);
        //canvas.drawRect(0, 0, shapeWidth, shapeHeight, paintShape);
        canvas.drawRoundRect(0,0,shapeWidth, shapeHeight, 5, 5, paintShape);

        paintShape.setAntiAlias(true);
        paintShape.setColor(txtColor);
        paintShape.setTextSize(35);
     //   paintShape.setFakeBoldText(true);
        paintShape.setTypeface(Typeface.create(Typeface.DEFAULT, Typeface.BOLD));

        canvas.drawText(String.format("%.1f°", temp) , 80, 40, paintShape);
     //   paintShape.setFakeBoldText(false);
        paintShape.setTextSize(25);
        paintShape.setTypeface(Typeface.create(Typeface.DEFAULT, Typeface.NORMAL));
        canvas.drawText(Integer.toString((int)humidity) + "%", 85, 70, paintShape);

        if(icon!=null) {
            icon.draw(canvas);
        }
    }

    private void setupPaint() {
        paintShape = new Paint();
        paintShape.setStyle(Paint.Style.FILL);
   }

    private void setupAttributes(AttributeSet attrs) {
        // Obtain a typed array of attributes
        TypedArray a = getContext().getTheme().obtainStyledAttributes(attrs, R.styleable.TempSensorView, 0, 0);
        // Extract custom attributes into member variables
        try {
            // ?android:attr/textColorSecondary
            txtColor = a.getColor(R.styleable.TempSensorView_textColor, Color.WHITE);
            backgroundColor = a.getColor(R.styleable.TempSensorView_backgroundColor, Color.BLACK);
            icon = a.getDrawable(R.styleable.TempSensorView_tempIcon);
            icon.setBounds(15, 20, 60, 60);
            icon.setAlpha(200);
         //   displayShapeName = a.getBoolean(R.styleable.ShapeSelectorView_displayShapeName, false);
        } finally {
            // TypedArray objects are shared and must be recycled.
            a.recycle();
        }
    }

    public void setTemp(float val) {
        this.temp = val;
        invalidate();
    }

    public void setHumidity(float val) {
        this.humidity = val;
        invalidate();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        // Defines the extra padding for the shape name text
        int textPadding = 10;
        int contentWidth = shapeWidth;

        // Resolve the width based on our minimum and the measure spec
        int minw = contentWidth + getPaddingLeft() + getPaddingRight();
        int w = resolveSizeAndState(minw, widthMeasureSpec, 0);

        // Ask for a height that would let the view get as big as it can
        int minh = shapeHeight + getPaddingBottom() + getPaddingTop();
     //   if (displayShapeName) {
       //     minh += textYOffset + textPadding;
        //}
        int h = resolveSizeAndState(minh, heightMeasureSpec, 0);

        // Calling this method determines the measured width and height
        // Retrieve with getMeasuredWidth or getMeasuredHeight methods later
        setMeasuredDimension(w, h);
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

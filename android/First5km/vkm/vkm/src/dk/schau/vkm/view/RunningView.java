package dk.schau.vkm.view;

import dk.schau.vkm.R;
import dk.schau.vkm.activity.FinishActivity;
import dk.schau.vkm.util.Program;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.View;

public class RunningView extends View {
    public static final int PAUSE = 0;
    public static final int RUNNING = 1;
    private static final float SCALE = (float) 0.3;
	private Bitmap runPng = null;
	private Bitmap runReturnPng = null;
	private Bitmap walkPng = null;
	private Bitmap walkReturnPng = null;
	private Bitmap finishedPng = null;
	private boolean paused = true;
	private Paint paint = new Paint();
	private Matrix matrix = new Matrix();
	private UpdateHandler updateHandler;
	private Context context;

	class UpdateHandler extends Handler {
		@Override
		public void handleMessage(Message message) {
			RunningView.this.update();
			RunningView.this.invalidate();
		}
		
		public void sleep(long pause) {
			this.removeMessages(0);
			sendMessageDelayed(obtainMessage(0), pause);
		}
	};

    public RunningView(Context context) {
        super(context);
        
        initView(context);
    }

    public RunningView(Context context, AttributeSet attributes) {
        super(context, attributes);
        
        initView(context);
    }
    
    private void initView(Context context) {
    	paused = true;
    	
    	this.context = context;
    	
    	if (runPng == null) {
    		runPng = BitmapFactory.decodeResource(getResources(), R.drawable.running);
    	}
    	
    	if (runReturnPng == null) {
    		runReturnPng = BitmapFactory.decodeResource(getResources(), R.drawable.return_running);
    	}

    	if (walkPng == null) {
    		walkPng = BitmapFactory.decodeResource(getResources(), R.drawable.walking);
    	}

    	if (walkReturnPng == null) {
    		walkReturnPng = BitmapFactory.decodeResource(getResources(), R.drawable.return_walking);
    	}
    	
    	if (finishedPng == null) {
    		finishedPng = BitmapFactory.decodeResource(getResources(), R.drawable.finish);
    	}
    }

    public void setMode(int mode) {
        if (mode == RUNNING) {
        	updateHandler = new UpdateHandler();
        	paused = false;
            update();
        } else if (mode == PAUSE) {
        	paused = true;
        	updateHandler = null;
        }
    }
    
    @Override
    protected void onDraw(Canvas canvas) {
    	int height = getHeight(), width = getWidth();
      	int quad = (height > width ? width : height) - 14;
 
      	paint.reset();
        paint.setStyle(Style.FILL);
        paint.setColor(getResources().getColor(R.color.background));
        
        canvas.drawRect(0, 0, width, height, paint);
        
        if (Program.isDone(Program.activity)) {
        	return;
        }

        int left = (width - quad) / 2;
        int right = left + quad - 1;
 
        Bitmap bigPng;
        short activity = Program.getActivity(Program.activityPointer + 1);
        if (Program.isDone(activity)) {
        	bigPng = finishedPng;
        } else {
        	if (Program.isTurned(activity)) {
        		if (Program.isRun(activity)) {
        			bigPng = runReturnPng;
        		} else {
        			bigPng = walkReturnPng;
        		}
        	} else {
        		if (Program.isRun(activity)) {
        			bigPng = runPng;
        		} else {
        			bigPng = walkPng;
        		}
        	}
        }
        
        float scaleWidth = ((float) quad * SCALE) / bigPng.getWidth();
        float scaleHeight = ((float) quad * SCALE) / bigPng.getHeight();
        matrix.reset();
        matrix.postScale(scaleWidth, scaleHeight);

        Bitmap bitmap = Bitmap.createBitmap(bigPng, 0, 0, bigPng.getWidth(), bigPng.getHeight(), matrix, true);
        int offsetX = left + quad - bitmap.getWidth() - 1;
        int offsetY = quad - bitmap.getHeight() - 1;
        canvas.drawBitmap(bitmap, offsetX, offsetY, null);
        bitmap = null;

        paint.setStyle(Style.STROKE);
        paint.setColor(getResources().getColor(R.color.text));        
        canvas.drawRect(offsetX, offsetY, right, quad - 1, paint);

        bigPng = getActivityGraphics(Program.activity);

        scaleWidth = ((float) quad) / bigPng.getWidth();
        scaleHeight = ((float) quad) / bigPng.getHeight();

        matrix.reset();
        matrix.postScale(scaleWidth, scaleHeight);
        bitmap = Bitmap.createBitmap(bigPng, 0, 0, bigPng.getWidth(), bigPng.getHeight(), matrix, true);
       	canvas.drawBitmap(bitmap, left, 0, null);
        bitmap = null;

        paint.setStyle(Style.STROKE);
    	paint.setColor(getResources().getColor(R.color.text));
    	canvas.drawRect(left, quad + 4, left, quad + 14, paint);
    	canvas.drawRect(right, quad + 4, right, quad + 14, paint);
    	
    	paint.setStyle(Style.FILL);
    	paint.setColor(getResources().getColor(R.color.runtime));

    	int barWidth = right - left, guideWidth;
    	
    	guideWidth = barWidth - (((Program.activityTime - 1) * barWidth) / Program.getTimeFromActivity(Program.activity));
    	canvas.drawRect(left + 1, quad + 5, left + guideWidth, quad + 8, paint);

    	paint.setColor(getResources().getColor(R.color.timeleft));

    	guideWidth = ((Program.activityPointer + 1) * barWidth) / Program.activities;
    	canvas.drawRect(left + 1, quad + 10, left + guideWidth, quad + 13, paint);
    }
      
    private void update() {
    	if (!paused) {
    		if (Program.countDown()) {
    			updateHandler.sleep(1000);
    		} else {
    			Program.markCompleted(Program.getDay());
    			Program.saveCompleted();

    			context.startActivity(new Intent(context, FinishActivity.class));
    		}
    	}
    }
    
    private Bitmap getActivityGraphics(short act) {
      	if (Program.isTurned(act)) {
    		if (Program.isRun(act)) {
    			return runReturnPng;
    		}
    		
    		return walkReturnPng;
    	}
      	
    	if (Program.isRun(act)) {
    		return runPng;
    	}
    	
    	return walkPng;
    }
}

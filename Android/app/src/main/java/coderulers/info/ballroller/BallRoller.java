package coderulers.info.ballroller;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class BallRoller extends Activity {
    private Engine engine;
    private BaseGLSurfaceView glView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        engine = new Engine();

        glView = new BaseGLSurfaceView(this, engine, getAssets());

        setContentView(glView);
    }

    private class BaseGLSurfaceView extends GLSurfaceView {
        private BaseGLRenderer renderer;
        private GestureDetector gestureDetector;

        public BaseGLSurfaceView(Context context, Engine engine, AssetManager assetMng) {
            super(context);

            setEGLContextClientVersion(2);

            renderer = new BaseGLRenderer(engine, assetMng);

            setRenderer(renderer);
            setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

            gestureDetector = new GestureDetector(this.getContext(), new MyGestureListener());
        }

        class MyGestureListener extends GestureDetector.SimpleOnGestureListener {

            @Override
            public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
//                if(Math.abs(distanceX) > Math.abs(distanceY)) {
//                    if(distanceX > 0.0) {
//                        renderer.onUserAction(BaseGLRenderer.UserActionType.MoveRight);
//                    }
//                    else {
//                        renderer.onUserAction(BaseGLRenderer.UserActionType.MoveLeft);
//                    }
//                    return true;
//                }
                return false;
            }

            @Override
            public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
                if(Math.abs(velocityX) > Math.abs(velocityY)) {
                    if(velocityX > 0.0) {
                        renderer.onUserAction(BaseGLRenderer.UserActionType.MoveRight);
                    }
                    else {
                        renderer.onUserAction(BaseGLRenderer.UserActionType.MoveLeft);
                    }
                    return true;
                }
                return false;
            }

            @Override
            public boolean onDown(MotionEvent e) {
                return true;
            }
        }

        @Override
        public boolean onTouchEvent(MotionEvent event) {
            return this.gestureDetector.onTouchEvent(event);
        }
    }
}

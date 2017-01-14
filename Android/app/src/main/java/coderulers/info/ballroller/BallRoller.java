package coderulers.info.ballroller;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.widget.TextView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class BallRoller extends Activity {
    private Engine engine;
    private GLSurfaceView glView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        engine = new Engine();

        glView = new BaseGLSurfaceView(this, engine, getAssets());

        setContentView(glView);
    }

    private class BaseGLSurfaceView extends GLSurfaceView {
        BaseGLRenderer renderer;

        public BaseGLSurfaceView(Context context, Engine engine, AssetManager assetMng) {
            super(context);

            setEGLContextClientVersion(2);

            renderer = new BaseGLRenderer(engine, assetMng);

            setRenderer(renderer);
            setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        }
    }
}

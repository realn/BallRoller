package coderulers.info.ballroller;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Noname on 2017-01-10.
 */

public class BaseGLRenderer implements GLSurfaceView.Renderer {
    private Engine engine;

    public BaseGLRenderer(Engine engine) {
        this.engine = engine;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        engine.Initialize();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        engine.ScreenChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        engine.FrameUpdate(1.0f/60.0f);
    }
}

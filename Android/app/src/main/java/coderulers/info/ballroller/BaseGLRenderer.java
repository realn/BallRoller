package coderulers.info.ballroller;

import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Noname on 2017-01-10.
 */

public class BaseGLRenderer implements GLSurfaceView.Renderer {
    public enum UserActionType {
        Click(0),
        DbClick(1),
        Cancel(2),
        MoveLeft(3),
        MoveRight(4),
        MoveUp(5),
        MoveDown (6),;

        private final int code;
        UserActionType(int code){
            this.code = code;
        }

        public int getCode() {
            return code;
        }
    };

    private Engine engine;
    private PngManager pngMng;

    public BaseGLRenderer(Engine engine, AssetManager assetMng) {
        this.engine = engine;
        this.pngMng = new PngManager(assetMng);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        engine.Initialize(pngMng);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        engine.ScreenChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        engine.FrameUpdate(1.0f/60.0f);
    }

    public void onUserAction(UserActionType action) {
        engine.UserAction(action.getCode());
    }
}

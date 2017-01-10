package coderulers.info.ballroller;

/**
 * Created by Noname on 2017-01-10.
 */

public final class Engine {
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native boolean Initialize();
    public native void ScreenChanged(int width, int height);
    public native void FrameUpdate(float timeDelta);
    public native void Release();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

}

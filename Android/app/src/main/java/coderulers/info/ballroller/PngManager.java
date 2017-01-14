package coderulers.info.ballroller;

import android.content.res.AssetManager;
import android.util.Log;


import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * Created by Noname on 2017-01-11.
 */

public class PngManager
{
    private AssetManager amgr;

    public PngManager(AssetManager amgr) {
        this.amgr = amgr;
    }

    public byte[] open(String path)
    {
        try (InputStream inStream = amgr.open(path)) {

            return readBytes(inStream);
        }
        catch (Exception e) {
            Log.d("BallRoller", e.getMessage());
        }
        return null;
    }

    public byte[] readBytes(InputStream inputStream) throws IOException {
        // this dynamically extends to take the bytes you read
        try (ByteArrayOutputStream byteBuffer = new ByteArrayOutputStream()) {

            // this is storage overwritten on each iteration with bytes
            int bufferSize = 1024;
            byte[] buffer = new byte[bufferSize];

            // we need to know how may bytes were read to write them to the byteBuffer
            int len = 0;
            while ((len = inputStream.read(buffer)) != -1) {
                byteBuffer.write(buffer, 0, len);
            }

            // and then we can return your byte array.
            return byteBuffer.toByteArray();
        }
    }
}
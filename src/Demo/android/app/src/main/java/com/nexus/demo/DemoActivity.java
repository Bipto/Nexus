package com.nexus.demo;

import android.content.res.AssetManager;
import android.os.Bundle;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class DemoActivity extends SDLActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        String fromPath = "";
        String toPath = "/data/data/" + getPackageName();
        AssetManager assetManager = this.getAssets();
        copyAssetFolder(assetManager, fromPath, toPath);
    }



    @Override
    protected String[] getLibraries() {
        // Return the list of libraries including custom ones
        return new String[]{
                "Nexus",
                "Demo"
        };
    }

    private static boolean copyAssetFolder(AssetManager assetManager, String fromAssetPath, String toPath)
    {
        try
        {
            if (fromAssetPath.length() > 0 && fromAssetPath.charAt(0) == '/'){
                fromAssetPath = fromAssetPath.substring(1);
            }

            String[] files = assetManager.list(fromAssetPath);
            new File(toPath).mkdirs();
            boolean res = true;
            for (String filename : files)
            {
                if (filename.contains("."))
                {
                    res &= copyAsset(assetManager,
                            fromAssetPath + "/" + filename,
                            toPath + "/" + filename);
                }
                else
                {
                    res &= copyAssetFolder(assetManager,
                            fromAssetPath + "/" + filename,
                            toPath + "/" + filename);
                }
            }
            return res;
        }
        catch (Exception e)
        {
            e.printStackTrace();
            return false;
        }
    }

    private static boolean copyAsset(AssetManager assetManager, String fromAssetPath, String toPath)
    {
        InputStream in = null;
        OutputStream out = null;
        try
        {
            in = assetManager.open(fromAssetPath);
            new File(toPath).createNewFile();
            out = new FileOutputStream(toPath);
            copyFile(in, out);
            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
            return true;
        }
        catch(Exception e)
        {
            e.printStackTrace();
            return false;
        }
    }

    private static void copyFile(InputStream in, OutputStream out) throws IOException
    {
        byte[] buffer = new byte[1024];
        int read;
        while ((read = in.read(buffer)) != -1)
        {
            out.write(buffer, 0, read);
        }
    }
}

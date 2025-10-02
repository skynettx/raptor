package com.raptor.skynettx;

import org.libsdl.app.SDLActivity;

public class RaptorSDL2Activity extends SDLActivity
{
    @Override
    protected String[] getLibraries() {
        return new String[]{
                "SDL2",
                "raptor"
        };
    }
}

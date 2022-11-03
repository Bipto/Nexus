#pragma once

#include <iostream>
#include "SDL.h"

extern "C" 
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

/* #include <Python.h> */

namespace NexusEngine
{
    static void Init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            std::cout << "Could not initialize SDL\n";
        }

        std::string cmd = "a = 7 + 11"; 
        lua_State *L = luaL_newstate();

        int r = luaL_dostring(L, cmd.c_str());

        if (r == LUA_OK)
        {
            lua_getglobal(L, "a");
            if (lua_isnumber(L, -1))
            {
                float a_in_cpp = (float)lua_tonumber(L, -1);
                std::cout << "a_in_cpp: " << a_in_cpp << std::endl;
            }
        }
        else
        {
            std::string errormsg = lua_tostring(L, -1);
            std::cout << errormsg << std::endl;
        }

        lua_close(L);

        /* Py_Initialize();
        PyRun_SimpleString("print('Hello from Python')"); */
    }
    static void Shutdown()
    {
        SDL_Quit();
        /* Py_Finalize(); */
    }
}
#pragma once
#include <cassert>

#include"lua.hpp"
#include"lualib.h"
#include"lauxlib.h"
//****************************************************************
// 
// Lua 
// 
//****************************************************************
namespace ULua
{
    //--------------------<Stateを初期化する関数>--------------------//
    inline lua_State* fCreateLuaState()
    {
        lua_State* State = nullptr;
        State = luaL_newstate();
        luaL_openlibs(State);
        luaopen_base(State);
        return State;
    }
    //--------------------<Luaファイルを開く>--------------------//
    inline void fLuaOpenFile(lua_State* Lua, const char* FileName)
    {
        const auto Result = luaL_dofile(Lua, FileName);  // Luaファイルを開く
        // Luaファイルを開けなかったらエラーメッセージを出力
        assert(Result != LUA_ERRFILE);
    }

    //--------------------<Luaの関数を呼び出す>--------------------//
    inline void fLuaCallFunc(lua_State* State_,
        int ArgCounts_, // 引数の数
        int RetCounts_) // 戻り値の数
    {
        const auto errorMassage = lua_pcall(State_, ArgCounts_, RetCounts_, 0);
        if (errorMassage != 0)
        {
            // ０以外ならエラーをキャッチ
            assert(lua_tostring(State_, -1));
        }
    }



}

//****************************************************************
// 
// Luaのサポート関数群
// 
//****************************************************************
namespace SupLua
{

    //--------------------<Luaのスタックの中身をすべて削除>--------------------//
    inline void fDestroyLuaStack(lua_State* State)
    {
        // スタック削除処理
        const int LuaStackSize = lua_gettop(State);
        for (int i = 0; i < LuaStackSize; i++)
        {
            lua_pop(State, -1);
        }
    }

    //****************************************************************
    // 
    //  以下Luaに値を挿入する関数
    // 
    //****************************************************************


    inline void fLuaPushNumeric(lua_State* Lua, double Numeric_)
    {
        lua_pushnumber(Lua, Numeric_);
    }

    inline void fLuaPushBoolean(lua_State* Lua, bool Is)
    {
        lua_pushboolean(Lua, 0);
    }



}

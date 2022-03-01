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
    //--------------------<State������������֐�>--------------------//
    inline lua_State* fCreateLuaState()
    {
        lua_State* State = nullptr;
        State = luaL_newstate();
        luaL_openlibs(State);
        luaopen_base(State);
        return State;
    }
    //--------------------<Lua�t�@�C�����J��>--------------------//
    inline void fLuaOpenFile(lua_State* Lua, const char* FileName)
    {
        const auto Result = luaL_dofile(Lua, FileName);  // Lua�t�@�C�����J��
        // Lua�t�@�C�����J���Ȃ�������G���[���b�Z�[�W���o��
        assert(Result != LUA_ERRFILE);
    }

    //--------------------<Lua�̊֐����Ăяo��>--------------------//
    inline void fLuaCallFunc(lua_State* State_,
        int ArgCounts_, // �����̐�
        int RetCounts_) // �߂�l�̐�
    {
        const auto errorMassage = lua_pcall(State_, ArgCounts_, RetCounts_, 0);
        if (errorMassage != 0)
        {
            // �O�ȊO�Ȃ�G���[���L���b�`
            assert(lua_tostring(State_, -1));
        }
    }



}

//****************************************************************
// 
// Lua�̃T�|�[�g�֐��Q
// 
//****************************************************************
namespace SupLua
{

    //--------------------<Lua�̃X�^�b�N�̒��g�����ׂč폜>--------------------//
    inline void fDestroyLuaStack(lua_State* State)
    {
        // �X�^�b�N�폜����
        const int LuaStackSize = lua_gettop(State);
        for (int i = 0; i < LuaStackSize; i++)
        {
            lua_pop(State, -1);
        }
    }

    //****************************************************************
    // 
    //  �ȉ�Lua�ɒl��}������֐�
    // 
    //****************************************************************


   
}

class LuaWorld final 
{
    //****************************************************************
    // 
    // �֐�
    // 
    //****************************************************************
public:
    LuaWorld() = default;
    ~LuaWorld() = default;

    void fCreate();  // �쐬�i�������j
    void fLoadFile(const char* FileName_);// �t�@�C�������[�h

    
    //****************************************************************
    // 
    // �l��}��
    // 
    //****************************************************************
    void fPushNumeric(double Numeric_)
    {
        lua_pushnumber(mLuaState, Numeric_);
    }

    inline void fPushBoolean( bool Is)
    {
        lua_pushboolean(Lua, 0);
    }

    //--------------------<Lua�̃O���[�o���֐��̒l���擾����>--------------------//
    inline float fGetGlobalFloat(lua_State* State_, const char* NumericName_)
    {

    }


private:
    lua_State* mLuaState{ nullptr };
};
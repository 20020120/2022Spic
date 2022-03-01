#include"UserLua.h"
#include <stdexcept>

void LuaWorld::fCreate()
{
    mLuaState = luaL_newstate();
    luaL_openlibs(mLuaState);
    luaopen_base(mLuaState);
}

void LuaWorld::fLoadFile(const char* FileName_) const
{
    const auto Result = luaL_dofile(mLuaState, FileName_);  // Lua�t�@�C�����J��
        // Lua�t�@�C�����J���Ȃ�������G���[���b�Z�[�W���o��
    assert(!Result);
}

void LuaWorld::fDestroyStack()
{
    // �X�^�b�N�폜����
    const int LuaStackSize = lua_gettop(mLuaState);
    for (int i = 0; i < LuaStackSize; i++)
    {
        lua_pop(mLuaState, -1);
    }
}

void LuaWorld::fCallFunc(int ArgCounts_, int RetCounts_) const
{
    const auto errorMassage = lua_pcall(mLuaState, ArgCounts_, RetCounts_, 0);
    if (errorMassage != 0)
    {
        // �O�ȊO�Ȃ�G���[���L���b�`
        assert(lua_tostring(mLuaState, -1));
    }
}

void LuaWorld::fSetNumeric(double Numeric_) const
{
    lua_pushnumber(mLuaState, Numeric_);
}

void LuaWorld::fSetBoolean(bool Is) const
{
    lua_pushboolean(mLuaState, 0);
}

void LuaWorld::fSetFunction(const char* FunctionName_) const
{
    lua_getglobal(mLuaState, FunctionName_);
}

double LuaWorld::fGetGlobalFloat(const char* NumericName_) const
{
    lua_getglobal(mLuaState, NumericName_);
    return  luaL_checknumber(mLuaState, -1);
}

double LuaWorld::fGetFloat(int StackPoint_) const
{
    return luaL_checknumber(mLuaState, StackPoint_);
}

void LuaWorld::fDebugShowStack()
{
    // Stack�̒��g������
    
}

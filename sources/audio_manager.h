#pragma once

#include "audio.h"
#include "user.h"
#include <vector>
// XAudio2
#include <wrl.h>
#include <xaudio2.h>
enum class BGM_INDEX
{
    TITLE,
    TUTORIAL,
    GAME,
    BOSS_BATTLESHIP,
    BOSS_HUMANOID,
    BOSS_DRAGON,

    BGM_COUNT
};

enum class SE_INDEX
{
    SELECT,             //選択音
    DECISION,           //決定音
    DRAW_SWORD,         //剣を抜く音
    RETURN_SWORD,       //剣を鞘に戻す
    SWING_SWORD,        //攻撃(当たっていない)
    ATTACK_SWORD,       //攻撃(当たっている)
    ENEMY_EMERGENCE,    //敵出現
    SHOT_BOW,           //弓攻撃
    CANNON,             //砲台音
    HEART_BEAT,         //心臓の鼓動音
    OPEN_FIRE,          //焚火
    TRANSFORM1,         //ボス変形候補1
    TRANSFORM2,         //ボス変形候補2
    TRANSFORM3,         //ボス変形候補3
    TRANSFORM4,         //ボス変形候補4
    TRANSFORM5,         //ボス変形候補5
    TRANSFORM6,         //ボス変形候補6
    BOSS_BEAM,          //ボスビーム攻撃
    DRAGON_ROAR,        //ドラゴンボス咆哮
    ENEMY_EXPLOSION,    //敵撃破音

    SE_COUNT
};

class AudioManager
{
public:
    //--------<constructor/destructor>--------//
    AudioManager();
    ~AudioManager();
public:
    //----bgm----//
    void play_bgm(BGM_INDEX index);
    void stop_bgm(BGM_INDEX index);
    void stop_all_bgm();
    void set_volume_bgm(BGM_INDEX index, float volume);
    void set_all_volume_bgm(float volume);
    //----se----//
    void play_se(SE_INDEX index);
    void stop_se(SE_INDEX index);
    void stop_all_se();
    void set_volume_se(SE_INDEX index, float volume);
    void set_all_volume_se(float volume);
private:
    //--------< 定数 >--------//
    static const int MAX_BGM_COUNT = 256;
    static const int MAX_SE_COUNT = 256;
    //--------< 変数 >--------//
    IXAudio2* pXAudio2;
    //----bgm----//
    IXAudio2SourceVoice* bgm_source_voices[MAX_BGM_COUNT];
    BYTE* bgm_pdata_buffer[MAX_BGM_COUNT];
    //----se----//
    IXAudio2SourceVoice* se_source_voices[MAX_SE_COUNT];
    XAUDIO2_BUFFER buffer_se[MAX_SE_COUNT] = { 0 };
    BYTE* se_pdata_buffer[MAX_BGM_COUNT];
private:
    //--------< 関数 >--------//
    void initialize_XAudio2();
    //----bgm----//
    void initialize_bgm();
    void register_bgm(const LPCWSTR filename, BGM_INDEX index, float volume = 1.0f, bool is_loop = true);
    //----se----//
    void initialize_se();
    void register_se(const LPCWSTR filename, SE_INDEX index, float volume = 1.0f, int loop_count = 0);
};
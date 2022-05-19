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
    CLEAR,

    BGM_COUNT
};

enum class SE_INDEX
{
    SELECT,                 //�I����
    DECISION,               //���艹
    DRAW_SWORD,             //���𔲂���
    RETURN_SWORD,           //������ɖ߂�
    SWING_SWORD,            //�U��(�������Ă��Ȃ�)
    ATTACK_SWORD,           //�U��(�������Ă���)
    ENEMY_EMERGENCE,        //�G�o��
    SHOT_BOW,               //�|�U��
    CANNON,                 //�C�䉹
    HEART_BEAT,             //�S���̌ۓ���
    OPEN_FIRE,              //����
    TRANSFORM1,             //�{�X�ό`���1
    TRANSFORM2,             //�{�X�ό`���2
    TRANSFORM3,             //�{�X�ό`���3
    TRANSFORM4,             //�{�X�ό`���4
    TRANSFORM5,             //�{�X�ό`���5
    TRANSFORM6,             //�{�X�ό`���6
    BOSS_BEAM,              //�{�X�r�[���U��
    DRAGON_ROAR,            //�h���S���{�X���K
    ENEMY_EXPLOSION,        //�G���j��
    STAN,                   //�G�X�^��
    WRAPAROUND_AVOIDANCE,   //��荞�݉��
    AVOIDANCE,              //���
    DRAGON_BOSS_DEATH,      //�h���S���{�X�̎�
    ENEMY_PRE_ATTACK_SOUND, //�G�U���\������
    ENEMY_SWORD_ATTACK,     //���̓G�̍U����
    ENEMY_SHIELD_ATTACK,    //���̓G�̍U����
    BOSS_CHARGE,            //�r�[���`���[�W��
    PLAYER_AWAKING,         //�v���C���[�o��
    PLAYER_DAMAGED,         //�v���C���[��_���[�W
    FIRE_BOLL,              //�΂̋ʂ�f��
    FIRE_BOLL_RUN,          //�΂̋ʂ�����
    ROCK_ON,                //���b�N�I�����̉�
    PLAYER_RUSH,            //�v���C���[�ːi
    SPEAR_RUSH,             //���̓G�ːi
    BOSS_RUSH,              //�{�X�ːi
    TITLE_CUT,              //�^�C�g���؂�鉹
    TITLE_CUTTING,          //�^�C�g���؂�Ă�����
    DRAW_PEN,               //�N���A�A�Q�[���I�[�o�[�̉�
    BOSS_EXPLOSION,         //�{�X���j��
    GAUGE_MAX,              //�o���Q�[�WMAX

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
    //--------< �萔 >--------//
    static const int MAX_BGM_COUNT = 256;
    static const int MAX_SE_COUNT = 256;
    //--------< �ϐ� >--------//
    IXAudio2* pXAudio2;
    //----bgm----//
    IXAudio2SourceVoice* bgm_source_voices[MAX_BGM_COUNT];
    BYTE* bgm_pdata_buffer[MAX_BGM_COUNT];
    //----se----//
    IXAudio2SourceVoice* se_source_voices[MAX_SE_COUNT];
    XAUDIO2_BUFFER buffer_se[MAX_SE_COUNT] = { 0 };
    BYTE* se_pdata_buffer[MAX_BGM_COUNT];
private:
    //--------< �֐� >--------//
    void initialize_XAudio2();
    //----bgm----//
    void initialize_bgm();
    void register_bgm(const LPCWSTR filename, BGM_INDEX index, float volume = 1.0f, bool is_loop = true);
    //----se----//
    void initialize_se();
    void register_se(const LPCWSTR filename, SE_INDEX index, float volume = 1.0f, int loop_count = 0);
};
#pragma once 
//****************************************************************
// 
// �E�F�[�u���Ǘ�����N���X
// 
//****************************************************************
class WaveManager final 
{
public:
    WaveManager() = default;
    ~WaveManager() = default;

    // ������
    void fInitialize();
    // �X�V
    void fUpdate(float elapsedTime_);
    // �I������
    void fFinalize();

    // �N���A����
    void fClear();

};
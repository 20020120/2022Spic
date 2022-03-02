#include "effect.h"

Effect::Effect(GraphicsPipeline& graphics, Effekseer::Manager* effekseer_manager, const char* filename)
{
    std::lock_guard<std::mutex> lock(graphics.get_mutex());
    //Effekseerのリソースを読み込む
    //EffekseerはUTF_16のファイルパス以外は対応していないので文字コード変換が必要
    char16_t utf16_filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16_filename, 256, filename);
    //Effekseer::Managerを取得
    //Effekseerエフェクトを読み込み
    effekseer_effect = Effekseer::Effect::Create(effekseer_manager, (EFK_CHAR*)utf16_filename);
}

Effect::~Effect()
{
    if (effekseer_effect != nullptr)
    {
        effekseer_effect->Release();
        effekseer_effect = nullptr;
    }
}

void Effect::play(Effekseer::Manager* effekseer_manager, const DirectX::XMFLOAT3& position, float scale)
{
    effekseer_handle = effekseer_manager->Play(effekseer_effect, position.x, position.y, position.z);
    effekseer_manager->SetScale(effekseer_handle, scale, scale, scale);
    //Effekseer::Color c = { static_cast<uint8_t>(255),static_cast<uint8_t>(255),static_cast<uint8_t>(255),static_cast<uint8_t>(120) };
    //effekseer_manager->SetAllColor(effekseer_handle, c);
}

void Effect::stop(Effekseer::Manager* effekseer_manager)
{
    effekseer_manager->StopEffect(effekseer_handle);
}

void Effect::set_position(Effekseer::Manager* effekseer_manager, const DirectX::XMFLOAT3& position)
{
    effekseer_manager->SetLocation(effekseer_handle, position.x, position.y, position.z);
}

void Effect::set_scale(Effekseer::Manager* effekseer_manager, const DirectX::XMFLOAT3& scale)
{
    effekseer_manager->SetScale(effekseer_handle, scale.x, scale.y, scale.z);
}

void Effect::set_speed(Effekseer::Manager* effekseer_manager, float speed)
{
    effekseer_manager->SetSpeed(effekseer_handle, speed);
}

void Effect::set_rotationY(Effekseer::Manager* effekseer_manager, const float angle)
{
    Effekseer::Vector3D up{ 0,1,0 };
    effekseer_manager->SetRotation(effekseer_handle, up, angle);
}

void Effect::set_quaternion(Effekseer::Manager* effekseer_manager, DirectX::XMFLOAT4 orientation)
{
    effekseer_manager->GetBaseMatrix(effekseer_handle);
}
#pragma once

#include <cereal/cereal.hpp>
#include "icon_base.h"
#include "practical_entities.h"
#include "number.h"

class GameFile
{
private:
    //--------<constructor/destructor>--------//
    GameFile() {}
    ~GameFile() {}
    //--------< 構造体 >--------//
    struct GameSource
    {
        //--------< 変数 >--------//
        bool shake        = true;
        bool vibration    = true;
        bool omission = true;
        float sensitivity = 0.5f;
        //--------< 関数 >--------//
        void initialize()
        {
            shake       = true;
            vibration   = true;
            omission    = true;
            sensitivity = 0.5f;
        }
        // シリアライズ
        template<class Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("shake", shake),
                cereal::make_nvp("vibration", vibration),
                cereal::make_nvp("omission", omission),
                cereal::make_nvp("sensitivity", sensitivity));
        }
    };
    //--------< 変数 >--------//
    GameSource source;
    const char* file_name = "./resources/Data/game.json";
public:
    //--------< singleton >--------//
    static GameFile& get_instance()
    {
        static GameFile instance;
        return instance;
    }
    //--------< 関数 >--------//
    void load();
    void save();
    //--------<getter/setter>--------//
    // shake
    bool get_shake() const { return source.shake; }
    void set_shake(bool s) { source.shake = s; }
    // vibration
    bool get_vibration() const { return source.vibration; }
    void set_vibration(bool v) { source.vibration = v; }
    // omission
    bool get_omission() const { return source.omission; }
    void set_omission(bool o) { source.omission = o; }
    // sensitivity
    float get_sensitivity() const { return source.sensitivity; }
    void set_sensitivity(float s) { source.sensitivity = s; }
};

class GameIcon : public IconBase, PracticalEntities
{
public:
    //--------<constructor/destructor>--------//
    GameIcon(ID3D11Device* device);
    ~GameIcon() override;
    //--------< 関数 >--------//
    void update(GraphicsPipeline& graphics, float elapsed_time) override;
    void render(std::string gui, ID3D11DeviceContext* dc, const DirectX::XMFLOAT2& add_pos) override;
    void vs_cursor(const DirectX::XMFLOAT2& cursor_pos) override;
private:
    void save_source();
    //--------< 定数 >--------//
    enum ChoicesType
    {
        SHAKE,
        VIBRATION,
        OMISSION,

        SENSITIVITY, // barは一番下
    };
    static const int MAX_SCALE_COUNT = 20;
    static constexpr float INTERVAL = 0.1f;
    static const int BUTTON_COUNT = 3;
    //--------< 変数 >--------//
    FontElement shake;
    FontElement vibration;
    FontElement omission;
    FontElement sensitivity;
    //--button--//
    FontElement choices[BUTTON_COUNT][2]; // 0:ON 1:OFF
    Element selecter[BUTTON_COUNT][2]; // 0:L 1:R
    DirectX::XMFLOAT2 selecter_arrival_pos[BUTTON_COUNT][2]; // 0:L 1:R
    bool setup[BUTTON_COUNT];
    //--bar--//
    float interval_LX = 0;
    std::vector<Element> scales;
    std::vector<Element> shell_scales;
    std::unique_ptr<SpriteBatch> sprite_scale = nullptr;
    std::unique_ptr<Number> sensitivity_number;
};
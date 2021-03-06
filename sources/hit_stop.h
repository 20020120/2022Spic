#pragma once
#include "camera_shake.h"

class HitStop
{
public:
    //--------<constructor/destructor>--------//
    HitStop() {}
    ~HitStop() {}
    //--------< 関数 >--------//
    /*呼び出しはFrameworkのみ*/
    void update(GraphicsPipeline& graphics, float elapsed_time, CameraShake& camera_shake);
    /* hit stop presets */
    void damage_hit_stop(); /* 1フレーム処理 */
    //--------<getter/setter>--------//
    const bool get_hit_stop() const { return hit_stop; }
private:
    //--------< 変数 >--------//
    float timer = 0;
    float hit_stop_time = 0;
    bool hit_stop = false;
    bool shake = false;
};
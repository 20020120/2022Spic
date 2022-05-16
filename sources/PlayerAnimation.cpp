#include"Player.h"
void Player::ExecFuncUpdate(float elapsed_time)
{
    (this->*player_title_activity)(elapsed_time);
}
void Player::UpdateTitleAnimationReadyIdle(float elaosed_time)
{
    if (start_title_animation)
    {
        model->play_animation(AnimationClips::TitleAnimationStart);
        player_title_activity = &Player::UpdateTitleAnimationStart;
    }
}
void Player::UpdateTitleAnimationStart(float elaosed_time)
{
    if (model->end_of_animation())
    {
        model->play_animation(AnimationClips::TitleAnimationStartIdle);
        player_title_activity = &Player::UpdateTitleAnimationStartIdle;
    }
}
void Player::UpdateTitleAnimationStartIdle(float elaosed_time)
{
    title_timer += 1.0f * elaosed_time;
    //モデルのアニメーションが終わった時かつ1秒たったら
    if (model->end_of_animation() && title_timer > 0.5f)
    {
        title_timer = 0;
        model->play_animation(AnimationClips::TitleAnimationEnd);
        player_title_activity = &Player::UpdateTitleAnimationEnd;
    }
}
void Player::UpdateTitleAnimationEnd(float elaosed_time)
{
    if (model->end_of_animation())
    {
        model->play_animation(AnimationClips::TitleAnimationEndIdle);
        player_title_activity = &Player::UpdateTitleAnimationEndIdle;
    }
}
void Player::UpdateTitleAnimationEndIdle(float elaosed_time)
{
    title_timer += 1.0f * elaosed_time;
    if(title_timer > 0.5) end_title_animation = true;

    if (model->end_of_animation())
    {
    }
}
void Player::TransitionTitleAnimationReadyIdle()
{
    model->play_animation(AnimationClips::TitleAnimationReadyIdle,true);
    //ここでupdateしておかないと1フレームだけ初期のアニメーションが映ってしまうから
    model->update_animation(1.0f);
    //タイトルはメッシュを隠さないから0にしておく
    threshold_mesh = 0.0f;
}
void Player::ExecFuncUpdate(float elapsed_time, SkyDome* sky_dome, std::vector<BaseEnemy*> enemies, GraphicsPipeline& Graphics_)
{
    switch (behavior_state)
    {
    case Player::Behavior::Normal:
        //自分のクラスの関数ポインタを呼ぶ
        (this->*player_activity)(elapsed_time, sky_dome);
        break;
    case Player::Behavior::Chain:
        (this->*player_chain_activity)(elapsed_time, enemies,Graphics_);
        break;
    default:
        break;
    }
}

void Player::IdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //移動に遷移
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
    {
        TransitionMove();
    }
    //回避に遷移
    float length{ Math::calc_vector_AtoB_length(position, target) };
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        //後ろに回り込める距離なら回り込みようのUpdate
        if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
        {
            TransitionBehindAvoidance();
        }
        //そうじゃなかったら普通の回避
        else TransitionAvoidance();
    }
    //突進開始に遷移
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }

    Awaiking();
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::MoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //移動入力がなくなったら待機に遷移
    if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) <= 0)
    {
        TransitionIdle();
    }
    //回避に遷移
    float length{ Math::calc_vector_AtoB_length(position, target) };
    if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
    {
        //後ろに回り込める距離なら回り込みようのUpdate
        if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
        {
            TransitionBehindAvoidance();
        }
        //そうじゃなかったら普通の回避
        else TransitionAvoidance();
    }
    //突進開始に遷移
    if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
    {
        TransitionChargeInit();
    }

    Awaiking();
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //エフェクトの位置，回転設定
    player_air_registance_effec->set_position(effect_manager->get_effekseer_manager(),position);
    player_air_registance_effec->set_quaternion(effect_manager->get_effekseer_manager(), orientation);

    AvoidanceAcceleration(elapsed_time);
    //回避のアニメーションが終わったら
    if (avoidance_boost_time > avoidance_easing_time && model->end_of_animation())
    {
        player_air_registance_effec->stop(effect_manager->get_effekseer_manager());
        //回避中かどうかの設定
        is_avoidance = false;
        is_behind_avoidance = false;
        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        //移動入力がなかったら待機に遷移
        else
        {
            TransitionIdle();
        }
        Awaiking();
        UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    }
    else
    {
        UpdateAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
    }
}

void Player::BehindAvoidanceUpdate(float elapsed_time, SkyDome* sky_dome)
{
    //behind_timer += 2.0f * elapsed_time;
    player_behind_effec->set_position(effect_manager->get_effekseer_manager(), { position.x,position.y + air_registance_offset_y ,position.z });
    //BehindAvoidanceMove(elapsed_time);
    if (BehindAvoidanceMove(elapsed_time, behind_transit_index,position,100.0f, behind_interpolated_way_points,1.0f))
    {
        player_behind_effec->stop(effect_manager->get_effekseer_manager());
        //回避中かどうかの設定
        is_avoidance = false;
        is_behind_avoidance = false;
        //ジャスト回避のフラグを初期化
        is_just_avoidance = false;
        TransitionIdle();
    }
    else
    {
        is_lock_on = true;
    }
    UpdateBehindAvoidanceVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::ChargeInitUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionCharge(attack_animation_blends_speeds.x);
    }
    //ChargeAcceleration(elapsed_time);
    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::ChargeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    start_dash_effect = false;
    charge_time += charge_add_time * elapsed_time;
    //ChargeAcceleration(elapsed_time);
    //攻撃の加速の設定
    SetAccelerationVelocity();
    //突進時間を超えたらそれぞれの遷移にとぶ
    if (charge_time > CHARGE_MAX_TIME)
    {
        end_dash_effect = true;
        velocity.x *= 0.2f;
        velocity.y *= 0.2f;
        velocity.z *= 0.2f;

        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            charge_time = 0;
            is_charge = false;
            charge_change_direction_count = CHARGE_DIRECTION_COUNT;
            TransitionMove();
        }
        //移動入力がなかったら待機に遷移
        else
        {
            charge_time = 0;
            is_charge = false;
            charge_change_direction_count = CHARGE_DIRECTION_COUNT;
            TransitionIdle();
        }
        Awaiking();
    }
    else
    {
        if (is_enemy_hit)
        {
            end_dash_effect = true;
            //敵に当たって攻撃ボタン(突進ボタン)を押したら一撃目
            is_charge = false;
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            charge_change_direction_count = CHARGE_DIRECTION_COUNT;
            TransitionAttackType1(attack_animation_blends_speeds.y);
        }
        if (is_lock_on == false && charge_change_direction_count > 0)
        {
            if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
            {
                charge_change_direction_count--;
                velocity = {};
                DirectX::XMFLOAT3 movevec = SetMoveVec(camera_forward, camera_right);
                if ((movevec.x * movevec.x) + (movevec.z * movevec.z) > 0)
                {
                    ChargeTurn(elapsed_time, movevec, turn_speed, position, orientation);
                    charge_point = Math::calc_designated_point(position, movevec, 200.0f);
                }
                else
                {
                    ChargeTurn(elapsed_time, forward, turn_speed, position, orientation);
                    charge_point = Math::calc_designated_point(position, forward, 200.0f);
                }
               //SetAccelerationVelocity();
                charge_time = 0;
                //TransitionCharge();
            }
        }
    }
    if (is_awakening)
    {
        mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);
        mSwordTrail[1].fAddTrailPoint(sword_capsule_param[1].start, sword_capsule_param[1].end);
    }
    else mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AttackType1Update(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {

        attack_time += attack_add_time * elapsed_time;
        //猶予時間を超えたら待機に遷移
        if (attack_time > ATTACK_TYPE1_MAX_TIME)
        {
            attack_time = 0;
            TransitionIdle();
        }
        //猶予時間よりも早く押したら攻撃2撃目に遷移
#if 0
            if (enemy_length > 20.0f)
            {
                TransitionChargeInit();
            }
            else
            {
                TransitionAttackType2(attack_animation_blends_speeds.z);
            }
#else
            if (target_enemy != nullptr && target_enemy->fGetPercentHitPoint() != 0)
            {
                if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
                {
                    attack_time = 0;
                    velocity.x *= 0.2f;
                    velocity.y *= 0.2f;
                    velocity.z *= 0.2f;
                    TransitionAttackType2(attack_animation_blends_speeds.z);
                }
            }
            else   TransitionIdle();

#endif // 0
    }
    if (is_awakening)
    {
        mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);
        mSwordTrail[1].fAddTrailPoint(sword_capsule_param[1].start, sword_capsule_param[1].end);
    }
    else mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AttackType2Update(float elapsed_time, SkyDome* sky_dome)
{
    attack_time += attack_add_time * elapsed_time;
    //敵に当たったか時間が2秒たったら加速を終わる
#if 0
    if (is_update_animation == false && (is_enemy_hit || attack_time >= 1.0f))
    {
        is_charge = false;
        attack_time = 0;
        is_update_animation = true;
    }
    else
    {
        float length{ Math::calc_vector_AtoB_length(position,target) };

        //if (length > 5.0f)ChargeAcceleration(elapsed_time);
        SetAccelerationVelocity();
    }
#else
    if (is_update_animation == false)
    {
        SetAccelerationVelocity();
        if (is_enemy_hit)
        {
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            is_charge = false;
            attack_time = 0;
            is_update_animation = true;
        }
        else if (attack_time >= 2.0f)
        {
            is_charge = false;
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            attack_time = 0;
            TransitionIdle();
        }

    }

#endif // 0
    if (model->end_of_animation())
    {
        //猶予時間を超えたら待機に遷移
        if (attack_time > ATTACK_TYPE2_MAX_TIME)
        {
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            attack_time = 0;
            TransitionIdle();
        }
        //猶予時間よりも早く押したら攻撃3撃目に遷移
#if 0
            if (enemy_length > 0.0f)
            {
                TransitionChargeInit();
            }
            else
            {
                TransitionAttackType3(attack_animation_blends_speeds.w);
            }
#else
            if (target_enemy != nullptr && target_enemy->fGetPercentHitPoint() != 0)
            {
                if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
                {
                    velocity.x *= 0.2f;
                    velocity.y *= 0.2f;
                    velocity.z *= 0.2f;
                    attack_time = 0;
                    TransitionAttackType3(attack_animation_blends_speeds.w);
                }
            }
            else
            {
                velocity.x *= 0.2f;
                velocity.y *= 0.2f;
                velocity.z *= 0.2f;
                TransitionIdle();
            }
#endif // 0
    }
    if (is_awakening)
    {
        mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);
        mSwordTrail[1].fAddTrailPoint(sword_capsule_param[1].start, sword_capsule_param[1].end);
    }
    else mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AttackType3Update(float elapsed_time, SkyDome* sky_dome)
{

    attack_time += attack_add_time * elapsed_time;
    //敵に当たったか時間が2秒たったら加速を終わる

#if 0
    if (is_update_animation == false && (is_enemy_hit || attack_time >= 1.0f))
    {
        is_charge = false;
        attack_time = 0;
        is_update_animation = true;
    }
    else
    {
        float length{ Math::calc_vector_AtoB_length(position,target) };
        //if (length > 5.0f) ChargeAcceleration(elapsed_time);
        SetAccelerationVelocity();
    }
#else
    if (is_update_animation == false)
    {
        SetAccelerationVelocity();
        if (is_enemy_hit)
        {
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            is_charge = false;
            attack_time = 0;
            is_update_animation = true;
        }
        else if (attack_time >= 2.0f)
        {
            is_charge = false;
            velocity.x *= 0.2f;
            velocity.y *= 0.2f;
            velocity.z *= 0.2f;
            attack_time = 0;
            TransitionIdle();
        }
    }

#endif // 0

    if (model->end_of_animation())
    {
        if (target_enemy != nullptr && target_enemy->fGetPercentHitPoint() != 0)
        {
            if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
            {
                attack_time = 0;
                velocity.x *= 0.2f;
                velocity.y *= 0.2f;
                velocity.z *= 0.2f;
                TransitionAttackType1(attack_animation_blends_speeds.x);
            }
        }
        else
        {
            //移動入力があったら移動に遷移
            if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
            {
                velocity.x *= 0.2f;
                velocity.y *= 0.2f;
                velocity.z *= 0.2f;
                charge_time = 0;
                TransitionMove();
            }
            //移動入力がなかったら待機に遷移
            else
            {
                velocity.x *= 0.2f;
                velocity.y *= 0.2f;
                velocity.z *= 0.2f;
                charge_time = 0;
                TransitionIdle();
            }
        }
    }
    if (is_awakening)
    {
        mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);
        mSwordTrail[1].fAddTrailPoint(sword_capsule_param[1].start, sword_capsule_param[1].end);
    }
    else mSwordTrail[0].fAddTrailPoint(sword_capsule_param[0].start, sword_capsule_param[0].end);

    UpdateAttackVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::SpecialSurgeUpdate(float elapsed_time, SkyDome* sky_dome)
{
    special_surge_timer += 1.0f * elapsed_time;

    if (special_surge_timer > 1.0f)
    {
        velocity = {};
        TransitionOpportunity();
    }
    UpdateSpecialSurgeVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::OpportunityUpdate(float elapsed_time, SkyDome* sky_dome)
{
    opportunity_timer += 1.0f * elapsed_time;
    if (special_surge_combo_count > 0)special_surge_opportunity = 2.0f / special_surge_combo_count;
    else special_surge_opportunity = 2.0f;

    //設定した隙よりも時間がたったらそれぞれの行動に遷移する
    if (opportunity_timer > special_surge_opportunity)
    {
        special_surge_combo_count = 0;
        TransitionTransformHum();
    }
        //UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::DamageUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionIdle();
    }
}

void Player::TransformHumUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        //移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        else
        {
            TransitionIdle();
        }
        //回避に遷移
        float length{ Math::calc_vector_AtoB_length(position, target) };
        if (game_pad->get_trigger_R() || game_pad->get_button_down() & GamePad::BTN_RIGHT_SHOULDER)
        {
            //後ろに回り込める距離なら回り込みようのUpdate
            if (is_lock_on && length < BEHIND_LANGE_MAX && length > BEHIND_LANGE_MIN)
            {
                TransitionBehindAvoidance();//プロト段階では回り込み回避は消しておく
            }
            //そうじゃなかったら普通の回避
            else TransitionAvoidance();
        }
        //突進開始に遷移
        if (game_pad->get_button_down() & GamePad::BTN_ATTACK_B)
        {
            TransitionChargeInit();
        }

    }
    UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::TransformWingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionSpecialSurge();
    }
    //ロックオンしている敵の方を見る
    UpdateRotateToTarget(elapsed_time, position, orientation, camera_forward, camera_position);
   //UpdateVelocity(elapsed_time, position, orientation, camera_forward, camera_right, camera_position, sky_dome);
}

void Player::AwakingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        //移動入力がなかったら待機に遷移
        else
        {
            TransitionIdle();
        }
    }
}

void Player::InvAwakingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        //移動入力があったら移動に遷移
        if (sqrtf((velocity.x * velocity.x) + (velocity.z * velocity.z)) > 0)
        {
            TransitionMove();
        }
        //移動入力がなかったら待機に遷移
        else
        {
            TransitionIdle();
        }
    }
}

void Player::StageMoveUpdate(float elapsed_time, SkyDome* sky_dome)
{
    model->update_animation(elapsed_time);
}

void Player::WingDashStartUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::WingDashIdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::WingDashEndUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::DieUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionDying();
    }
}

void Player::DyingUpdate(float elapsed_time, SkyDome* sky_dome)
{
    is_dying_update = true;
    threshold += 1.0f * elapsed_time;
    threshold_mesh += 1.0f * elapsed_time;
    if (threshold > 1.0f && threshold_mesh > 1.0f)
    {
        is_alive = false;
    }
}

void Player::StartMothinUpdate(float elapsed_time, SkyDome* sky_dome)
{
}

void Player::NamelessMotionUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        TransitionNamelessMotionIdle();
    }
}

void Player::NamelessMotionIdleUpdate(float elapsed_time, SkyDome* sky_dome)
{
    if (model->end_of_animation())
    {
        //クリアモーションが終わったことを伝える
        is_end_clear_motion = true;
    }
}

void Player::Awaiking()
{
    //ボタン入力
    if (game_pad->get_button() & GamePad::BTN_A)
    {
        if (combo_count >= MAX_COMBO_COUNT - 5.0f)TransitionAwaking();//コンボカウントが最大のときは覚醒状態になる
    }
    if (is_awakening &&combo_count <= 0) TransitionInvAwaking();//覚醒状態のときにカウントが0になったら通常状態になる
}

void Player::TransitionIdle(float blend_second)
{
    //クリア演出中なら解除する
    if (during_clear) during_clear = false;
    condition_state = ConditionState::Alive;
    //ダッシュエフェクトの終了
    //end_dash_effect = true;
    //覚醒状態の時の待機アニメーションにセット
    if(is_awakening)model->play_animation(AnimationClips::AwakingIdle, true,true,blend_second);
    //通常状態の待機アニメーションにセット
    else model->play_animation(AnimationClips::Idle, true,true,blend_second);
    //攻撃中かどうかの設定
    is_attack = false;
    //アニメーション速度
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //待機状態の時の更新関数に切り替える
    player_activity = &Player::IdleUpdate;
}

void Player::TransitionMove(float blend_second)
{
    //ダッシュエフェクトの終了
    //end_dash_effect = true;
    //覚醒状態の時の移動アニメーションの設定
    if(is_awakening)model->play_animation(AnimationClips::AwakingMove, true,true, blend_second);
    //通常状態の時に移動アニメーションの設定
    else model->play_animation(AnimationClips::Move, true, true,blend_second);
    //攻撃中かどうかの設定
    is_attack = false;
    //アニメーション速度
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //移動状態の時の更新関数に切り替える
    player_activity = &Player::MoveUpdate;
}

void Player::TransitionAvoidance()
{
    //エフェクト再生
    player_air_registance_effec->play(effect_manager->get_effekseer_manager(), position,0.3f);
    //回避中かどうかの設定
    is_avoidance = true;
    //回り込み回避かどうか
    is_behind_avoidance = false;
    //--------------------------イージング加速の変数初期化---------------------------------//
    avoidance_boost_time = 0;
    avoidance_start = velocity;
    if (is_lock_on)
    {
        leverage = 15.0f;
        DirectX::XMFLOAT3 movevec = SetMoveVec(camera_forward, camera_right);
        if (sqrtf((movevec.x * movevec.x) + (movevec.y * movevec.y) + (movevec.z * movevec.z)) <= 0.0f)
        {
            avoidance_end = { forward.x * leverage ,forward.y * leverage,forward.z * leverage };
        }
        else
        {
            avoidance_end = { movevec.x * leverage ,movevec.y * leverage,movevec.z * leverage };
        }
    }
    else
    {
        leverage = 30.0f;
        avoidance_end = { forward.x * leverage ,forward.y * leverage,forward.z * leverage };
    }
    //-----------------------------------------------------------------------------------------//
    //覚醒状態の時の回避アニメーションの設定
    if(is_awakening)model->play_animation(AnimationClips::AwakingAvoidance, false,true);
    //通常状態の時のアニメーションの設定
    else model->play_animation(AnimationClips::Avoidance, false,true);
    //攻撃中かどうかの設定
    is_attack = false;
    //アニメーションの速度
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //回避状態の時の更新関数に切り替える
    player_activity = &Player::AvoidanceUpdate;
}

void Player::TransitionBehindAvoidance()
{
    if (is_just_avoidance_capsul)
    {
        //ロックオンしている敵をスタンさせる
        if (target_enemy != nullptr)
        {
            target_enemy->fSetStun(true, true);
        }
        is_just_avoidance = true;
    }
    else
    {
        if (target_enemy != nullptr)
        {
            //ロックオンしている敵をスタンさせる
            target_enemy->fSetStun(true);
        }
    }
    player_behind_effec->play(effect_manager->get_effekseer_manager(), {position.x,position.y + air_registance_offset_y ,position.z});
    velocity = {};
    //回避中かどうかの設定
    is_avoidance = true;
    //回り込み回避かどうか
    is_behind_avoidance = true;
    //覚醒状態の時の回避アニメーションの設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingAvoidance, false, true);
    //通常状態の時のアニメーションの設定
    else model->play_animation(AnimationClips::Avoidance, false, true);
    //後ろに回り込む座標の取得
    BehindAvoidancePosition();
    //回り込むときのタイマー
    behind_timer = 0;
    //回り込みの補完レート
    behind_late = 0;
    //移動速度の初期化
    velocity = {};
    //攻撃中かどうかの設定
    is_attack = false;
    //アニメーションの速度
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //背後に回り込むときの関数に切り替える
    player_activity = &Player::BehindAvoidanceUpdate;
}

void Player::TransitionChargeInit()
{
    //覚醒状態の時の突進の始まりのアニメーションに設定
   if(is_awakening)model->play_animation(AnimationClips::AwakingChargeInit, false,true);
   //通常状態の時の突進の始まりのアニメーションに設定
   else model->play_animation(AnimationClips::ChargeInit, false,true);
   //攻撃中かどうかの設定
   is_attack = true;
   //突進中かどうかの設定
   is_charge = true;
   //アニメーション速度の設定
    animation_speed = CHARGEINIT_ANIMATION_SPEED;
    //ロックオンしてない場合のターゲットの設定
    charge_point = Math::calc_designated_point(position, forward, 200.0f);
    //加速のレート
    lerp_rate = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //突進の始まりの時の更新関数に切り替える
    player_activity = &Player::ChargeInitUpdate;
}

void Player::TransitionCharge(float blend_seconds)
{
    //ダッシュポストエフェクトをかける
    start_dash_effect = true;
    //覚醒状態の時の突進アニメーションに設定
    if (is_awakening)
    {
        //プレイヤーの攻撃力
        player_attack_power = 1;
        model->play_animation(AnimationClips::AwakingCharge, false, true, blend_seconds);
    }
    //通常状態の時の突進アニメーションに設定
    else
    {
        //プレイヤーの攻撃力
        player_attack_power = 1;
        model->play_animation(AnimationClips::Charge, false, true, blend_seconds);
    }
    //攻撃中かどうかの設定
    is_attack = true;
    //突進中かどうかの設定
    is_charge = true;
    //アニメーションスピードの設定
#if 1
    animation_speed = CHARGE_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.x;
#endif // 0
    //charge_point = Math::calc_designated_point(position, forward, 60.0f);
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //加速のレート
    lerp_rate = 4.0f;
    //攻撃の加速の設定
    //SetAccelerationVelocity();
    //突進中の更新関数に切り替える
    player_activity = &Player::ChargeUpdate;

}

void Player::TransitionAttackType1(float blend_seconds)
{
    //覚醒状態の時の１撃目のアニメーションに設定
    if (is_awakening)
    {
        //プレイヤーの攻撃力
        player_attack_power = 4;
        model->play_animation(AnimationClips::AwakingAttackType1, false, true, blend_seconds);
    }
    //通常状態の時の１撃目のアニメーションに設定
    else
    {
        //プレイヤーの攻撃力
        player_attack_power = 2;
        model->play_animation(AnimationClips::AttackType1, false, true, blend_seconds);
    }
    //攻撃中かどうかの設定
    is_attack = true;
    //アニメーションスピードの設定
#if 1
    animation_speed = ATTACK1_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.y;
#endif // 0
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //加速のレート
    lerp_rate = 4.0f;
    //１撃目の更新関数に切り替える
    player_activity = &Player::AttackType1Update;
}

void Player::TransitionAttackType2(float blend_seconds)
{
    //覚醒状態の時の２撃目のアニメーションに設定
    if (is_awakening)
    {
        //プレイヤーの攻撃力
        player_attack_power = 6;
        model->play_animation(AnimationClips::AwakingAttackType2, false, true, blend_seconds);
    }
    //通常状態の時の２撃目のアニメーションに設定
    else
    {
        //プレイヤーの攻撃力
        player_attack_power = 4;
        model->play_animation(AnimationClips::AttackType2, false, true, blend_seconds);
    }
    //攻撃中かどうかの設定
    is_attack = true;
    //アニメーション速度の設定
#if 1
    animation_speed = ATTACK2_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.z;
#endif // 0
    //攻撃の加速の設定
    //SetAccelerationVelocity();
        //ロックオンしてない場合のターゲットの設定
    charge_point = Math::calc_designated_point(position, forward, 200.0f);
    //加速のレート
    lerp_rate = 2.0f;
    //攻撃の時間
    attack_time = 0;
    //アニメーションをしていいかどうか
    is_update_animation = false;
    //突進中かどうかの設定
    is_charge = true;
    //２撃目の更新関数に切り替える
    player_activity = &Player::AttackType2Update;
}

void Player::TransitionAttackType3(float blend_seconds)
{
    //覚醒状態の時の３撃目のアニメーションに設定
    if (is_awakening)
    {
        //プレイヤーの攻撃力
        player_attack_power = 9;
        model->play_animation(AnimationClips::AwakingAttackType3, false, true, blend_seconds);
    }
    //通常状態の時の３撃目ののアニメーションに設定
    else
    {
        //プレイヤーの攻撃力
        player_attack_power = 5;
        model->play_animation(AnimationClips::AttackType3, false, true, blend_seconds);
    }
    //攻撃中かどうかの設定
    is_attack = true;
    //アニメーション速度の設定
#if 1
    animation_speed = ATTACK3_ANIMATION_SPEED;
#else
    //デバッグ用
    animation_speed = attack_animation_speeds.w;
#endif // 0
    //攻撃の加速の設定
    //SetAccelerationVelocity();
    //加速のレート
    lerp_rate =2.0f;
    //攻撃の時間
    attack_time = 0;
    //アニメーションをしていいかどうか
    is_update_animation = false;
    //突進中かどうかの設定
    is_charge = true;
    //３撃目の更新関数に切り替える
    player_activity = &Player::AttackType3Update;
}

void Player::TransitionSpecialSurge()
{
    //飛行機モードになるアニメーションに設定
    model->play_animation(AnimationClips::IdleWing, true,true);
    //ゲージ消費の突進中に当たった敵の数を初期化
    special_surge_combo_count = 0;
    //ゲージ消費の突進かどうかの設定
    is_special_surge = true;
    //攻撃中かどうかの設定
    is_attack = true;
    //ロックオンしてない場合のターゲットの設定
    charge_point = Math::calc_designated_point(position, forward, 100.0f);
    //ゲージ消費の突進の移動速度を設定
    SpecialSurgeAcceleration();
    //コンボカウントの消費
    combo_count -= 10.0f;
    //コンボカウントの制限
    combo_count = Math::clamp(combo_count, 0.0f, MAX_COMBO_COUNT);
    //ゲージ消費の突進のタイマー
    special_surge_timer = 0.0f;
    //アニメーションスピードの設定
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //ゲージ消費の突進の更新関数に切り替える
    player_activity = &Player::SpecialSurgeUpdate;
}

void Player::TransitionOpportunity()
{
    //ゲージ消費の突進攻撃終了
    is_special_surge = false;
    //攻撃中かどうかの設定
    is_attack = false;
    //隙が生じた時の経過時間をリセット
    special_surge_timer = 0;
    //アニメーションスピードの設定
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //ゲージ消費の突進の隙の更新関数に切り替える
    player_activity = &Player::OpportunityUpdate;
}

void Player::TransitionDamage()
{
    //ダッシュエフェクトの終了
    start_dash_effect = false;
    //攻撃中かどうかの設定
    is_attack = false;
    //覚醒状態の時のダメージアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingDamage, false, true);
    //通常状態の時のアニメーションに設定
    else model->play_animation(AnimationClips::Damage, false, true);
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //ダメージ受けたときの更新関数に切り替える
    player_activity = &Player::DamageUpdate;
}

void Player::TransitionTransformHum()
{
    //人型になるアニメーションに設定
    model->play_animation(AnimationClips::TransformHum, false,true);
    //アニメーション速度の設定
    animation_speed = TRANSFORM_HUM_ANIMATION_SPEED;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //人型になってるときの更新関数に切り替える
    player_activity = &Player::TransformHumUpdate;
}

void Player::TransitionTransformWing()
{
    velocity = {};
    //飛行機モードになるアニメーションに設定
    model->play_animation(AnimationClips::TransformWing, false,true);
    //アニメーション速度の設定
    animation_speed = TRANSFORM_WING_ANIMATION_SPEED;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //飛行機モード中の更新関数に切り替える
    player_activity = &Player::TransformWingUpdate;
}

void Player::TransitionAwaking()
{
    //覚醒状態になるアニメーションに設定
    model->play_animation(AnimationClips::Awaking, false,true);
    //覚醒状態かどうかの設定
    is_awakening = true;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //覚醒状態になる途中の更新関数に切り替える
    player_activity = &Player::AwakingUpdate;
}

void Player::TransitionInvAwaking()
{
    //通常状態に戻るアニメーションに設定
    model->play_animation(AnimationClips::InvAwaking, false,true);
    //覚醒状態かどうかの設定
    is_awakening = false;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //通常状態に戻ってるときの更新関数に切り替える
    player_activity = &Player::InvAwakingUpdate;

}

void Player::TransitionWingDashStart()
{
}

void Player::TransitionWingDashIdle()
{
}

void Player::TransitionWingDashEnd()
{
}
void Player::TransitionDie()
{
    condition_state = ConditionState::Dye;
    //攻撃中かどうかの設定
    is_attack = false;
    velocity = {};
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //覚醒状態の時のダメージアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingDie, false, true);
    //通常状態の時のアニメーションに設定
    else model->play_animation(AnimationClips::Die, false, true);
    //更新関数に切り替え
    player_activity = &Player::DieUpdate;
    if (GameFile::get_instance().get_vibration())game_pad->set_vibration(1.0f, 1.0f, 1.0f);
}

void Player::TransitionDying()
{
    //攻撃中かどうかの設定
    is_attack = false;
    velocity = {};
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //覚醒状態の時のダメージアニメーションに設定
    if (is_awakening)model->play_animation(AnimationClips::AwakingDying, true, true);
    //通常状態の時のアニメーションに設定
    else model->play_animation(AnimationClips::Dying, true, true);
    //更新関数に切り替え
    player_activity = &Player::DyingUpdate;

}

void Player::TransitionNamelessMotionIdle()
{
    PostEffect::clear_post_effect();
    //攻撃中かどうかの設定
    is_attack = false;
    velocity = {};
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //アニメーションに設定
    model->play_animation(AnimationClips::NamelessMotionIdle, false, true);
    //更新関数に切り替え
    player_activity = &Player::NamelessMotionIdleUpdate;

}

void Player::TransitionStartMothin()
{
    //攻撃中かどうかの設定
    is_attack = false;
    velocity = {};
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //アニメーションに設定
    //model->play_animation(AnimationClips::StartMothin, false, true);
    //更新関数に切り替え
    player_activity = &Player::StartMothinUpdate;

}

void Player::TransitionNamelessMotion()
{
    PostEffect::wipe_effect(0.15f);
    //クリア用モーションが始まったらからtrueにする
    is_start_cleear_motion = true;
    //攻撃中かどうかの設定
    is_attack = false;
    velocity = {};
    //アニメーションをしていいかどうか
    is_update_animation = true;
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //アニメーションに設定
    model->play_animation(AnimationClips::NamelessMotion, false, true);
    //更新関数に切り替え
    player_activity = &Player::NamelessMotionUpdate;
}

void Player::TransitionStageMove()
{

    //移動のアニメーションにする(回避)
    model->play_animation(AnimationClips::Charge, false);
    //アニメーション速度の設定
    animation_speed = 1.0f;
    //アニメーションをしていいかどうか
    is_update_animation = true;
    velocity = {};
    //通常状態に戻ってるときの更新関数に切り替える
    player_activity = &Player::StageMoveUpdate;
    during_clear = true;
}

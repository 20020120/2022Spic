--中ボスのLua関数を定義

gTourRotSpeed=2  -- 回転速度
gRotRadian=0.0     -- 回転角
gTourLength=150.0  -- 回転の半径




LaserReadyTable={ -- レーザー発射準備の変数
PointerLength=0.0, -- レーザーポインターの長さ
LengthGlowPower=10.0, -- レーザーを伸ばす時間の倍率
Timer=0.0,         -- 制御時間
NextStateTime=6.0
}


function fTourMove(elapsedTime_)  -- 回転を制御
gRotRadian=gRotRadian+(elapsedTime_*gTourRotSpeed)
local posX=math.sin(gRotRadian)*gTourLength 
local posZ=math.cos(gRotRadian)*gTourLength
return posZ,posX
end


-- ビームを撃つ準備の初期化
function fTourLaserReadyInit()
    -- body
    LaserReadyTable.Timer=0.0
    LaserReadyTable.PointerLength=0.0
    return true
end

function fTourLaserReady(elapsedTime_) -- ビームを撃つ準備をする関数
    
    -- ステートを移行してもいいかどうか
    local isNextState=false
    -- ポインターの透明度
    local pointerAlpha=0.0
    
    -- タイマーを更新
    LaserReadyTable.Timer=LaserReadyTable.Timer+elapsedTime_

    if LaserReadyTable.PointerLength>=1.0 then
      -- ポインターの長さが１を超えたら更新しない
        LaserReadyTable.PointerLength=1.0  
     else
      -- レーザーポインターの長さを伸ばす
      LaserReadyTable.PointerLength=LaserReadyTable.PointerLength+(elapsedTime_*LaserReadyTable.LengthGlowPower)
    end

    -- タイマーが規定値以上ならステートを移行する
    if LaserReadyTable.Timer>=LaserReadyTable.NextStateTime then
     isNextState=true
    else
        -- 残り時間に応じて点滅速度を変える
        if LaserReadyTable.NextStateTime - LaserReadyTable.Timer > 3.0 then
            -- 遅い
            pointerAlpha= LaserReadyTable.Timer*7 %2        
        else
            -- 早い
            pointerAlpha= LaserReadyTable.Timer*20 %2

        end
    end

   local length=LaserReadyTable.PointerLength;

    return isNextState,pointerAlpha,length
end

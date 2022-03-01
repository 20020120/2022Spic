--中ボスのLua関数を定義

gTourRotSpeed=10  -- 回転速度
gRotRadian=0.0     -- 回転角
gTourLength=150.0  -- 回転の半径

LaserReadyTable={ -- レーザー発射準備の変数


}


function fTourMove(elapsedTime_)  -- 回転を制御
gRotRadian=gRotRadian+(elapsedTime_)
local posX=math.sin(gRotRadian)*gTourLength 
local posZ=math.cos(gRotRadian)*gTourLength
return posX,posZ
end

function fTourLaserReady(elapsedTIme) -- ビームを撃つ準備をする関数
    
    
end
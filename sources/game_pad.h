#pragma once

using GamePadButton = unsigned int;

// ゲームパッド
class GamePad
{ 
public:
	static const GamePadButton BTN_UP             = (1 << 0);
	static const GamePadButton BTN_RIGHT          = (1 << 1);
	static const GamePadButton BTN_DOWN           = (1 << 2);
	static const GamePadButton BTN_LEFT           = (1 << 3);
	static const GamePadButton BTN_A              = (1 << 4);
	static const GamePadButton BTN_B              = (1 << 5);
	static const GamePadButton BTN_X              = (1 << 6);
	static const GamePadButton BTN_Y              = (1 << 7);
	static const GamePadButton BTN_START          = (1 << 8);
	static const GamePadButton BTN_BACK           = (1 << 9);
	static const GamePadButton BTN_LEFT_THUMB     = (1 << 10);
	static const GamePadButton BTN_RIGHT_THUMB    = (1 << 11);
	static const GamePadButton BTN_LEFT_SHOULDER  = (1 << 12);
	static const GamePadButton BTN_RIGHT_SHOULDER = (1 << 13);
	static const GamePadButton BTN_LEFT_TRIGGER   = (1 << 14);
	static const GamePadButton BTN_RIGHT_TRIGGER  = (1 << 15);
public:
	GamePad() {}
	~GamePad() {}
	// 更新
	/*呼び出しはFrameworkのみ*/
	void update(float elapsedTime);
	// スロット設定
	void set_slot(int slot) { this->slot = slot; }
	// ボタン入力状態の取得
	GamePadButton get_button() const { return button_state[0]; }
	// ボタン押下状態の取得
	GamePadButton get_button_down() const { return button_down; }
	// ボタン押上状態の取得
	GamePadButton get_button_up() const { return button_up; }
	// 左スティックX軸入力状態の取得
	float get_axis_LX() const { return axisLx; }
	// 左スティックY軸入力状態の取得
	float get_axis_LY() const { return axisLy; }
	// 右スティックX軸入力状態の取得
	float get_axis_RX() const { return axisRx; }
	// 右スティックY軸入力状態の取得
	float get_axis_RY() const { return axisRy; }
	// 左トリガー入力状態の取得
	float get_trigger_L() const { return triggerL; }
	// 右トリガー入力状態の取得
	float get_trigger_R() const { return triggerR; }
	// バイブレーション
	bool set_vibration(float R, float L, float StopTime);
	// 操作有効、無効
	void operation_disablement() { operable = false; }
	void operation_activation() { operable = true; }
private:
	GamePadButton		button_state[2] = { 0 };
	GamePadButton		button_down = 0;
	GamePadButton		button_up = 0;
	float				axisLx = 0.0f;
	float				axisLy = 0.0f;
	float				axisRx = 0.0f;
	float				axisRy = 0.0f;
	float				triggerL = 0.0f;
	float				triggerR = 0.0f;
	int					slot = 0;

	float stack_stop_vib_sec;
	float vib_stack_sec;

	bool operable = true;
};
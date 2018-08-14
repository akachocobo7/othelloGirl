#include "DxLib.h"

using ll = long long;

ll black_board;
ll white_board;

void init_board_draw() {
	SetDrawScreen(DX_SCREEN_BACK);

	DrawBox(50, 50, 580, 580, GetColor(255, 255, 255), FALSE);
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			DrawBox(51 + x * 66, 51 + y * 66, 117 + x * 66, 117 + y * 66, GetColor(255, 255, 255), FALSE);
			DrawBox(52 + x * 66, 52 + y * 66, 116 + x * 66, 116 + y * 66, GetColor(0, 94, 21), TRUE);
		}
	}

	ScreenFlip();
}

void board_draw() {
	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	DrawBox(50, 50, 581, 581, GetColor(255, 255, 255), FALSE);
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			DrawBox(51 + x * 66, 51 + y * 66, 118 + x * 66, 118 + y * 66, GetColor(255, 255, 255), FALSE);
			DrawBox(52 + x * 66, 52 + y * 66, 117 + x * 66, 117 + y * 66, GetColor(0, 94, 21), TRUE);
			if ((black_board >> (63 - (y * 8 + x))) & 1) {
				DrawCircle(52 + x * 66 + 32, 52 + y * 66 + 32, 30, GetColor(0, 0, 0), TRUE);
			}
			if ((white_board >> (63 - (y * 8 + x))) & 1) {
				DrawCircle(52 + x * 66 + 32, 52 + y * 66 + 32, 30, GetColor(255, 255, 255), TRUE);
			}
		}
	}

	ScreenFlip();
}

void girl_draw(int p) {
	SetDrawScreen(DX_SCREEN_BACK);

	DrawGraph(900, 450, p, FALSE);

	ScreenFlip();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1280, 720, 32);	// 画面のサイズ
	ChangeWindowMode(TRUE);			// ウィンドウモードにする
	SetBackgroundColor(255, 255, 255);

	if (DxLib_Init() == -1) return -1;		// ＤＸライブラリ初期化処理


	init_board_draw();

	black_board = 0x0000001008000000;
	white_board = 0x0000000810000000;
	board_draw();

	int p = LoadGraph("picture\\1.png");
	girl_draw(p);
	
	WaitKey();				// キー入力待ち

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

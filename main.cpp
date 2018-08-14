#include "DxLib.h"
#include "draw.h"

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1280, 720, 32);	// 画面のサイズ
	ChangeWindowMode(TRUE);			// ウィンドウモードにする
	if (DxLib_Init() == -1) return -1;		// ＤＸライブラリ初期化処理


	init_board_draw();

	int p = LoadGraph("picture\\1.png");
	girl_draw(p);

	WaitKey();				// キー入力待ち

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

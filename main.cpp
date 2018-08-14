#include "DxLib.h"

using ll = long long;

ll black_board;
ll white_board;
bool turn_black = true;

void draw_board(int p) {
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

	DrawGraph(900, 450, p, FALSE);

	ScreenFlip();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1280, 720, 32);	// 画面のサイズ
	ChangeWindowMode(TRUE);			// ウィンドウモードにする
	SetBackgroundColor(255, 255, 255);

	if (DxLib_Init() == -1) return -1;		// ＤＸライブラリ初期化処理

	// ユーザーが×ボタンを押しても自動的に終了しないようにする
	SetWindowUserCloseEnableFlag(FALSE);

	int girl_pic = LoadGraph("picture\\1.png");

	black_board = 0x0000001008000000;
	white_board = 0x0000000810000000;

	draw_board(girl_pic);
	
	for (;;) {
		if (GetMouseInput() & MOUSE_INPUT_LEFT) {
			int mouse_x, mouse_y;
			GetMousePoint(&mouse_x, &mouse_y);

			[&] {
			for (int y = 0; y < 8; y++) {
				for (int x = 0; x < 8; x++) {
					if (52 + x * 66 <= mouse_x && mouse_x < 117 + x * 66 && 52 + y * 66 <= mouse_y && mouse_y < 117 + y * 66) {
						if ((black_board & (1LL << (8 * (7 - y) + 7 - x))) || (white_board & (1LL << (8 * (7 - y) + 7 - x)))) continue;
						if (turn_black) {
							black_board |= 1LL << (8 * (7 - y) + 7 - x);
						}
						else {
							white_board |= 1LL << (8 * (7 - y) + 7 - x);
						}
						turn_black = !turn_black;

						draw_board(girl_pic);

						WaitTimer(200);

						return;
					}
				}
			}}();
		}

		// ×ボタンかエンターキーを押すと終了
		if (GetWindowUserCloseFlag(TRUE) || CheckHitKey(KEY_INPUT_RETURN)) {
			break;
		}
	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;
}

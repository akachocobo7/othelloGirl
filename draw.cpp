#include "DxLib.h"
#include "draw.h"

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

void girl_draw(int p) {
	SetDrawScreen(DX_SCREEN_BACK);

	DrawGraph(900, 450, p, FALSE);

	ScreenFlip();
}
#include "DxLib.h"
#include <intrin.h>

using ll = long long;
using namespace std;

ll black_board;
ll white_board;
int put_count;
int font_title;
int girl_picture;
const int BLACK_STONE = 1;
const int WHITE_STONE = -1;
bool turn_white = false;
bool AI_color_white = true;
bool now_playing_game = false;

void draw_board();
void init_board();
ll can_put(ll &mov);
void put_stone(ll &mov, ll &rev);
ll count_stone(const int &stone);
void end_game();
int mouse_input_in_game();
void draw_title();
int mouse_input_in_title();
void title();
void othello_AI();
int game();

void draw_board() {
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

	DrawGraph(900, 450, girl_picture, FALSE);

	DrawBoxAA(900, 200, 1000, 300, GetColor(255, 192, 203), TRUE);
	DrawString(910, 225, "終了", GetColor(0, 0, 0));

	ScreenFlip();
}

void init_board() {
	black_board = 0x0000000810000000;
	white_board = 0x0000001008000000;

	put_count = 0;
	turn_white = false;

	draw_board();
}

ll can_put(ll &mov) {
	if ((black_board | white_board) & mov)return 0;		//着手箇所が空白で無い場合


	ll rev = 0, r, mask;

	if (turn_white) {						//白が打つ場合
		r = 0;
		mask = (mov >> 1) & 0x7f7f7f7f7f7f7f7f;	//右方向へ返せるかを調べる
		while (mask != 0 && (mask & black_board) != 0) {	//黒石が連続する間
			r |= mask;
			mask = (mask >> 1) & 0x7f7f7f7f7f7f7f7f;
		}
		if (mask & white_board)rev |= r;				//白石があれば返す位置を確定する

		r = 0;
		mask = (mov >> 8) & 0x00ffffffffffffff;	//下方向へ返せるかを調べる
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask >> 8) & 0x00ffffffffffffff;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov << 1) & 0xfefefefefefefefe;	//左方向へ返せるかを調べる
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask << 1) & 0xfefefefefefefefe;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov << 8) & 0xffffffffffffff00;	//上方向へ返せるかを調べる
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask << 8) & 0xffffffffffffff00;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov >> 9) & 0x007f7f7f7f7f7f7f;	//右下方向へ返せるかを調べる
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask >> 9) & 0x007f7f7f7f7f7f7f;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov >> 7) & 0x00fefefefefefefe;	//左下方向へ返せるかを調べる
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask >> 7) & 0x00fefefefefefefe;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov << 9) & 0xfefefefefefefe00;	//左上方向へ返せるかを調べる
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask << 9) & 0xfefefefefefefe00;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov << 7) & 0x7f7f7f7f7f7f7f00;	//右上方向へ返せるかを調べる
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask << 7) & 0x7f7f7f7f7f7f7f00;
		}
		if (mask & white_board)rev |= r;
	}
	else {										//黒が打つ場合
		r = 0;
		mask = (mov >> 1) & 0x7f7f7f7f7f7f7f7f;	//右方向へ返せるかを調べる
		while (mask != 0 && (mask & white_board) != 0) {	//白石が連続する間
			r |= mask;
			mask = (mask >> 1) & 0x7f7f7f7f7f7f7f7f;
		}
		if (mask & black_board)rev |= r;				//黒石があれば返す位置を確定する

		r = 0;
		mask = (mov >> 8) & 0x00ffffffffffffff;	//下方向へ返せるかを調べる
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask >> 8) & 0x00ffffffffffffff;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov << 1) & 0xfefefefefefefefe;	//左方向へ返せるかを調べる
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask << 1) & 0xfefefefefefefefe;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov << 8) & 0xffffffffffffff00;	//上方向へ返せるかを調べる
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask << 8) & 0xffffffffffffff00;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov >> 9) & 0x007f7f7f7f7f7f7f;	//右下方向へ返せるかを調べる
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask >> 9) & 0x007f7f7f7f7f7f7f;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov >> 7) & 0x00fefefefefefefe;	//左下方向へ返せるかを調べる
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask >> 7) & 0x00fefefefefefefe;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov << 9) & 0xfefefefefefefe00;	//左上方向へ返せるかを調べる
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask << 9) & 0xfefefefefefefe00;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov << 7) & 0x7f7f7f7f7f7f7f00;	//右上方向へ返せるかを調べる
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask << 7) & 0x7f7f7f7f7f7f7f00;
		}
		if (mask & black_board)rev |= r;
	}

	return rev;
}

void put_stone(ll &mov, ll &rev) {
	if (turn_white) {
		white_board ^= mov | rev;
		black_board ^= rev;
	}
	else {
		black_board ^= mov | rev;
		white_board ^= rev;
	}

	turn_white = !turn_white;
}

ll count_stone(const int &stone) {
	return __popcnt64((stone == BLACK_STONE) ? black_board : white_board);
}

void end_game() {
	// 両プレイヤーが置けなくなったらゲーム終了
	ll mov = 0x8000000000000000;
	for (int i = 0; i < 64; i++) {
		if (can_put(mov))return;
		mov = (mov >> 1) & 0x7fffffffffffffff;
	}
	turn_white = !turn_white;
	mov = 0x8000000000000000;
	for (int i = 0; i < 64; i++) {
		if (can_put(mov)) { turn_white = !turn_white;  return; }
		mov = (mov >> 1) & 0x7fffffffffffffff;
	}


	ll b = count_stone(BLACK_STONE);
	ll w = count_stone(WHITE_STONE);

	if (b > w) {
		MessageBox(NULL, "黒の勝ちです", "othelloGirl", MB_OK);
		// DrawString(700, 150, "黒の勝ちです", GetColor(0, 0, 0));
	}
	else if (b < w) {
		MessageBox(NULL, "白の勝ちです", "othelloGirl", MB_OK);
		// DrawString(700, 150, "白の勝ちです", GetColor(0, 0, 0));
	}
	else {
		MessageBox(NULL, "引き分けです", "othelloGirl", MB_OK);
		// DrawString(700, 150, "引き分けです", GetColor(0, 0, 0));
	}
}

int mouse_input_in_game() {
	if (GetMouseInput() & MOUSE_INPUT_LEFT) {
		int mouse_x, mouse_y;
		GetMousePoint(&mouse_x, &mouse_y);

		
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				if (52 + x * 66 <= mouse_x && mouse_x < 117 + x * 66 && 52 + y * 66 <= mouse_y && mouse_y < 117 + y * 66) {
					ll mov = 1LL << (8 * (7 - y) + 7 - x), rev = can_put(mov);
					if (rev) {
						put_stone(mov, rev);
						put_count++;
						printfDx("%d", put_count);
						draw_board();
						end_game();
						return 1;
					}
					return 0;
				}
			}
		}

		if (900 <= mouse_x && mouse_x < 1000 && 200 <= mouse_y && mouse_y < 300) {
			now_playing_game = false;
		}
	}

	return 0;
}

void draw_title() {
	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	DrawStringToHandle(100, 100, "タイトル", GetColor(0, 0, 0), font_title);

	DrawBoxAA(300, 300, 400, 400, GetColor(255, 192, 203), TRUE);
	DrawString(305, 350, "プレイヤー:黒", GetColor(0, 0, 0));
	DrawBoxAA(300, 500, 400, 600, GetColor(255, 192, 203), TRUE);
	DrawString(305, 550, "プレイヤー:白", GetColor(0, 0, 0));

	DrawGraph(900, 450, 0, FALSE);

	ScreenFlip();
}

int mouse_input_in_title() {
	if (GetMouseInput() & MOUSE_INPUT_LEFT) {
		int mouse_x, mouse_y;
		GetMousePoint(&mouse_x, &mouse_y);

		if (300 <= mouse_x && mouse_x < 400 && 300 <= mouse_y && mouse_y < 400) {
			AI_color_white = true;
			now_playing_game = true;
			init_board();
			return 1;
		}
		else if (300 <= mouse_x && mouse_x < 400 && 500 <= mouse_y && mouse_y < 600) {
			AI_color_white = false;
			now_playing_game = true;
			init_board();
			othello_AI();
			return 1;
		}
	}

	return 0;
}

void title() {
	draw_title();

	for (;;) {
		if (mouse_input_in_title()) {
			if (game() == -1) {
				return;
			}
			draw_title();
		}

		// ×ボタンかエンターキーを押すと終了
		if (GetWindowUserCloseFlag(TRUE) || CheckHitKey(KEY_INPUT_RETURN)) {
			return;
		}
	}
}

void othello_AI() {
	ll mov = 0x8000000000000000;

	int i;
	for (i = 0; i < 64; i++) {
		ll rev = can_put(mov);
		if (rev) {
			put_stone(mov, rev);
			put_count++;
			printfDx("%d", put_count);
			draw_board();
			end_game();
			break;
		}
		mov = (mov >> 1) & 0x7fffffffffffffff;
	}
	if (i == 64) {
		turn_white = !turn_white;
		return;
	}

	mov = 0x8000000000000000;
	for (i = 0; i < 64; i++) {
		if (can_put(mov))return;
		mov = (mov >> 1) & 0x7fffffffffffffff;
	}
	if (i == 64) {
		turn_white = !turn_white;
		othello_AI();
	}
}

int game() {
	for (;;) {
		if (mouse_input_in_game()) {
			printfDx("aiueo");
			othello_AI();
		}
		if (!now_playing_game) {
			return 0;
		}

		// ×ボタンかエンターキーを押すと終了
		if (GetWindowUserCloseFlag(TRUE) || CheckHitKey(KEY_INPUT_RETURN)) {
			return -1;
		}
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1280, 720, 32);	// 画面のサイズ
	ChangeWindowMode(TRUE);			// ウィンドウモードにする
	SetBackgroundColor(229, 182, 203);

	if (DxLib_Init() == -1) return -1;		// ＤＸライブラリ初期化処理

	// ユーザーが×ボタンを押しても自動的に終了しないようにする
	SetWindowUserCloseEnableFlag(FALSE);

	girl_picture = LoadGraph("picture\\1.png");
	font_title = CreateFontToHandle(NULL, 70, 5);

	title();

	DeleteFontToHandle(font_title);

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;
}

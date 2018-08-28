#include "DxLib.h"
#include <intrin.h>
#include <vector>

using ll = long long;
using namespace std;

ll black_board;
ll white_board;
int put_count;
int font_title;
int girl_picture;
const int BLACK_STONE = 1;
const int WHITE_STONE = -1;
bool is_white_turn = false;
bool is_AI_color_white = true;
bool now_playing_game = false;
int INF = 1e9;
int search_depth;
vector<ll> stone_empty_place;
vector<bool> stone_empty_place_used;

const int SEARCH_LV = 5;
const int FINAL_STAGE_NUM = 48;
const int value_place[64] = {
	150, -10, 10,  5,  5, 10, -10, 150,
	-10,-200,  2,  1,  1,  2,-200, -10,
	 10,   2,  0,  1,  1,  0,   2,  10,
	  5,   1,  1,  0,  0,  1,   1,   5,
	  5,   1,  1,  0,  0,  1,   1,   5,
	 10,   2,  0,  1,  1,  0,   2,  10,
	-10,-200,  2,  1,  1,  2,-200, -10,
	150, -10, 10,  5,  5, 10, -10, 150
};

void draw_board();
void init_board();
ll can_put(ll &mov);
void put_stone(ll &mov, ll &rev);
void ando(ll &mov, ll &rev);
int count_stone(const int &stone);
void end_game();
int mouse_input_in_game();
void draw_title();
int mouse_input_in_title();
void title();
void othello_AI();
int game();
ll nega_max(int depth, bool is_put_before_this, int alpha, int beta);
int board_value();
int value_stone_place();
int value_can_put();
int value_fixed_stone();

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

	DrawBoxAA(900, 200, 1000, 300, GetColor(255, 255, 255), TRUE);
	DrawString(910, 225, "終了", GetColor(0, 0, 0));

	ScreenFlip();
}

void init_board() {
	black_board = 0x0000000810000000;
	white_board = 0x0000001008000000;

	put_count = 0;
	is_white_turn = false;

	draw_board();
}

ll can_put(ll &mov) {
	if ((black_board | white_board) & mov)return 0;		//着手箇所が空白でない場合


	ll rev = 0, r, mask;

	if (is_white_turn) {						//白が打つ場合
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
	if (is_white_turn) {
		white_board ^= mov | rev;
		black_board ^= rev;
	}
	else {
		black_board ^= mov | rev;
		white_board ^= rev;
	}

	is_white_turn = !is_white_turn;
}

void ando(ll &mov, ll &rev) {
	is_white_turn = !is_white_turn;

	if (is_white_turn) {
		white_board ^= mov | rev;
		black_board ^= rev;
	}
	else {
		black_board ^= mov | rev;
		white_board ^= rev;
	}
}

int count_stone(const int &stone) {
	return __popcnt64((stone == BLACK_STONE) ? black_board : white_board);
}

void end_game() {
	// 両プレイヤーが置けなくなったらゲーム終了
	ll mov = 0x8000000000000000;
	for (int i = 0; i < 64; i++) {
		if (can_put(mov))return;
		mov = (mov >> 1) & 0x7fffffffffffffff;
	}
	is_white_turn = !is_white_turn;
	mov = 0x8000000000000000;
	for (int i = 0; i < 64; i++) {
		if (can_put(mov)) { is_white_turn = !is_white_turn;  return; }
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

	PlaySoundFile("sound\\put.mp3", DX_PLAYTYPE_BACK);
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
						// printfDx("%d", put_count);
						PlaySoundFile("sound\\put.mp3", DX_PLAYTYPE_BACK);
						draw_board();
						return 1;
					}
					return 0;
				}
			}
		}

		if (900 <= mouse_x && mouse_x < 1000 && 200 <= mouse_y && mouse_y < 300) {
			PlaySoundFile("sound\\reset.mp3", DX_PLAYTYPE_BACK);
			now_playing_game = false;
		}
	}

	return 0;
}

void draw_title() {
	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	DrawStringToHandle(100, 100, "タイトル", GetColor(0, 0, 0), font_title);

	DrawBoxAA(300, 300, 400, 400, GetColor(255, 255, 255), TRUE);
	DrawString(305, 350, "プレイヤー:黒", GetColor(0, 0, 0));
	DrawBoxAA(300, 500, 400, 600, GetColor(255, 255, 255), TRUE);
	DrawString(305, 550, "プレイヤー:白", GetColor(0, 0, 0));

	DrawGraph(900, 450, 0, FALSE);

	ScreenFlip();
}

int mouse_input_in_title() {
	if (GetMouseInput() & MOUSE_INPUT_LEFT) {
		int mouse_x, mouse_y;
		GetMousePoint(&mouse_x, &mouse_y);

		if (300 <= mouse_x && mouse_x < 400 && 300 <= mouse_y && mouse_y < 400) {
			is_AI_color_white = true;
			now_playing_game = true;
			PlaySoundFile("sound\\start.mp3", DX_PLAYTYPE_BACK);
			init_board();
			return 1;
		}
		else if (300 <= mouse_x && mouse_x < 400 && 500 <= mouse_y && mouse_y < 600) {
			is_AI_color_white = false;
			now_playing_game = true;
			PlaySoundFile("sound\\start.mp3", DX_PLAYTYPE_BACK);
			init_board();
			Sleep(1200);
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
	ll mov;

	// 打てないならパス
	for (mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (can_put(mov)) break;
	}
	if (!mov) return;

	stone_empty_place.clear();
	for (mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (!((black_board | white_board) & mov)) {
			stone_empty_place.push_back(mov);
		}
	}
	stone_empty_place_used.assign(stone_empty_place.size(), false);

	mov = nega_max(search_depth = ((put_count < FINAL_STAGE_NUM)? SEARCH_LV : 60 - put_count), TRUE, -INF, INF);

	ll rev = can_put(mov);
	put_stone(mov, rev);
	put_count++;
	PlaySoundFile("sound\\put.mp3", DX_PLAYTYPE_BACK);
	draw_board();

	// 相手が打てないなら、もう一度
	for (mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (can_put(mov))return;
	}
	is_white_turn = !is_white_turn;
	othello_AI();
}

ll nega_max(int depth, bool is_put_before_this, int alpha, int beta) {
	if (depth == 0) return -board_value();

	int max = alpha;
	bool is_put = false;
	ll best_mov;

	for (int i = 0; i < stone_empty_place.size(); i++) {
		if (stone_empty_place_used[i]) continue;

		ll mov = stone_empty_place[i];
		ll rev = can_put(mov);
		if (rev) {
			is_put = true;
			put_stone(mov, rev);
			stone_empty_place_used[i] = true;
			int tmp = -nega_max(depth - 1, TRUE, -beta, -max);
			stone_empty_place_used[i] = false;
			ando(mov, rev);

			if (tmp >= beta) return tmp;
			if (tmp > max) {
				max = tmp;
				best_mov = mov;
			}
		}
	}

	if (is_put) {
		if (depth == search_depth) return best_mov;
		return max;
	}
	else if (!is_put_before_this) return -board_value();
	else {
		is_white_turn = !is_white_turn;
		int tmp = -nega_max(depth - 1, FALSE, -beta, -max);
		is_white_turn = !is_white_turn;
		return tmp;
	}
}

int board_value() {
	int value = 0;
	const int VSP = 1, VCP = 40, VFS = 45;	// 評価値の重み

	if (put_count >= FINAL_STAGE_NUM) {
		value += count_stone(WHITE_STONE) - count_stone(BLACK_STONE);
	}
	else {
		value += value_stone_place() * VSP;
		value += value_can_put() * VCP;
		value += value_fixed_stone() *  VFS;
	}

	if (!is_AI_color_white) value = -value;		// AIが黒なら反転

	return value;
}

int value_stone_place() {
	int value = 0, c = 0;

	if (is_AI_color_white) {
		if (__popcnt64(white_board) >= 1) c = 0;		// 全滅対策
		else if (__popcnt64(black_board) == 0) c = -50000;		// 相手を全滅させたら評価値大アップ
		else c = 50000;
	}
	else {
		if (__popcnt64(black_board) >= 1) c = 0;
		else if (__popcnt64(white_board) == 0) c = 50000;
		else c = -50000;
	}

	ll b = black_board, w = white_board;
	for (int i = 63; i >= 0; i--) {
		value += (b & 1) * value_place[i];
		value -= (w & 1) * value_place[i];

		b >>= 1;
		w >>= 1;
	}

	return -(value + c);
}

int value_can_put() {
	int value = 0;

	for (ll mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (can_put(mov)) value++;
	}

	if (is_white_turn) return value;
	else return -value;
}

int value_fixed_stone() {
	ll i;
	int value = 0;

	if ((black_board | white_board) & 0x8000000000000000) {
		if (black_board & 0x8000000000000000) {
			i = 0x8000000000000000;
			while (black_board & i) {
				value += 1;
				i = (i >> 1) & 0x7fffffffffffffff;
			}
			i = 0x8000000000000000;
			while (black_board & i) {
				value += 1;
				i = (i >> 8) & 0x00ffffffffffffff;
			}
		}
		else {
			i = 0x8000000000000000;
			while (white_board & i) {
				value -= 1;
				i = (i >> 1) & 0x7fffffffffffffff;
			}
			i = 0x8000000000000000;
			while (white_board & i) {
				value -= 1;
				i = (i >> 8) & 0x00ffffffffffffff;
			}
		}
	}
	if ((black_board | white_board) & 0x0100000000000000) {
		if (black_board & 0x0100000000000000) {
			i = 0x0100000000000000;
			while (black_board & i) {
				value += 1;
				i = (i << 1) & 0xfffffffffffffffe;
			}
			i = 0x0100000000000000;
			while (black_board & i) {
				value += 1;
				i = (i >> 8) & 0x00ffffffffffffff;
			}
		}
		else {
			i = 0x0100000000000000;
			while (white_board & i) {
				value -= 1;
				i = (i << 1) & 0xfffffffffffffffe;
			}
			i = 0x0100000000000000;
			while (white_board & i) {
				value -= 1;
				i = (i >> 8) & 0x00ffffffffffffff;
			}
		}
	}
	if ((black_board | white_board) & 0x0000000000000080) {
		if (black_board & 0x0000000000000080) {
			i = 0x0000000000000080;
			while (black_board & i) {
				value += 1;
				i = (i >> 1) & 0x7fffffffffffffff;
			}
			i = 0x0000000000000080;
			while (black_board & i) {
				value += 1;
				i = (i << 8) & 0xffffffffffffff00;
			}
		}
		else {
			i = 0x0000000000000080;
			while (white_board & i) {
				value -= 1;
				i = (i >> 1) & 0x7fffffffffffffff;
			}
			i = 0x0000000000000080;
			while (white_board & i) {
				value -= 1;
				i = (i << 8) & 0xffffffffffffff00;
			}
		}
	}
	if ((black_board | white_board) & 0x0000000000000001) {
		if (black_board & 0x0000000000000001) {
			i = 0x0000000000000001;
			while (black_board & i) {
				value += 1;
				i = (i << 1) & 0xfffffffffffffffe;
			}
			i = 0x0000000000000001;
			while (black_board & i) {
				value += 1;
				i = (i << 8) & 0xffffffffffffff00;
			}
		}
		else {
			i = 0x0000000000000001;
			while (white_board & i) {
				value -= 1;
				i = (i << 1) & 0xfffffffffffffffe;
			}
			i = 0x0000000000000001;
			while (white_board & i) {
				value -= 1;
				i = (i << 8) & 0xffffffffffffff00;
			}
		}
	}

	return -value;
}

int game() {
	Sleep(500);

	for (;;) {
		if (mouse_input_in_game()) {
			othello_AI();
			Sleep(200);
			end_game();
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
	SetBackgroundColor(255, 192, 203);

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

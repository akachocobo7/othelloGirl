#include "DxLib.h"
#include <intrin.h>
#include <vector>
#include <random>
#include <tuple>


using ll = long long;
using namespace std;


// フォント
int font_board_num;

// 画像
int girl_picture;
int flower_flame;
int title_picture;
int black_turn_picture;
int white_turn_picture;
int reset_picture;
int t_start;
int t_thinking;
int t_put1;
int t_put2;
int t_put3;
int t_no_put;
int t_win;
int t_lose;
int t_draw;

// 女の子のセリフ
enum girl_status {
	start,
	thinking,
	put,
	no_put,
	win,
	lose,
	draw
};
girl_status girl_text;

// 音
int start_sound;
int reset_sound;
int put_sound;
int end_sound;

// 乱数
random_device rnd;
mt19937 mt(rnd());
uniform_int_distribution<> rnd3(1, 3); // 1から3までの整数をランダムに

ll black_board;
ll white_board;
int put_count;
bool is_white_turn = false;
bool is_AI_color_white = true;
bool now_playing_game = false;
int search_depth;
vector<ll> stone_empty_place;
vector<bool> stone_empty_place_used;
char put_text[30] = "女の子が打った場所: ";

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int BOARD_X = 150;
const int BOARD_Y = 100;
const int BLACK_STONE = 1;
const int WHITE_STONE = -1;
constexpr const int INF = (1 << 30);
const int SEARCH_LV = 7;			// 探索する手数
const int FINAL_STAGE_NUM = 43;		// 終盤が始まる手数
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

void draw_board();	// 盤面の描画
void init_board();	// オセロを初期化
ll can_put(const ll &mov);	// movの場所に石を置いて、石が返った位置にbitを立てて返す
void put_stone(const ll &mov, const ll &rev);	// movで指定した場所に石を置き、revで指定した場所の石をひっくり返す
void ando(const ll &mov, const ll &rev);	// 一手戻す
int count_stone(const int &stone);	// 黒（または白）石の数を数える
void end_game();	// ゲームの終了判定と終了処理
int mouse_input_in_game();	// プレイ中にマウスをクリックしたときの処理
void draw_title();	// タイトル画面の描画
int mouse_input_in_title();	// タイトル画面でマウスをクリックしたときの処理
void title();	// タイトルでの処理
void othello_AI();	// AI
int play_game();	// ゲーム中の処理
ll nega_max(const int depth, const bool is_put_before_this, const int alpha, const int beta);	// nega_max探索
int board_value();	// 盤面の価値を返す
int value_stone_place();	// オセロの石の位置によって価値を計算
int value_can_put();	// 石の置ける位置の数によって価値を計算
int value_fixed_stone();	// 確定石の数によって価値を計算
ll speed_preferred_serch(const int depth, const bool is_put_before_this, const int alpha, const int beta);	// 速さ優先探索

// 盤面の描画
void draw_board() {
	// 描画中は画面に映さないようにする
	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	// 盤面の上にA~H, 盤面の左に1~8を表示
	for (int i = 0; i < 8; i++) {
		char c[2] = { 'A' + i };
		DrawStringToHandle(BOARD_X + 66 * i + 28, BOARD_Y - 28, c, GetColor(0, 0, 0), font_board_num);
		c[0] = '1' + i;
		DrawStringToHandle(BOARD_X - 20, BOARD_Y + 66 * i + 24, c, GetColor(0, 0, 0), font_board_num);
	}

	// 盤面を表示
	DrawBox(BOARD_X, BOARD_Y, BOARD_X + 531, BOARD_Y + 531, GetColor(255, 255, 255), FALSE);
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			DrawBox((BOARD_X + 1) + x * 66, (BOARD_Y + 1) + y * 66, (BOARD_X + 68) + x * 66, (BOARD_Y + 68) + y * 66, GetColor(255, 255, 255), FALSE);
			DrawBox((BOARD_X + 2) + x * 66, (BOARD_Y + 2) + y * 66, (BOARD_X + 67) + x * 66, (BOARD_Y + 67) + y * 66, GetColor(0, 94, 21), TRUE);
			if ((black_board >> (63 - (y * 8 + x))) & 1) {
				DrawCircle((BOARD_X + 2) + x * 66 + 32, (BOARD_Y + 2) + y * 66 + 32, 30, GetColor(0, 0, 0), TRUE);
			}
			if ((white_board >> (63 - (y * 8 + x))) & 1) {
				DrawCircle((BOARD_X + 2) + x * 66 + 32, (BOARD_Y + 2) + y * 66 + 32, 30, GetColor(255, 255, 255), TRUE);
			}
		}
	}

	// 女の子の画像を表示
	DrawGraph(800, 400, girl_picture, TRUE);
	// 女の子のセリフを表示
	switch (girl_text) {
		case start:
			DrawGraph(850, 310, t_start, TRUE);
			break;
		case thinking:
			DrawGraph(850, 310, t_thinking, TRUE);
			break;
		case put:
			{
				int t = rnd3(mt);
				if (t == 1) {
					DrawGraph(850, 310, t_put1, TRUE);
				}
				else if (t == 2) {
					DrawGraph(850, 310, t_put2, TRUE);
				}
				else {
					DrawGraph(850, 310, t_put3, TRUE);
				}
			}
			break;
		case no_put:
			DrawGraph(850, 310, t_no_put, TRUE);
			break;
		case win:
			DrawGraph(850, 310, t_win, TRUE);
			break;
		case lose:
			DrawGraph(850, 310, t_lose, TRUE);
			break;
		case draw:
			DrawGraph(850, 310, t_start, TRUE);
			break;
		default:
			break;
	}

	// リセットボタンを表示
	DrawExtendGraph(900, 100, 1000, 140, reset_picture, FALSE);

	// 画面右下にAIが打った場所を表示
	DrawFormatStringToHandle(SCREEN_WIDTH - 400, SCREEN_HEIGHT - 50, GetColor(0, 0, 0), font_board_num, "%s", put_text);

	// 描画した画面を表示
	ScreenFlip();
}

// オセロを初期化
void init_board() {
	black_board = 0x0000000810000000;
	white_board = 0x0000001008000000;

	put_count = 0;
	is_white_turn = false;

	for (int i = 20; i < 29; i++) {
		put_text[i] = '*';
	}

	girl_text = start;

	draw_board();
}

// movの場所に石を置いて、石が返った位置にbitを立てて返す
ll can_put(const ll &mov) {
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

// movで指定した場所に石を置き、revで指定した場所の石をひっくり返す
void put_stone(const ll &mov, const ll &rev) {
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

// 一手戻す
void ando(const ll &mov, const ll &rev) {
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

// 黒（または白）石の数を数える
int count_stone(const int &stone) {
	return __popcnt64((stone == BLACK_STONE) ? black_board : white_board);
}

// ゲームの終了判定と終了処理
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
		if (!is_AI_color_white) {
			girl_text = win;
		}
		else {
			girl_text = lose;
		}
		draw_board();
		MessageBox(NULL, "黒の勝ちです", "othelloGirl", MB_OK);
	}
	else if (b < w) {
		if (is_AI_color_white) {
			girl_text = win;
		}
		else {
			girl_text = lose;
		}
		draw_board();
		MessageBox(NULL, "白の勝ちです", "othelloGirl", MB_OK);
	}
	else {
		girl_text = draw;
		draw_board();
		MessageBox(NULL, "引き分けです", "othelloGirl", MB_OK);
	}
	PlaySoundMem(end_sound, DX_PLAYTYPE_BACK);
}

// プレイ中にマウスをクリックしたときの処理
int mouse_input_in_game() {
	if (GetMouseInput() & MOUSE_INPUT_LEFT) {
		int mouse_x, mouse_y;
		GetMousePoint(&mouse_x, &mouse_y);

		
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				if (BOARD_X + x * 66 <= mouse_x && mouse_x < (BOARD_X + 65) + x * 66 && BOARD_Y + y * 66 <= mouse_y && mouse_y < (BOARD_Y + 65) + y * 66) {
					ll mov = 1LL << (8 * (7 - y) + 7 - x), rev = can_put(mov);
					if (rev) {
						put_stone(mov, rev);
						put_count++;
						// printfDx("%d", put_count);
						PlaySoundMem(put_sound, DX_PLAYTYPE_BACK);
						girl_text = thinking;
						draw_board();
						return 1;
					}
					return 0;
				}
			}
		}

		if (900 <= mouse_x && mouse_x < 1000 && 100 <= mouse_y && mouse_y < 140) {
			PlaySoundMem(reset_sound, DX_PLAYTYPE_BACK);
			now_playing_game = false;
		}
	}

	return 0;
}

// タイトルでの処理
void draw_title() {
	// 描画中は画面に映さないようにする
	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	DrawExtendGraph(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, flower_flame, TRUE);
	DrawExtendGraph(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 250, SCREEN_WIDTH / 2 + 250, SCREEN_HEIGHT / 2 - 100, title_picture, TRUE);
	DrawExtendGraph(SCREEN_WIDTH / 2 - 245, SCREEN_HEIGHT / 2 + 10, SCREEN_WIDTH / 2 + 245, SCREEN_HEIGHT / 2 + 120, black_turn_picture, FALSE);
	DrawExtendGraph(SCREEN_WIDTH / 2 - 245, SCREEN_HEIGHT / 2 + 140, SCREEN_WIDTH / 2 + 245, SCREEN_HEIGHT / 2 + 250, black_turn_picture, FALSE);
	DrawExtendGraph(SCREEN_WIDTH / 2 - 245, SCREEN_HEIGHT / 2 + 140, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 250, white_turn_picture, FALSE);

	DrawGraph(900, 450, 0, FALSE);

	// 描画した画面を表示
	ScreenFlip();
}

// タイトル画面でマウスをクリックしたときの処理
int mouse_input_in_title() {
	if (GetMouseInput() & MOUSE_INPUT_LEFT) {
		int mouse_x, mouse_y;
		GetMousePoint(&mouse_x, &mouse_y);

		if (SCREEN_WIDTH / 2 - 245 <= mouse_x && mouse_x < SCREEN_WIDTH / 2 + 245 && SCREEN_HEIGHT / 2 + 10 <= mouse_y && mouse_y < SCREEN_HEIGHT / 2 + 120) {
			is_AI_color_white = true;
			now_playing_game = true;
			PlaySoundMem(start_sound, DX_PLAYTYPE_BACK);
			init_board();
			return 1;
		}
		else if (SCREEN_WIDTH / 2 - 245 <= mouse_x && mouse_x < SCREEN_WIDTH / 2 + 245 && SCREEN_HEIGHT / 2 + 140 <= mouse_y && mouse_y < SCREEN_HEIGHT / 2 + 250) {
			is_AI_color_white = false;
			now_playing_game = true;
			PlaySoundMem(start_sound, DX_PLAYTYPE_BACK);
			init_board();
			Sleep(700);
			othello_AI();
			return 1;
		}
	}

	return 0;
}

// タイトルでの処理
void title() {
	draw_title();

	for (;;) {
		if (mouse_input_in_title()) {
			if (play_game() == -1) { // ×ボタンかエンターキーを押すと終了
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

// AI
void othello_AI() {
	ll mov;

	// 打てないならパス
	for (mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (can_put(mov)) break;
	}
	if (!mov) {
		if (put_count < 60) {
			girl_text = no_put;
			is_white_turn = !is_white_turn;
			draw_board();
		}
		return;
	}

	// 空所表の設定
	stone_empty_place.clear();
	for (mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (!((black_board | white_board) & mov)) {
			stone_empty_place.push_back(mov);
		}
	}
	stone_empty_place_used.assign(stone_empty_place.size(), false);

	// 終盤なら速さ優先探索へ
	if (put_count >= FINAL_STAGE_NUM) {
		mov = speed_preferred_serch(search_depth = 60 - put_count, TRUE, -INF, INF);
	}
	else{
		mov = nega_max(search_depth = SEARCH_LV, TRUE, -INF, INF);
	}

	ll rev = can_put(mov);
	put_stone(mov, rev);
	put_count++;
	PlaySoundMem(put_sound, DX_PLAYTYPE_BACK);

	// AIが打った場所を画面右下に出力
	for (int i = 0; i < 64; i++) {
		if (mov & (1LL << (63 - i))) {
			for (int j = 20; j < 26; j++) {
				put_text[j] = put_text[j + 3];
			}
			put_text[27] = 'A' + (i % 8);
			put_text[28] = '1' + (i / 8);
			break;
		}
	}

	girl_text = put;
	draw_board();

	// 相手が打てないなら、もう一度
	for (mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (can_put(mov))return;
	}
	is_white_turn = !is_white_turn;
	othello_AI();
}

// nega_max探索
ll nega_max(const int depth, const bool is_put_before_this, const int alpha, const int beta) {
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

// 速さ優先探索
ll speed_preferred_serch(const int depth, const bool is_put_before_this, const int alpha, const int beta) {
	if (depth == 0) return -board_value();

	int max = alpha;
	bool is_put = false;
	ll best_mov;

	if (depth >= 6) {
		vector<tuple<int, int, ll>> p;

		for(int i = 0; i < stone_empty_place.size(); i++){
			if (stone_empty_place_used[i]) continue;

			ll mov = stone_empty_place[i];
			ll rev = can_put(mov);
			if (rev) {
				put_stone(mov, rev);
				ll value = abs(value_can_put());
				p.push_back(make_tuple(value, i, rev));
				ando(mov, rev);
			}
		}

		// 着手可能数が少ない順にソート
		sort(p.begin(), p.end());

		for (auto &x : p) {
			ll mov = stone_empty_place[get<1>(x)];
			ll rev = get<2>(x);
			is_put = true;
			put_stone(mov, rev);
			stone_empty_place_used[get<1>(x)] = true;
			int tmp = -speed_preferred_serch(depth - 1, TRUE, -beta, -max);
			stone_empty_place_used[get<1>(x)] = false;
			ando(mov, rev);

			if (tmp >= beta) return tmp;
			if (tmp > max) {
				max = tmp;
				best_mov = mov;
			}
		}
	}
	else{
		for (int i = 0; i < stone_empty_place.size(); i++) {
			if (stone_empty_place_used[i]) continue;

			ll mov = stone_empty_place[i];
			ll rev = can_put(mov);
			if (rev) {
				is_put = true;
				put_stone(mov, rev);
				stone_empty_place_used[i] = true;
				int tmp = -speed_preferred_serch(depth - 1, TRUE, -beta, -max);
				stone_empty_place_used[i] = false;
				ando(mov, rev);

				if (tmp >= beta) return tmp;
				if (tmp > max) {
					max = tmp;
					best_mov = mov;
				}
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
		int tmp = -speed_preferred_serch(depth - 1, FALSE, -beta, -max);
		is_white_turn = !is_white_turn;
		return tmp;
	}
}

// 盤面の価値を返す
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

// オセロの石の位置によって価値を計算
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

// 石の置ける位置の数によって価値を計算
int value_can_put() {
	int value = 0;

	for (ll mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (can_put(mov)) value++;
	}

	if (is_white_turn) return value;
	else return -value;
}

// 確定石の数によって価値を計算
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

// ゲーム中の処理
int play_game() {
	Sleep(500);

	for (;;) {
		if (mouse_input_in_game()) {
			othello_AI();
			Sleep(200);
			end_game();
		}

		// ゲーム終了のフラグになってたら終了
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	SetOutApplicationLogValidFlag(FALSE); // logを出さないようにする
	SetMainWindowText("othelloGirl");	// タイトル名
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);	// 画面のサイズ
	ChangeWindowMode(TRUE);			// ウィンドウモードにする
	SetBackgroundColor(255, 192, 203);

	if (DxLib_Init() == -1) return -1;		// ＤＸライブラリ初期化処理

	// ユーザーが×ボタンを押しても自動的に終了しないようにする
	SetWindowUserCloseEnableFlag(FALSE);

	// 絵の読み込み
	girl_picture = LoadGraph("picture\\smale2.png");
	flower_flame = LoadGraph("picture\\flower_flame.png");
	title_picture = LoadGraph("picture\\title.png");
	black_turn_picture = LoadGraph("picture\\black.png");
	white_turn_picture = LoadGraph("picture\\white.png");
	reset_picture = LoadGraph("picture\\reset.png");
	t_start = LoadGraph("picture\\t_start.png");
	t_thinking = LoadGraph("picture\\t_thinking.png");
	t_put1 = LoadGraph("picture\\t_put1.png");
	t_put2 = LoadGraph("picture\\t_put2.png");
	t_put3 = LoadGraph("picture\\t_put3.png");
	t_no_put = LoadGraph("picture\\t_no_put.png");
	t_win = LoadGraph("picture\\t_win.png");
	t_lose = LoadGraph("picture\\t_lose.png");
	t_draw = LoadGraph("picture\\t_draw.png");

	// 音の読み込み
	start_sound = LoadSoundMem("sound\\start.mp3");
	put_sound = LoadSoundMem("sound\\put.mp3");
	reset_sound = LoadSoundMem("sound\\reset.mp3");
	end_sound = LoadSoundMem("sound\\end.mp3");

	// フォントの設定
	font_board_num = CreateFontToHandle(NULL, 24, -1);

	// タイトルへ
	title();

	DeleteFontToHandle(font_board_num);

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;
}

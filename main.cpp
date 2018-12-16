#include "DxLib.h"
#include <intrin.h>
#include <vector>
#include <random>
#include <tuple>


using ll = long long;
using namespace std;


// �t�H���g
int font_board_num;

// �摜
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

// ���̎q�̃Z���t
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

// ��
int start_sound;
int reset_sound;
int put_sound;
int end_sound;

// ����
random_device rnd;
mt19937 mt(rnd());
uniform_int_distribution<> rnd3(1, 3); // 1����3�܂ł̐����������_����

ll black_board;
ll white_board;
int put_count;
bool is_white_turn = false;
bool is_AI_color_white = true;
bool now_playing_game = false;
int search_depth;
vector<ll> stone_empty_place;
vector<bool> stone_empty_place_used;
char put_text[30] = "���̎q���ł����ꏊ: ";

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int BOARD_X = 150;
const int BOARD_Y = 100;
const int BLACK_STONE = 1;
const int WHITE_STONE = -1;
constexpr const int INF = (1 << 30);
const int SEARCH_LV = 7;			// �T������萔
const int FINAL_STAGE_NUM = 43;		// �I�Ղ��n�܂�萔
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

void draw_board();	// �Ֆʂ̕`��
void init_board();	// �I�Z����������
ll can_put(const ll &mov);	// mov�̏ꏊ�ɐ΂�u���āA�΂��Ԃ����ʒu��bit�𗧂ĂĕԂ�
void put_stone(const ll &mov, const ll &rev);	// mov�Ŏw�肵���ꏊ�ɐ΂�u���Arev�Ŏw�肵���ꏊ�̐΂��Ђ�����Ԃ�
void ando(const ll &mov, const ll &rev);	// ���߂�
int count_stone(const int &stone);	// ���i�܂��͔��j�΂̐��𐔂���
void end_game();	// �Q�[���̏I������ƏI������
int mouse_input_in_game();	// �v���C���Ƀ}�E�X���N���b�N�����Ƃ��̏���
void draw_title();	// �^�C�g����ʂ̕`��
int mouse_input_in_title();	// �^�C�g����ʂŃ}�E�X���N���b�N�����Ƃ��̏���
void title();	// �^�C�g���ł̏���
void othello_AI();	// AI
int play_game();	// �Q�[�����̏���
ll nega_max(const int depth, const bool is_put_before_this, const int alpha, const int beta);	// nega_max�T��
int board_value();	// �Ֆʂ̉��l��Ԃ�
int value_stone_place();	// �I�Z���̐΂̈ʒu�ɂ���ĉ��l���v�Z
int value_can_put();	// �΂̒u����ʒu�̐��ɂ���ĉ��l���v�Z
int value_fixed_stone();	// �m��΂̐��ɂ���ĉ��l���v�Z
ll speed_preferred_serch(const int depth, const bool is_put_before_this, const int alpha, const int beta);	// �����D��T��

// �Ֆʂ̕`��
void draw_board() {
	// �`�撆�͉�ʂɉf���Ȃ��悤�ɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	// �Ֆʂ̏��A~H, �Ֆʂ̍���1~8��\��
	for (int i = 0; i < 8; i++) {
		char c[2] = { 'A' + i };
		DrawStringToHandle(BOARD_X + 66 * i + 28, BOARD_Y - 28, c, GetColor(0, 0, 0), font_board_num);
		c[0] = '1' + i;
		DrawStringToHandle(BOARD_X - 20, BOARD_Y + 66 * i + 24, c, GetColor(0, 0, 0), font_board_num);
	}

	// �Ֆʂ�\��
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

	// ���̎q�̉摜��\��
	DrawGraph(800, 400, girl_picture, TRUE);
	// ���̎q�̃Z���t��\��
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

	// ���Z�b�g�{�^����\��
	DrawExtendGraph(900, 100, 1000, 140, reset_picture, FALSE);

	// ��ʉE����AI���ł����ꏊ��\��
	DrawFormatStringToHandle(SCREEN_WIDTH - 400, SCREEN_HEIGHT - 50, GetColor(0, 0, 0), font_board_num, "%s", put_text);

	// �`�悵����ʂ�\��
	ScreenFlip();
}

// �I�Z����������
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

// mov�̏ꏊ�ɐ΂�u���āA�΂��Ԃ����ʒu��bit�𗧂ĂĕԂ�
ll can_put(const ll &mov) {
	if ((black_board | white_board) & mov)return 0;		//����ӏ����󔒂łȂ��ꍇ


	ll rev = 0, r, mask;

	if (is_white_turn) {						//�����łꍇ
		r = 0;
		mask = (mov >> 1) & 0x7f7f7f7f7f7f7f7f;	//�E�����֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & black_board) != 0) {	//���΂��A�������
			r |= mask;
			mask = (mask >> 1) & 0x7f7f7f7f7f7f7f7f;
		}
		if (mask & white_board)rev |= r;				//���΂�����ΕԂ��ʒu���m�肷��

		r = 0;
		mask = (mov >> 8) & 0x00ffffffffffffff;	//�������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask >> 8) & 0x00ffffffffffffff;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov << 1) & 0xfefefefefefefefe;	//�������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask << 1) & 0xfefefefefefefefe;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov << 8) & 0xffffffffffffff00;	//������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask << 8) & 0xffffffffffffff00;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov >> 9) & 0x007f7f7f7f7f7f7f;	//�E�������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask >> 9) & 0x007f7f7f7f7f7f7f;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov >> 7) & 0x00fefefefefefefe;	//���������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask >> 7) & 0x00fefefefefefefe;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov << 9) & 0xfefefefefefefe00;	//��������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask << 9) & 0xfefefefefefefe00;
		}
		if (mask & white_board)rev |= r;

		r = 0;
		mask = (mov << 7) & 0x7f7f7f7f7f7f7f00;	//�E������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & black_board) != 0) {
			r |= mask;
			mask = (mask << 7) & 0x7f7f7f7f7f7f7f00;
		}
		if (mask & white_board)rev |= r;
	}
	else {										//�����łꍇ
		r = 0;
		mask = (mov >> 1) & 0x7f7f7f7f7f7f7f7f;	//�E�����֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & white_board) != 0) {	//���΂��A�������
			r |= mask;
			mask = (mask >> 1) & 0x7f7f7f7f7f7f7f7f;
		}
		if (mask & black_board)rev |= r;				//���΂�����ΕԂ��ʒu���m�肷��

		r = 0;
		mask = (mov >> 8) & 0x00ffffffffffffff;	//�������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask >> 8) & 0x00ffffffffffffff;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov << 1) & 0xfefefefefefefefe;	//�������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask << 1) & 0xfefefefefefefefe;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov << 8) & 0xffffffffffffff00;	//������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask << 8) & 0xffffffffffffff00;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov >> 9) & 0x007f7f7f7f7f7f7f;	//�E�������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask >> 9) & 0x007f7f7f7f7f7f7f;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov >> 7) & 0x00fefefefefefefe;	//���������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask >> 7) & 0x00fefefefefefefe;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov << 9) & 0xfefefefefefefe00;	//��������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask << 9) & 0xfefefefefefefe00;
		}
		if (mask & black_board)rev |= r;

		r = 0;
		mask = (mov << 7) & 0x7f7f7f7f7f7f7f00;	//�E������֕Ԃ��邩�𒲂ׂ�
		while (mask != 0 && (mask & white_board) != 0) {
			r |= mask;
			mask = (mask << 7) & 0x7f7f7f7f7f7f7f00;
		}
		if (mask & black_board)rev |= r;
	}

	return rev;
}

// mov�Ŏw�肵���ꏊ�ɐ΂�u���Arev�Ŏw�肵���ꏊ�̐΂��Ђ�����Ԃ�
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

// ���߂�
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

// ���i�܂��͔��j�΂̐��𐔂���
int count_stone(const int &stone) {
	return __popcnt64((stone == BLACK_STONE) ? black_board : white_board);
}

// �Q�[���̏I������ƏI������
void end_game() {
	// ���v���C���[���u���Ȃ��Ȃ�����Q�[���I��
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
		MessageBox(NULL, "���̏����ł�", "othelloGirl", MB_OK);
	}
	else if (b < w) {
		if (is_AI_color_white) {
			girl_text = win;
		}
		else {
			girl_text = lose;
		}
		draw_board();
		MessageBox(NULL, "���̏����ł�", "othelloGirl", MB_OK);
	}
	else {
		girl_text = draw;
		draw_board();
		MessageBox(NULL, "���������ł�", "othelloGirl", MB_OK);
	}
	PlaySoundMem(end_sound, DX_PLAYTYPE_BACK);
}

// �v���C���Ƀ}�E�X���N���b�N�����Ƃ��̏���
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

// �^�C�g���ł̏���
void draw_title() {
	// �`�撆�͉�ʂɉf���Ȃ��悤�ɂ���
	SetDrawScreen(DX_SCREEN_BACK);

	ClearDrawScreen();

	DrawExtendGraph(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, flower_flame, TRUE);
	DrawExtendGraph(SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 250, SCREEN_WIDTH / 2 + 250, SCREEN_HEIGHT / 2 - 100, title_picture, TRUE);
	DrawExtendGraph(SCREEN_WIDTH / 2 - 245, SCREEN_HEIGHT / 2 + 10, SCREEN_WIDTH / 2 + 245, SCREEN_HEIGHT / 2 + 120, black_turn_picture, FALSE);
	DrawExtendGraph(SCREEN_WIDTH / 2 - 245, SCREEN_HEIGHT / 2 + 140, SCREEN_WIDTH / 2 + 245, SCREEN_HEIGHT / 2 + 250, black_turn_picture, FALSE);
	DrawExtendGraph(SCREEN_WIDTH / 2 - 245, SCREEN_HEIGHT / 2 + 140, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 250, white_turn_picture, FALSE);

	DrawGraph(900, 450, 0, FALSE);

	// �`�悵����ʂ�\��
	ScreenFlip();
}

// �^�C�g����ʂŃ}�E�X���N���b�N�����Ƃ��̏���
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

// �^�C�g���ł̏���
void title() {
	draw_title();

	for (;;) {
		if (mouse_input_in_title()) {
			if (play_game() == -1) { // �~�{�^�����G���^�[�L�[�������ƏI��
				return;
			}
			draw_title();
		}

		// �~�{�^�����G���^�[�L�[�������ƏI��
		if (GetWindowUserCloseFlag(TRUE) || CheckHitKey(KEY_INPUT_RETURN)) {
			return;
		}
	}
}

// AI
void othello_AI() {
	ll mov;

	// �łĂȂ��Ȃ�p�X
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

	// �󏊕\�̐ݒ�
	stone_empty_place.clear();
	for (mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (!((black_board | white_board) & mov)) {
			stone_empty_place.push_back(mov);
		}
	}
	stone_empty_place_used.assign(stone_empty_place.size(), false);

	// �I�ՂȂ瑬���D��T����
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

	// AI���ł����ꏊ����ʉE���ɏo��
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

	// ���肪�łĂȂ��Ȃ�A������x
	for (mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (can_put(mov))return;
	}
	is_white_turn = !is_white_turn;
	othello_AI();
}

// nega_max�T��
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

// �����D��T��
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

		// ����\�������Ȃ����Ƀ\�[�g
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

// �Ֆʂ̉��l��Ԃ�
int board_value() {
	int value = 0;
	const int VSP = 1, VCP = 40, VFS = 45;	// �]���l�̏d��

	if (put_count >= FINAL_STAGE_NUM) {
		value += count_stone(WHITE_STONE) - count_stone(BLACK_STONE);
	}
	else {
		value += value_stone_place() * VSP;
		value += value_can_put() * VCP;
		value += value_fixed_stone() *  VFS;
	}

	if (!is_AI_color_white) value = -value;		// AI�����Ȃ甽�]

	return value;
}

// �I�Z���̐΂̈ʒu�ɂ���ĉ��l���v�Z
int value_stone_place() {
	int value = 0, c = 0;

	if (is_AI_color_white) {
		if (__popcnt64(white_board) >= 1) c = 0;		// �S�ő΍�
		else if (__popcnt64(black_board) == 0) c = -50000;		// �����S�ł�������]���l��A�b�v
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

// �΂̒u����ʒu�̐��ɂ���ĉ��l���v�Z
int value_can_put() {
	int value = 0;

	for (ll mov = 0x8000000000000000; mov != 0; mov = (mov >> 1) & 0x7fffffffffffffff) {
		if (can_put(mov)) value++;
	}

	if (is_white_turn) return value;
	else return -value;
}

// �m��΂̐��ɂ���ĉ��l���v�Z
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

// �Q�[�����̏���
int play_game() {
	Sleep(500);

	for (;;) {
		if (mouse_input_in_game()) {
			othello_AI();
			Sleep(200);
			end_game();
		}

		// �Q�[���I���̃t���O�ɂȂ��Ă���I��
		if (!now_playing_game) {
			return 0;
		}

		// �~�{�^�����G���^�[�L�[�������ƏI��
		if (GetWindowUserCloseFlag(TRUE) || CheckHitKey(KEY_INPUT_RETURN)) {
			return -1;
		}
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	SetOutApplicationLogValidFlag(FALSE); // log���o���Ȃ��悤�ɂ���
	SetMainWindowText("othelloGirl");	// �^�C�g����
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);	// ��ʂ̃T�C�Y
	ChangeWindowMode(TRUE);			// �E�B���h�E���[�h�ɂ���
	SetBackgroundColor(255, 192, 203);

	if (DxLib_Init() == -1) return -1;		// �c�w���C�u��������������

	// ���[�U�[���~�{�^���������Ă������I�ɏI�����Ȃ��悤�ɂ���
	SetWindowUserCloseEnableFlag(FALSE);

	// �G�̓ǂݍ���
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

	// ���̓ǂݍ���
	start_sound = LoadSoundMem("sound\\start.mp3");
	put_sound = LoadSoundMem("sound\\put.mp3");
	reset_sound = LoadSoundMem("sound\\reset.mp3");
	end_sound = LoadSoundMem("sound\\end.mp3");

	// �t�H���g�̐ݒ�
	font_board_num = CreateFontToHandle(NULL, 24, -1);

	// �^�C�g����
	title();

	DeleteFontToHandle(font_board_num);

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;
}

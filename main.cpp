#include "DxLib.h"

using ll = long long;

ll black_board;
ll white_board;
bool turn_white = false;

void draw_board(int &p) {
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

ll can_put(ll &mov) {
	if ((black_board | white_board) & mov)return 0;		//����ӏ����󔒂Ŗ����ꍇ


	ll rev = 0, r, mask;

	if (turn_white) {						//�����łꍇ
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1280, 720, 32);	// ��ʂ̃T�C�Y
	ChangeWindowMode(TRUE);			// �E�B���h�E���[�h�ɂ���
	SetBackgroundColor(255, 255, 255);

	if (DxLib_Init() == -1) return -1;		// �c�w���C�u��������������

	// ���[�U�[���~�{�^���������Ă������I�ɏI�����Ȃ��悤�ɂ���
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
						ll mov = 1LL << (8 * (7 - y) + 7 - x), rev = can_put(mov);
						if (rev) {
							put_stone(mov, rev);
							draw_board(girl_pic);
							WaitTimer(200);
						}
						return;
					}
				}
			}}();
		}

		// �~�{�^�����G���^�[�L�[�������ƏI��
		if (GetWindowUserCloseFlag(TRUE) || CheckHitKey(KEY_INPUT_RETURN)) {
			break;
		}
	}

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;
}

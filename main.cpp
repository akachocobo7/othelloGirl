#include "DxLib.h"
#include "draw.h"

// �v���O������ WinMain ����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1280, 720, 32);	// ��ʂ̃T�C�Y
	ChangeWindowMode(TRUE);			// �E�B���h�E���[�h�ɂ���
	if (DxLib_Init() == -1) return -1;		// �c�w���C�u��������������


	init_board_draw();

	int p = LoadGraph("picture\\1.png");
	girl_draw(p);

	WaitKey();				// �L�[���͑҂�

	DxLib_End();				// �c�w���C�u�����g�p�̏I������

	return 0;				// �\�t�g�̏I�� 
}

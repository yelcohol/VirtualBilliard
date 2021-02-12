
#include "font.h"

font* font::TextInst = NULL;

font::font(void)         //������
{
}
font::~font(void)         //�Ҹ���
{
}

void    font::Init(IDirect3DDevice9* Device, int height, int width)//��������
{
	D3DXFONT_DESC DXFont_DESC;
	ZeroMemory(&DXFont_DESC, sizeof(D3DXFONT_DESC));

	DXFont_DESC.Height = height;                            // ��ü ���� ����
	DXFont_DESC.Width = width;                             // ��ü ���� ����
	DXFont_DESC.Weight = FW_BOLD;                     // ���� ���� ( FW_BOLD �ϸ� ���� )
	DXFont_DESC.MipLevels = D3DX_DEFAULT;
	DXFont_DESC.Italic = false;                         // ���ڸ�
	DXFont_DESC.CharSet = DEFAULT_CHARSET;
	DXFont_DESC.OutputPrecision = OUT_DEFAULT_PRECIS;
	DXFont_DESC.Quality = DEFAULT_QUALITY;
	DXFont_DESC.PitchAndFamily = DEFAULT_PITCH;
	DXFont_DESC.FaceName, TEXT("���ü");              // �۾�ü
	m_nMax_X = 1200;                               // ������ �ִ� X
	m_nMax_Y = 860;                                // ������ �ִ� Y
	D3DXCreateFontIndirect(Device, &DXFont_DESC, &m_pFont);
}

void    font::Print(LPCSTR cSTR, int nX/*���ڿ��� ������ǥ*/, int nY/*���ڿ��� ������ǥ*/, D3DXCOLOR ARGB)
{
	RECT rt = { nX, nY, m_nMax_X, m_nMax_Y };
	m_pFont->DrawTextA(0, cSTR, -1, &rt, DT_TOP | DT_LEFT, ARGB);
	//#ifdef UNICODE // �����ڵ� �κ�
	//   m_pFont->DrawTextW(0, cSTR, -1, &rt, DT_TOP | DT_LEFT, ARGB);

	//#else
	//   m_pFont->DrawTextA(0, "Hello, World", -1, &rt, DT_TOP | DT_LEFT, ARGB);

	//#endif
}

font* font::GetInst(void)
{
	if (!TextInst)
	{
		TextInst = new font;
	}
	return TextInst;
}

void    font::FreeInst(void)
{
	if (TextInst)
	{
		m_pFont->Release();
		delete  TextInst;
		TextInst = NULL;
	}
}
#pragma once
#include "d3dUtility.h"

class font
{
private:
	static  font* TextInst;
	ID3DXFont* m_pFont;
	int         m_nMax_X;
	int         m_nMax_Y;


public:
	font(void);
	~font(void);
	void    Init(IDirect3DDevice9* Device, int height, int width);//��������
	void    Print(LPCSTR cSTR, int nX/*���ڿ��� ������ǥ*/ = 0, int nY/*���ڿ��� ������ǥ*/ = 0, D3DXCOLOR ARGB = 0xFFFFFFFF);
	static  font* GetInst(void);
	void        FreeInst(void);
};
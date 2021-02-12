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
	void    Init(IDirect3DDevice9* Device, int height, int width);//최종수정
	void    Print(LPCSTR cSTR, int nX/*문자열의 왼쪽좌표*/ = 0, int nY/*문자열의 윗쪽좌표*/ = 0, D3DXCOLOR ARGB = 0xFFFFFFFF);
	static  font* GetInst(void);
	void        FreeInst(void);
};
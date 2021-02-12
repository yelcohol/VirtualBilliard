#pragma once
#include "d3dUtility.h"
#include "CSphere.h"

#define M_HEIGHT 0.01

class CWall {

private:

	int               index;
	float               m_x;
	float               m_z;
	float               m_width;
	float               m_depth;
	float               m_height;

public:
	CWall(void);
	~CWall(void);
public:
	bool create(IDirect3DDevice9* pDevice, float ix, float iz, float iwidth, float iheight, float idepth, D3DXCOLOR color);
	void destroy(void);
	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld);
	bool hasIntersected(CSphere& ball);
	void hitBy(CSphere& ball);
	void setPosition(float x, float y, float z);
	void setIndex(int n);
	float getHeight(void) const;
private:
	void setLocalTransform(const D3DXMATRIX& mLocal);
	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pBoundMesh;
};

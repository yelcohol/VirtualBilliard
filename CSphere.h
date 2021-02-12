#pragma once
#include "d3dUtility.h"

#define M_RADIUS 0.21   // ball radius
#define ALPHA 142.8571428571429
#define DECREASE_RATE 0.9982
#define TORK_DECREASE_RATE 0.9982



class CSphere {
private:
	int index;
	int hasHit[4];
	float               center_x, center_y, center_z;
	float               m_radius;
	float               m_velocity_x;
	float               m_velocity_z;
	float            m_tork_x;
	float            m_tork_y;
public:
	CSphere(void);
	~CSphere(void);
	void updateTork(float x, float y);
	void setIndex(int _index);
	bool create(IDirect3DDevice9* pDevice, D3DXCOLOR color = d3d::WHITE);
	void destroy(void);
	void draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld);
	bool hasIntersected(CSphere& ball);
	void hitBy(CSphere& ball);
	void ballUpdate(float timeDiff);
	double getVelocity_X();
	double getVelocity_Z();
	double getTork_X();
	double getTork_Y();
	int gethasHit(int index);
	void sethasHit(int i, int j);
	void setTork(float mesh_x, float mesh_y);
	void setPower(double vx, double vz);
	void setCenter(float x, float y, float z);
	float getRadius(void)  const;
	const D3DXMATRIX& getLocalTransform(void) const;
	void setLocalTransform(const D3DXMATRIX& mLocal);
	D3DXVECTOR3 getCenter(void) const;
private:
	D3DXMATRIX              m_mLocal;
	D3DMATERIAL9            m_mtrl;
	ID3DXMesh* m_pSphereMesh;
};
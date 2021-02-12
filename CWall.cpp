#include "CWall.h"

int signOf(double num) {      //수의 부호를 구해주는 함수
	if (num >= 0)
		return 1;
	return -1;
}
void rotate(CSphere& ball, float velocity_theta) {
	ball.setPower(ball.getVelocity_X() * cos(velocity_theta) - (ball.getVelocity_Z() * sin(velocity_theta)), ball.getVelocity_Z() * (cos(velocity_theta)) + (ball.getVelocity_X() * sin(velocity_theta)));
}
CWall::CWall(void)
{
	D3DXMatrixIdentity(&m_mLocal);
	ZeroMemory(&m_mtrl, sizeof(m_mtrl));
	m_width = 0;
	m_depth = 0;
	m_pBoundMesh = NULL;
}
CWall::~CWall(void) {}
bool CWall::create(IDirect3DDevice9* pDevice, float ix, float iz, float iwidth, float iheight, float idepth, D3DXCOLOR color)
{
	if (NULL == pDevice)
		return false;

	m_mtrl.Ambient = color;
	m_mtrl.Diffuse = color;
	m_mtrl.Specular = color;
	m_mtrl.Emissive = (D3DXCOLOR)D3DCOLOR_XRGB(20, 20, 40);
	m_mtrl.Power = 5.0f;

	m_width = iwidth;
	m_depth = idepth;

	if (FAILED(D3DXCreateBox(pDevice, iwidth, iheight, idepth, &m_pBoundMesh, NULL)))
		return false;
	return true;
}
void CWall::destroy(void)
{
	if (m_pBoundMesh != NULL) {
		m_pBoundMesh->Release();
		m_pBoundMesh = NULL;
	}
}
void CWall::draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
{
	if (NULL == pDevice)
		return;
	pDevice->SetTransform(D3DTS_WORLD, &mWorld);
	pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
	pDevice->SetMaterial(&m_mtrl);
	m_pBoundMesh->DrawSubset(0);
}
bool CWall::hasIntersected(CSphere& ball)
{
	D3DXVECTOR3 ball_pos = ball.getCenter();

	if (m_width > m_depth) {   //가로벽 충돌   
		if (ball_pos.z - m_z <= M_RADIUS + 0.06f && ball_pos.z - m_z >= -M_RADIUS - 0.06f)
			return true;
	}
	else {               //세로벽 충돌
		if (ball_pos.x - m_x <= M_RADIUS + 0.06f && ball_pos.x - m_x >= -M_RADIUS - 0.06f)
			return true;
	}
	// Insert your code here.
	return false;
}
void CWall::hitBy(CSphere& ball)
{
	D3DXVECTOR3 ball_pos = ball.getCenter();

	double gap;      //벽과 공 사이의 겹치는 부분

	if (hasIntersected(ball)) {
		if (m_width > m_depth) {            //가로벽이면 z 방향 속도만 바꾸면 됨!

			gap = M_RADIUS + 0.06f - abs(ball_pos.z - m_z);         //z 방향 gap 계산

			ball.setCenter(ball_pos.x, (float)M_RADIUS, ball_pos.z - gap * signOf(ball.getVelocity_Z()));
			ball.setPower(ball.getVelocity_X(), -ball.getVelocity_Z());
		}
		else {                           //세로벽이면 x 방향 속도만 바꾸면 됨!

			gap = M_RADIUS + 0.06f - abs(ball_pos.x - m_x);         //x 방향 gap 계산
			ball.setCenter(ball_pos.x - gap * signOf(ball.getVelocity_X()), (float)M_RADIUS, ball_pos.z);
			ball.setPower(-ball.getVelocity_X(), ball.getVelocity_Z());
		}

		if (ball.getTork_X() != 0) {
			float velocity_theta = ball.getTork_X() / 100;
			float theta;
			if (this->index == 0 || this->index == 1)
				theta = abs(atan(ball.getVelocity_Z() / ball.getVelocity_X()));
			else {
				theta = abs(atan(ball.getVelocity_X() / ball.getVelocity_Z()));
			}

			switch (this->index) {
			case 0:
				rotate(ball, theta * velocity_theta);
				break;
			case 1:
				rotate(ball, theta * velocity_theta);
				break;
			case 2:
				rotate(ball, theta * velocity_theta);
				break;
			case 3:
				rotate(ball, theta * velocity_theta);
				break;
			}
		}
	}
}
void CWall::setPosition(float x, float y, float z)
{
	D3DXMATRIX m;
	this->m_x = x;
	this->m_z = z;

	D3DXMatrixTranslation(&m, x, y, z);
	setLocalTransform(m);
}
void CWall::setIndex(int n) {
	this->index = n;
}
float CWall::getHeight(void) const { return M_HEIGHT; }
void CWall::setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }

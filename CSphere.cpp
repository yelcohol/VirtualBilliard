#include "CSphere.h"

float mesh_x = 0;
float mesh_y = 0;
float g_mesh_x = 0;
float g_mesh_y = 0;
int mesh_x_count;
int mesh_y_count;
int count1 = 0;
int count2 = 0;
CSphere::CSphere(void)
{
	for (int i = 0; i < 4; i++) {
		hasHit[i] = 0;
	}
	D3DXMatrixIdentity(&m_mLocal);
	ZeroMemory(&m_mtrl, sizeof(m_mtrl));
	m_radius = 0;
	m_velocity_x = 0;
	m_velocity_z = 0;
	m_tork_x = 0;
	m_tork_y = 0;
	m_pSphereMesh = NULL;

}
CSphere::~CSphere(void) {}


void CSphere::setIndex(int _index)
{
	index = _index;
}
bool CSphere::create(IDirect3DDevice9* pDevice, D3DXCOLOR color)
{
	if (NULL == pDevice)
		return false;

	m_mtrl.Ambient = color;
	m_mtrl.Diffuse = color;
	m_mtrl.Specular = color;
	m_mtrl.Emissive = d3d::BLACK;
	m_mtrl.Power = 5.0f;

	if (FAILED(D3DXCreateSphere(pDevice, getRadius(), 50, 50, &m_pSphereMesh, NULL)))
		return false;
	return true;
}

void CSphere::destroy(void)
{
	if (m_pSphereMesh != NULL) {
		m_pSphereMesh->Release();
		m_pSphereMesh = NULL;
	}
}

void CSphere::draw(IDirect3DDevice9* pDevice, const D3DXMATRIX& mWorld)
{
	if (NULL == pDevice)
		return;
	pDevice->SetTransform(D3DTS_WORLD, &mWorld);
	pDevice->MultiplyTransform(D3DTS_WORLD, &m_mLocal);
	pDevice->SetMaterial(&m_mtrl);
	m_pSphereMesh->DrawSubset(0);
}


bool CSphere::hasIntersected(CSphere& ball)
{
	D3DXVECTOR3 ball_pos = ball.getCenter();
	bool result = pow(center_x - ball_pos.x, 2) + pow(center_z - ball_pos.z, 2) < pow(1.9999999f * M_RADIUS, 2);

	//if (result)   hashit[ball.index]++;
	return result;
}

void CSphere::hitBy(CSphere& ball)
{
	D3DXVECTOR3 ball_pos = ball.getCenter();

	if (hasIntersected(ball)) {
		this->hasHit[ball.index]++;
		if (ball.index == 2 && this->index == 3) {
			ball.hasHit[this->index]++;
		}
		D3DXVECTOR2 vec_ball_to_this(center_x - ball_pos.x, center_z - ball_pos.z);      //ball에서 this로의 벡터
		D3DXVec2Normalize(&vec_ball_to_this, &vec_ball_to_this);                           //단위벡터 (크기가 1인 벡터)로 전환

		double gap = M_RADIUS - (sqrt(pow(center_x - ball_pos.x, 2) + pow(center_z - ball_pos.z, 2)) / 2);      //두 공이 겹치는 부분의 절반

		setCenter(center_x + (gap * vec_ball_to_this.x), (float)M_RADIUS, center_z + (gap * vec_ball_to_this.y));
		ball.setCenter(ball.center_x - (gap * vec_ball_to_this.x), (float)M_RADIUS, ball.center_z - (gap * vec_ball_to_this.y));
		//gap 만큼 거리 벌리기

		D3DXVECTOR2 vec_this(m_velocity_x, m_velocity_z);
		D3DXVECTOR2 vec_ball(ball.m_velocity_x, ball.m_velocity_z);

		D3DXVECTOR2 vec_ball_to_this_perpend(-vec_ball_to_this.y, vec_ball_to_this.x);   //vec_ball_to_this의 법선벡터

		D3DXVECTOR2 vec_this_after = D3DXVec2Dot(&vec_this, &vec_ball_to_this_perpend) * vec_ball_to_this_perpend + D3DXVec2Dot(&vec_ball, &vec_ball_to_this) * vec_ball_to_this;
		D3DXVECTOR2 vec_ball_after = D3DXVec2Dot(&vec_this, &vec_ball_to_this) * vec_ball_to_this + D3DXVec2Dot(&vec_ball, &vec_ball_to_this_perpend) * vec_ball_to_this_perpend;
		D3DXVECTOR2 vec_this_after2 = vec_this_after;

		setPower(vec_this_after.x, vec_this_after.y);
		ball.setPower(vec_ball_after.x, vec_ball_after.y);

		if (this->getTork_Y() > 0) {      // 밀어치기
			D3DXVec2Normalize(&vec_this, &vec_this);
			D3DXVec2Normalize(&vec_this_after2, &vec_this_after2);
			if (D3DXVec2Dot(&vec_this, &vec_this_after2) <= 0.4 && D3DXVec2Dot(&vec_this, &vec_this_after2) >= -0.4) {      // 두께가 어느정도 이상일 때 
				D3DXVECTOR2 vec_tork(m_velocity_x, m_velocity_z);
				//D3DXVECTOR2 vec_this_after1 = (-ball.m_velocity_x, ball.m_velocity_z);
				setPower((vec_this.x * (this->getTork_Y() / ALPHA) * 10 + vec_this_after.x), (vec_this.y * (this->getTork_Y() / ALPHA) * 10 + vec_this_after.y));
				setTork(getTork_X(), 0);
				mesh_y = 0.000f;
				mesh_y_count = 0;

			}
		}
		else if (this->getTork_Y() < 0) {   //끌어치기
			if (D3DXVec2Dot(&vec_this, &vec_this_after2) <= 0.4 && D3DXVec2Dot(&vec_this, &vec_this_after2) >= -0.4) {         // 두께가 어느정도 이상일때
				setPower((vec_this.x * (this->getTork_Y() / ALPHA) * 5 + vec_this_after.x), (vec_this.y * (this->getTork_Y() / ALPHA) * 5 + vec_this_after.y));
				setTork(getTork_X(), 0);
				mesh_y = 0.000f;
				mesh_y_count = 0;
			}
		}
	}
}

void CSphere::ballUpdate(float timeDiff)
{
	const float TIME_SCALE = 3.3;
	D3DXVECTOR3 cord = this->getCenter();
	double vx = abs(this->getVelocity_X());
	double vz = abs(this->getVelocity_Z());

	if (vx > 0.01 || vz > 0.01)
	{
		float tX = cord.x + TIME_SCALE * timeDiff * m_velocity_x;
		float tZ = cord.z + TIME_SCALE * timeDiff * m_velocity_z;

		//correction of position of ball
		// Please uncomment this part because this correction of ball position is necessary when a ball collides with a wall
		if (tX >= (4.5 - M_RADIUS))
			tX = 4.5 - M_RADIUS;
		else if (tX <= (-4.5 + M_RADIUS))
			tX = -4.5 + M_RADIUS;
		else if (tZ <= (-3 + M_RADIUS))
			tZ = -3 + M_RADIUS;
		else if (tZ >= (3 - M_RADIUS))
			tZ = 3 - M_RADIUS;

		this->setCenter(tX, cord.y, tZ);
	}
	else { this->setPower(0, 0); }
	double rate = 1 - (1 - DECREASE_RATE) * timeDiff * 400;
	if (rate < 0)
		rate = 0;
	this->setPower(getVelocity_X() * rate, getVelocity_Z() * rate);
	if (!(this->getVelocity_X() == 0 && this->getVelocity_Z() == 0)) {
		double tork_rate1 = 1 - (1 - TORK_DECREASE_RATE) * timeDiff * 500;
		double tork_rate2 = 1 - (1 - TORK_DECREASE_RATE) * timeDiff * 2000;
		if (tork_rate1 < 0)
			tork_rate1 = 0;
		if (tork_rate2 < 0)
			tork_rate2 = 0;
		float nexttorkX = this->getTork_X() * (float)tork_rate1;
		float nexttorkY = this->getTork_Y() * (float)tork_rate2;
		this->updateTork(nexttorkX, nexttorkY);
		mesh_x *= tork_rate1;
		mesh_y *= tork_rate2;
		count1++;
		count2++;
		if (mesh_x_count != 0) {
			if (abs(mesh_x) < (M_RADIUS / 100)) {
				mesh_x = 0.0000f;
				mesh_x_count = 0;
			}
			else if (pow(tork_rate1, count1) >= 0.86f && pow(tork_rate1, count1) <= 0.94f) {
				if (mesh_x_count > 0) {
					mesh_x_count -= 5;
				}

				else {
					mesh_x_count += 10;
				}
				count1 = 0;
			}
		}
		if (mesh_y_count != 0) {
			if (mesh_y < (M_RADIUS / 100)) {
				mesh_y = 0.0000f;
				mesh_y_count = 0;
			}
			else if (pow(tork_rate2, count2) >= 0.86f && pow(tork_rate2, count2) <= 0.94f) {
				if (mesh_y_count > 0) {
					mesh_y_count -= 5;
				}
				else {
					mesh_y_count += 5;
				}
				count2 = 0;
			}
		}
	}

	double tork_rate = 1 - (1 - TORK_DECREASE_RATE) * timeDiff * 400;
	if (tork_rate < 0)
		tork_rate = 0;
	float nexttorkX = this->getTork_X() * (float)tork_rate;
	float nexttorkY = this->getTork_Y() * (float)tork_rate;
	this->updateTork(nexttorkX, nexttorkY);
}

double CSphere::getVelocity_X() { return this->m_velocity_x; }
double CSphere::getVelocity_Z() { return this->m_velocity_z; }
double CSphere::getTork_X() { return this->m_tork_x; }
double CSphere::getTork_Y() { return this->m_tork_y; }
int CSphere::gethasHit(int index) { return hasHit[index]; }
void CSphere::sethasHit(int i, int j) {
	this->hasHit[i] = j;
}
void CSphere::setTork(float mesh_x, float mesh_y) {
	this->m_tork_x = mesh_x * ALPHA;
	this->m_tork_y = mesh_y * ALPHA;
}
void CSphere::updateTork(float x, float y)
{
	this->m_tork_x = x;
	this->m_tork_y = y;
}
void CSphere::setPower(double vx, double vz)
{
	this->m_velocity_x = vx;
	this->m_velocity_z = vz;
}
void CSphere::setCenter(float x, float y, float z)
{
	D3DXMATRIX m;
	center_x = x;   center_y = y;   center_z = z;
	D3DXMatrixTranslation(&m, x, y, z);
	setLocalTransform(m);
}
float CSphere::getRadius(void)  const { return (float)(M_RADIUS); }
const D3DXMATRIX& CSphere::getLocalTransform(void) const { return m_mLocal; }
void CSphere::setLocalTransform(const D3DXMATRIX& mLocal) { m_mLocal = mLocal; }
D3DXVECTOR3 CSphere::getCenter(void) const
{
	D3DXVECTOR3 org(center_x, center_y, center_z);
	return org;
}

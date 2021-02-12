////////////////////////////////////////////////////////////////////////////////
//
// File: virtualLego.cpp
//
// Original Author: 박창현 Chang-hyeon Park, 
// Modified by Bong-Soo Sohn and Dong-Jun Kim
// 
// Originally programmed for Virtual LEGO. 
// Modified later to program for Virtual Billiard.
//        
////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "font.h"
#include "CSphere.h"
#include "CWall.h"
#include "CLight.h"
#include "CPlayer.h"
#include "sound.h"
#include <tchar.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <Windows.h>


IDirect3DDevice9* Device = NULL;
HWND hwnd = 0;

// window size
const int Width = 1200;
const int Height = 860;

extern float mesh_x;
extern	float mesh_y;

// There are four balls
// initialize the position (coordinate) of each ball (ball0 ~ ball3)
const float spherePos[4][2] = { { -2.7f,0 } ,{ +2.4f,0 } ,{ 3.3f,0 } ,{ -2.7f,-0.9f } };
// initialize the color of each ball (ball0 ~ ball3)
const D3DXCOLOR sphereColor[4] = { d3d::RED, d3d::RED, d3d::YELLOW, d3d::WHITE };

// -----------------------------------------------------------------------------
// Transform matrices
// -----------------------------------------------------------------------------
D3DXMATRIX g_mWorld;
D3DXMATRIX g_mView;
D3DXMATRIX g_mProj;

ID3DXFont* Font;

//D3DXCreateSprite(Device, &pSprite);
//////////////////////////////////////////////////
#define PI 3.14159265
#define BETA 0.01
#define GAMMA 71.42857142
///////////////////////////////////////////////////////////////////////////////////////////////
bool first_space = false;
float display_count = 0.000000f;

extern int mesh_x_count;
extern int mesh_y_count;

int menu = 0;	//0: 첫 화면, 1: mode1, 2: mode2, 3: manual, 4: Exit 5: Ending
//font mode[6];
////////////////////////////////////////////

void pos_cam() {
	float dx;
	float dy;
	float dz;
	D3DXMATRIX mX;
	D3DXMATRIX mY;
	D3DXMATRIX mZ;

	dx = 0.0f;
	dy = 0.0f;
	dz = 0.0f;
	D3DXMatrixRotationY(&mX, dx);
	D3DXMatrixRotationX(&mY, dy);
	D3DXMatrixRotationZ(&mZ, dz);
	g_mWorld = mX * mY * mZ;
}
void rotate(CSphere& ball, float velocity_theta);
bool turn(bool _first_player) {
	if (_first_player == true)
		return false;
	else
		return true;
}

// -----------------------------------------------------------------------------
// Global variables
// -----------------------------------------------------------------------------
CWall   g_legoPlane;
CWall   g_legowall[4];
CSphere   g_sphere[4];
CSphere   g_target_blueball;
CWall   g_scoreBoard;
CLight   g_light;
CPlayer g_player[2] = { CPlayer(150),CPlayer(150) };
Sound intro, ddack;
double g_camera_pos[3] = { 0.0, 5.0, -8.0 };
int flag = 0;
int power_flag = 0;

D3DXIMAGE_INFO info;	// 디바이스가 능력을가진다. 불러올위치

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------

bool allStop(CSphere* p_Sphere) {

	int cnt = 0;

	for (int i = 0; i < 4; i++)
		if ((p_Sphere[i].getVelocity_X() == 0) && (p_Sphere[i].getVelocity_Z() == 0)) cnt++;

	if (cnt == 4) return true;
	else return false;
}

void destroyAllLegoBlock(void)
{
}

bool Setup()
{
	int i;

	D3DXMatrixIdentity(&g_mWorld);
	D3DXMatrixIdentity(&g_mView);
	D3DXMatrixIdentity(&g_mProj);

	intro.CreateDirectSound(hwnd);
	intro.LoadWave("intro.wav");
	intro.Play(FALSE);

	ddack.CreateDirectSound(hwnd);
	ddack.LoadWave("ddack.wav");

	// create plane and set the position
	if (false == g_legoPlane.create(Device, -1, -1, 9, 0.03f, 6, D3DCOLOR_XRGB(80, 188, 223))) return false;
	g_legoPlane.setPosition(0.0f, -0.0006f / 5, 0.0f);

	// create walls and set the position. note that there are four walls
	if (false == g_legowall[0].create(Device, -1, -1, 9, 0.3f, 0.12f, d3d::DARKRED)) return false;
	g_legowall[0].setPosition(0.0f, 0.12f, 3.06f);
	g_legowall[0].setIndex(0);
	if (false == g_legowall[1].create(Device, -1, -1, 9, 0.3f, 0.12f, d3d::DARKRED)) return false;
	g_legowall[1].setPosition(0.0f, 0.12f, -3.06f);
	g_legowall[1].setIndex(1);
	if (false == g_legowall[2].create(Device, -1, -1, 0.12f, 0.3f, 6.24f, d3d::DARKRED)) return false;
	g_legowall[2].setPosition(4.56f, 0.12f, 0.0f);
	g_legowall[2].setIndex(2);
	if (false == g_legowall[3].create(Device, -1, -1, 0.12f, 0.3f, 6.24f, d3d::DARKRED)) return false;
	g_legowall[3].setPosition(-4.56f, 0.12f, 0.0f);
	g_legowall[3].setIndex(3);

	// create four balls and set the position
	for (i = 0; i < 4; i++) {
		if (false == g_sphere[i].create(Device, sphereColor[i])) return false;
		g_sphere[i].setIndex(i);
		g_sphere[i].setCenter(spherePos[i][0], (float)M_RADIUS, spherePos[i][1]);
		g_sphere[i].setPower(0, 0);

	}

	// create scoreBorad and set the position
	if (false == g_legoPlane.create(Device, -1, -1, 9, 0.03f, 6, D3DCOLOR_XRGB(80, 188, 223))) return false;
	g_legoPlane.setPosition(0.0f, -0.0006f / 5, 0.0f);

	if (false == g_target_blueball.create(Device, d3d::BLUE)) return false;
	g_target_blueball.setCenter(.0f, (float)M_RADIUS, .0f);

	// light setting 
	D3DLIGHT9 lit;
	::ZeroMemory(&lit, sizeof(lit));
	lit.Type = D3DLIGHT_POINT;
	lit.Diffuse = d3d::WHITE;
	lit.Specular = d3d::WHITE * 0.9f;
	lit.Ambient = d3d::WHITE * 0.9f;
	lit.Position = D3DXVECTOR3(0.0f, 3.0f, 0.0f);
	lit.Range = 100.0f;
	lit.Attenuation0 = 0.0f;
	lit.Attenuation1 = 0.9f;
	lit.Attenuation2 = 0.0f;
	if (false == g_light.create(Device, lit, 0.1f))
		return false;
	

	// Position and aim the camera.
	D3DXVECTOR3 pos(0.0f, 5.0f, -8.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 2.0f, 0.0f);
	D3DXMatrixLookAtLH(&g_mView, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &g_mView);

	// Set the projection matrix.
	D3DXMatrixPerspectiveFovLH(&g_mProj, D3DX_PI / 4,
		(float)Width / (float)Height, 1.0f, 100.0f);
	Device->SetTransform(D3DTS_PROJECTION, &g_mProj);

	// Set render states.
	Device->SetRenderState(D3DRS_LIGHTING, TRUE);
	Device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

	g_light.setLight(Device, g_mWorld);
/*
	//set background image
	if (FAILED(D3DXCreateTextureFromFileEx(g_pd3dDevice, _T("./sample.jpg"),
		D3DX_DEFAULT_NONPOW2,	// 이미지가로크기   
		D3DX_DEFAULT_NONPOW2,	// 이미지세로크기 	// 자동으로 이미지의 크기에 맞추어 불러온다.
		1,		// Miplavels   //3D 상에서확대축소시사용한다.깨지는현상방지를위해
		0,		// Usage    //사용용도출력용도로는 0으로지정
		D3DFMT_A8R8G8B8,// 이미지색상포멧 //각 8bit 사용이미지
		D3DPOOL_MANAGED,// pool    //이미지관리다이렉트가직접관리  
		D3DX_FILTER_NONE,// Filter    //확대축소시사용하는필터   
		D3DX_FILTER_NONE,// MipFilter   //확대축소시사용하는필터   
		NULL,// 컬러키설정  //배경을지울때.. png는투명을가진다. 그래서컬러키사용안해도된다.   
		&info,// 불러온이미지의정보를저장할구조체   
		NULL,// 팔레트설정  //8bit 인경우색상표   
		&g_tex)))// 생성된텍스처의주소반환  
	{
		return false;
	}
	else {
		g_tex_width = info.Width;
		g_tex_height = info.Height;
	}
	return true;

	*/
	return true;
}

void Cleanup(void)
{
	g_legoPlane.destroy();
	for (int i = 0; i < 4; i++) {
		g_legowall[i].destroy();
	}
	destroyAllLegoBlock();
	g_light.destroy();
}

// timeDelta represents the time between the current image frame and the last image frame.
// the distance of moving balls should be "velocity * timeDelta"
bool Display(float timeDelta)
{
	int i = 0;
	int j = 0;

	if (Device)
	{
		if (menu == 0) {
			Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
			Device->BeginScene();
			//ys
			font* mode = mode->GetInst();

			mode->Init(Device, 60, 30);

			g_legoPlane.draw(Device, g_mWorld);
			for (i = 0; i < 4; i++) {
				g_legowall[i].draw(Device, g_mWorld);
				g_sphere[i].draw(Device, g_mWorld);
			}

			mode->Print("Amazing Great Billiard Game!!!", 150, 150, 0xFFFFFFFFF);
			mode->FreeInst();

			mode->GetInst();
			mode->Init(Device, 40, 25);
			mode->Print("1. Player-1 Mode", 380, 320, 0xFFFFFFFFF);
			mode->Print("2. Player-2 Mode", 380, 380, 0xFFFFFFFFF);
			mode->Print("3. Manual", 380, 440, 0xFFFFFFFFF);
			mode->Print("4. Exit", 380, 500, 0xFFFFFFFFF);

			/*char print_turn2[30] = " Turn!";
			strcat(print_spin_x, itoa(NULL, temp, 10));
			mode->Print(print_turn2, 610, 2, 0xFFFFFFFFF);
			
			for(int i = 0; i < 6; i++)
				mode[i]->Init(Device, 40, 20);
			mode[0]->Print("1. Player-1 Mode", 150, 440, 0xFFFFFFFFF);
			mode[1]->Print("2. Player-3 Mode", 350, 440, 0xFFFFFFFFF);
			mode[2]->Print("3. Manual", 550, 440, 0xFFFFFFFFF);
			mode[3]->Print("4. Exit", 750, 440, 0xFFFFFFFFF);
			*/
			mode->FreeInst();

			//g_legoPlane.draw(Device, g_mWorld);
			//g_light.draw(Device);
			//ys
			Device->EndScene();
			Device->Present(0, 0, 0, 0);
			Device->SetTexture(0, NULL);


		}
		else if (menu == 1) {
			Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
			Device->BeginScene();

			font* in3 = in3->GetInst();

			in3->Init(Device, 23, 15);

			g_player[0].setscore(0);
			char print_score1[30] = " 플레이어의 점수 = ";               //상단 스코어
			char temp[5];
			strcat(print_score1, itoa(g_player[0].printscore(), temp, 10));
			in3->Print(print_score1, 0, 2, 0xFFFFFFFFF);

			char print_power[30] = " 파워     = ";
			strcat(print_power, itoa(display_count * 10, temp, 10));
			in3->Print(print_power, 1000, 2, 0xFFFFFFFFF);

			char print_spin_x[30] = " 회전  x = ";
			strcat(print_spin_x, itoa(mesh_x_count, temp, 10));
			in3->Print(print_spin_x, 1000, 26, 0xFFFFFFFFF);

			char print_spin_y[30] = " y = ";
			strcat(print_spin_y, itoa(mesh_y_count, temp, 10));
			in3->Print(print_spin_y, 1063, 50, 0xFFFFFFFFF);

			in3->FreeInst();

			
			//delete &in;
			//Font->DrawTextA(0, "Hello, World", -1, &rt, DT_TOP | DT_LEFT, 0xFF00FFFF);
			//Font->DrawTextA(pSprite,score, 1, &rt, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
			//DrawText(pSprite, score, 1, &rt, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
			//DrawText()
			//////////////////////////////////////////////////////////////////////////////////////////////

			// update the position of each ball. during update, check whether each ball hit by walls.
			for (i = 0; i < 4; i++) {
				g_sphere[i].ballUpdate(timeDelta);
				for (j = 0; j < 4; j++) { g_legowall[i].hitBy(g_sphere[j]); }
			}

			// check whether any two balls hit together and update the direction of balls
			for (i = 0; i < 4; i++) {

				for (j = 0; j < 4; j++) {
					if (i <= j) { continue; }
					if (g_sphere[i].hasIntersected(g_sphere[j])) {
						ddack.Play(false);
					}
					g_sphere[i].hitBy(g_sphere[j]);
				}
			}
			if (allStop(g_sphere)) {                  //?????????????????????????????????????????????왜 매개인자 없어?
				int ball_index[2];
				int player_index;
				if (flag == 1)
				{
					/////////////////////////////////////////////////순서와 점수
					if (menu == 1) {

						if (g_sphere[3].gethasHit(2)) {   //빡
							g_player[0].getscore();
						}
						else if (g_sphere[3].gethasHit(0) && (g_sphere[3].gethasHit(1))) {   //점수
							g_player[0].losescore();
						}
						else {      //빡
							g_player[0].getscore();
						}
						flag = 0;
						for (int i = 0; i < 4; i++) {
							for (int j = 0; j < 4; j++) {
								g_sphere[i].sethasHit(j, 0);
							}
						}
					}
					else {


						if (g_player[0].get_first_player() == true) {
							ball_index[0] = 3;
							ball_index[1] = 2;
							player_index = 0;
						}
						else {
							ball_index[0] = 2;
							ball_index[1] = 3;
							player_index = 1;
						}

						if (g_sphere[ball_index[0]].gethasHit(ball_index[1])) {   //빡, 다음 사람으로
							g_player[player_index].losescore();
							g_player[0].turn();
							g_player[1].turn();
						}
						else if (g_sphere[ball_index[0]].gethasHit(0) && (g_sphere[ball_index[0]].gethasHit(1))) {   //점수, 한번 더
							g_player[player_index].getscore();
						}
						else if (g_sphere[ball_index[0]].gethasHit(0) || (g_sphere[ball_index[0]].gethasHit(1))) {      //다음 사람으로
							g_player[0].turn();
							g_player[1].turn();
						}
						else {      //빡, 다음 사람으로
							g_player[player_index].losescore();
							g_player[1].turn();
							g_player[0].turn();
						}
						flag = 0;
						for (int i = 0; i < 4; i++) {
							for (int j = 0; j < 4; j++) {
								g_sphere[i].sethasHit(j, 0);
							}
						}
					}
				}
			}


			g_legoPlane.draw(Device, g_mWorld);
			for (i = 0; i < 4; i++) {
				g_legowall[i].draw(Device, g_mWorld);
				g_sphere[i].draw(Device, g_mWorld);
			}
			if (allStop(g_sphere))
				g_target_blueball.draw(Device, g_mWorld);
			g_light.draw(Device);

			Device->EndScene();
			Device->Present(0, 0, 0, 0);
			Device->SetTexture(0, NULL);
		}
		/////////////////////////////////////////////////////////////////////////////////////////
		else if (menu == 2) {
			Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
			Device->BeginScene();
			ID3DXFont* Font;
			D3DXFONT_DESC DXFont_DESC;
			D3DXCreateFontIndirect(Device, &DXFont_DESC, &Font);
			RECT rt = { 0,10,0,0 };
			//RECT rt2 = { 10,40,0,0 };
			font* in = in->GetInst();

			in->Init(Device, 23, 15);

			char print_score1[30] = " 플레이어 1의 점수 = ";               //상단 스코어
			char temp[5];
			strcat(print_score1, itoa(g_player[0].printscore(), temp, 10));
			in->Print(print_score1, 0, 2, 0xFFFFFFFFF);

			char print_score2[30] = " 플레이어 2의 점수 = ";
			strcat(print_score2, itoa(g_player[1].printscore(), temp, 10));
			in->Print(print_score2, 0, 26, 0xFFFFFFFFF);

			char print_power[30] = " 파워     = ";
			strcat(print_power, itoa(display_count * 10, temp, 10));
			in->Print(print_power, 1000, 2, 0xFFFFFFFFF);

			char print_spin_x[30] = " 회전  x = ";
			strcat(print_spin_x, itoa(mesh_x_count, temp, 10));
			in->Print(print_spin_x, 1000, 26, 0xFFFFFFFFF);

			char print_spin_y[30] = " y = ";
			strcat(print_spin_y, itoa(mesh_y_count, temp, 10));
			in->Print(print_spin_y, 1063, 50, 0xFFFFFFFFF);

			in->FreeInst();

			////////////////////////////////////////////최종수정
			font* in2 = in2->GetInst();

			in2->Init(Device, 30, 20);
			char print_turn[30] = " Player ";
			int first_player_int;
			if (g_player[0].get_first_player())	first_player_int = 1;
			else first_player_int = 2;
			strcat(print_turn, itoa(first_player_int, temp, 10));
			in2->Print(print_turn, 450, 2, 0xFFFFFFFFF);

			char print_turn2[30] = " Turn!";
			strcat(print_spin_x, itoa(NULL, temp, 10));
			in2->Print(print_turn2, 610, 2, 0xFFFFFFFFF);

			in2->FreeInst();
			//delete &in;
			//Font->DrawTextA(0, "Hello, World", -1, &rt, DT_TOP | DT_LEFT, 0xFF00FFFF);
			//Font->DrawTextA(pSprite,score, 1, &rt, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
			//DrawText(pSprite, score, 1, &rt, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
			//DrawText()
			//////////////////////////////////////////////////////////////////////////////////////////////

			// update the position of each ball. during update, check whether each ball hit by walls.
			for (i = 0; i < 4; i++) {
				g_sphere[i].ballUpdate(timeDelta);
				for (j = 0; j < 4; j++) { g_legowall[i].hitBy(g_sphere[j]); }
			}

			// check whether any two balls hit together and update the direction of balls
			for (i = 0; i < 4; i++) {

				for (j = 0; j < 4; j++) {
					if (i <= j) { continue; }
					if (g_sphere[i].hasIntersected(g_sphere[j])) {
						ddack.Play(false);
					}
					g_sphere[i].hitBy(g_sphere[j]);
				}
			}
			if (allStop(g_sphere)) {                  //?????????????????????????????????????????????왜 매개인자 없어?
				int ball_index[2];
				int player_index;
				if (flag == 1)
				{
					/////////////////////////////////////////////////순서와 점수
					if (menu == 1) {
			
						if (g_sphere[3].gethasHit(2)) {   //빡
							g_player[0].getscore();
						}
						else if (g_sphere[3].gethasHit(0) && (g_sphere[3].gethasHit(1))) {   //점수
							g_player[0].losescore();
						}
						else {      //빡
							g_player[0].getscore();
						}
						flag = 0;
						for (int i = 0; i < 4; i++) {
							for (int j = 0; j < 4; j++) {
								g_sphere[i].sethasHit(j, 0);
							}
						}
					}
					else {


						if (g_player[0].get_first_player() == true) {
							ball_index[0] = 3;
							ball_index[1] = 2;
							player_index = 0;
						}
						else {
							ball_index[0] = 2;
							ball_index[1] = 3;
							player_index = 1;
						}

						if (g_sphere[ball_index[0]].gethasHit(ball_index[1])) {   //빡, 다음 사람으로
							g_player[player_index].losescore();
							g_player[0].turn();
							g_player[1].turn();
						}
						else if (g_sphere[ball_index[0]].gethasHit(0) && (g_sphere[ball_index[0]].gethasHit(1))) {   //점수, 한번 더
							g_player[player_index].getscore();
						}
						else if (g_sphere[ball_index[0]].gethasHit(0) || (g_sphere[ball_index[0]].gethasHit(1))) {      //다음 사람으로
							g_player[0].turn();
							g_player[1].turn();
						}
						else {      //빡, 다음 사람으로
							g_player[player_index].losescore();
							g_player[1].turn();
							g_player[0].turn();
						}
						flag = 0;
						for (int i = 0; i < 4; i++) {
							for (int j = 0; j < 4; j++) {
								g_sphere[i].sethasHit(j, 0);
							}
						}
					}
				}
			}


			// draw plane, walls, and spheres
			g_legoPlane.draw(Device, g_mWorld);
			for (i = 0; i < 4; i++) {
				g_legowall[i].draw(Device, g_mWorld);
				g_sphere[i].draw(Device, g_mWorld);
			}
			if (allStop(g_sphere))
				g_target_blueball.draw(Device, g_mWorld);
			g_light.draw(Device);

			Device->EndScene();
			Device->Present(0, 0, 0, 0);
			Device->SetTexture(0, NULL);

			if (g_player[0].printscore() == 0) {
				menu = 5;
			}
			else if (g_player[1].printscore() == 0) {
				menu = 5;
			}
			//추가
		}
		else if (menu == 3) {
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
		Device->BeginScene();
		//ys
		font* mode = mode->GetInst();

		mode->Init(Device, 70, 50);

		g_legoPlane.draw(Device, g_mWorld);
		for (i = 0; i < 4; i++) {
			g_legowall[i].draw(Device, g_mWorld);
			g_sphere[i].draw(Device, g_mWorld);
		}

		mode->Print("Manual", 420, 150, 0xFFFFFFFFF);
		mode->FreeInst();

		mode->GetInst();
		mode->Init(Device, 25, 15);
		mode->Print("Ball direction: Blue ball's positon By moving right mouse button", 80, 340, 0xFFFFFFFFF);
		mode->Print("Ball rotation control: By clicking direction key", 80, 390, 0xFFFFFFFFF);
		mode->Print("Power control: the time difference between the first and second space clicks", 80, 440, 0xFFFFFFFFF);
		mode->Print("Go to default point: By clicking tab key", 80, 490, 0xFFFFFFFFF);

		/*char print_turn2[30] = " Turn!";
		strcat(print_spin_x, itoa(NULL, temp, 10));
		mode->Print(print_turn2, 610, 2, 0xFFFFFFFFF);

		for(int i = 0; i < 6; i++)
		mode[i]->Init(Device, 40, 20);
		mode[0]->Print("1. Player-1 Mode", 150, 440, 0xFFFFFFFFF);
		mode[1]->Print("2. Player-3 Mode", 350, 440, 0xFFFFFFFFF);
		mode[2]->Print("3. Manual", 550, 440, 0xFFFFFFFFF);
		mode[3]->Print("4. Exit", 750, 440, 0xFFFFFFFFF);
		*/
		mode->FreeInst();

		//g_legoPlane.draw(Device, g_mWorld);
		//g_light.draw(Device);
		//ys
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
		Device->SetTexture(0, NULL);

	  }
		else if (menu == 5) {
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00afafaf, 1.0f, 0);
		Device->BeginScene();
		//ys
		font* mode = mode->GetInst();

		mode->Init(Device, 60, 30);

		g_legoPlane.draw(Device, g_mWorld);
		for (i = 0; i < 4; i++) {
			g_legowall[i].draw(Device, g_mWorld);
			g_sphere[i].draw(Device, g_mWorld);
		}

		mode->Print("Game Over!!!", 400, 150, 0xFFFFFFFFF);
		mode->FreeInst();

		mode->GetInst();

		mode->Init(Device, 50, 20);

		char print_end1[30] = "Player";
		int first_player_int = 1;
		char temp[5];
		//if (g_player[0].get_first_player())	first_player_int = 1;
		//else first_player_int = 2;
		strcat(print_end1, itoa(first_player_int, temp, 10));
		mode->Print(print_end1, 400, 380, 0xFFFFFFFFF);

		char print_end2[30] = " Win!";
		mode->Print(print_end2, 550, 380, 0xFFFFFFFFF);
		mode->Print("Press ESC to Exit...", 380, 480, 0xFFFFFFFFF);
		mode->FreeInst();

		//g_legoPlane.draw(Device, g_mWorld);
		//g_light.draw(Device);
		//ys
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
		Device->SetTexture(0, NULL);
		}


		if (power_flag == 0) {
			if (first_space && display_count < 10)
				display_count += 0.07f;
			else
				power_flag = 1;
		}
		else {
			if (first_space && display_count > 0)
				display_count -= 0.07f;
			else
				power_flag = 0;
		}
	}
	return true;
}

LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool wire = false;
	static bool isReset = true;
	static int old_x = 0;
	static int old_y = 0;
	static enum { WORLD_MOVE, LIGHT_MOVE, BLOCK_MOVE } move = WORLD_MOVE;
	if (menu == 0) {///////////////////////////////////////////////////////최종수정
		switch (msg) {
		case WM_KEYDOWN: {
			switch (wParam) {
			case 0x31:		//1번 메뉴 선택
				menu = 1;
				break;
			case 0x32:		//2번 메뉴 선택
				menu = 2;
				break;
			case 0x33:		//3번 메뉴 선택
				menu = 3;
				break;
			case 0x34:		//4번 메뉴 선택
				::DestroyWindow(hwnd);
				break;
			case VK_ESCAPE:
				::DestroyWindow(hwnd);
				break;
			}

		}


		}
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}
	else if (menu == 3) {							//메뉴얼일때
		switch (msg) {
		case WM_KEYDOWN: {
			switch (wParam) {
			case VK_BACK:
				menu = 0;
				break;
			}

		}


		}
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}
	else if (menu == 5) {							//메뉴얼일때
		switch (msg) {
		case WM_KEYDOWN: {
			switch (wParam) {
			case VK_ESCAPE:
				::DestroyWindow(hwnd);
				break;
			}

		}


		}
		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}
	else {
		switch (msg) {
		case WM_DESTROY:
		{
			::PostQuitMessage(0);
			break;
		}
		case WM_KEYDOWN:
		{
			switch (wParam) {
				////////////////////////////////////////////////////////////////////////////////////
			case VK_UP:
				if (mesh_y_count < 100) {
					mesh_y_count += 10;
					mesh_y += 0.021f;
				}
				else {
					mesh_y_count == 100;
					mesh_y == M_RADIUS;
				}
				break;
			case VK_DOWN:
				if (mesh_y_count > -100) {
					mesh_y_count -= 10;
					mesh_y -= 0.021f;
				}
				else {
					mesh_y_count == -100;
					mesh_y == -M_RADIUS;
				}
				break;
			case VK_RIGHT:
				if (mesh_x_count < 100) {
					mesh_x_count += 10;
					mesh_x += 0.021f;
				}
				else {
					mesh_x_count == 100;
					mesh_x == M_RADIUS;
				}
				break;
			case VK_LEFT:
				if (mesh_x_count > -100) {
					mesh_x_count -= 10;
					mesh_x -= 0.021f;
				}
				else {
					mesh_x_count == -100;
					mesh_x == -M_RADIUS;
				}
				break;

				///////////////////////////////////////////////////////////////////////////////////
			case VK_ESCAPE:
				::DestroyWindow(hwnd);
				break;
			case VK_RETURN:
				if (NULL != Device) {
					wire = !wire;
					Device->SetRenderState(D3DRS_FILLMODE,
						(wire ? D3DFILL_WIREFRAME : D3DFILL_SOLID));
				}
				break;
			case VK_TAB:
				if (NULL != Device) {
					pos_cam();
				}
				break;
			case VK_SPACE:
				if (!allStop(g_sphere)) break;         //다 안멈췄으면 동작 무시

				if (!first_space) {                  //첫번째 space일때
													 //타임 저장 함수 호출
													 //파워 표시 함수 호출
													 //Sleep(1000);
					display_count = 0;
					first_space = true;
				}
				else {                           //두번째 space일때

												 //////////////////////////////////////////////////////////////////
												 //추가
												 //if (GetKeyState(VK_SPACE) < 0) {
												 //   first_space = true;
												 //}
												 //else
												 //   first_space = false;

												 ///////////////////////////////////////////////////////////////////
					D3DXVECTOR3 targetpos = g_target_blueball.getCenter();
					D3DXVECTOR3   whitepos;
					if (menu == 2) {
						if (g_player[0].get_first_player() == true) {
							whitepos = g_sphere[3].getCenter();
						}
						else {
							whitepos = g_sphere[2].getCenter();
						}
					}
					else {
						whitepos = g_sphere[3].getCenter();
					}
					double theta = acos(sqrt(pow(targetpos.x - whitepos.x, 2)) / sqrt(pow(targetpos.x - whitepos.x, 2) +
						pow(targetpos.z - whitepos.z, 2)));
					if (targetpos.z - whitepos.z <= 0 && targetpos.x - whitepos.x >= 0) { theta = -theta; }
					if (targetpos.z - whitepos.z >= 0 && targetpos.x - whitepos.x <= 0) { theta = PI - theta; }
					if (targetpos.z - whitepos.z <= 0 && targetpos.x - whitepos.x <= 0) { theta = PI + theta; }
					double distance = sqrt(pow(targetpos.x - whitepos.x, 2) + pow(targetpos.z - whitepos.z, 2));
					//display_count;
					if (menu == 2) {
						if (g_player[0].get_first_player() == true) {
							g_sphere[3].setPower(display_count * cos(theta), display_count * sin(theta));
							g_sphere[3].setTork(mesh_x, mesh_y);
						}
						else {
							g_sphere[2].setPower(display_count * cos(theta), display_count * sin(theta));
							g_sphere[2].setTork(mesh_x, mesh_y);
						}
					}
					else {
						g_sphere[3].setPower(display_count * cos(theta), display_count * sin(theta));
						g_sphere[3].setTork(mesh_x, mesh_y);
					}

					first_space = false;

					flag = 1;
					break;
				}
			}
			break;
		}

		case WM_MOUSEMOVE:
		{
			int new_x = LOWORD(lParam);
			int new_y = HIWORD(lParam);
			float dx;
			float dy;

			if (LOWORD(wParam) & MK_LBUTTON) {

				if (isReset) {
					isReset = false;
				}
				else {
					D3DXVECTOR3 vDist;
					D3DXVECTOR3 vTrans;
					D3DXMATRIX mTrans;
					D3DXMATRIX mX;
					D3DXMATRIX mY;

					switch (move) {
					case WORLD_MOVE:
						dx = (old_x - new_x) * 0.01f;
						dy = (old_y - new_y) * 0.01f;
						D3DXMatrixRotationY(&mX, dx);
						D3DXMatrixRotationX(&mY, dy);
						g_mWorld = g_mWorld * mX * mY;

						break;
					}
				}

				old_x = new_x;
				old_y = new_y;

			}
			else {
				isReset = true;

				if (LOWORD(wParam) & MK_RBUTTON) {
					D3DXVECTOR3 coord3d = g_target_blueball.getCenter();

					if (coord3d.x > 4.5) {
						coord3d.x = 4.4999999f;
					}
					if (coord3d.x < -4.5) {
						coord3d.x = -4.4999999f;
					}
					if (coord3d.z > 3) {
						coord3d.z = 2.9999999f;
					}
					if (coord3d.z < -3) {
						coord3d.z = -2.9999999f;
					}
					dx = (old_x - new_x);// * 0.01f;
					dy = (old_y - new_y);// * 0.01f;

					g_target_blueball.setCenter(coord3d.x + dx * (-0.007f), coord3d.y, coord3d.z + dy * 0.007f);
				}
				old_x = new_x;
				old_y = new_y;

				move = WORLD_MOVE;
			}
			break;
		}
		}

		return ::DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	srand(static_cast<unsigned int>(time(NULL)));

	if (!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device, &hwnd))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
	//d3d::EnterMsgLoop(Display_Main);
	if (!Setup())
		if (!Setup())
		{
			::MessageBox(0, "Setup() - FAILED", 0, 0);
			return 0;
		}
	g_player[0].set_first_player(true);
	g_player[1].set_first_player(false);
	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}
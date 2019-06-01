//機三甲 E14056431 趙珈葦 版權所有

#include "stdafx.h"
#include "InputStl.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <gl\GLee.h>
#include <gl\GL.H>
#include <gl\GLU.H>
#include <gl\GLAUX.H>
#include <gl\GLUT.H>
#include <cmath>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glaux.lib")
#pragma comment(lib,"glut32.lib")
#pragma comment(lib, "GLee.lib" )

using std::cout;
using std::cin;
using std::string;

//光照參數(白光)
GLfloat pos[] = { -100.0f, -100.0f, 100.0f, 0.0f };
GLfloat ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat spot[] = { 1.0f, 1.0f, -1.0f };               

//視角旋轉
GLfloat xRot_Ini = 15.0f;//初始視角角度
GLfloat xRot = 0.0f;
GLfloat yRot = 0.0f;

//VBO模型使用變數
unsigned int VBO_index[2];
float *VBOv, *VBOn;
STriangle * triArray = NULL;
int nTriangles = 0;
float cent[3];

//===================================自定義===================================

//定義球球 x座標.y座標.V速度.W轉速
typedef struct{
	GLfloat x;  //X座標
	GLfloat z;  //Z座標
	GLfloat Vx; //X方向速度
	GLfloat Vz; //Z方向速度
	GLfloat W_h; //左右塞給的y軸轉速
	GLfloat W_v; //推拉桿給的x軸轉速
}Ball;


Ball balls[10];//球球物件(10顆球)
GLfloat r = 28.575f;//球的半徑


int mode = 0;//0是白球視角，1是俯視
int Hitting_Position = 5; //0~8表中央、各塞
int strength = 1;//1~10表力道，初速=力道*10
GLfloat z_adj, y_adj;//球桿修正量

GLfloat keep = -15.0f;//擊球運桿距離
GLfloat keep_buffer;

int hit_available = 1;//全球靜止時才能擊球

GLfloat Hitting_Angle;
int controlled = 0;
int count;
//============================================================================

//寫入STL檔
void InitVBO(void)
{
	//寫入球桿數據
	VBOv = new float[nTriangles * 9];
	VBOn = new float[nTriangles * 9];
	int i;

	for (i = 0; i < nTriangles; i++)
	{
		VBOv[i * 9 + 0] = triArray[i].a[0];
		VBOv[i * 9 + 1] = triArray[i].a[1];
		VBOv[i * 9 + 2] = triArray[i].a[2];

		VBOv[i * 9 + 3] = triArray[i].b[0];
		VBOv[i * 9 + 4] = triArray[i].b[1];
		VBOv[i * 9 + 5] = triArray[i].b[2];

		VBOv[i * 9 + 6] = triArray[i].c[0];
		VBOv[i * 9 + 7] = triArray[i].c[1];
		VBOv[i * 9 + 8] = triArray[i].c[2];

		VBOn[i * 9 + 0] = triArray[i].n[0];
		VBOn[i * 9 + 1] = triArray[i].n[1];
		VBOn[i * 9 + 2] = triArray[i].n[2];

		VBOn[i * 9 + 3] = triArray[i].n[0];
		VBOn[i * 9 + 4] = triArray[i].n[1];
		VBOn[i * 9 + 5] = triArray[i].n[2];

		VBOn[i * 9 + 6] = triArray[i].n[0];
		VBOn[i * 9 + 7] = triArray[i].n[1];
		VBOn[i * 9 + 8] = triArray[i].n[2];
	}

	glGenBuffers(2, VBO_index);

	// Vertex
	glBindBuffer(GL_ARRAY_BUFFER, VBO_index[0]);
	glBufferData(GL_ARRAY_BUFFER, nTriangles * 9 * sizeof(float), VBOv, GL_STATIC_DRAW);

	Sleep(200);
	// Normal
	glBindBuffer(GL_ARRAY_BUFFER, VBO_index[1]);
	glBufferData(GL_ARRAY_BUFFER, nTriangles * 9 * sizeof(float), VBOn, GL_STATIC_DRAW);
}

//調整兩球行為模式(根據其數據、位置關係)
void motion(Ball *a, Ball *b){

	GLfloat X, Z, V, angle_hit, angle_V, angle_N, angle;

	//因為球質量相等，動量守恆 -> 撞擊前後的 X.Z方向球速總和相同
	//並我暫且假設兩球撞擊後速度夾角為90度

	//首先計算兩方向的速度和
	X = a->Vx + b->Vx;
	Z = a->Vz + b->Vz;
	V = sqrt(X*X + Z*Z);//此為合速度值

	
	angle_hit = atan2((b->x - a->x) , (b->z - a->z));//此為撞擊角度(a -> b)
	angle_V = atan2(X , Z);
	angle = angle_V - angle_hit;
	if (angle_V >= angle_hit){
		angle_N = angle_hit + 90 / 3.1415926 * 180;
	}
	else{ angle_N = angle_hit - 90 / 3.1415926 * 180; }
	

	if (angle_hit == angle_V){
		if (X > 0 && a->x > b->x){
			a->Vx = X;
			a->Vz = Z;
			b->Vx = 0;
			b->Vz = 0;
		}
		else if (X < 0 && a->x < b->x){
			a->Vx = X;
			a->Vz = Z;
			b->Vx = 0;
			b->Vz = 0;
		}
		else if (Z > 0 && a->z > b->z){
			a->Vx = X;
			a->Vz = Z;
			b->Vx = 0;
			b->Vz = 0;
		}
		else if (Z < 0 && a->z < b->z){
			a->Vx = X;
			a->Vz = Z;
			b->Vx = 0;
			b->Vz = 0;
		}
		else{
			a->Vx = 0;
			a->Vz = 0;
			b->Vx = X;
			b->Vz = Z;
		}
		return;
	}

	b->Vx = abs(V*cos(angle))*sin(angle_V);
	b->Vz = abs(V*cos(angle))*cos(angle_V);
	a->Vx = abs(V*sin(angle))*sin(angle_N);
	a->Vz = abs(V*sin(angle))*cos(angle_N);

	return;
}

//判定兩顆球撞擊，若撞擊則回傳1
int Hit(Ball a, Ball b, GLfloat r){

	if (sqrt((a.x - b.x)*(a.x - b.x) + (a.z - b.z)*(a.z - b.z)) <= r * 2 + 5){
		return 1;
	}
	else{ return 0; }
}


void KeyboardKeys(unsigned char key, int x, int y)
{
	//力道控制按鍵( X:提高力道 Z:降低力道 )					//力道分十級(1 ~ 10)，用strength儲存
	//X - 提高力道										//設定擊球後初速為 strengt*10
	if (strength < 10 && key == 'x'){					
		strength++;										//strength: 力道變數
		keep -= 7;										//keep: 球桿與球距離 (力道提高會離球較遠，做出加長運桿距離效果
		keep_buffer = keep;								//keep_buffer: 因出桿會瞬間使球桿向前，需要儲存現在位置便於回復
	}
	if (strength > 1 && key == 'z'){
		strength--;
		keep += 7;
		keep_buffer = keep;
	}

	//控制擊球位置 - 推拉定/下塞( ASWD上下左右控制擊球位置 - 九宮格選擇 )
	if (Hitting_Position < 7 && key == 's'){
		Hitting_Position += 3;
		mode = 0;
		controlled = 0;
	}
	if (Hitting_Position > 1 && key == 'w'){
		Hitting_Position -= 3;
		mode = 0;
		controlled = 0;
	}
	if (Hitting_Position % 3 != 0 && key == 'd'){
		Hitting_Position += 1;
		mode = 0;
		controlled = 0;
	}
	if (Hitting_Position % 3 != 1 && key == 'a'){
		Hitting_Position -= 1;
		mode = 0;
		controlled = 0;
	}

	//Enter鍵: 視角選擇 - 使mode值在 1 和 0 間互換
	if (key == 13){
		mode = 1 - mode;
		controlled = 1;
	}

	//空白鍵: 擊球															
	if (hit_available == 1 && mode == 0 && key == 32){						//擊球條件: 所有球速均須為0 + 白球視角
		keep = 0.0f;														//擊球後，球桿向前觸球(變更keep值為0)
		balls[0].Vx = strength * 10 * sin(yRot * 3.1415926 / 180);			//擊球後，賦予白球初速為 strength*10 至Vx,Vz
		balls[0].Vz = -strength * 10 * cos(yRot * 3.1415926 / 180);
		mode = 1;															//更改視角至俯視，方便觀看球局
		
		//左右塞轉速
		if (Hitting_Position % 3 == 1){						//偏左 - 1.4.7
			balls[0].W_h = - strength * 4;
		}
		else if (Hitting_Position % 3 == 0){				//偏右 - 3.6.9
			balls[0].W_h = strength * 4;
		}

		//推拉桿轉速
		if (Hitting_Position <= 3){							//偏上 - 1.2.3
			balls[0].W_v = strength * 4;
		}
		else if (Hitting_Position >= 7){					//偏下 - 7.8.9
			balls[0].W_v = - strength * 4;
		}
	}
	
	// Refresh the Window
	glutPostRedisplay();
}

void TimerFunction(int value){

	int j = 0;
	int count = 0;
	int count_buffer;
	GLfloat X_Ini;
	GLfloat Z_Ini;
	GLfloat X_buffer;
	GLfloat Z_buffer;

	//依據每顆球 X、Z 方向速度分量更新球的位置
	for (int i = 0; i <= 9; i++){
		X_buffer = 0.0f;
		Z_buffer = 0.0f;
		count_buffer = 0;
		j = 0;
		X_Ini = balls[i].x;
		Z_Ini = balls[i].z;
		
		if (balls[i].Vx != 0 || balls[i].Vz != 0){
			while (j < 10){
				count = 0;
				balls[i].x = X_Ini;
				balls[i].z = Z_Ini;
				if (j != i){
					while (count < 3000){
						balls[i].x += balls[i].Vx / 3000;
						balls[i].z += balls[i].Vz / 3000;
						count++;
						if (sqrt((balls[i].x - balls[j].x)*(balls[i].x - balls[j].x) + (balls[i].z - balls[j].z)*(balls[i].z - balls[j].z)) < 2 * r){
							balls[i].x -= balls[i].Vx / 3000;
							balls[i].z -= balls[i].Vz / 3000;
							if (count <= count_buffer){
								X_buffer = balls[i].x;
								Z_buffer = balls[i].z;
								count_buffer = count ;
							}
							break;
						}		
					}
				}
				j++;
			}
			if (X_buffer == 0.0f && Z_buffer == 0.0f){
				X_Ini += balls[i].Vx;
				Z_Ini += balls[i].Vz;
				balls[i].x = X_Ini;
				balls[i].z = Z_Ini;
			}
			else{
				balls[i].x = X_buffer;
				balls[i].z = Z_buffer;

			}

			for (int p = 0; p <= 9; p++){
				if (i != p){
					if (Hit(balls[i], balls[p], r) == 1){
						motion(&balls[i], &balls[p]);
					}
				}
			}
		}
		
		//*****球的減速系統(目前簡化為沿移動方向其反向等速率減速)*****
		
		//首先計算當前球速
		GLfloat unit_v = sqrt(balls[i].Vx * balls[i].Vx + balls[i].Vz * balls[i].Vz);

		//預防球速爆表
		if (unit_v > 100){
			balls[i].Vx = 100 * balls[i].Vx / unit_v; 
			balls[i].Vz = 100 * balls[i].Vz / unit_v; 
		}

		//加速度 = -0.6
		balls[i].Vx -= 0.6 * balls[i].Vx / unit_v; //0.6 * sin
		balls[i].Vz -= 0.6 * balls[i].Vz / unit_v; //0.6 * cos

		//球速度接近0使其停下
		if (abs(unit_v) <= 0.3f){
			balls[i].Vx = 0;
			balls[i].Vz = 0;
		}


		//邊界判定 - 遇邊界時將速度一分量更改為負
		if (balls[i].x < -1270 + r){
			balls[i].x = -1270 + r + 2;
			balls[i].Vx = -balls[i].Vx;

			balls[i].Vz -= balls[i].W_h*0.7; //下塞並歸零轉速
			balls[i].W_h = 0;

		}
		if (balls[i].x > 1270 - r){
			balls[i].x = 1270 - r - 2;
			balls[i].Vx = -balls[i].Vx;

			balls[i].Vz += balls[i].W_h*0.7;
			balls[i].W_h = 0;

	
		}
		if (balls[i].z > -r){
			balls[i].z = -r - 2;
			balls[i].Vz = -balls[i].Vz;
	
			balls[i].Vx -= balls[i].W_h*0.7;
			balls[i].W_h = 0;
	
		}
		if (balls[i].z < -1270 + r){
			balls[i].z = -1270 + r + 2;
			balls[i].Vz = -balls[i].Vz;
	
			balls[i].Vx += balls[i].W_h*0.7;
			balls[i].W_h = 0;

		}
		
	}

	//*******擊球許可設定*******

	hit_available = 1;//預設可擊球

	for (int i = 0; i < 10; i++){
		if (balls[i].Vx != 0 || balls[i].Vz != 0){   //若有一球某速度分量非零
			hit_available = 0;						 //收回擊球許可(設值為0)
			break;
		}
	}
	//此判斷式是為能觀察全場情況而設(否則當球皆靜止，按下enter改變視角將直接跳回白球視角)
	if (controlled == 0){
		if (hit_available == 1){					 //若經過上面迴圈仍能擊球
			keep = keep_buffer;						 //將球桿退回運桿前位置
			mode = 0;								 //並將視角改回白球視角
		}
	}
	//************************


	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 1);
}

void RenderScene(void)
{
	//=============================基本設定及光源設定================================
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	glPushMatrix();
	glPushMatrix();

	glTranslatef(pos[0], pos[1], pos[2]);

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot);

	glPopMatrix();
	//============================================================================

	//================================白球視角繪圖==================================

	//僅能以白球視角瞄球、擊球，因此只在此視角畫出球桿

	if (mode == 0){

		//針對視角調整座標( 斜上方視角 + 瞄球旋轉 )
		glTranslatef(0.0f, -50.0f, -500.0f);
		glRotatef(xRot + xRot_Ini, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	

		//畫球桿 ( 以擊球位置計算繪圖偏移量 )
		if (hit_available == 1){
			//左右位置判定
			if (Hitting_Position % 3 == 1){						//偏左 - 1.4.7
				z_adj = -20.0f;
			}
			else if (Hitting_Position % 3 == 0){				//偏右 - 3.6.9
				z_adj = 20.0f;
			}
			else{ z_adj = 0; }									//中間 - 2.5.8

			//上下位置判定
			if (Hitting_Position <= 3){							//偏上 - 1.2.3
				y_adj = 20.0f;
			}
			else if (Hitting_Position >= 7){					//偏下 - 7.8.9
				y_adj = -20.0f;
			}
			else{ y_adj = 0; }									//中間 - 4.5.6


			//開始繪圖
			glPushMatrix();
			glRotatef(90 - yRot, 0.0f, 1.0f, 0.0f); //旋轉角度						  //X方向: 考慮桿長、運桿距離
			glTranslatef(-cent[0] - 307 + keep, -cent[1] + y_adj, -cent[2] + z_adj);  //Y方向: 考慮上下偏移
			glColor3f(0.22f, 0.22f, 0.22f);											  //Z方向: 考慮左右偏移

			//以VBO畫三角網格
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			// Vertex
			glBindBuffer(GL_ARRAY_BUFFER, VBO_index[0]);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			// Normal
			glBindBuffer(GL_ARRAY_BUFFER, VBO_index[1]);
			glNormalPointer(GL_FLOAT, 0, 0);

			glDrawArrays(GL_TRIANGLES, 0, nTriangles * 3);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);

			glPopMatrix();
		}
		glTranslatef(-balls[0].x, -r, -balls[0].z);//此模式時，以白球為中心去畫出其他球
	}
	//============================================================================


	//================================俯視視角繪圖==================================

	if (mode == 1){

		//針對此模式改變座標 ( 旋轉視角 + 斜上方視角 )
		glRotatef(80, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, -1800.0f, 300.0f);
	}

	//畫球桌 - 綠色長方形
	glPushMatrix();
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.25f, 0.0f);
	glRectf(-1270.0f, 0.0f, 1270.0f, 1270.0f);
	glPopMatrix();


	//***以下畫球***

	//白球
	glPushMatrix();
	glTranslatef(balls[0].x, r, balls[0].z);
	glColor3f(1.0f, 1.0f, 1.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//一號(黃)
	glPushMatrix();
	glTranslatef(balls[1].x, r, balls[1].z);
	glColor3f(0.7f, 0.5f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//二號(藍)
	glPushMatrix();
	glTranslatef(balls[2].x, r, balls[2].z);
	glColor3f(0.0f, 0.0f, 0.7f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//三號(亮橘)
	glPushMatrix();
	glTranslatef(balls[3].x, r, balls[3].z);
	glColor3f(0.75f, 0.0f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//四號(紫)
	glPushMatrix();
	glTranslatef(balls[4].x, r, balls[4].z);
	glColor3f(0.5f, 0.0f, 0.7f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//五號(橘)
	glPushMatrix();
	glTranslatef(balls[5].x, r, balls[5].z);
	glColor3f(0.9f, 0.3f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//六號(綠)
	glPushMatrix();
	glTranslatef(balls[6].x, r, balls[6].z);
	glColor3f(0.0f, 0.2f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//七號(褐)
	glPushMatrix();
	glTranslatef(balls[7].x, r, balls[7].z);
	glColor3f(0.3f, 0.1f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//八號(黑)
	glPushMatrix();
	glTranslatef(balls[8].x, r, balls[8].z);
	glColor3f(0.0f, 0.0f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//九號(土黃 - 代替)
	glPushMatrix();
	glTranslatef(balls[9].x, r, balls[9].z);
	glColor3f(0.3f, 0.3f, 0.1f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	glPopMatrix();

	//***畫球結束***
	//============================================================================

	//=================================力道顯示區===================================
	
	//僅在能擊球(球速均為0時 + 白球視角)才會顯示
	if (hit_available == 1 && mode == 0){
		glPushMatrix();
		glDisable(GL_LIGHTING);//不受光照影響
		glTranslatef(-73.0f, -55.0f, -100.0f);//設定位置
		glColor3f(0.5f, 0.0f, 0.0f);//深紅色

		//strength是多少就畫幾個方形
		for (int i = 0; i < strength; i++){
			glRectf(0.0f, 0.0f, 2.0f, 10.0f);
			glTranslatef(3.0f, 0.0f, 0.0f);
		}

		glEnable(GL_LIGHTING);
		glPopMatrix();
	}
	//============================================================================


	glutSwapBuffers();
}

void SetupRC()
{
	//==================================基本設定====================================
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15.0f);           

	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	//============================================================================


	//===============================初始化球球位置=================================

	balls[0].x = 350.0f;
	balls[0].z = -335.0f;

	balls[1].x = -360.0f;
	balls[1].z = -635.0f;

	balls[2].x = 0.0f;
	balls[2].z = -850.0f;

	balls[3].x = -800.0f;
	balls[3].z = -350.0f;

	balls[4].x = 1000.0f;
	balls[4].z = -1100.0f;
	 
	balls[5].x = 650.0f;
	balls[5].z = -50.0f;

	balls[6].x = -1200.0f;
	balls[6].z = -780.0f;

	balls[7].x = 200.0f;
	balls[7].z = -950.0f;

	balls[8].x = 1000.0f;
	balls[8].z = -300.0f;

	balls[9].x = 60.0f;
	balls[9].z = -635.0f;

	//============================================================================


	InitVBO();
}

void SpecialKeys(int key, int x, int y)
{
	//方向鍵左右控制Y軸旋轉( 瞄球方向 )， 上下控制X軸旋轉( 視角調整 )

	if (key == GLUT_KEY_UP){
		xRot -= 2.0f;
		mode = 0;
		controlled = 0;
	}

	if (key == GLUT_KEY_DOWN){
		xRot += 2.0f;
		mode = 0;
		controlled = 0;
	}

	if (key == GLUT_KEY_LEFT){
		yRot -= 2.0f;
		mode = 0;
		controlled = 0;
	}

	if (key == GLUT_KEY_RIGHT){
		yRot += 2.0f;
		mode = 0;
		controlled = 0;
	}

	if (key > 356.0f)
		xRot = 0.0f;

	if (key < -1.0f)
		xRot = 355.0f;

	if (key > 356.0f)
		yRot = 0.0f;

	if (key < -1.0f)
		yRot = 355.0f;

	glutPostRedisplay();
}

void ChangeSize(int w, int h)
{
	GLfloat fAspect;

	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	fAspect = (GLfloat)w / (GLfloat)h;

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Produce the perspective projection
	gluPerspective(60.0f, fAspect, 1.0, 5000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char* argv[])
{
	//================================球桿VBO處理==================================

	VBOv = 0; VBOn = 0;
	triArray = LoadStlBinary("Part1.STL", nTriangles);
	cout << "***STL檔案讀取狀況***\n\n";


	if (triArray)
	{
		float min[3] = { 1e7, 1e7, 1e7 };
		float max[3] = { -1e7, -1e7, -1e7 };
		for (int i = 0; i < nTriangles; i++)
		{
			//a
			if (triArray[i].a[0] < min[0])
				min[0] = triArray[i].a[0];
			else if (triArray[i].a[0] > max[0])
				max[0] = triArray[i].a[0];

			if (triArray[i].a[1] < min[1])
				min[1] = triArray[i].a[1];
			else if (triArray[i].a[1] > max[1])
				max[1] = triArray[i].a[1];

			if (triArray[i].a[2] < min[2])
				min[2] = triArray[i].a[2];
			else if (triArray[i].a[2] > max[2])
				max[2] = triArray[i].a[2];

			//b
			if (triArray[i].b[0] < min[0])
				min[0] = triArray[i].b[0];
			else if (triArray[i].b[0] > max[0])
				max[0] = triArray[i].b[0];

			if (triArray[i].b[1] < min[1])
				min[1] = triArray[i].b[1];
			else if (triArray[i].b[1] > max[1])
				max[1] = triArray[i].b[1];

			if (triArray[i].b[2] < min[2])
				min[2] = triArray[i].b[2];
			else if (triArray[i].b[2] > max[2])
				max[2] = triArray[i].b[2];

			//c
			if (triArray[i].c[0] < min[0])
				min[0] = triArray[i].c[0];
			else if (triArray[i].c[0] > max[0])
				max[0] = triArray[i].c[0];

			if (triArray[i].c[1] < min[1])
				min[1] = triArray[i].c[1];
			else if (triArray[i].c[1] > max[1])
				max[1] = triArray[i].c[1];

			if (triArray[i].c[2] < min[2])
				min[2] = triArray[i].c[2];
			else if (triArray[i].c[2] > max[2])
				max[2] = triArray[i].c[2];
		}
		cent[0] = (min[0] + max[0]) / 2.0f;
		cent[1] = (min[1] + max[1]) / 2.0f;
		cent[2] = (min[2] + max[2]) / 2.0f;
		cout << "Successfully imported into the STL file!!\n"
			<< "In this model-bag:" << nTriangles << "Triangles\n"
			<< "The center is at ( " << cent[0] << " , " << cent[1] << " , " << cent[2] << " )\n\n";
	}
	else
	{
		cout << "\n Importing the model failed!!\n\n";
	}
	//============================================================================


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("E14056431_HW4");


	SetupRC();
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	glutKeyboardFunc(KeyboardKeys);
	glutTimerFunc(33, TimerFunction, 1);
	glutMainLoop();



	if (VBOv)
		delete[] VBOv;
	if (VBOn)
		delete[] VBOn;	



	return 0;
}


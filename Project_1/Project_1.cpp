//���T�� E14056431 ���ɸ� ���v�Ҧ�

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

//���ӰѼ�(�ե�)
GLfloat pos[] = { -100.0f, -100.0f, 100.0f, 0.0f };
GLfloat ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat specref[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat spot[] = { 1.0f, 1.0f, -1.0f };               

//��������
GLfloat xRot_Ini = 15.0f;//��l��������
GLfloat xRot = 0.0f;
GLfloat yRot = 0.0f;

//VBO�ҫ��ϥ��ܼ�
unsigned int VBO_index[2];
float *VBOv, *VBOn;
STriangle * triArray = NULL;
int nTriangles = 0;
float cent[3];

//===================================�۩w�q===================================

//�w�q�y�y x�y��.y�y��.V�t��.W��t
typedef struct{
	GLfloat x;  //X�y��
	GLfloat z;  //Z�y��
	GLfloat Vx; //X��V�t��
	GLfloat Vz; //Z��V�t��
	GLfloat W_h; //���k�뵹��y�b��t
	GLfloat W_v; //���Ա쵹��x�b��t
}Ball;


Ball balls[10];//�y�y����(10���y)
GLfloat r = 28.575f;//�y���b�|


int mode = 0;//0�O�ղy�����A1�O����
int Hitting_Position = 5; //0~8�����B�U��
int strength = 1;//1~10��O�D�A��t=�O�D*10
GLfloat z_adj, y_adj;//�y��ץ��q

GLfloat keep = -15.0f;//���y�B��Z��
GLfloat keep_buffer;

int hit_available = 1;//���y�R��ɤ~�����y

GLfloat Hitting_Angle;
int controlled = 0;
int count;
//============================================================================

//�g�JSTL��
void InitVBO(void)
{
	//�g�J�y��ƾ�
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

//�վ��y�欰�Ҧ�(�ھڨ�ƾڡB��m���Y)
void motion(Ball *a, Ball *b){

	GLfloat X, Z, V, angle_hit, angle_V, angle_N, angle;

	//�]���y��q�۵��A�ʶq�u�� -> �����e�᪺ X.Z��V�y�t�`�M�ۦP
	//�çڼȥB���]��y������t�ק�����90��

	//�����p����V���t�שM
	X = a->Vx + b->Vx;
	Z = a->Vz + b->Vz;
	V = sqrt(X*X + Z*Z);//�����X�t�׭�

	
	angle_hit = atan2((b->x - a->x) , (b->z - a->z));//������������(a -> b)
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

//�P�w�����y�����A�Y�����h�^��1
int Hit(Ball a, Ball b, GLfloat r){

	if (sqrt((a.x - b.x)*(a.x - b.x) + (a.z - b.z)*(a.z - b.z)) <= r * 2 + 5){
		return 1;
	}
	else{ return 0; }
}


void KeyboardKeys(unsigned char key, int x, int y)
{
	//�O�D�������( X:�����O�D Z:���C�O�D )					//�O�D���Q��(1 ~ 10)�A��strength�x�s
	//X - �����O�D										//�]�w���y���t�� strengt*10
	if (strength < 10 && key == 'x'){					
		strength++;										//strength: �O�D�ܼ�
		keep -= 7;										//keep: �y��P�y�Z�� (�O�D�����|���y�����A���X�[���B��Z���ĪG
		keep_buffer = keep;								//keep_buffer: �]�X��|�����ϲy��V�e�A�ݭn�x�s�{�b��m�K��^�_
	}
	if (strength > 1 && key == 'z'){
		strength--;
		keep += 7;
		keep_buffer = keep;
	}

	//�������y��m - ���ԩw/�U��( ASWD�W�U���k�������y��m - �E�c���� )
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

	//Enter��: ������� - ��mode�Ȧb 1 �M 0 ������
	if (key == 13){
		mode = 1 - mode;
		controlled = 1;
	}

	//�ť���: ���y															
	if (hit_available == 1 && mode == 0 && key == 32){						//���y����: �Ҧ��y�t������0 + �ղy����
		keep = 0.0f;														//���y��A�y��V�eĲ�y(�ܧ�keep�Ȭ�0)
		balls[0].Vx = strength * 10 * sin(yRot * 3.1415926 / 180);			//���y��A�ᤩ�ղy��t�� strength*10 ��Vx,Vz
		balls[0].Vz = -strength * 10 * cos(yRot * 3.1415926 / 180);
		mode = 1;															//�������ܭ����A��K�[�ݲy��
		
		//���k����t
		if (Hitting_Position % 3 == 1){						//���� - 1.4.7
			balls[0].W_h = - strength * 4;
		}
		else if (Hitting_Position % 3 == 0){				//���k - 3.6.9
			balls[0].W_h = strength * 4;
		}

		//���Ա���t
		if (Hitting_Position <= 3){							//���W - 1.2.3
			balls[0].W_v = strength * 4;
		}
		else if (Hitting_Position >= 7){					//���U - 7.8.9
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

	//�̾ڨC���y X�BZ ��V�t�פ��q��s�y����m
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
		
		//*****�y����t�t��(�ثe²�Ƭ��u���ʤ�V��ϦV���t�v��t)*****
		
		//�����p���e�y�t
		GLfloat unit_v = sqrt(balls[i].Vx * balls[i].Vx + balls[i].Vz * balls[i].Vz);

		//�w���y�t�z��
		if (unit_v > 100){
			balls[i].Vx = 100 * balls[i].Vx / unit_v; 
			balls[i].Vz = 100 * balls[i].Vz / unit_v; 
		}

		//�[�t�� = -0.6
		balls[i].Vx -= 0.6 * balls[i].Vx / unit_v; //0.6 * sin
		balls[i].Vz -= 0.6 * balls[i].Vz / unit_v; //0.6 * cos

		//�y�t�ױ���0�Ϩ䰱�U
		if (abs(unit_v) <= 0.3f){
			balls[i].Vx = 0;
			balls[i].Vz = 0;
		}


		//��ɧP�w - �J��ɮɱN�t�פ@���q��אּ�t
		if (balls[i].x < -1270 + r){
			balls[i].x = -1270 + r + 2;
			balls[i].Vx = -balls[i].Vx;

			balls[i].Vz -= balls[i].W_h*0.7; //�U����k�s��t
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

	//*******���y�\�i�]�w*******

	hit_available = 1;//�w�]�i���y

	for (int i = 0; i < 10; i++){
		if (balls[i].Vx != 0 || balls[i].Vz != 0){   //�Y���@�y�Y�t�פ��q�D�s
			hit_available = 0;						 //���^���y�\�i(�]�Ȭ�0)
			break;
		}
	}
	//���P�_���O�����[��������p�ӳ](�_�h��y���R��A���Uenter���ܵ����N�������^�ղy����)
	if (controlled == 0){
		if (hit_available == 1){					 //�Y�g�L�W���j�餴�����y
			keep = keep_buffer;						 //�N�y��h�^�B��e��m
			mode = 0;								 //�ñN������^�ղy����
		}
	}
	//************************


	glutPostRedisplay();
	glutTimerFunc(33, TimerFunction, 1);
}

void RenderScene(void)
{
	//=============================�򥻳]�w�Υ����]�w================================
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

	//================================�ղy����ø��==================================

	//�ȯ�H�ղy�����˲y�B���y�A�]���u�b�������e�X�y��

	if (mode == 0){

		//�w������վ�y��( �פW����� + �˲y���� )
		glTranslatef(0.0f, -50.0f, -500.0f);
		glRotatef(xRot + xRot_Ini, 1.0f, 0.0f, 0.0f);
		glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	

		//�e�y�� ( �H���y��m�p��ø�ϰ����q )
		if (hit_available == 1){
			//���k��m�P�w
			if (Hitting_Position % 3 == 1){						//���� - 1.4.7
				z_adj = -20.0f;
			}
			else if (Hitting_Position % 3 == 0){				//���k - 3.6.9
				z_adj = 20.0f;
			}
			else{ z_adj = 0; }									//���� - 2.5.8

			//�W�U��m�P�w
			if (Hitting_Position <= 3){							//���W - 1.2.3
				y_adj = 20.0f;
			}
			else if (Hitting_Position >= 7){					//���U - 7.8.9
				y_adj = -20.0f;
			}
			else{ y_adj = 0; }									//���� - 4.5.6


			//�}�lø��
			glPushMatrix();
			glRotatef(90 - yRot, 0.0f, 1.0f, 0.0f); //���ਤ��						  //X��V: �Ҽ{����B�B��Z��
			glTranslatef(-cent[0] - 307 + keep, -cent[1] + y_adj, -cent[2] + z_adj);  //Y��V: �Ҽ{�W�U����
			glColor3f(0.22f, 0.22f, 0.22f);											  //Z��V: �Ҽ{���k����

			//�HVBO�e�T������
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
		glTranslatef(-balls[0].x, -r, -balls[0].z);//���Ҧ��ɡA�H�ղy�����ߥh�e�X��L�y
	}
	//============================================================================


	//================================��������ø��==================================

	if (mode == 1){

		//�w�惡�Ҧ����ܮy�� ( ������� + �פW����� )
		glRotatef(80, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, -1800.0f, 300.0f);
	}

	//�e�y�� - �������
	glPushMatrix();
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.25f, 0.0f);
	glRectf(-1270.0f, 0.0f, 1270.0f, 1270.0f);
	glPopMatrix();


	//***�H�U�e�y***

	//�ղy
	glPushMatrix();
	glTranslatef(balls[0].x, r, balls[0].z);
	glColor3f(1.0f, 1.0f, 1.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//�@��(��)
	glPushMatrix();
	glTranslatef(balls[1].x, r, balls[1].z);
	glColor3f(0.7f, 0.5f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//�G��(��)
	glPushMatrix();
	glTranslatef(balls[2].x, r, balls[2].z);
	glColor3f(0.0f, 0.0f, 0.7f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//�T��(�G��)
	glPushMatrix();
	glTranslatef(balls[3].x, r, balls[3].z);
	glColor3f(0.75f, 0.0f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//�|��(��)
	glPushMatrix();
	glTranslatef(balls[4].x, r, balls[4].z);
	glColor3f(0.5f, 0.0f, 0.7f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//����(��)
	glPushMatrix();
	glTranslatef(balls[5].x, r, balls[5].z);
	glColor3f(0.9f, 0.3f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//����(��)
	glPushMatrix();
	glTranslatef(balls[6].x, r, balls[6].z);
	glColor3f(0.0f, 0.2f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//�C��(��)
	glPushMatrix();
	glTranslatef(balls[7].x, r, balls[7].z);
	glColor3f(0.3f, 0.1f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//�K��(��)
	glPushMatrix();
	glTranslatef(balls[8].x, r, balls[8].z);
	glColor3f(0.0f, 0.0f, 0.0f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	//�E��(�g�� - �N��)
	glPushMatrix();
	glTranslatef(balls[9].x, r, balls[9].z);
	glColor3f(0.3f, 0.3f, 0.1f);
	glutSolidSphere(r, 50, 50);
	glPopMatrix();

	glPopMatrix();

	//***�e�y����***
	//============================================================================

	//=================================�O�D��ܰ�===================================
	
	//�Ȧb�����y(�y�t����0�� + �ղy����)�~�|���
	if (hit_available == 1 && mode == 0){
		glPushMatrix();
		glDisable(GL_LIGHTING);//�������Ӽv�T
		glTranslatef(-73.0f, -55.0f, -100.0f);//�]�w��m
		glColor3f(0.5f, 0.0f, 0.0f);//�`����

		//strength�O�h�ִN�e�X�Ӥ��
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
	//==================================�򥻳]�w====================================
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


	//===============================��l�Ʋy�y��m=================================

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
	//��V�䥪�k����Y�b����( �˲y��V )�A �W�U����X�b����( �����վ� )

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
	//================================�y��VBO�B�z==================================

	VBOv = 0; VBOn = 0;
	triArray = LoadStlBinary("Part1.STL", nTriangles);
	cout << "***STL�ɮ�Ū�����p***\n\n";


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


#pragma comment(lib,"winmm.lib")

#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/freeglut.h>
#include "ObjParser.h"
#include "bmpfuncs.h"
#include <MMsystem.h>
#include <Windows.h>
#include <stdio.h>
#include <Digitalv.h>


#define M_PI 3.1415926535897
using std::cos;
using std::sin;
using std::sqrt;

// global variable for counting fps
volatile int frame = 0, time, timebase = 0;
volatile float fps;

/* texture mapping set variable */
GLuint textureMonkey, texturetray, texturefan, texture_metal_tray, texture_mainboard, texturehdd, textureplastic, texturewhite, texturelock;
GLuint texture_nas_left, texture_nas_right, texture_nas_back;
GLuint g_nCubeTex;

int fanrad = 0, current_width = 1920, current_height = 1080;
double trayrad = 0, traypos = 0, traypos1 = 0, hddpos = 0;
bool antialiase_on = true, spectate = false, trayopen = false, poweron = false, music = false;
double radius = 10;
double theta = 45, phi = 240;
double cam[3];
double center[3] = { 0, 0, 0 };
double up[3] = { 0, 1, 0 };
int xf=0, yf=0,zf = 0;
// object var
ObjParser *monkey, *tray, *tray2, *fan, *tray_lock, *tray_lock2, *hdd_metal, *mainboard, *nasleft, *hdd, *hdd2, *nasright, *nasback;

// user-defined function
void init(void);
void light_default();
void add_menu();
void get_resource(const char* str);
void mouse(int, int, int, int);
void mouseWheel(int, int, int, int);
void motion(int, int);
void passiveMotion(int, int);
void keyboard(unsigned char, int, int);
void special_keyboard(int, int, int);
void draw(void);
void resize(int, int);
void idle();
void draw_string(void* font, const char* str, float x, float y, float red, float green, float blue);
void draw_text();

//lab16 skybox
void tex_skybox(void);
void draw_skyBox(void);
//...

/* Main method */
int main(int argc, char** argv)
{
	// glut initialize
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1920, 1080);
	glutInitWindowPosition(250, 250);
	glutCreateWindow("GLUT Test");	// 위의 순서 무조건 고정
	
	// 사용자 초기화 함수
	init();

	// pop-up 메뉴 등록 함수
	add_menu();

	// 리소스 로드 함수
	get_resource("monkey.obj");
	tray = new ObjParser("tray_final.obj");
	tray2 = new ObjParser("tray_final.obj");
	fan = new ObjParser("fan_final.obj");
	tray_lock = new ObjParser("tray_lock_final.obj");
	tray_lock2 = new ObjParser("tray_lock_final.obj");
	hdd_metal = new ObjParser("hdd_metal.obj");
	mainboard = new ObjParser("mainboard.obj");
	nasleft = new ObjParser("nas_left.obj");
	hdd = new ObjParser("hdd_final.obj");
	hdd2 = new ObjParser("hdd_final.obj");
	nasright = new ObjParser("nas_right.obj");
	nasback = new ObjParser("nas_back.obj");

	printf("=======================HOW TO USE ===========================\n");
	printf("4,5번을 통해 하드디스크 트레이 걸쇠의 움직임을 결정할 수 있습니다\n");
	printf("r,t를 통해 하드디스크 트레이를 꽂고 뺄 수 있습니다\n");
	printf("y,h를 통해 하드디스크를 트레이에 장착/해제 할 수 있습니다\n");
	printf("p를 통해 전원을 on/off toggle할 수 있습니다\n");
	printf("1을 통해 1인칭 모드로 들어갈 수 있습니다. 이때는 wasd를 통해 전후좌우, qe를 통해 상하로 움직입니다\n");
	printf("m 키를 통해 음악을 켜고 끌 수 있습니다\n");

	/* Create a single window with a keyboard and display callback */
	glutMouseFunc(&mouse);
	glutMouseWheelFunc(&mouseWheel);
	glutMotionFunc(&motion);
	glutPassiveMotionFunc(&passiveMotion);
	glutKeyboardFunc(&keyboard);
	glutSpecialFunc(&special_keyboard);
	glutDisplayFunc(&draw);
	glutReshapeFunc(&resize);

	glutIdleFunc(&idle);

	/* Run the GLUT event loop */
	glutMainLoop();

	return EXIT_SUCCESS;
}

void light_default() {
	glClearColor(0.f, 0.f, 0.f, 1.0f);

	/* Light0 조명 관련 설정 */
	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.9f, 1.0f };
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	/********* light point position setting **********/
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	/* Light1 조명 관련 설정 */
	GLfloat ambientLight1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuseLight1[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat specularLight1[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat light_position1[] = { 0, 0, 0, 1.0 };

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);

	/********* light point position setting **********/
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	/************* spot position setting *************/
	/*GLfloat spot_direction[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);*/

	GLfloat specularMaterial[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuseMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambientMaterial[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	/************* Material  setting *************/
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);


	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glDisable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	/* DEPTH TEST ENABLE */
	glFrontFace(GL_CW);	// CW CCW바꿔보면서 front face 변경해보기!
}

void setTextureMapping() {
	int imgWidth, imgHeight, channels;
	uchar* img;
	int texNum;

	img = readImageData("nas/3072.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &texture_metal_tray);
	glBindTexture(GL_TEXTURE_2D, texture_metal_tray);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("nas/tex_fan.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &texturefan);
	glBindTexture(GL_TEXTURE_2D, texturefan);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("nas/plastic.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &textureplastic);
	glBindTexture(GL_TEXTURE_2D, textureplastic);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("nas/mainboard_final.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &texture_mainboard);
	glBindTexture(GL_TEXTURE_2D, texture_mainboard);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("nas/hdd_tex.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &texturehdd);
	glBindTexture(GL_TEXTURE_2D, texturehdd);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



	img = readImageData("nas/white.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &texturewhite);
	glBindTexture(GL_TEXTURE_2D, texturewhite);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	img = readImageData("nas/nas_left.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &texture_nas_left);
	glBindTexture(GL_TEXTURE_2D, texture_nas_left);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("nas/nas_back_metal.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &texture_nas_back);
	glBindTexture(GL_TEXTURE_2D, texture_nas_back);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("nas/nas_right.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &texture_nas_right);
	glBindTexture(GL_TEXTURE_2D, texture_nas_right);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	img = readImageData("nas/tray_lock.bmp", &imgWidth, &imgHeight, &channels);

	texNum = 1;
	glGenTextures(texNum, &texturelock);
	glBindTexture(GL_TEXTURE_2D, texturelock);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);


	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//GL_REPEAT 둘중 하나 선택
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void init()
{
	printf("init func called\n");
	// clear background color
	glClearColor(1.f, 1.f, 1.f, 0.5);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluOrtho2D(0, 500, 500, 0);

	// set blend
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// set antialiasing
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
		
	light_default();

	/* TEXTURE MAPPING SET */

	glEnable(GL_TEXTURE_2D);
	setTextureMapping();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); //GL_REPLACE : polygon의 원래 색상은 무시하고 texture로 덮음
	tex_skybox();
}

void special_keyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT && spectate == false) {
		phi -= 5;
		if (phi < 0) phi = 355;
	}
	else if (key == GLUT_KEY_RIGHT && spectate == false) {
		phi += 5;
		if (phi >= 360) phi = 0;
	}
	else if (key == GLUT_KEY_UP && spectate == false) {
		if(theta > 10) theta -= 5;
	}
	else if (key == GLUT_KEY_DOWN && spectate == false) {
		if(theta < 170) theta += 5;
	}

	std::cout << "theta : " << theta << ", phi : " << phi << "\n";
	glutPostRedisplay();
}

/* Keyboard callback function */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		/* Exit on escape key press */
	case '\x1B':
	{
		exit(EXIT_SUCCESS);
		break;
	}
	case 'o':
	{
		if (antialiase_on) {
			antialiase_on = false;
			glDisable(GL_POLYGON_SMOOTH);
		}
		else
		{
			antialiase_on = true;
			glEnable(GL_POLYGON_SMOOTH);
		}
		break;
	}
	}
	if (key == '3')
	{
		fanrad -= 25;
		if (fanrad < 0) fanrad += 360;
		if (fanrad > 360) fanrad -= 360;
		printf("fanrad : %d\n", fanrad);
	}
	else if (key == 'a' && spectate == true)
	{
		xf += 1;
	}
	else if (key == 'd' && spectate == true)
	{
		xf -= 1;
	}
	else if (key == 'q' && spectate == true)
	{
		yf += 1;
	}
	else if (key == 'e' && spectate == true)
	{
		yf -= 1;
	}
	else if (key == 's' && spectate == true)
	{
		zf -= 1;
	}
	else if (key == 'w' && spectate == true)
	{
		zf += 1;
	}
	else if (key == '1' && spectate == false)
	{
		theta = 60;
		phi = 180;
		cam[0] = 0; cam[1] = 0; cam[2] = 0;
		xf = 0; yf = 0; zf = 0;
		spectate = true;
	}
	else if (key == '1' && spectate == true)
	{
		theta = 45;
		phi = 45;
		cam[0] = 0; cam[1] = 0; cam[2] = 0;
		xf = 0; yf = 0; zf = 0;
		spectate = false;
	}
	else if (key == '4' && trayrad < 24 && trayopen == false)
	{
		trayrad += 1;
		traypos -= 0.01;
	}
	else if (key == '5' && trayrad > 0 && trayopen == false)
	{
		trayrad -= 1;
		traypos += 0.01;
	}
	else if (key == 'r' && trayrad == 24 && traypos1 < 10)
	{
		traypos1 += 2;
		trayopen = true;
	}
	else if (key == 't' && trayrad == 24 && traypos1 > 0)
	{
		traypos1 -= 2;
		if (traypos1 == 0)
			trayopen = false;
	}
	else if (key == 'h' && trayopen == true && traypos1 == 10 && hddpos < 8)
	{
		hddpos += 2;
	}
	else if (key == 'y' && trayopen == true && traypos1 == 10 && hddpos > 0)
	{
		hddpos -= 2;
	}
	else if (key == 'p' && poweron == false)
		poweron = true;
	else if (key == 'p' && poweron == true)
		poweron = false;
	else if (key == 'm' && music == true) {
		sndPlaySound(NULL, SND_ASYNC);
		music = false;
	}
	else if (key == 'm' && music == false) {
		sndPlaySoundA("1_01.wav", SND_ASYNC | SND_NODEFAULT | SND_LOOP);
		music = true;
	}
	glutPostRedisplay();
}

void draw_axis(void)
{
	glLineWidth(1.5f);
	glBegin(GL_LINES);

	glColor4f(1.f, 0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(4.f, 0.f, 0.f);

	glColor4f(0.f, 1.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 4.f, 0.f);

	glColor4f(0.f, 0.f, 1.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 4.f);

	glEnd();
	glLineWidth(1);
}

void draw_obj(ObjParser *objParser)
{
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n+=3) {
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
}

void draw_obj_with_texture(ObjParser *objParser, GLuint tex) // texture  여러개 받을 수 있도록 코드 약간 수정
{
	glDisable(GL_BLEND);
	// glEnable(GL_TEXTURE_2D);	// texture 색 보존을 위한 enable
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glTexCoord2f(objParser->textures[objParser->textureIdx[n] - 1].x,
			objParser->textures[objParser->textureIdx[n] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n] - 1].x,
			objParser->normal[objParser->normalIdx[n] - 1].y,
			objParser->normal[objParser->normalIdx[n] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 1] - 1].x,
			objParser->textures[objParser->textureIdx[n + 1] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 1] - 1].x,
			objParser->normal[objParser->normalIdx[n + 1] - 1].y,
			objParser->normal[objParser->normalIdx[n + 1] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glTexCoord2f(objParser->textures[objParser->textureIdx[n + 2] - 1].x,
			objParser->textures[objParser->textureIdx[n + 2] - 1].y);
		glNormal3f(objParser->normal[objParser->normalIdx[n + 2] - 1].x,
			objParser->normal[objParser->normalIdx[n + 2] - 1].y,
			objParser->normal[objParser->normalIdx[n + 2] - 1].z);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
	glEnable(GL_BLEND);
}

void draw_cube_textures()
{
	int size = 2;
	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glBegin(GL_QUADS);
	//Quad 1
	glNormal3f(1.0f, 0.0f, 0.0f);   //N1
	glTexCoord2f(0.0f, 1.0f); glVertex3f(size / 2, size / 2, size / 2);   //V2
	glTexCoord2f(0.0f, 0.0f); glVertex3f(size / 2, -size / 2, size / 2);   //V1
	glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, -size / 2, -size / 2);   //V3
	glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, size / 2, -size / 2);   //V4
  //Quad 2
	glNormal3f(0.0f, 0.0f, -1.0f);  //N2
	glTexCoord2f(0.0f, 1.0f); glVertex3f(size / 2, size / 2, -size / 2);   //V4
	glTexCoord2f(0.0f, 0.0f); glVertex3f(size / 2, -size / 2, -size / 2);   //V3
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-size / 2, -size / 2, -size / 2);   //V5
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-size / 2, size / 2, -size / 2);   //V6
  //Quad 3
	glNormal3f(-1.0f, 0.0f, 0.0f);  //N3
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, size / 2, -size / 2);   //V6
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, -size / 2, -size / 2);   //V5
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-size / 2, -size / 2, size / 2);   //V7
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-size / 2, size / 2, size / 2);   //V8
  //Quad 4
	glNormal3f(0.0f, 0.0f, 1.0f);   //N4
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, size / 2, size / 2);   //V8
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, -size / 2, size / 2);   //V7
	glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, -size / 2, size / 2);   //V1
	glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, size / 2, size / 2);   //V2
  //Quad 5
	glNormal3f(0.0f, 1.0f, 0.0f);   //N5
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, size / 2, -size / 2);   //V6
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, size / 2, size / 2);   //V8
	glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, size / 2, size / 2);   //V2
	glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, size / 2, -size / 2);   //V4
  //Quad 6
	glNormal3f(1.0f, -1.0f, 0.0f);  //N6
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-size / 2, -size / 2, size / 2);   //V7
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-size / 2, -size / 2, -size / 2);   //V5
	glTexCoord2f(1.0f, 0.0f); glVertex3f(size / 2, -size / 2, -size / 2);   //V3
	glTexCoord2f(1.0f, 1.0f); glVertex3f(size / 2, -size / 2, size / 2);   //V1
	glEnd();
}

void draw_textureCube() {
	glColor3f(1.0, 1.0, 1.0);	// white로 color를 set해주어야 texture색상이 제대로 적용 됨!
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			// polygon의 원래 색상은 무시하고 texture로 덮음
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// polygon의 원래 색상과 texture 색상을 곱하여 덮음, texture가 입혀진 표면에 광원 효과 설정 가능
//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		// texture의 색상으로 덮어 씌운다

	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glBegin(GL_QUADS);
	glNormal3f(-1.0, 0, 0);	// -x axis
	glTexCoord2f(0, 0); glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(1, 0); glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(1, 1); glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(0, 1); glVertex3f(-1.0, 1.0, 1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glBegin(GL_QUADS);
	glNormal3f(1.0, 0, 0);	//x axis
	glTexCoord2f(0, 0); glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(1, 0); glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(1, 1); glVertex3f(1.0, -1.0, -1.0);
	glTexCoord2f(0, 1); glVertex3f(1.0, 1.0, -1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glBegin(GL_QUADS);
	glNormal3f(0, -1.0, 0);	// -y axis
	glTexCoord2f(0, 0); glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(1, 0); glVertex3f(1.0, -1.0, -1.0);
	glTexCoord2f(1, 1); glVertex3f(1.0, -1.0, 1.0);
	glTexCoord2f(0, 1); glVertex3f(-1.0, -1.0, 1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glBegin(GL_QUADS);
	glNormal3f(0, 1.0, 0);	// y axis
	glTexCoord2f(0, 0); glVertex3f(-1.0, 1.0, 1.0);
	glTexCoord2f(1, 0); glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(1, 1); glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(0, 1); glVertex3f(-1.0, 1.0, -1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1.0);	//z axis
	glTexCoord2f(0, 0); glVertex3f(1.0, 1.0, 1.0);
	glTexCoord2f(1, 0); glVertex3f(-1.0, 1.0, 1.0);
	glTexCoord2f(1, 1); glVertex3f(-1.0, -1.0, 1.0);
	glTexCoord2f(0, 1); glVertex3f(1.0, -1.0, 1.0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textureMonkey);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1.0);	//-z축
	glTexCoord2f(0, 0); glVertex3f(1.0, 1.0, -1.0);
	glTexCoord2f(1, 0); glVertex3f(-1.0, 1.0, -1.0);
	glTexCoord2f(1, 1); glVertex3f(-1.0, -1.0, -1.0);
	glTexCoord2f(0, 1); glVertex3f(1.0, -1.0, -1.0);
	glEnd();

	draw_axis();
}

/* Display callback function */
void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	cam[0] = radius * sin(theta*M_PI / 180)*sin(phi*M_PI / 180);
	cam[1] = radius * cos(theta*M_PI / 180);
	cam[2] = radius * sin(theta*M_PI / 180)*cos(phi*M_PI / 180);



	gluLookAt(cam[0]+xf, cam[1]+yf, cam[2]+zf, center[0]+xf, center[1]+yf, center[2]+zf, up[0], up[1], up[2]);

	glDisable(GL_LIGHT1);
	GLfloat light_position0[] = { cam[0],cam[1],cam[2],1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	

	//skybox code start
	draw_skyBox();

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_CUBE_MAP);


	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	//skybox code end

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	draw_axis();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glColor3f(1.f, 1.f, 1.f);
	//glutWireSphere(2, 50, 50);
	//glRotatef(90, 1, 0, 0);
	//draw_axis();
	//draw_obj(monkey);

	glPushMatrix();
	draw_text();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, traypos);
	glTranslatef(0, 0, -traypos1);
	draw_obj_with_texture(tray, textureplastic);
	glTranslatef(0.3, 2, -2.1 + traypos);
	glRotatef(trayrad, 1, 0, 0);
	glTranslatef(0, -1.5, 0);
	draw_obj_with_texture(tray_lock, texturelock);
	glPopMatrix();
	// end of tray plastic

	glPushMatrix();
	glRotatef(90, 0, 0, 1);
	draw_obj_with_texture(hdd_metal, texture_metal_tray);
	glPopMatrix();
	// end of metal tray

	glPushMatrix();
	draw_obj_with_texture(mainboard, texture_mainboard);
	glPopMatrix();
	// end of mainboard
	
	glPushMatrix();
	glTranslatef(-0.5, 1.4, 0);
	glRotatef(fanrad, 0, 0, 1);
	draw_obj_with_texture(fan, texturefan);
	glPopMatrix();
	//end of fan
	
	glPushMatrix();
	glTranslatef(1.8, -2, -1.65);
	draw_obj_with_texture(nasleft,texture_nas_left);
	glPopMatrix();
	//end of nas left chassis

	glPushMatrix();
	glTranslatef(-hddpos, 0, 0);
	glTranslatef(0, 0, -traypos1);
	glTranslatef(0, 0, traypos);
	draw_obj_with_texture(hdd, texturehdd);
	glPopMatrix();
	//end of hdd

	glPushMatrix();
	glTranslatef(-1, 0, 0);
	draw_obj_with_texture(tray2, textureplastic);
	glTranslatef(0.3, 0.55, -2.1);
	draw_obj_with_texture(tray_lock, texturelock);
	glPopMatrix();
	//end of hdd tray 2

	glPushMatrix();
	glTranslatef(0, 0, 5);
	draw_obj_with_texture(nasback,texture_nas_back);
	glPopMatrix();
	//end of nas back aluminum

	glPushMatrix();
	glTranslatef(0, 0, 5);
	draw_obj_with_texture(nasright,texture_nas_right);
	glPopMatrix();
	//end of nas right chassis 

	//draw_obj(monkey);
	//draw_cube_textures();
	//draw_textureCube();
	
	glutSwapBuffers();
	glFlush();
}

void mouse(int button, int state, int x, int y)
{
	if (state) {
		// printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
	}
	else {
		// printf("button(%d), state(%d), x(%d), y(%d)\n", button, state, x, y);
	}
	glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0) {
		// printf("button(%d), dir(%d), x(%d), y(%d)\n", button, dir, x, y);
		if (radius > 1) radius -= 0.5;
	}
	else {
		// printf("button(%d), dir(%d), x(%d), y(%d)\n", button, dir, x, y);
		if (radius < 100) radius += 0.5;
	}
	glutPostRedisplay();
}

void passiveMotion(int x, int y)
{
	//printf("Mouse movement x, y = (%d, %d)\n", x, y);
	glutPostRedisplay();
}

void motion(int x, int y)
{
	//printf("Mouse movement x, y = (%d, %d)\n", x, y);
	glutPostRedisplay();
}

void main_menu(int option)
{
	if (option == 99) exit(0);
	else if (option == 1) {
		radius = 10;
		theta = 45; phi = 45;
	}
	glutPostRedisplay();
}

void sub_menu(int option)
{
	printf("Submenu %d has been selected\n", option);
}

void add_menu()
{
	int mainmenu1 = glutCreateMenu(&main_menu);
	glutAddMenuEntry("Init", 1);
	glutAddMenuEntry("Quit", 99);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void idle()
{
	if (poweron == true) {
		fanrad -= 8;
		if (fanrad < 0) fanrad += 360;
		glutPostRedisplay();
	}
}

void resize(int width, int height)
{
	printf("resize func called\n");
	glViewport(0, 0, width, height);
	current_width = width; current_height = height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, 500, 0, 500);
	gluPerspective(60, (double)width / (double)height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void get_resource(const char* str)
{
	monkey = new ObjParser(str);
}

void tex_skybox(void)
{
	int imgWidth, imgHeight, channels;
	char buf[100];
	//cube형태의 텍스쳐로 지정
	glGenTextures(1, &g_nCubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
	uchar* img;
	img = readImageData("sb/left.bmp", &imgWidth, &imgHeight, &channels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	img = readImageData("sb/right.bmp", &imgWidth, &imgHeight, &channels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	img = readImageData("sb/bottom.bmp", &imgWidth, &imgHeight, &channels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	img = readImageData("sb/sky.bmp", &imgWidth, &imgHeight, &channels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	img = readImageData("sb/front.bmp", &imgWidth, &imgHeight, &channels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	img = readImageData("sb/back.bmp", &imgWidth, &imgHeight, &channels);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, img);

	//텍스쳐 속성 지정
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//텍스쳐 ㅘ표 자동 생성
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

	//큐브 텍스쳐 활성화
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);


}

void draw_skyBox(void) {
	// 여기서부터 draw_skyBox 강의노트와 같음
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_nCubeTex);

	float g_nSkySize = 50.0f;
	glBegin(GL_QUADS);
	// px
	glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(g_nSkySize, g_nSkySize, -g_nSkySize);

	//nx
	glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-g_nSkySize, g_nSkySize, -g_nSkySize);

	//py
	glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-g_nSkySize, g_nSkySize, -g_nSkySize);
	glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(g_nSkySize, g_nSkySize, -g_nSkySize);

	//ny
	glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(g_nSkySize, -g_nSkySize, -g_nSkySize);

	//pz
	glTexCoord3d(1.0, -1.0, 1.0); glVertex3f(g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, -1.0, 1.0); glVertex3f(-g_nSkySize, -g_nSkySize, g_nSkySize);
	glTexCoord3d(-1.0, 1.0, 1.0); glVertex3f(-g_nSkySize, g_nSkySize, g_nSkySize);
	glTexCoord3d(1.0, 1.0, 1.0); glVertex3f(g_nSkySize, g_nSkySize, g_nSkySize);

	//nz
	glTexCoord3d(1.0, -1.0, -1.0); glVertex3f(g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(-1.0, -1.0, -1.0); glVertex3f(-g_nSkySize, -g_nSkySize, -g_nSkySize);
	glTexCoord3d(-1.0, 1.0, -1.0); glVertex3f(-g_nSkySize, g_nSkySize, -g_nSkySize);
	glTexCoord3d(1.0, 1.0, -1.0); glVertex3f(g_nSkySize, g_nSkySize, -g_nSkySize);

	glEnd();
}

void draw_text()
{

	//glViewport(0, current_width / 2,
	//	current_width / 2, current_height / 2);
	glLoadIdentity(); // 기존의 회전 변환을 무시


	glColor3f(1.0f, 1.0f, 1.0f);
	if (music == false)
	{
		draw_string(GLUT_BITMAP_TIMES_ROMAN_24,
			"MUSIC : OFF", 2, 4, 1, 1, 1);
	}
	else if(music == true)
	{
		draw_string(GLUT_BITMAP_TIMES_ROMAN_24,
			"MUSIC : ON", 2, 4, 1, 1, 1);
	}
	glColor3f(0.5f, 1.0f, 0);
	char buf[100];
	sprintf(buf, "cam[0] : %f, cam[1] : %f, cam[2] : %f", cam[0], cam[1], cam[2]);
	draw_string(GLUT_BITMAP_HELVETICA_18,
		buf, 2, 3.5, 1, 1, 0);
	glFlush();
}

void draw_string(void* font, const char* str, float x, float y, float red, float green, float blue)
{
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-5, 5, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(red, green, blue);
	glRasterPos3f(x, y, 0);
	for (unsigned int i = 0; i < strlen(str); i++) {
		glutBitmapCharacter(font, str[i]);
	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopAttrib();
}

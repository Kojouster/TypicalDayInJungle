#include <iostream>
#include "GL/glew.h"
#include "GL/3dgl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

// 3D Models
C3dglTerrain terrain, water;

C3dglProgram ProgramBasic,ProgramWater,ProgramTerrain,ProgramParticle,ProgramAnim;

C3dglModel frog,shark,palm;
vec3 frogPos = vec3(0, 0, 0);
vec3 frogVel = vec3(0, 0, 0);
GLuint idTexSand, idTexGrass, idTexFrog,idBufferVelocity, idBufferStartTime, idTexParticle,idBufferInitialPosition, idTexFrogNormal,idTexFish, idTexPalmBase, idTexPalmNormal, idTexFishNormal;
// Water specific variables
float waterLevel = 4.6f;

// camera position (for first person type camera navigation)
mat4 matrixView;			// The View Matrix
float angleTilt = 15.f;		// Tilt Angle
vec3 cam(0);				// Camera movement values



// Particle System Params
const float PERIOD = 0.00075f;
const float LIFETIME = 3;
const int NPARTICLES = (int)(LIFETIME / PERIOD);

float M_PI;

bool init()
{
	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	// Initialise Shaders
	C3dglShader VertexShader;
	C3dglShader FragmentShader;

	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/basic.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/basic.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramBasic.Create()) return false;
	if (!ProgramBasic.Attach(VertexShader)) return false;
	if (!ProgramBasic.Attach(FragmentShader)) return false;
	if (!ProgramBasic.Link()) return false;
	if (!ProgramBasic.Use(true)) return false;


	// glut additional setup
	glutSetVertexAttribCoord3(ProgramBasic.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(ProgramBasic.GetAttribLocation("aNormal"));

	if (!frog.load("models\\frog.dae")) return false;
	if (!shark.load("models\\fish1.dae")) return false;
	if (!palm.load("models\\palm.obj")) return false;


	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/water.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/water.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramWater.Create()) return false;
	if (!ProgramWater.Attach(VertexShader)) return false;
	if (!ProgramWater.Attach(FragmentShader)) return false;
	if (!ProgramWater.Link()) return false;
	if (!ProgramWater.Use(true)) return false;

	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/ter.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/ter.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramTerrain.Create()) return false;
	if (!ProgramTerrain.Attach(VertexShader)) return false;
	if (!ProgramTerrain.Attach(FragmentShader)) return false;
	if (!ProgramTerrain.Link()) return false;
	if (!ProgramTerrain.Use(true)) return false;

	// Initialise Shader - Particle
	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/particles.vert")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/particles.frag")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramParticle.Create()) return false;
	if (!ProgramParticle.Attach(VertexShader)) return false;
	if (!ProgramParticle.Attach(FragmentShader)) return false;
	if (!ProgramParticle.Link()) return false;
	if (!ProgramParticle.Use(true)) return false;
	





	glutSetVertexAttribCoord3(ProgramWater.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(ProgramWater.GetAttribLocation("aNormal"));

	glutSetVertexAttribCoord3(ProgramTerrain.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(ProgramTerrain.GetAttribLocation("aNormal"));




	// load your 3D models here!
	if (!terrain.loadHeightmap("models\\heightmap.png", 10)) return false;
	if (!water.loadHeightmap("models\\watermap.png", 10)) return false;


	C3dglBitmap bm;

	bm.Load("models/sand.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &idTexSand);
	glBindTexture(GL_TEXTURE_2D, idTexSand);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/grass.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &idTexGrass);
	glBindTexture(GL_TEXTURE_2D, idTexGrass);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());



	bm.Load("models/froggo.jpg", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexFrog);
	glBindTexture(GL_TEXTURE_2D, idTexFrog);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());


	bm.Load("models/fish.jpg", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexFish);
	glBindTexture(GL_TEXTURE_2D, idTexFish);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());


	bm.Load("models/PalmBase.jpg", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexPalmBase);
	glBindTexture(GL_TEXTURE_2D, idTexPalmBase);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());


	bm.Load("models/PalmNormal.jpg", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexPalmNormal);
	glBindTexture(GL_TEXTURE_2D, idTexPalmNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());


	bm.Load("models/frogNormal.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &idTexFrogNormal);
	glBindTexture(GL_TEXTURE_2D, idTexFrogNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());


	bm.Load("models/FishNormal.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &idTexFishNormal);
	glBindTexture(GL_TEXTURE_2D, idTexFishNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());


	// Setup the Texture
	 bm.Load("models/wat.bmp", GL_RGBA);
	glGenTextures(1, &idTexParticle);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexParticle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());
	
	// Prepare the particle buffers
	// Prepare the particle buffers
	std::vector<float> bufferVelocity;
	std::vector<float> bufferStartTime;
	std::vector<float> bufferInitialPosition;

	float time = 0;
	for (int i = 0; i < NPARTICLES; i++)
	{
		float theta = (float)M_PI / 6.f * (float)rand() / (float)RAND_MAX;
		float phi = (float)M_PI * 2.f * (float)rand() / (float)RAND_MAX;
		float x = -120.0+ 250.0f * (float)rand() / (float)RAND_MAX;
		//float y = 3.0 + 0.0f * (float)rand() / (float)RAND_MAX; 
		float y =500.0 + 0.0f * (float)rand() / (float)RAND_MAX;
		float z = -120.0 + 250.0f * (float)rand() / (float)RAND_MAX;
		float v = -2.0+ 2.0f * (float)rand() / (float)RAND_MAX;

		bufferVelocity.push_back(x * v);
		bufferVelocity.push_back(y * v);
		bufferVelocity.push_back(z * v);

		bufferInitialPosition.push_back(x * v);
		bufferInitialPosition.push_back(y * v);
		bufferInitialPosition.push_back(z * v);


		bufferStartTime.push_back(time);
		time += PERIOD;



	}
	glGenBuffers(1, &idBufferVelocity);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferVelocity);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferVelocity.size(), &bufferVelocity[0],
		GL_STATIC_DRAW);

	glGenBuffers(1, &idBufferStartTime);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferStartTime);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferStartTime.size(), &bufferStartTime[0],
		GL_STATIC_DRAW);


	glGenBuffers(1, &idBufferInitialPosition);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferInitialPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferInitialPosition.size(), &bufferInitialPosition[0],
		GL_STATIC_DRAW);



	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// setup the point size
	glEnable(GL_POINT_SPRITE);
	glPointSize(9);



	ProgramParticle.SendUniform("texture0", 0);

	// setup lights (for basic and terrain programs only, water does not use these lights):
	ProgramBasic.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
	ProgramBasic.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
	ProgramBasic.SendUniform("lightDir.diffuse", 1.0, 1.0, 1.0);

	ProgramBasic.SendUniform("lightDirNormal.direction", -1.0, 1.0, 1.0);
	ProgramBasic.SendUniform("lightDirNormal.diffuse", 1.0f, 1.0f, 1.0f);


	ProgramTerrain.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
	ProgramTerrain.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
	ProgramTerrain.SendUniform("lightDir.diffuse", 1.0, 1.0, 1.0);



	// setup materials (for basic and terrain programs only, water does not use these materials):
	ProgramBasic.SendUniform("materialAmbient", 1.0, 1.0, 1.0);		// full power (note: ambient light is extremely dim)
	ProgramBasic.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);

	ProgramTerrain.SendUniform("materialAmbient", 1.0, 1.0, 1.0);		// full power (note: ambient light is extremely dim)
	ProgramTerrain.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);


	// setup the water colours and level
	ProgramWater.SendUniform("waterColor", 0.2f, 0.22f, 0.02f,0.2f);
	ProgramWater.SendUniform("skyColor", 0.2f, 0.6f, 1.f,1.0f);

	ProgramTerrain.SendUniform("waterColor", 0.5f, 0.25f, 0.05f);
	ProgramTerrain.SendUniform("fogDensity", 0.5);

	ProgramTerrain.SendUniform("waterLevel", waterLevel);

	// Setup the particle system
	ProgramParticle.SendUniform("initialPos", 0.0, 300.58, 0.0);
	//ProgramParticle.SendUniform("gravity", 0.0, -1.0, 0.0);
	ProgramParticle.SendUniform("particleLifetime", LIFETIME);





	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(
		vec3(4.0, 5.4, 30.0),
		vec3(4.0, 0.4, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.5, 0.5, 0.5,0.5);   // grey Sky colour

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;

	return true;
}

void done()
{
}

void render()
{

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// this global variable controls the animation
	float theta = glutGet(GLUT_ELAPSED_TIME) * 0.01f;
	float times = GetTickCount64() / 85;


	// send the animation time to shaders
	ProgramWater.SendUniform("t", glutGet(GLUT_ELAPSED_TIME) / 1000.f);
	ProgramParticle.SendUniform("time", glutGet(GLUT_ELAPSED_TIME) / 1000.f - 2);





	// setup the View Matrix (camera)
	mat4 m = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));// switch tilt off
	m = translate(m, cam);												// animate camera motion (controlled by WASD keys)
	m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));			// switch tilt on
	matrixView = m * matrixView;

	// move the camera up following the profile of terrain (Y coordinate of the terrain)
	float terrainY = -std::max(terrain.getInterpolatedHeight(inverse(matrixView)[3][0], inverse(matrixView)[3][2]), waterLevel);
	matrixView = translate(matrixView, vec3(0, terrainY, 0));


	float Y = -terrain.getInterpolatedHeight(inverse(matrixView)[3][0], inverse(matrixView)[3][2]);
	// setup View Matrix
	ProgramBasic.SendUniform("matrixView", matrixView);
	ProgramWater.SendUniform("matrixView", matrixView);
	ProgramTerrain.SendUniform("matrixView", matrixView);
	



	vec3 amendY = vec3(vec3(0, terrain.getInterpolatedHeight(frogPos.x, frogPos.z), 0));

	std::vector<float> transforms;
	frog.getBoneTransforms(0, 0.5*times, transforms);
	ProgramBasic.SendUniformMatrixv("bones", (float*)&transforms[0], transforms.size() / 16);



	// Setup NORMAL texturing
	ProgramBasic.SendUniform("lightDirNormal.on", 2);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexFrogNormal);
	ProgramBasic.SendUniform("textureNormal", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexFrog);

	ProgramBasic.SendUniform("texture0", 0);
	ProgramBasic.Use();
	m = matrixView;

	m = rotate(m, radians(-70.f), vec3(0, 1, 0));
	m = scale(m, vec3(0.5, 0.5, 0.5));
	m = translate(m, vec3(20.5f, 16.3f, -25.5f));

	frog.render(m);
	
	

	ProgramBasic.Use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexFishNormal);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexFish);

	shark.getBoneTransforms(0, 1.7*times, transforms);
	ProgramBasic.SendUniformMatrixv("bones", (float*)&transforms[0], transforms.size() / 16);

	float z = -32 + (glutGet(GLUT_ELAPSED_TIME) % (64 * 200)) / 200.f;
	float h1 = terrain.getInterpolatedHeight(5.5, z - 1);
	float h2 = terrain.getInterpolatedHeight(5.5, z + 1);

	m = matrixView;
	m = translate(m, vec3(1.5f, (h1 + h2) / 2 + 1.8 + terrainY, z));

	m = scale(m, vec3(0.05, 0.05, 0.05));
	m = translate(m, vec3(80.5f, 57.7f, -25.5f));
	shark.render(m);

	
	ProgramBasic.Use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexPalmNormal);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexPalmBase);
	m = matrixView;

	m = rotate(m, radians(-70.f), vec3(0, 1, 0));
	m = scale(m, vec3(1.5, 1.5, 1.5));
	m = translate(m, vec3(5.5f, 5.3f, 7.5f));

	palm.render(m);
	ProgramBasic.SendUniform("lightDirNormal.on", 0);

	


	// Setup the Diffuse Material to: Green Grass
	ProgramTerrain.SendUniform("materialDiffuse", 0.5f, 0.35f, 0.05f);
	// setup the textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexSand);
	ProgramTerrain.SendUniform("textureBed", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, idTexGrass);
	ProgramTerrain.SendUniform("textureShore", 2);
	// render the terrain
	ProgramTerrain.Use();
	m = matrixView;
	terrain.render(m);



	// Setup the Diffuse Material to: Watery Green
	ProgramWater.SendUniform("materialDiffuse", 0.2f, 0.22f, 0.02f);

	// render the water
	ProgramWater.Use();
	m = matrixView;
	m = translate(m, vec3(0, waterLevel, 0));
	m = scale(m, vec3(0.5f, 1.0f, 0.5f));
	ProgramWater.SendUniform("matrixModelView", m);
	water.render(m);

	
	

	ProgramParticle.Use();
	m = matrixView;
	ProgramParticle.SendUniform("matrixModelView", m);
	// particles
	glDepthMask(GL_FALSE);				// disable depth buffer updates
	glActiveTexture(GL_TEXTURE0);			// choose the active texture
	glBindTexture(GL_TEXTURE_2D, idTexParticle);	// bind the texture

	// render the buffer
	glEnableVertexAttribArray(0);	// velocity
	glEnableVertexAttribArray(1);	// start time
	glEnableVertexAttribArray(2);   // initialposition


	glBindBuffer(GL_ARRAY_BUFFER, idBufferVelocity);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, idBufferStartTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, idBufferInitialPosition);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_POINTS, 0, NPARTICLES);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);


	glDepthMask(GL_TRUE);


	

	// essential for double-buffering technique
	// essential for double-buffering technique

		// the camera must be moved down by terrainY to avoid unwanted effects
	matrixView = translate(matrixView, vec3(0, -terrainY, 0));
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

// called before window opened or resized - to setup the Projection Matrix
void reshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 m = perspective(radians(60.f), ratio, 0.02f, 1000.f);
	ProgramBasic.SendUniform("matrixProjection", m);
	ProgramWater.SendUniform("matrixProjection", m);
	ProgramTerrain.SendUniform("matrixProjection", m);
	ProgramParticle.SendUniform("matrixProjection", m);


}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': cam.z = std::max(cam.z * 1.05f, 0.01f); break;
	case 's': cam.z = std::min(cam.z * 1.05f, -0.01f); break;
	case 'a': cam.x = std::max(cam.x * 1.05f, 0.01f); break;
	case 'd': cam.x = std::min(cam.x * 1.05f, -0.01f); break;
	case 'e': cam.y = std::max(cam.y * 1.05f, 0.01f); break;
	case 'q': cam.y = std::min(cam.y * 1.05f, -0.01f); break;
	}
	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': cam.z = 0; break;
	case 'a':
	case 'd': cam.x = 0; break;
	case 'q':
	case 'e': cam.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
bool bJustClicked = false;
void onMouse(int button, int state, int x, int y)
{
	bJustClicked = (state == GLUT_DOWN);
	glutSetCursor(bJustClicked ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
}

// handle mouse move
void onMotion(int x, int y)
{
	if (bJustClicked)
		bJustClicked = false;
	else
	{
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// find delta (change to) pan & tilt
		float deltaPan = 0.25f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
		float deltaTilt = 0.25f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// View = Tilt * DeltaPan * Tilt^-1 * DeltaTilt * View;
		angleTilt += deltaTilt;
		mat4 m = mat4(1.f);
		m = rotate(m, radians(angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaPan), vec3(0.f, 1.f, 0.f));
		m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaTilt), vec3(1.f, 0.f, 0.f));
		matrixView = m * matrixView;
	}
}

int main(int argc, char **argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("CI5520 3D Graphics Programming");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "GLEW Error: " << glewGetErrorString(err) << endl;
		return 0;
	}
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	// register callbacks
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);

	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Version: " << glGetString(GL_VERSION) << endl;

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		cerr << "Application failed to initialise" << endl;
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	done();

	return 1;
}


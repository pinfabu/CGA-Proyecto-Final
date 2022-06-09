#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

// Font rendering
#include "Headers/FontTypeRendering.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// ShadowBox include
#include "Headers/ShadowBox.h"

// OpenAL include
#include <AL/alut.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#define NUM_COVID 5
#define NUM_MASKS 8

//Vida personaje
int hp = 3;
int bullets = 0;
std::string cadena = "";
std::string cadena1 = "";
std::string cadena2 = "Recarga";
std::string cadena3 = "";
int screenWidth;
int screenHeight;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

GLFWwindow* window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para visualizar el buffer de profundidad
Shader shaderViewDepth;
//Shader para dibujar el buffer de profunidad
Shader shaderDepth;
//Shader para las particulas de humo
Shader shaderParticlesSmoke;
//Shader para las particulas de lluvia
Shader shaderParticlesRain;

//std::shared_ptr<FirstPersonCamera> cameraFP(new FirstPersonCamera());
std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = -0.5;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;

ShadowBox* shadowBox;

// Models complex instances
//Model modelRock;
// Lamps
Model modelLamp1;
Model modelLamp2;
//Model modelLampPost2;
// Hierba
Model modelGrass;
// Fountain
Model modelFountain;
// Model animate instance
/*// Mayow
Model mayowModelAnimate;*/
// Osmosis
Model modelOsmosisAnimate;
// Covid
Model covidArray[NUM_COVID];
// Masks
Model maskArray[NUM_MASKS];
// Bullet
Model modelBulletAnimate;
// Map
Model modelMapTest;
//Model modelMapRef;
Model mapArray[27];
std::string mapDirs[27] = {
	"../models/Map/L_HortBackLeft.obj",		//0
	"../models/Map/L_HortBackRight.obj",	//1
	"../models/Map/L_HortFrontLeft.obj",	//2
	"../models/Map/L_HortFrontRight.obj",	//3
	"../models/Map/L_VertBackLeft.obj",		//4
	"../models/Map/L_VertBackRight.obj",	//5
	"../models/Map/L_VertFrontLeft.obj",
	"../models/Map/L_VertFrontRight.obj",
	"../models/Map/Small_BackLeft.obj",
	"../models/Map/Small_BackRight.obj",
	"../models/Map/Small_FrontLeft.obj",	//10
	"../models/Map/Small_FrontRight.obj",
	"../models/Map/Square_FrontLeft.obj",
	"../models/Map/Square_FrontRight.obj",
	"../models/Map/Square_Left.obj",
	"../models/Map/Square_Middle.obj",		//15
	"../models/Map/Square_Right.obj",
	"../models/Map/Wall_InBackLeft.obj",
	"../models/Map/Wall_InBackMiddle.obj",
	"../models/Map/Wall_InBackRight.obj",
	"../models/Map/Wall_InFrontLeft.obj",	//20
	"../models/Map/Wall_InFrontMiddle.obj",
	"../models/Map/Wall_InFrontRight.obj",
	"../models/Map/Wall_OutBack.obj",
	"../models/Map/Wall_OutFront.obj",
	"../models/Map/Wall_OutLeft.obj",		//25
	"../models/Map/Wall_OutRight.obj"
};

// Terrain model instance
Terrain terrain(-1, -1, 200, 16, "../Textures/heightmap2.png");

GLuint textureCespedID, textureWallID, textureWindowID, textureRainID,
textureSmokeID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID,
textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticleFountainID, textureParticleSmokeID, textureParticleRainID, texIdSmoke, texIdRain;
GLuint skyboxTextureID;

// Modelo para el redener de texto
FontTypeRendering::FontTypeRendering* modelText;
FontTypeRendering::FontTypeRendering* modelText2;
FontTypeRendering::FontTypeRendering* modelText3;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/mp_bloodvalley/blood-valley_ft.tga",
		"../Textures/mp_bloodvalley/blood-valley_bk.tga",
		"../Textures/mp_bloodvalley/blood-valley_up.tga",
		"../Textures/mp_bloodvalley/blood-valley_dn.tga",
		"../Textures/mp_bloodvalley/blood-valley_rt.tga",
		"../Textures/mp_bloodvalley/blood-valley_lf.tga" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
//glm::mat4 modelMatrixMayow = glm::mat4(1.0f);
//glm::mat4 matrixModelRock = glm::mat4(1.0);
glm::mat4 modelMatrixOsmosis = glm::mat4(1.0f);
glm::mat4 modelMatrixCovid[NUM_COVID] = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
glm::mat4 modelMatrixMask = glm::mat4(1.0f);
glm::mat4 modelMatrixBullet = glm::mat4(1.0f);
glm::mat4 modelMatrixBulletBody = glm::mat4(1.0f);
glm::mat4 modelMatrixBulletRef = glm::mat4(1.0f);
glm::mat4 modelMatrixMapTest = glm::mat4(1.0f);
//glm::mat4 modelMatrixMapRef = glm::mat4(1.0f);
glm::mat4 modelMatrixMap = glm::mat4(1.0f);
glm::mat4 modelMatrixFountain = glm::mat4(1.0f);

int animationIndex = 1;
int modelSelected = 0;
bool enableCountSelected = true;

// Bullet states
bool enableBulletFiring = true;
bool bulletIsActive = false;
float bulletMovement = 0.0;
float bulletMaxMovement = 10.0;

// Variables to animations keyframes
bool saveFrame = false, availableSave = true;
std::ofstream myfile;
std::string fileName = "";
bool record = false;

// Rendering flags
bool renderMask[NUM_MASKS] = { true, true, true, true, true, true, true, true };
bool renderCovid[NUM_COVID] = { true, true, true, false, true };
int freeCovid = 3;

// Var Covid machines
float stepCountCovid[NUM_COVID] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
int stateCovid[NUM_COVID] = { 0, 0, 0, 0, 0 };

glm::vec3 colorNeb = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 neblinaGris = glm::vec3(0.25, 0.25, 0.25);
glm::vec3 neblinaRoja = glm::vec3(0.9, 0.2, 0.2);
glm::vec3 colorLluvia = glm::vec3(0.5, 0.5, 0.5);
glm::vec3 lluviaGris = glm::vec3(0.25, 0.25, 0.25);
glm::vec3 lluviaRoja = glm::vec3(0.9, 0.2, 0.2);

//bool banderaDisparo = true;

// Lamps positions
std::vector<glm::vec3> lamp1Position = { glm::vec3(-7.03, 0, -19.14), glm::vec3(
		24.41, 0, -34.57), glm::vec3(-10.15, 0, -54.10) };
std::vector<float> lamp1Orientation = { -17.0, -82.67, 23.70 };
std::vector<glm::vec3> lamp2Position = { glm::vec3(-36.52, 0, -23.24),
		glm::vec3(-52.73, 0, -3.90) };
std::vector<float> lamp2Orientation = { 21.37 + 90, -65.0 + 90 };

// Covid positions
// Left Down, Left Up, Right Down, Right Up, Middle
std::vector<glm::vec3> covidPositions = { glm::vec3(-69.0f, 0.1684f, 35.0f), glm::vec3(-69.0f, 0.1684f, -32.0f), glm::vec3(69.0f, 0.1684f, 35.0f),
	glm::vec3(69.0f, 0.1684f, -32.0f), glm::vec3(-30.0f, 0.1684f, -15.0f) };

// Mask positions
std::vector<glm::vec3> maskPositions = { glm::vec3(0.0f, 0.0f, -32.0f), glm::vec3(0.0f, 0.0f, 35.0f), glm::vec3(-40.0f, 0.0f, 2.0f),
	glm::vec3(40.0f, 0.0f, 2.0f), glm::vec3(-69.0f, 0.0f, 1.0f), glm::vec3(69.0f, 0.0f, 1.0f), glm::vec3(12.0f, 0.0f, 1.0f),
	glm::vec3(0.0f, 0.0f, 18.5f) };

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = {
	{ "Smoke1", glm::vec3(-8.0, 0.0, -15.0)},
	{ "Smoke2", glm::vec3(-2.0, 0.0, -15.0)},
	{ "Smoke3", glm::vec3(4.0, 0.0, -15.0)},
	{ "Smoke4", glm::vec3(10.0, 0.0, -15.0)},
	{ "Smoke5", glm::vec3(16.0, 0.0, -15.0)},
	{ "Rain", glm::vec3(0.0, 0.0, 0.0)}
};

double deltaTime;
double currTime, lastTime;

// Jump variables
bool isJump = false;
float GRAVITY = 1.81;
double tmv = 0;
double startTimeJump = 0;

// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles = 8000;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Definition for the particle system smoke
GLuint initVelSmoke, startTimeSmoke;
GLuint VAOParticlesSmoke;
GLuint nParticlesSmoke = 2000;
GLuint posBufSmoke[2], velBufSmoke[2], ageSmoke[2];
GLuint particleArraySmoke[2];
GLuint feedbackSmoke[2];
GLuint drawBufSmoke = 1;
float particleSizeSmoke = 0.5, particleLifetimeSmoke = 6.0;
double currTimeParticlesAnimationSmoke, lastTimeParticlesAnimationSmoke;

// Definition for the particle system rain
GLuint initVelRain, startTimeRain;
GLuint VAOParticlesRain;
GLuint nParticlesRain = 15000;
GLuint posBufRain[2], velBufRain[2], ageRain[2];
GLuint particleArrayRain[2];
GLuint feedbackRain[2];
GLuint drawBufRain = 1;
float particleSizeRain = 0.5, particleLifetimeRain = 8.0;
double currTimeParticlesAnimationRain, lastTimeParticlesAnimationRain;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 */

 // OpenAL Defines
#define NUM_BUFFERS 5
#define NUM_SOURCES 5
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
//ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
//ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source for Covid
ALfloat sourceCovidPos[NUM_COVID][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } };
ALfloat sourceCovidVel[NUM_COVID][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } };
//ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
//ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };
// Source 2
//ALfloat source2Pos[] = { 2.0, 0.0, 0.0 };
//ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };
// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid* data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = { true, true, true, false, true };

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
//void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);

//void initParticleBuffers() {
//	// Generate the buffers
//	glGenBuffers(1, &initVel);   // Initial velocity buffer
//	glGenBuffers(1, &startTime); // Start time buffer
//
//	// Allocate space for all buffers
//	int size = nParticles * 3 * sizeof(float);
//	glBindBuffer(GL_ARRAY_BUFFER, initVel);
//	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, startTime);
//	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL,
//		GL_STATIC_DRAW);
//
//	// Fill the first velocity buffer with random velocities
//	glm::vec3 v(0.0f);
//	float velocity, theta, phi;
//	GLfloat* data = new GLfloat[nParticles * 3];
//	for (unsigned int i = 0; i < nParticles; i++) {
//
//		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f,
//			((float)rand() / RAND_MAX));
//		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));
//
//		v.x = 5 * sinf(theta) * 5 * cosf(phi);
//		v.y = 5 * cosf(theta);
//		v.z = 5 * sinf(theta) * 5 * sinf(phi);
//
//		velocity = glm::mix(2.0f, 1.0f, ((float)rand() / RAND_MAX));
//		v = glm::normalize(v) * velocity;
//
//		data[3 * i] = v.x;
//		data[3 * i + 1] = v.y;
//		data[3 * i + 2] = v.z;
//	}
//	glBindBuffer(GL_ARRAY_BUFFER, initVel);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
//
//	// Fill the start time buffer
//	delete[] data;
//	data = new GLfloat[nParticles];
//	float time = 0.0f;
//	float rate = 0.00075f;
//	for (unsigned int i = 0; i < nParticles; i++) {
//		data[i] = time;
//		time += rate;
//	}
//	glBindBuffer(GL_ARRAY_BUFFER, startTime);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	delete[] data;
//
//	glGenVertexArrays(1, &VAOParticles);
//	glBindVertexArray(VAOParticles);
//	glBindBuffer(GL_ARRAY_BUFFER, initVel);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//	glEnableVertexAttribArray(0);
//
//	glBindBuffer(GL_ARRAY_BUFFER, startTime);
//	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
//	glEnableVertexAttribArray(1);
//
//	glBindVertexArray(0);
//}

void resetGame() {

}

void initParticleBuffersSmoke() {
	// Generate the buffers
	glGenBuffers(2, posBufSmoke);    // position buffers
	glGenBuffers(2, velBufSmoke);    // velocity buffers
	glGenBuffers(2, ageSmoke);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesSmoke * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBufSmoke[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBufSmoke[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBufSmoke[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBufSmoke[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, ageSmoke[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, ageSmoke[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesSmoke);
	float rate = particleLifetimeSmoke / nParticlesSmoke;
	for (unsigned int i = 0; i < nParticlesSmoke; i++) {
		int index = i - nParticlesSmoke;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine{ };
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, ageSmoke[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArraySmoke);

	// Set up particle array 0
	glBindVertexArray(particleArraySmoke[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBufSmoke[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBufSmoke[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, ageSmoke[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArraySmoke[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBufSmoke[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBufSmoke[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, ageSmoke[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedbackSmoke);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackSmoke[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBufSmoke[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBufSmoke[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, ageSmoke[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackSmoke[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBufSmoke[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBufSmoke[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, ageSmoke[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void initParticleBuffersRain() {
	// Generate the buffers
	glGenBuffers(2, posBufRain);    // position buffers
	glGenBuffers(2, velBufRain);    // velocity buffers
	glGenBuffers(2, ageRain);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesRain * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBufRain[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBufRain[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBufRain[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBufRain[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, ageRain[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, ageRain[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesRain);
	float rate = particleLifetimeRain / nParticlesRain;
	for (unsigned int i = 0; i < nParticlesRain; i++) {
		int index = i - nParticlesRain;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine{ };
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, ageRain[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArrayRain);

	// Set up particle array 0
	glBindVertexArray(particleArrayRain[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBufRain[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBufRain[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, ageRain[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArrayRain[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBufRain[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBufRain[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, ageRain[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedbackRain);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackRain[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBufRain[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBufRain[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, ageRain[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackRain[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBufRain[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBufRain[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, ageRain[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs","../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs","../Shaders/terrain_shadow.fs");
	shaderParticlesSmoke.initialize("../Shaders/particlesSmoke.vs", "../Shaders/particlesSmoke.fs", { "Position", "Velocity", "Age" });
	shaderParticlesRain.initialize("../Shaders/particlesRain.vs","../Shaders/particlesRain.fs", { "Position", "Velocity", "Age" });
	shaderViewDepth.initialize("../Shaders/texturizado.vs","../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs","../Shaders/shadow_mapping_depth.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(100.0f, 100.0f, 100.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	boxViewDepth.init();
	boxViewDepth.setShader(&shaderViewDepth);

	boxLightViewBox.init();
	boxLightViewBox.setShader(&shaderViewDepth);

	/*modelRock.loadModel("../models/rock/rock.obj");
	modelRock.setShader(&shaderMulLighting);*/

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));

	//Lamp models
	modelLamp1.loadModel("../models/Lamps/PointLamp/PointLamp.fbx");
	modelLamp1.setShader(&shaderMulLighting);
	modelLamp2.loadModel("../models/Lamps/SpotLamp/SpotLamp2.fbx");
	modelLamp2.setShader(&shaderMulLighting);
	/*modelLampPost2.loadModel("../models/Street_Light/LampPost.obj");
	modelLampPost2.setShader(&shaderMulLighting);*/

	//Grass
	modelGrass.loadModel("../models/grass/grassModel.obj");
	modelGrass.setShader(&shaderMulLighting);

	//Fountain
	modelFountain.loadModel("../models/fountain/fountain.obj");
	modelFountain.setShader(&shaderMulLighting);

	//Mayow
	/*mayowModelAnimate.loadModel("../models/mayow/personaje2.fbx");
	mayowModelAnimate.setShader(&shaderMulLighting);*/

	//Osmosis
	modelOsmosisAnimate.loadModel("../models/Osmosis/OsmosisDisparo.fbx");
	modelOsmosisAnimate.setShader(&shaderMulLighting);

	//Map
	modelMapTest.loadModel("../models/Map/Map.obj");
	modelMapTest.setShader(&shaderMulLighting);

	//modelMapRef.loadModel("../models/Map/Reference.obj");
	//modelMapRef.setShader(&shaderMulLighting);

	for (unsigned int i = 0; i < 27; i++)
	{
		mapArray[i].loadModel(mapDirs[i]);
		mapArray[i].setShader(&shaderMulLighting);
	}

	// Covid
	for (unsigned int i = 0; i < NUM_COVID; i++)
	{
		covidArray[i].loadModel("../models/Covid/Covid.fbx");
		covidArray[i].setShader(&shaderMulLighting);
	}

	// Masks
	for (unsigned int i = 0; i < NUM_MASKS; i++)
	{
		maskArray[i].loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
		maskArray[i].setShader(&shaderMulLighting);
	}

	// Bullet
	modelBulletAnimate.loadModel("../models/Osmosis/bala.fbx");
	modelBulletAnimate.setShader(&shaderMulLighting);

	//Camara en primera persona
	//cameraFP->setPosition(glm::vec3(30.0f, 7.0f, -2.0f));

	//Camara en tercera persona
	camera->setPosition(glm::vec3(0.0, 5.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP* bitmap;
	unsigned char* data;

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP* bitmap = skyboxTexture.loadImage(true);
		unsigned char* data = skyboxTexture.convertToData(bitmap, imageWidth,
			imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
				GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

	// Definiendo la textura a utilizar
	Texture textureCesped("../Textures/cesped.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureCesped.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureCesped.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureCespedID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCesped.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureWall("../Textures/whiteWall.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureWall.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureWall.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureWallID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureWallID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureWall.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureWindow("../Textures/ventana.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureWindow.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureWindow.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureWindowID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureWindowID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureWindow.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureRain("../Textures/copo.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureRain.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureRain.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureRainID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureRainID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureRain.freeImage(bitmap);

	// Definiendo la textura a utilizar
	//Texture textureSmoke("../Textures/blod.png");
	//// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	//bitmap = textureSmoke.loadImage();
	//// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	//data = textureSmoke.convertToData(bitmap, imageWidth, imageHeight);
	//// Creando la textura con id 1
	//glGenTextures(1, &textureSmokeID);
	//// Enlazar esa textura a una tipo de textura de 2D.
	//glBindTexture(GL_TEXTURE_2D, textureSmokeID);
	//// set the texture wrapping parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//// set texture filtering parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//// Verifica si se pudo abrir la textura
	//if (data) {
	//	// Transferis los datos de la imagen a memoria
	//	// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
	//	// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
	//	// a los datos
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
	//		GL_BGRA, GL_UNSIGNED_BYTE, data);
	//	// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//	std::cout << "Failed to load texture" << std::endl;
	//// Libera la memoria de la textura
	//textureSmoke.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/grassy2.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Textures/mud.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Textures/grassFlowers.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Textures/path.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/blendMap.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);

	Texture textureParticlesRain("../Textures/copo.png");
	bitmap = textureParticlesRain.loadImage();
	data = textureParticlesRain.convertToData(bitmap, imageWidth,
		imageHeight);
	glGenTextures(1, &textureParticleRainID);
	glBindTexture(GL_TEXTURE_2D, textureParticleRainID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticlesRain.freeImage(bitmap);

	Texture textureParticleSmoke("../Textures/blood.png");
	bitmap = textureParticleSmoke.loadImage();
	data = textureParticleSmoke.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleSmokeID);
	glBindTexture(GL_TEXTURE_2D, textureParticleSmokeID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticleSmoke.freeImage(bitmap);

	//Smoke
	std::uniform_real_distribution<float> distr01 =	std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesSmoke * 2;
	std::vector<GLfloat> randData(size);
	for (int i = 0; i < randData.size(); i++) {
		randData[i] = distr01(generator);
	}
	glGenTextures(1, &texIdSmoke);
	glBindTexture(GL_TEXTURE_1D, texIdSmoke);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT,randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesSmoke.setInt("Pass", 1);
	shaderParticlesSmoke.setInt("ParticleTex", 0);
	shaderParticlesSmoke.setInt("RandomTex", 1);
	shaderParticlesSmoke.setFloat("ParticleLifetime", particleLifetimeSmoke);
	shaderParticlesSmoke.setFloat("ParticleSize", particleSizeSmoke);
	shaderParticlesSmoke.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0.0f, 0.1f, 0.0f)));
	shaderParticlesSmoke.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	//Rain
	std::uniform_real_distribution<float> distr02 = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator2;
	std::random_device rd2;
	generator2.seed(rd2());
	int size2 = nParticlesRain * 2;
	std::vector<GLfloat> randData2(size);
	for (int i = 0; i < randData2.size(); i++) {
		randData2[i] = distr02(generator2);
	}
	glGenTextures(1, &texIdRain);
	glBindTexture(GL_TEXTURE_1D, texIdRain);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size2);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size2, GL_RED, GL_FLOAT, randData2.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesRain.setInt("Pass", 1);
	shaderParticlesRain.setInt("ParticleTex", 0);
	shaderParticlesRain.setInt("RandomTex", 1);
	shaderParticlesRain.setFloat("ParticleLifetime", particleLifetimeRain);
	shaderParticlesRain.setFloat("ParticleSize", particleSizeRain);
	shaderParticlesRain.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0.0f, -1.2f, 0.0f)));
	shaderParticlesRain.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f, 10.0f, 0.0f)));

	//Smoke
	glm::mat3 basis;
	glm::vec3 u, v, n;
	v = glm::vec3(0, 1, 0);
	n = glm::cross(glm::vec3(1, 0, 0), v);
	if (glm::length(n) < 0.00001f) {
		n = glm::cross(glm::vec3(0, 1, 0), v);
	}
	u = glm::cross(v, n);
	basis[0] = glm::normalize(u);
	basis[1] = glm::normalize(v);
	basis[2] = glm::normalize(n);
	shaderParticlesSmoke.setMatrix3("EmitterBasis", 1, false,glm::value_ptr(basis));
	shaderParticlesRain.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));

	/*******************************************
	 * Inicializacion de los buffers de la fuente
	 *******************************************/
	//initParticleBuffers();

	/*******************************************
	 * Inicializacion de los buffers del Smoke
	 *******************************************/
	initParticleBuffersSmoke();

	/*******************************************
	 * Inicializacion de los buffers del Rain
	 *******************************************/
	initParticleBuffersRain();

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
		SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/zero.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/one.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/two.wav");
	buffer[3] = alutCreateBufferFromFile("../sounds/three.wav");
	buffer[4] = alutCreateBufferFromFile("../sounds/four.wav");
	//buffer[2] = alutCreateBufferFromFile("../sounds/darth_vader.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR) {
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}

	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	/*alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 3.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);*/

	for (unsigned int i = 0; i < NUM_COVID; i++) {
		alSourcef(source[i], AL_PITCH, 1.0f);
		alSourcef(source[i], AL_GAIN, 3.0f);
		alSourcefv(source[i], AL_POSITION, sourceCovidPos[i]);
		alSourcefv(source[i], AL_VELOCITY, sourceCovidVel[i]);
		alSourcei(source[i], AL_BUFFER, buffer[i]);
		alSourcei(source[i], AL_LOOPING, AL_TRUE);
		alSourcef(source[i], AL_MAX_DISTANCE, 500);
	}

	/*alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 0.3f);
	alSourcefv(source[2], AL_POSITION, source2Pos);
	alSourcefv(source[2], AL_VELOCITY, source2Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 500);*/

	// Se inicializa el modelo de texeles.
	modelText = new FontTypeRendering::FontTypeRendering(screenWidth,
		screenHeight);
	modelText->Initialize();
	modelText2 = new FontTypeRendering::FontTypeRendering(screenWidth,
		screenHeight);
	modelText2->Initialize();
	modelText3 = new FontTypeRendering::FontTypeRendering(screenWidth,
		screenHeight);
	modelText3->Initialize();
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	shaderParticlesSmoke.destroy();
	shaderParticlesRain.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	boxViewDepth.destroy();
	boxLightViewBox.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	//modelRock.destroy();
	modelLamp1.destroy();
	modelLamp2.destroy();
	//modelLampPost2.destroy();
	modelGrass.destroy();
	modelFountain.destroy();

	// Custom objects animate
	//mayowModelAnimate.destroy();
	modelOsmosisAnimate.destroy();
	for (unsigned int i = 0; i < NUM_COVID; i++)
	{
		covidArray[i].destroy();
	}
	for (unsigned int i = 0; i < NUM_MASKS; i++)
	{
		maskArray[i].destroy();
	}
	modelBulletAnimate.destroy();
	modelMapTest.destroy();
	//modelMapRef.destroy();
	//modelMapSquareFL.destroy();
	for (unsigned int i = 0; i < 27; i++)
	{
		mapArray[i].destroy();
	}

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureRainID);
	glDeleteTextures(1, &textureSmokeID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleFountainID);
	glDeleteTextures(1, &textureParticleSmokeID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	//// Remove the buffer of the fountain particles
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glDeleteBuffers(1, &initVel);
	//glDeleteBuffers(1, &startTime);
	//glBindVertexArray(0);
	//glDeleteVertexArrays(1, &VAOParticles);

	// Remove the buffer of the Smoke particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBufSmoke);
	glDeleteBuffers(2, velBufSmoke);
	glDeleteBuffers(2, ageSmoke);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedbackSmoke);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesSmoke);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBufRain);
	glDeleteBuffers(2, velBufRain);
	glDeleteBuffers(2, ageRain);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedbackRain);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesRain);
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	//Camara en tercera persona
	distanceFromTarget -= yoffset;
	//camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE) {
		//std::cout << "Esta presente el joystick" << std::endl;
		int axesCount, buttonCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		/*std::cout << "Número de ejes disponibles :=>" << axesCount << std::endl;
		std::cout << "Left Stick X axis: " << axes[0] << std::endl;
		std::cout << "Left Stick Y axis: " << axes[1] << std::endl;
		std::cout << "Left Trigger/LT: " << axes[4] << std::endl;
		std::cout << "Right Stick X axis: " << axes[3] << std::endl;
		std::cout << "Right Stick Y axis: " << axes[2] << std::endl;
		std::cout << "Right Trigger/RT: " << axes[5] << std::endl;*/

		//Mover Adelante Atras
		if (fabs(axes[1]) != 0.0) {
			modelMatrixOsmosis = glm::translate(modelMatrixOsmosis,	glm::vec3(0, 0, axes[1] * 0.2));
			animationIndex = 1;
		}

		//Mover Izquierda Derecha
		if (fabs(axes[0]) != 0.0) {
			modelMatrixOsmosis = glm::translate(modelMatrixOsmosis, glm::vec3(-axes[0] * 0.2,0,0));
			animationIndex = 1;
		}

		//Girar Izquierda Derecha
		if (fabs(axes[2]) != 0.0) {
			modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(-axes[2] * 2), glm::vec3(0, 1, 0));
			//camera->mouseMoveCamera(axes[2], 0, deltaTime);
			animationIndex = 1;
		}

		if (enableBulletFiring && axes[5] > 0) {
			if (bullets > 0) {
				bullets -= 1;
				enableBulletFiring = false;
				bulletIsActive = true;
				std::cout << "Bullet fired" << std::endl;
			}
			//if (banderaDisparo) {
			//	//aqui lo del disparo
			//	std::cout << "Disparo" << std::endl;
			//	if (bullets > 0) {
			//		bullets -= 1;
			//		banderaDisparo = false;
			//	}
			//}
			animationIndex = 1;
		}
		else if (axes[5] <= 0) {
			if (!bulletIsActive) {
				enableBulletFiring = true;
			}
		}
			
			
		/*if (fabs(axes[3]) > 0.2) {
			camera->mouseMoveCamera(0.0, axes[3], deltaTime);
		}*/
		/*if (fabs(axes[4]) > 0.2) {
			camera->mouseMoveCamera(0.0, axes[4], deltaTime);
		}*/

		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1,
			&buttonCount);
		//std::cout << "Número de botones disponibles :=>" << buttonCount << std::endl;
		if (buttons[0] == GLFW_PRESS)
			std::cout << "Se presiona A" << std::endl;
		if (buttons[1] == GLFW_PRESS)
			std::cout << "Se presiona B" << std::endl;
		/*if (buttons[2] == GLFW_PRESS) {
			std::cout << "Se presiona X" << std::endl;
			std::cout << "Recargas" << std::endl;
			if (bullets != 0) {
				banderaDisparo = true;
			}
		}*/
		if (buttons[3] == GLFW_PRESS)
			std::cout << "Se presiona Y" << std::endl;
		if (buttons[4] == GLFW_PRESS)
			std::cout << "LB" << std::endl;
		if (buttons[5] == GLFW_PRESS)
			std::cout << "RB" << std::endl;
		if (buttons[6] == GLFW_PRESS)
			std::cout << "Back" << std::endl;
		if (buttons[7] == GLFW_PRESS)
			std::cout << "Start" << std::endl;
		if (buttons[8] == GLFW_PRESS)
			std::cout << "Stick Izquierdo" << std::endl;
		if (buttons[9] == GLFW_PRESS)
			std::cout << "Stick Derecho" << std::endl;
		if (buttons[10] == GLFW_PRESS)
			std::cout << "Cruceta Arriba" << std::endl;
		if (buttons[11] == GLFW_PRESS)
			std::cout << "Cruceta Derecha" << std::endl;
		if (buttons[12] == GLFW_PRESS)
			std::cout << "Cruceta Abajo" << std::endl;
		if (buttons[13] == GLFW_PRESS)
			std::cout << "Cruceta Izquierda" << std::endl;

		if (!isJump && buttons[0] == GLFW_PRESS) {
			isJump = true;
			startTimeJump = currTime;
			tmv = 0;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		modelMatrixOsmosis = glm::translate(modelMatrixOsmosis, glm::vec3(0, 0, 0.2));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		modelMatrixOsmosis = glm::translate(modelMatrixOsmosis, glm::vec3(0, 0, -0.2));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(2.0f), glm::vec3(0, 1, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(-2.0f), glm::vec3(0, 1, 0));
	}

	if (enableBulletFiring && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (bullets > 0) {
			bullets -= 1;
			enableBulletFiring = false;
			bulletIsActive = true;
			std::cout << "Bullet fired" << std::endl;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
		if (!bulletIsActive) {
			enableBulletFiring = true;
		}
	}

	/*if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (banderaDisparo) {
			//aqui lo del disparo
			std::cout << "Disparo" << std::endl;
			if (bullets > 0) {
				bullets -= 1;
				banderaDisparo = false;
			}
		}
		animationIndex = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		std::cout << "Recargas" << std::endl;
		if (bullets != 0) {
			banderaDisparo = true;
		}
	}*/

	//Para que no pueda mover la camara el usuario
	//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	//	camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	//	camera->mouseMoveCamera(0.0, offsetY, deltaTime);
	offsetX = 0;
	offsetY = 0;

	// Seleccionar modelo
	/*if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		enableCountSelected = false;
		modelSelected++;
		if (modelSelected > 2)
			modelSelected = 0;
		if (modelSelected == 1)

			if (modelSelected == 2)

				std::cout << "modelSelected:" << modelSelected << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		enableCountSelected = true;*/
	//Movimiento Osmosis
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(3.0f),
			glm::vec3(0, 1, 0));
	}
	else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(-3.0f),
			glm::vec3(0, 1, 0));
	}
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		modelMatrixOsmosis = glm::translate(modelMatrixOsmosis,
			glm::vec3(0, 0, 0.2));
	}
	else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		modelMatrixOsmosis = glm::translate(modelMatrixOsmosis,
			glm::vec3(0, 0, -0.2));
	}
	//Movimiento mayow
	/*if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		modelMatrixMayow = glm::rotate(modelMatrixMayow, glm::radians(1.0f),
				glm::vec3(0, 1, 0));
		animationIndex = 0;
	} else if (modelSelected
			== 2&& glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		modelMatrixMayow = glm::rotate(modelMatrixMayow, glm::radians(-1.0f),
				glm::vec3(0, 1, 0));

		animationIndex = 0;
	}
	if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		modelMatrixMayow = glm::translate(modelMatrixMayow,
				glm::vec3(0, 0, 0.02));
		animationIndex = 0;
	} else if (modelSelected
			== 2&& glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		modelMatrixMayow = glm::translate(modelMatrixMayow,
				glm::vec3(0, 0, -0.02));
		animationIndex = 0;
	}*/

	bool keySpaceStatus = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if (!isJump && keySpaceStatus) {
		isJump = true;
		startTimeJump = currTime;
		tmv = 0;
	}
		

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	std::map<std::string, glm::vec3> blendingUnsorted = {
	{ "fountain", glm::vec3(5.0, 0.0, -40.0) },
	{ "fire", glm::vec3(0.0, 0.0, 7.0) }
	};

	//matrixModelRock = glm::translate(matrixModelRock,
	//	glm::vec3(-3.0, 0.0, 2.0));
	/*modelMatrixMayow = glm::translate(modelMatrixMayow,
			glm::vec3(13.0f, 0.05f, -5.0f));
	modelMatrixMayow = glm::rotate(modelMatrixMayow, glm::radians(-180.0f),
			glm::vec3(0, 1, 0));*/

	// Ubicación Osmosis
	modelMatrixOsmosis = glm::translate(modelMatrixOsmosis,
		glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(180.0f),
		glm::vec3(0, 1, 0));

	// Covid
	for (unsigned int i = 0; i < NUM_COVID; i++)
	{
		modelMatrixCovid[i] = glm::translate(modelMatrixCovid[i], covidPositions[i]);
	}

	// Bullet
	//modelMatrixBullet = glm::translate(modelMatrixBullet, glm::vec3(-13.0f, 5.0f, 2.0f));

	modelMatrixFountain = glm::translate(modelMatrixFountain,glm::vec3(69, 0.0, -13.0));
	modelMatrixFountain[3][1] = terrain.getHeightTerrain(
		modelMatrixFountain[3][0], modelMatrixFountain[3][2]) + 0.2;
	modelMatrixFountain = glm::scale(modelMatrixFountain,
		glm::vec3(10.0f, 10.0f, 10.0f));

	//Map
	//modelMatrixMapSquareFL = glm::translate(modelMatrixMapSquareFL,
	//	glm::vec3(-15.1655f, 16.93653f, 4.13053f));

	lastTime = TimeManager::Instance().GetTime();

	// Time for the particles animation
	/*currTimeParticlesAnimation = lastTime;
	lastTimeParticlesAnimation = lastTime;*/

	currTimeParticlesAnimationSmoke = lastTime;
	lastTimeParticlesAnimationSmoke = lastTime;

	currTimeParticlesAnimationRain = lastTime;
	lastTimeParticlesAnimationRain = lastTime;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);

	shadowBox = new ShadowBox(-lightPos, camera.get(), 15.0f, 0.1f, 45.0f);

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if (currTime - lastTime < 0.016666667) {
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;

		// Variables donde se guardan las matrices de cada articulacion por 1 frame
		std::vector<float> matrixDartJoints;
		std::vector<glm::mat4> matrixDart;

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

		axis = glm::axis(glm::quat_cast(modelMatrixOsmosis));
		angleTarget = glm::angle(glm::quat_cast(modelMatrixOsmosis));
		target = glm::vec3 (modelMatrixOsmosis[3].x, modelMatrixOsmosis[3].y + 4.25, modelMatrixOsmosis[3].z);

		if (std::isnan(angleTarget))
			angleTarget = 0.0;
		if (axis.y < 0)
			angleTarget = -angleTarget;
		//if (modelSelected == 1)
		//	angleTarget -= glm::radians(90.0f);
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		view = camera->getViewMatrix();

		shadowBox->update(screenWidth, screenHeight);
		glm::vec3 centerBox = shadowBox->getCenter();

		// Projection light shadow mapping
		glm::mat4 lightProjection = glm::mat4(1.0f), lightView = glm::mat4(
			1.0f);
		glm::mat4 lightSpaceMatrix;

		lightProjection[0][0] = 2.0f / shadowBox->getWidth();
		lightProjection[1][1] = 2.0f / shadowBox->getHeight();
		lightProjection[2][2] = -2.0f / shadowBox->getLength();
		lightProjection[3][3] = 1.0f;

		lightView = glm::lookAt(centerBox,
			centerBox + glm::normalize(-lightPos),
			glm::vec3(0.0, 1.0, 0.0));

		lightSpaceMatrix = lightProjection * lightView;
		shaderDepth.setMatrix4("lightSpaceMatrix", 1, false,
			glm::value_ptr(lightSpaceMatrix));

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
			glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false, glm::value_ptr(view));
		shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
			glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false, glm::value_ptr(view));
		shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
			glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader para el Smoke
		shaderParticlesSmoke.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderParticlesSmoke.setMatrix4("view", 1, false, glm::value_ptr(view));
		shaderParticlesSmoke.setVectorFloat3("colorSmoke", glm::value_ptr(glm::vec3(1,1,1)));

		if (hp <= 1)
			colorLluvia = lluviaRoja;
		else
			colorLluvia = lluviaGris;
		// Settea la matriz de vista y projection al shader para el Rain
		shaderParticlesRain.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderParticlesRain.setMatrix4("view", 1, false, glm::value_ptr(view));
		shaderParticlesRain.setVectorFloat3("colorRain", glm::value_ptr(colorLluvia));

		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		if (hp <= 1)
			colorNeb = neblinaRoja;
		else
			colorNeb = neblinaGris;
		shaderMulLighting.setVectorFloat3("fogColor",glm::value_ptr(colorNeb));
		shaderMulLighting.setFloat("density", 0.055);
		shaderTerrain.setVectorFloat3("fogColor",glm::value_ptr(colorNeb));
		shaderTerrain.setFloat("density", 0.08);
		shaderSkybox.setVectorFloat3("fogColor",glm::value_ptr(colorNeb));
		shaderSkybox.setFloat("density", 0.08);


		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos",
			glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient",
			glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse",
			glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular",
			glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction",
			glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/
		shaderTerrain.setVectorFloat3("viewPos",
			glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient",
			glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse",
			glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular",
			glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.direction",
			glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		 /*glm::vec3 spotPosition = glm::vec3(
				 modelMatrixHeli * glm::vec4(0.32437, 0.226053, 1.79149, 1.0));
		 shaderMulLighting.setInt("spotLightCount", 1);
		 shaderTerrain.setInt("spotLightCount", 1);
		 shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient",
				 glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		 shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse",
				 glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		 shaderMulLighting.setVectorFloat3("spotLights[0].light.specular",
				 glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		 shaderMulLighting.setVectorFloat3("spotLights[0].position",
				 glm::value_ptr(spotPosition));
		 shaderMulLighting.setVectorFloat3("spotLights[0].direction",
				 glm::value_ptr(glm::vec3(0, -1, 0)));
		 shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		 shaderMulLighting.setFloat("spotLights[0].linear", 0.074);
		 shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
		 shaderMulLighting.setFloat("spotLights[0].cutOff",
				 cos(glm::radians(12.5f)));
		 shaderMulLighting.setFloat("spotLights[0].outerCutOff",
				 cos(glm::radians(15.0f)));
		 shaderTerrain.setVectorFloat3("spotLights[0].light.ambient",
				 glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		 shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse",
				 glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		 shaderTerrain.setVectorFloat3("spotLights[0].light.specular",
				 glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		 shaderTerrain.setVectorFloat3("spotLights[0].position",
				 glm::value_ptr(spotPosition));
		 shaderTerrain.setVectorFloat3("spotLights[0].direction",
				 glm::value_ptr(glm::vec3(0, -1, 0)));
		 shaderTerrain.setFloat("spotLights[0].constant", 1.0);
		 shaderTerrain.setFloat("spotLights[0].linear", 0.074);
		 shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
		 shaderTerrain.setFloat("spotLights[0].cutOff",
				 cos(glm::radians(12.5f)));
		 shaderTerrain.setFloat("spotLights[0].outerCutOff",
				 cos(glm::radians(15.0f)));*/

				 /*******************************************
				  * Propiedades PointLights
				  *******************************************/
		shaderMulLighting.setInt("pointLightCount",
			lamp1Position.size() + lamp2Orientation.size());
		shaderTerrain.setInt("pointLightCount",
			lamp1Position.size() + lamp2Orientation.size());
		for (int i = 0; i < lamp1Position.size(); i++) {
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp,
				lamp1Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp,
				glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustLamp = glm::scale(matrixAdjustLamp,
				glm::vec3(0.5, 0.5, 0.5));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp,
				glm::vec3(0, 10.3585, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3(
				"pointLights[" + std::to_string(i) + "].light.ambient",
				glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3(
				"pointLights[" + std::to_string(i) + "].light.diffuse",
				glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderMulLighting.setVectorFloat3(
				"pointLights[" + std::to_string(i) + "].light.specular",
				glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3(
				"pointLights[" + std::to_string(i) + "].position",
				glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat(
				"pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderMulLighting.setFloat(
				"pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderMulLighting.setFloat(
				"pointLights[" + std::to_string(i) + "].quadratic", 0.01);
			shaderTerrain.setVectorFloat3(
				"pointLights[" + std::to_string(i) + "].light.ambient",
				glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3(
				"pointLights[" + std::to_string(i) + "].light.diffuse",
				glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderTerrain.setVectorFloat3(
				"pointLights[" + std::to_string(i) + "].light.specular",
				glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3(
				"pointLights[" + std::to_string(i) + "].position",
				glm::value_ptr(lampPosition));
			shaderTerrain.setFloat(
				"pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderTerrain.setFloat(
				"pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderTerrain.setFloat(
				"pointLights[" + std::to_string(i) + "].quadratic", 0.02);
		}
		for (int i = 0; i < lamp2Position.size(); i++) {
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp,
				lamp2Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp,
				glm::radians(lamp2Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustLamp = glm::scale(matrixAdjustLamp,
				glm::vec3(1.0, 1.0, 1.0));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp,
				glm::vec3(0.759521, 5.00174, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].light.ambient",
				glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].light.diffuse",
				glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderMulLighting.setVectorFloat3(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].light.specular",
				glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].constant", 1.0);
			shaderMulLighting.setFloat(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].linear", 0.09);
			shaderMulLighting.setFloat(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].quadratic", 0.01);
			shaderTerrain.setVectorFloat3(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].light.ambient",
				glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].light.diffuse",
				glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderTerrain.setVectorFloat3(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].light.specular",
				glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setFloat(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].constant", 1.0);
			shaderTerrain.setFloat(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].linear", 0.09);
			shaderTerrain.setFloat(
				"pointLights[" + std::to_string(lamp1Position.size() + i)
				+ "].quadratic", 0.02);
		}

		/*******************************************
		 * 1.- We render the depth buffer
		 *******************************************/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render scene from light's point of view
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);
		prepareDepthScene();
		renderScene(false);
		//glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*******************************************
		 * Debug to view the texture view map
		 *******************************************/
		 // reset viewport
		 /*glViewport(0, 0, screenWidth, screenHeight);
		  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		  // render Depth map to quad for visual debugging
		  shaderViewDepth.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		  shaderViewDepth.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		  glActiveTexture(GL_TEXTURE0);
		  glBindTexture(GL_TEXTURE_2D, depthMap);
		  boxViewDepth.setScale(glm::vec3(2.0, 2.0, 1.0));
		  boxViewDepth.render();*/

		  /*******************************************
		   * 2.- We render the normal objects
		   *******************************************/
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prepareScene();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderMulLighting.setInt("shadowMap", 10);
		shaderTerrain.setInt("shadowMap", 10);
		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		renderScene();
		/*******************************************
		 * Debug to box light box
		 *******************************************/
		 /*glm::vec3 front = glm::normalize(-lightPos);
		  glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), front));
		  glm::vec3 up = glm::normalize(glm::cross(front, right));
		  glDisable(GL_CULL_FACE);
		  glm::mat4 boxViewTransform = glm::mat4(1.0f);
		  boxViewTransform = glm::translate(boxViewTransform, centerBox);
		  boxViewTransform[0] = glm::vec4(right, 0.0);
		  boxViewTransform[1] = glm::vec4(up, 0.0);
		  boxViewTransform[2] = glm::vec4(front, 0.0);
		  boxViewTransform = glm::scale(boxViewTransform, glm::vec3(shadowBox->getWidth(), shadowBox->getHeight(), shadowBox->getLength()));
		  boxLightViewBox.enableWireMode();
		  boxLightViewBox.render(boxViewTransform);
		  glEnable(GL_CULL_FACE);*/

		  /*******************************************
		   * Creacion de colliders
		   * IMPORTANT do this before interpolations
		   *******************************************/

		//   //Collider del la rock
		//AbstractModel::SBB rockCollider;
		//glm::mat4 modelMatrixColliderRock = glm::mat4(matrixModelRock);
		//modelMatrixColliderRock = glm::scale(modelMatrixColliderRock,
		//	glm::vec3(1.0, 1.0, 1.0));
		//modelMatrixColliderRock = glm::translate(modelMatrixColliderRock,
		//	modelRock.getSbb().c);
		//rockCollider.c = glm::vec3(modelMatrixColliderRock[3]);
		//rockCollider.ratio = modelRock.getSbb().ratio * 1.0;
		//addOrUpdateColliders(collidersSBB, "rock", rockCollider,
		//	matrixModelRock);

		// Covid colliders
		for (unsigned int i = 0; i < NUM_COVID; i++)
		{
			glm::vec3 scale;
			glm::vec3 position;
			float ratio;
			float height;
			if (renderCovid[i])
			{
				scale = glm::vec3(0.001, 0.001, 0.001);
				position = glm::vec3(covidArray[i].getSbb().c.x, covidArray[i].getSbb().c.y + 25 * 100, covidArray[i].getSbb().c.z);
				ratio = 0.03;
				height = 0.0f;
			}
			else
			{
				scale = glm::vec3(0.0, 0.0, 0.0);
				position = glm::vec3(0.0f, -100.0f * (i + 1), 0.0f);
				ratio = 0.0;
				height = -10.f;
			}
			AbstractModel::SBB covidCollider;
			glm::mat4 modelMatrixColliderCovid = glm::mat4(modelMatrixCovid[i]);
			modelMatrixColliderCovid = glm::scale(modelMatrixColliderCovid,
				scale);
			modelMatrixColliderCovid = glm::translate(modelMatrixColliderCovid,
				position);
			covidCollider.c = glm::vec3(modelMatrixColliderCovid[3].x, modelMatrixColliderCovid[3].y - height, modelMatrixColliderCovid[3].z);
			covidCollider.ratio = covidArray[i].getSbb().ratio * ratio;
			addOrUpdateColliders(collidersSBB, "covid-" + std::to_string(i),
				covidCollider, modelMatrixCovid[i]);
		}

		// Mask colliders
		for (unsigned int i = 0; i < NUM_MASKS; i++)
		{
			glm::vec3 scale;
			glm::vec3 position;
			if (renderMask[i])
			{
				scale = glm::vec3(0.28, 0.30, 0.18);
				position = glm::vec3(maskPositions[i].x - 0.8, maskPositions[i].y + 2.0, maskPositions[i].z - 0.2);
			}
			else
			{
				scale = glm::vec3(0.0, 0.0, 0.0);
				position = glm::vec3(0.0f, -10.0f * (i + 1), 0.0f);
			}
			AbstractModel::OBB maskCollider;
			glm::mat4 modelMatrixColliderMask = glm::mat4(modelMatrixMask);
			modelMatrixColliderMask = glm::translate(modelMatrixColliderMask,
				position);
			modelMatrixColliderMask = glm::rotate(modelMatrixColliderMask,
				glm::radians(90.0f), glm::vec3(0, 1, 0));
			addOrUpdateColliders(collidersOBB, "mask-" + std::to_string(i),
				maskCollider, modelMatrixColliderMask);
			maskCollider.u = glm::quat_cast(modelMatrixColliderMask);
			modelMatrixColliderMask = glm::translate(modelMatrixColliderMask,
				glm::vec3(maskArray[i].getObb().c));
			maskCollider.e = mapArray[i].getObb().e * scale;
			maskCollider.c = glm::vec3(modelMatrixColliderMask[3]);
			std::get<0>(collidersOBB.find("mask-" + std::to_string(i))->second) =
				maskCollider;
		}

		//Collider Osmosis
		AbstractModel::OBB osmosisCollider;
		glm::mat4 modelMatrixColliderOsmosis = glm::mat4(modelMatrixOsmosis);
		modelMatrixColliderOsmosis = glm::rotate(modelMatrixColliderOsmosis, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Set the orientation of collider before doing the scale
		osmosisCollider.u = glm::quat_cast(modelMatrixColliderOsmosis);
		modelMatrixColliderOsmosis = glm::scale(modelMatrixColliderOsmosis, glm::vec3(0.1, 0.1, 0.1));
		modelMatrixColliderOsmosis = glm::translate(modelMatrixColliderOsmosis,
			glm::vec3(modelOsmosisAnimate.getObb().c.x - 12.5,
				modelOsmosisAnimate.getObb().c.y + 22,
				modelOsmosisAnimate.getObb().c.z + 2));
		osmosisCollider.e = modelOsmosisAnimate.getObb().e * glm::vec3(0.1, 0.1, 0.1) * glm::vec3(0.5, 0.75, 0.5);
		osmosisCollider.c = glm::vec3(modelMatrixColliderOsmosis[3]);
		addOrUpdateColliders(collidersOBB, "Osmosis", osmosisCollider, modelMatrixOsmosis);

		// Map colliders
		//AbstractModel::OBB mapColliders[27];
		for (unsigned int i = 0; i < 27; i++)
		{
			AbstractModel::OBB mapCollider;
			glm::mat4 modelMatrixColliderMap = glm::mat4(1.0f);
			addOrUpdateColliders(collidersOBB, "map-" + std::to_string(i),
				mapCollider, modelMatrixColliderMap);
			mapCollider.u = glm::quat_cast(modelMatrixColliderMap);
			//modelMatrixColliderMap = glm::scale(modelMatrixColliderMap,
			//	glm::vec3(1.1, 1.1, 1.1));
			modelMatrixColliderMap = glm::translate(modelMatrixColliderMap,
				mapArray[i].getObb().c);
			mapCollider.c = glm::vec3(modelMatrixColliderMap[3]);
			mapCollider.e = mapArray[i].getObb().e;
			std::get<0>(collidersOBB.find("map-" + std::to_string(i))->second) =
				mapCollider;
		}

		// Collider Bullet
		AbstractModel::SBB bulletCollider;
		glm::mat4 modelMatrixColliderBullet;
		glm::vec3 bulletPosition;
		float dxBullet;
		float dyBullet;
		float dzBullet;
		if (bulletIsActive)
		{
			modelMatrixColliderBullet = glm::mat4(modelMatrixBulletBody);
			bulletPosition = glm::vec3(modelBulletAnimate.getSbb().c.x,
				modelBulletAnimate.getSbb().c.y,
				modelBulletAnimate.getSbb().c.z);
			dxBullet = -0.5;
			dyBullet = 4;
			dzBullet = 1 + bulletMovement;
		}
		else
		{
			modelMatrixColliderBullet = glm::mat4(1.0f);
			bulletPosition = glm::vec3(modelBulletAnimate.getSbb().c.x,
				modelBulletAnimate.getSbb().c.y - 100,
				modelBulletAnimate.getSbb().c.z + 1.5);
			dxBullet = 0;
			dyBullet = -100;
			dzBullet = 0;
		}		
		//modelMatrixColliderBullet = glm::rotate(modelMatrixColliderOsmosis, glm::radians(90.0f), glm::vec3(0, 1, 0));
		//modelMatrixColliderBullet = glm::scale(modelMatrixColliderBullet, glm::vec3(0.25, 0.25, 0.25));
		modelMatrixColliderBullet = glm::translate(modelMatrixColliderBullet, bulletPosition);
		bulletCollider.c = glm::vec3(modelMatrixColliderBullet[3]);
		bulletCollider.ratio = modelBulletAnimate.getSbb().ratio * 0.25;
		addOrUpdateColliders(collidersSBB, "bullet", bulletCollider, modelMatrixBulletBody);

		// Collider de mayow
		//AbstractModel::OBB mayowCollider;
		//glm::mat4 modelmatrixColliderMayow = glm::mat4(modelMatrixMayow);
		//modelmatrixColliderMayow = glm::rotate(modelmatrixColliderMayow,
		//		glm::radians(-90.0f), glm::vec3(1, 0, 0));
		//// Set the orientation of collider before doing the scale
		//mayowCollider.u = glm::quat_cast(modelmatrixColliderMayow);
		//modelmatrixColliderMayow = glm::scale(modelmatrixColliderMayow,
		//		glm::vec3(0.021, 0.021, 0.021));
		//modelmatrixColliderMayow = glm::translate(modelmatrixColliderMayow,
		//		glm::vec3(mayowModelAnimate.getObb().c.x,
		//				mayowModelAnimate.getObb().c.y,
		//				mayowModelAnimate.getObb().c.z));
		//mayowCollider.e = mayowModelAnimate.getObb().e
		//		* glm::vec3(0.021, 0.021, 0.021)
		//		* glm::vec3(0.787401574, 0.787401574, 0.787401574);
		//mayowCollider.c = glm::vec3(modelmatrixColliderMayow[3]);
		//addOrUpdateColliders(collidersOBB, "mayow", mayowCollider,
		//		modelMatrixMayow);

		// Lamps1 colliders
		//for (int i = 0; i < lamp1Position.size(); i++) {
		//	AbstractModel::OBB lampCollider;
		//	glm::mat4 modelMatrixColliderLamp = glm::mat4(1.0);
		//	modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp,
		//		lamp1Position[i]);
		//	modelMatrixColliderLamp = glm::rotate(modelMatrixColliderLamp,
		//		glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
		//	addOrUpdateColliders(collidersOBB, "lamp1-" + std::to_string(i),
		//		lampCollider, modelMatrixColliderLamp);
		//	// Set the orientation of collider before doing the scale
		//	lampCollider.u = glm::quat_cast(modelMatrixColliderLamp);
		//	modelMatrixColliderLamp = glm::scale(modelMatrixColliderLamp,
		//		glm::vec3(0.5, 0.5, 0.5));
		//	modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp,
		//		modelLamp1.getObb().c);
		//	lampCollider.c = glm::vec3(modelMatrixColliderLamp[3]);
		//	lampCollider.e = modelLamp1.getObb().e * glm::vec3(0.5, 0.5, 0.5);
		//	std::get<0>(collidersOBB.find("lamp1-" + std::to_string(i))->second) =
		//		lampCollider;
		//}

		//// Lamps2 colliders
		//for (int i = 0; i < lamp2Position.size(); i++) {
		//	AbstractModel::OBB lampCollider;
		//	glm::mat4 modelMatrixColliderLamp = glm::mat4(1.0);
		//	modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp,
		//		lamp2Position[i]);
		//	modelMatrixColliderLamp = glm::rotate(modelMatrixColliderLamp,
		//		glm::radians(lamp2Orientation[i]), glm::vec3(0, 1, 0));
		//	addOrUpdateColliders(collidersOBB, "lamp2-" + std::to_string(i),
		//		lampCollider, modelMatrixColliderLamp);
		//	// Set the orientation of collider before doing the scale
		//	lampCollider.u = glm::quat_cast(modelMatrixColliderLamp);
		//	modelMatrixColliderLamp = glm::scale(modelMatrixColliderLamp,
		//		glm::vec3(1.0, 1.0, 1.0));
		//	modelMatrixColliderLamp = glm::translate(modelMatrixColliderLamp,
		//		modelLampPost2.getObb().c);
		//	lampCollider.c = glm::vec3(modelMatrixColliderLamp[3]);
		//	lampCollider.e = modelLampPost2.getObb().e
		//		* glm::vec3(1.0, 1.0, 1.0);
		//	std::get<0>(collidersOBB.find("lamp2-" + std::to_string(i))->second) =
		//		lampCollider;
		//}

		// Collider de mayow
		//AbstractModel::OBB mayowCollider;
		//glm::mat4 modelmatrixColliderMayow = glm::mat4(modelMatrixMayow);
		//modelmatrixColliderMayow = glm::rotate(modelmatrixColliderMayow,
		//		glm::radians(-90.0f), glm::vec3(1, 0, 0));
		//// Set the orientation of collider before doing the scale
		//mayowCollider.u = glm::quat_cast(modelmatrixColliderMayow);
		//modelmatrixColliderMayow = glm::scale(modelmatrixColliderMayow,
		//		glm::vec3(0.021, 0.021, 0.021));
		//modelmatrixColliderMayow = glm::translate(modelmatrixColliderMayow,
		//		glm::vec3(mayowModelAnimate.getObb().c.x,
		//				mayowModelAnimate.getObb().c.y,
		//				mayowModelAnimate.getObb().c.z));
		//mayowCollider.e = mayowModelAnimate.getObb().e
		//		* glm::vec3(0.021, 0.021, 0.021)
		//		* glm::vec3(0.787401574, 0.787401574, 0.787401574);
		//mayowCollider.c = glm::vec3(modelmatrixColliderMayow[3]);
		//addOrUpdateColliders(collidersOBB, "mayow", mayowCollider,
		//		modelMatrixMayow);

		/*******************************************
		 * Render de colliders
		 *******************************************/
		for (std::map<std::string,
			std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			collidersOBB.begin(); it != collidersOBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider,
				std::get<0>(it->second).c);
			matrixCollider = matrixCollider
				* glm::mat4(std::get<0>(it->second).u);
			matrixCollider = glm::scale(matrixCollider,
				std::get<0>(it->second).e * 2.0f);
			boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			boxCollider.enableWireMode();
			boxCollider.render(matrixCollider);
		}

		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider,
				std::get<0>(it->second).c);
			matrixCollider = glm::scale(matrixCollider,
				glm::vec3(std::get<0>(it->second).ratio * 2.0f));
			sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			sphereCollider.enableWireMode();
			sphereCollider.render(matrixCollider);
		}

		// Esto es para ilustrar la transformacion inversa de los coliders
		/*glm::vec3 cinv = glm::inverse(mayowCollider.u) * glm::vec4(rockCollider.c, 1.0);
		 glm::mat4 invColliderS = glm::mat4(1.0);
		 invColliderS = glm::translate(invColliderS, cinv);
		 invColliderS =  invColliderS * glm::mat4(mayowCollider.u);
		 invColliderS = glm::scale(invColliderS, glm::vec3(rockCollider.ratio * 2.0, rockCollider.ratio * 2.0, rockCollider.ratio * 2.0));
		 sphereCollider.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
		 sphereCollider.enableWireMode();
		 sphereCollider.render(invColliderS);
		 glm::vec3 cinv2 = glm::inverse(mayowCollider.u) * glm::vec4(mayowCollider.c, 1.0);
		 glm::mat4 invColliderB = glm::mat4(1.0);
		 invColliderB = glm::translate(invColliderB, cinv2);
		 invColliderB = glm::scale(invColliderB, mayowCollider.e * 2.0f);
		 boxCollider.setColor(glm::vec4(1.0, 1.0, 0.0, 1.0));
		 boxCollider.enableWireMode();
		 boxCollider.render(invColliderB);
		 // Se regresa el color blanco
		 sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
		 boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));*/

		 /*******************************************
		  * Test Colisions
		  *******************************************/

		// Box  vs Box
		for (std::map<std::string,
			std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			collidersOBB.begin(); it != collidersOBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				if (it != jt
					&& testOBBOBB(std::get<0>(it->second),
						std::get<0>(jt->second))
					&& !(it->first.substr(0, 3) == "map"
					&& jt->first.substr(0, 3) == "map")) {
					isCollision = true;
					if (it->first.substr(0, 4) == "mask" && jt->first == "Osmosis") {
						bullets += 1;
						int noMask = it->first.substr(5, 1)[0] - '0';
						renderMask[noMask] = !renderMask[noMask];
						std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
						std::cout << "EXTRACTED " << noMask << " TYPE " << typeid(noMask).name() << std::endl;
					}
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first,
				isCollision);
		}

		// Sphere vs Sphere
		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersSBB.begin(); jt != collidersSBB.end(); jt++) {
				if (it != jt
					&& testSphereSphereIntersection(std::get<0>(it->second),
						std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
						<< jt->first << std::endl;
					isCollision = true;
					if (it->first.substr(0, 5) == "covid" && jt->first == "bullet") {
						int noCovid = it->first.substr(6, 1)[0] - '0';
						bulletIsActive = false;
						bulletMovement = 0.0;
						renderCovid[noCovid] = !renderCovid[noCovid];
						sourcesPlay[noCovid] = false;
						std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
						std::cout << "EXTRACTED " << noCovid << " TYPE " << typeid(noCovid).name() << std::endl;
						std::cout << "SOUND deactivated " << noCovid << " is " << sourcesPlay[noCovid] << std::endl;
					}
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first,
				isCollision);
		}
		// Sphere vs Box
		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.begin();
			for (; jt != collidersOBB.end(); jt++) {
				if (testSphereOBox(std::get<0>(it->second),
					std::get<0>(jt->second))) {
					if (it->first.substr(0, 5) == "covid" && jt->first == "Osmosis") {
						isCollision = true;
						addOrUpdateCollisionDetection(collisionDetection, jt->first,
							isCollision);
						int noCovid = it->first.substr(6, 1)[0] - '0';
						std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
						std::cout << "EXTRACTED " << noCovid << " TYPE " << typeid(noCovid).name() << std::endl;
						std::cout << renderCovid[noCovid] << std::endl;
						if (renderCovid[noCovid])
						{
							hp -= 1;
							renderCovid[noCovid] = !renderCovid[noCovid];
							sourcesPlay[noCovid] = false;
							renderCovid[freeCovid] = !renderCovid[freeCovid];
							sourcesPlay[freeCovid] = true;
							freeCovid = noCovid;
							std::cout << "Colision " << it->first << " with "
								<< jt->first << std::endl;
							std::cout << "EXTRACTED " << noCovid << " TYPE " << typeid(noCovid).name() << std::endl;
							std::cout << "SOUND deactivated " << noCovid << " is " << sourcesPlay[noCovid] << std::endl;
							std::cout << "SOUND activated " << freeCovid << " is " << sourcesPlay[freeCovid] << std::endl;
						}
					}
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first,
				isCollision);

		}

		// Impedir que avance el modelo
		std::map<std::string, bool>::iterator colIt;
		for (colIt = collisionDetection.begin();
			colIt != collisionDetection.end(); colIt++) {
			std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.find(colIt->first);
			std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.find(colIt->first);
			if (it != collidersSBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersSBB, it->first);
			}
			if (jt != collidersOBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersOBB, jt->first);

				else {
					/*if (jt->first.compare("mayow") == 0)
						modelMatrixMayow = std::get<1>(jt->second);*/
					if (jt->first.compare("Osmosis") == 0) {
						modelMatrixOsmosis = std::get<1>(jt->second);
					}
				}
			}
		}

		// Constantes de animaciones

		/*******************************************
		 * State machines
		 *******************************************/
		cadena = "Vidas: " + std::to_string(hp);
		cadena1 = " Disparos: " + std::to_string(bullets);
		if (hp > 0) {
			modelText->render(cadena, -.95, 0.9, 50, 0.0, 0.63, 0.16);
			modelText2->render(cadena1, -.15, 0.9, 50, 0.9, 0.0, 0.0);
			//modelText->render(cadena, -.95, 0.9, 50, 1.0, 1.0, 1.0);
			//modelText2->render(cadena1, -.20, 0.9, 50, 1.0, 1.0, 1.0);
			//if (recarga)
			//	modelText3->render(cadena2, .65, 0.9, 50, 1.0, 1.0, 1.0);
			glfwSwapBuffers(window);
		}
		else {
			//modelText->render("GAME OVER!", -0.55, 0.0, 160, 1.0, 1.0, 1.0);
			modelText->render("GAME OVER!", -0.55, 0.0, 160, 1.0, 0.0, 0.0);
			glfwSwapBuffers(window);
		}

		/****************************+
		 * Open AL sound data
		 */
		/*source0Pos[0] = modelMatrixFountain[3].x;
		source0Pos[1] = modelMatrixFountain[3].y;
		source0Pos[2] = modelMatrixFountain[3].z;
		alSourcefv(source[0], AL_POSITION, source0Pos);*/

		/*source2Pos[0] = modelMatrixDart[3].x;
		source2Pos[1] = modelMatrixDart[3].y;
		source2Pos[2] = modelMatrixDart[3].z;
		alSourcefv(source[2], AL_POSITION, source2Pos);*/

		// Listener for the Thris person camera
		/*listenerPos[0] = modelMatrixMayow[3].x;
		listenerPos[1] = modelMatrixMayow[3].y;
		listenerPos[2] = modelMatrixMayow[3].z;
		alListenerfv(AL_POSITION, listenerPos);*/

		listenerPos[0] = modelMatrixOsmosis[3].x;
		listenerPos[1] = modelMatrixOsmosis[3].y;
		listenerPos[2] = modelMatrixOsmosis[3].z;
		alListenerfv(AL_POSITION, listenerPos);

		/*glm::vec3 upModel = glm::normalize(modelMatrixMayow[1]);
		glm::vec3 frontModel = glm::normalize(modelMatrixMayow[2]);*/

		glm::vec3 upModel = glm::normalize(modelMatrixOsmosis[1]);
		glm::vec3 frontModel = glm::normalize(modelMatrixOsmosis[2]);

		listenerOri[0] = frontModel.x;
		listenerOri[1] = frontModel.y;
		listenerOri[2] = frontModel.z;
		listenerOri[3] = upModel.x;
		listenerOri[4] = upModel.y;
		listenerOri[5] = upModel.z;

		// Listener for the First person camera
		/*listenerPos[0] = camera->getPosition().x;
		 listenerPos[1] = camera->getPosition().y;
		 listenerPos[2] = camera->getPosition().z;
		 alListenerfv(AL_POSITION, listenerPos);
		 listenerOri[0] = camera->getFront().x;
		 listenerOri[1] = camera->getFront().y;
		 listenerOri[2] = camera->getFront().z;
		 listenerOri[3] = camera->getUp().x;
		 listenerOri[4] = camera->getUp().y;
		 listenerOri[5] = camera->getUp().z;*/
		alListenerfv(AL_ORIENTATION, listenerOri);

		for (unsigned int i = 0; i < sourcesPlay.size(); i++) {
			if (sourcesPlay[i] && renderCovid[i]) {
				sourcesPlay[i] = false;
				alSourcePlay(source[i]);
			}
			else if (!sourcesPlay[i] && !renderCovid[i]) {
				alSourceStop(source[i]);
			}
		}
	}
}

void prepareScene() {

	skyboxSphere.setShader(&shaderSkybox);

	//modelRock.setShader(&shaderMulLighting);

	terrain.setShader(&shaderTerrain);

	//Lamp models
	modelLamp1.setShader(&shaderMulLighting);
	modelLamp2.setShader(&shaderMulLighting);
	//modelLampPost2.setShader(&shaderMulLighting);

	//Grass
	modelGrass.setShader(&shaderMulLighting);

	//Mayow
	//mayowModelAnimate.setShader(&shaderMulLighting);

	//Osmosis
	modelOsmosisAnimate.setShader(&shaderMulLighting);

	// Covid
	for (unsigned int i = 0; i < NUM_COVID; i++)
	{
		covidArray[i].setShader(&shaderMulLighting);
	}

	// Masks
	for (unsigned int i = 0; i < NUM_MASKS; i++)
	{
		maskArray[i].setShader(&shaderMulLighting);
	}

	// Bullet
	modelBulletAnimate.setShader(&shaderMulLighting);

	//Map
	modelMapTest.setShader(&shaderMulLighting);
	//modelMapRef.setShader(&shaderMulLighting);
	//modelMapSquareFL.setShader(&shaderMulLighting);
	for (unsigned int i = 0; i < 27; i++)
	{
		mapArray[i].setShader(&shaderMulLighting);
	}
}

void prepareDepthScene() {

	skyboxSphere.setShader(&shaderDepth);

	//modelRock.setShader(&shaderDepth);

	terrain.setShader(&shaderDepth);

	//Lamp models
	modelLamp1.setShader(&shaderDepth);
	modelLamp2.setShader(&shaderDepth);
	//modelLampPost2.setShader(&shaderDepth);

	//Grass
	modelGrass.setShader(&shaderDepth);

	//Mayow
	//mayowModelAnimate.setShader(&shaderDepth);

	// Osmosis
	modelOsmosisAnimate.setShader(&shaderDepth);

	// Covid
	for (unsigned int i = 0; i < NUM_COVID; i++)
	{
		covidArray[i].setShader(&shaderDepth);
	}

	// Masks
	for (unsigned int i = 0; i < NUM_MASKS; i++)
	{
		maskArray[i].setShader(&shaderDepth);
	}

	// Bullet
	modelBulletAnimate.setShader(&shaderDepth);

	// Map
	modelMapTest.setShader(&shaderDepth);
	//modelMapRef.setShader(&shaderDepth);
	//modelMapSquareFL.setShader(&shaderDepth);
	for (unsigned int i = 0; i < 27; i++)
	{
		mapArray[i].setShader(&shaderDepth);
	}
}

void renderScene(bool renderParticles) {
	/*******************************************
	 * Terrain Cesped
	 *******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);

	/*******************************************
	 * Custom objects obj
	 *******************************************/
	 //Rock render
	/*matrixModelRock[3][1] = terrain.getHeightTerrain(matrixModelRock[3][0],
		matrixModelRock[3][2]);
	modelRock.render(matrixModelRock);*/
	// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
	glActiveTexture(GL_TEXTURE0);

	// Render the lamps
	/*for (int i = 0; i < lamp1Position.size(); i++) {
		lamp1Position[i].y = terrain.getHeightTerrain(lamp1Position[i].x,
			lamp1Position[i].z);
		modelLamp1.setPosition(lamp1Position[i]);
		modelLamp1.setScale(glm::vec3(0.5, 0.5, 0.5));
		modelLamp1.setOrientation(glm::vec3(0, lamp1Orientation[i], 0));
		modelLamp1.render();
	}

	for (int i = 0; i < lamp2Position.size(); i++) {
		lamp2Position[i].y = terrain.getHeightTerrain(lamp2Position[i].x,
			lamp2Position[i].z);
		modelLamp2.setPosition(lamp2Position[i]);
		modelLamp2.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelLamp2.setOrientation(glm::vec3(0, lamp2Orientation[i], 0));
		modelLamp2.render();
		modelLampPost2.setPosition(lamp2Position[i]);
		modelLampPost2.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelLampPost2.setOrientation(glm::vec3(0, lamp2Orientation[i], 0));
		modelLampPost2.render();
	}*/
	glm::mat4 modelMatrixBodyLamp1 = glm::mat4(1.0);
	modelMatrixBodyLamp1 = glm::translate(modelMatrixBodyLamp1, glm::vec3(69.0f, 10.0f, -13.0f));
	modelMatrixBodyLamp1 = glm::rotate(modelMatrixBodyLamp1, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	modelLamp1.render(modelMatrixBodyLamp1);

	glm::mat4 modelMatrixBodyLamp2 = glm::mat4(1.0);
	modelMatrixBodyLamp2 = glm::translate(modelMatrixBodyLamp2, glm::vec3(69.0f, 10.0f, -19.0f));
	modelMatrixBodyLamp2 = glm::rotate(modelMatrixBodyLamp2, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	modelLamp2.render(modelMatrixBodyLamp2);

	// Grass
	glDisable(GL_CULL_FACE);
	glm::vec3 grassPosition = glm::vec3(0.0, 0.0, 0.0);
	grassPosition.y = terrain.getHeightTerrain(grassPosition.x,
		grassPosition.z);
	modelGrass.setPosition(grassPosition);
	modelGrass.render();
	glEnable(GL_CULL_FACE);

	// Fountain
	glDisable(GL_CULL_FACE);
	modelFountain.render(modelMatrixFountain);
	glEnable(GL_CULL_FACE);

	/*******************************************
	 * Custom Anim objects obj
	 *******************************************/
	 //modelMatrixMayow[3][1] = -GRAVITY * tmv * tmv + 3.5 * tmv
	 //		+ terrain.getHeightTerrain(modelMatrixMayow[3][0],
	 //				modelMatrixMayow[3][2]);
	 //tmv = currTime - startTimeJump;
	 //if (modelMatrixMayow[3][1]
	 //		< terrain.getHeightTerrain(modelMatrixMayow[3][0],
	 //				modelMatrixMayow[3][2])) {
	 //	isJump = false;
	 //	modelMatrixMayow[3][1] = terrain.getHeightTerrain(
	 //			modelMatrixMayow[3][0], modelMatrixMayow[3][2]);
	 //}
	 ////modelMatrixMayow[3][1] = terrain.getHeightTerrain(modelMatrixMayow[3][0], modelMatrixMayow[3][2]);
	 //glm::mat4 modelMatrixMayowBody = glm::mat4(modelMatrixMayow);
	 //modelMatrixMayowBody = glm::scale(modelMatrixMayowBody,
	 //		glm::vec3(0.021, 0.021, 0.021));
	 //mayowModelAnimate.setAnimationIndex(animationIndex);
	 //mayowModelAnimate.render(modelMatrixMayowBody);



	 /*glm::vec3 ejeyS = glm::normalize(terrain.getNormalTerrain(modelMatrixOsmosis[3][0], modelMatrixOsmosis[3][2]));
	 glm::vec3 ejexS = glm::normalize(modelMatrixOsmosis[0]);
	 glm::vec3 ejezS = glm::normalize(glm::cross(ejexS, ejeyS));
	 ejexS = glm::normalize(glm::cross(ejeyS, ejezS));
	 modelMatrixOsmosis[0] = glm::vec4(ejexS, 0.0);
	 modelMatrixOsmosis[1] = glm::vec4(ejeyS, 0.0);
	 modelMatrixOsmosis[2] = glm::vec4(ejezS, 0.0);*/
	modelMatrixOsmosis[3][1] = terrain.getHeightTerrain(modelMatrixOsmosis[3][0], modelMatrixOsmosis[3][2]);
	glm::mat4 modelMatrixOsmosisBody = glm::mat4(modelMatrixOsmosis);
	modelMatrixOsmosisBody = glm::scale(modelMatrixOsmosisBody, glm::vec3(0.004, 0.004, 0.004));
	modelOsmosisAnimate.setAnimationIndex(animationIndex);
	modelOsmosisAnimate.render(modelMatrixOsmosisBody);


	// Map
	modelMatrixMapTest[3][1] = terrain.getHeightTerrain(modelMatrixMapTest[3][0], modelMatrixMapTest[3][2]) + 0.2f;
	//glm::mat4 modelMatrixMapTestMain = glm::mat4(modelMatrixMapTest);
	//modelMapTest.render(modelMatrixMapTest);

	//modelMatrixMapRef[3][1] = terrain.getHeightTerrain(modelMatrixMapRef[3][0], modelMatrixMapRef[3][2]) + 1.0f;
	//modelMapRef.render(modelMatrixMapRef);

	//modelMatrixMapSquareFL[3][1] = terrain.getHeightTerrain(modelMatrixMapSquareFL[3][0], modelMatrixMapSquareFL[3][2]);
	//glm::mat4 modelMatrixMapSquareFLMain = glm::mat4(modelMatrixMapSquareFL);
	//modelMapSquareFL.render(modelMatrixMapSquareFL);
	for (unsigned int i = 0; i < 27; i++)
	{
		modelMatrixMap[3][1] = terrain.getHeightTerrain(modelMatrixMap[3][0], modelMatrixMap[3][2]);
		mapArray[i].render(modelMatrixMap);
	}

	// Covid
	for (unsigned int i = 0; i < NUM_COVID; i++) {
		modelMatrixCovid[i][3][1] = terrain.getHeightTerrain(modelMatrixCovid[i][3][0], modelMatrixCovid[i][3][2]) + 1.0f;
		glm::mat4 modelMatrixCovidBody = glm::mat4(modelMatrixCovid[i]);
		modelMatrixCovidBody = glm::scale(modelMatrixCovidBody, glm::vec3(0.015, 0.015, 0.015));
		covidArray[i].setAnimationIndex(0);
		if (renderCovid[i])
		{
			covidArray[i].render(modelMatrixCovidBody);
		}
	}

	// Masks
	for (unsigned int i = 0; i < NUM_MASKS; i++) {
		maskPositions[i].y = terrain.getHeightTerrain(maskPositions[i].x,
			maskPositions[i].z);
		maskArray[i].setPosition(maskPositions[i]);
		maskArray[i].setScale(glm::vec3(0.015, 0.015, 0.015));
		maskArray[i].setAnimationIndex(0);
		if (renderMask[i])
		{
			maskArray[i].render();
		}
	}
	// Bullet
	//modelMatrixBala[3][1] = terrain.getHeightTerrain(modelMatrixBala[3][0], modelMatrixBala[3][2]);
	if (!bulletIsActive) {
		modelMatrixBulletBody = glm::mat4(modelMatrixOsmosis);
		modelMatrixBulletBody = glm::translate(modelMatrixBulletBody, glm::vec3(-0.5, 3.25, 1));
		modelMatrixBulletRef = glm::mat4(modelMatrixBulletBody);
	}
	else {
		modelMatrixBulletBody = glm::mat4(modelMatrixBulletRef);
		modelMatrixBulletBody = glm::translate(modelMatrixBulletBody, glm::vec3(0, 0, bulletMovement));
	}
	modelMatrixBulletBody = glm::scale(modelMatrixBulletBody, glm::vec3(0.25, 0.25, 0.25));
	if (bulletIsActive) {
		modelBulletAnimate.render(modelMatrixBulletBody);
	}

	// Left Down, Left Up, Right Down, Right Up, Middle
	float maxSteps[NUM_COVID][6] = {
		{ 30.0f, 17.0f, 13.0f, 7.0f, 17.0f, 24.0f },
		{ 24.0f, 17.0f, 7.0f, 13.0f, 17.0f, 30.0f },
		{ 30.0f, 17.0f, 13.0f, 7.0f, 17.0f, 24.0f },
		{ 24.0f, 17.0f, 7.0f, 13.0f, 17.0f, 30.0f },
		{ 60.0f, 34.0f, 60.0f, 34.0f, 0.0f, 0.0f }
	};
	// 0 Up, 1 Down, 2 Left, 3 Right
	int individualDirections[NUM_COVID][6] = {
		{ 0, 3, 1, 3, 1, 2 },
		{ 3, 1, 2, 1, 2, 0 },
		{ 0, 2, 1, 2, 1, 3 },
		{ 2, 1, 3, 1, 3, 0 },
		{ 3, 1, 2, 0, 0, 0 }
	};
	std::vector<glm::vec3> direction = {
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	};
	float individualStepSize[NUM_COVID] = { 0.07, 0.07, 0.08, 0.09, 0.09 };

	// State machine for Covid
	for (unsigned int i = 0; i < NUM_COVID; i++) {
		switch (stateCovid[i])
		{
		case 0:
			modelMatrixCovid[i] = glm::translate(modelMatrixCovid[i], direction[individualDirections[i][0]] * individualStepSize[i]);
			stepCountCovid[i] += individualStepSize[i];
			break;
		case 1:
			modelMatrixCovid[i] = glm::translate(modelMatrixCovid[i], direction[individualDirections[i][1]] * individualStepSize[i]);
			stepCountCovid[i] += individualStepSize[i];
			break;
		case 2:
			modelMatrixCovid[i] = glm::translate(modelMatrixCovid[i], direction[individualDirections[i][2]] * individualStepSize[i]);
			stepCountCovid[i] += individualStepSize[i];
			break;
		case 3:
			modelMatrixCovid[i] = glm::translate(modelMatrixCovid[i], direction[individualDirections[i][3]] * individualStepSize[i]);
			stepCountCovid[i] += individualStepSize[i];
			break;
		case 4:
			modelMatrixCovid[i] = glm::translate(modelMatrixCovid[i], direction[individualDirections[i][4]] * individualStepSize[i]);
			stepCountCovid[i] += individualStepSize[i];
			break;
		case 5:
			modelMatrixCovid[i] = glm::translate(modelMatrixCovid[i], direction[individualDirections[i][5]] * individualStepSize[i]);
			stepCountCovid[i] += individualStepSize[i];
			break;
		default:
			break;
		}
		if (stepCountCovid[i] > maxSteps[i][stateCovid[i]])
		{
			stepCountCovid[i] = 0.0f;
			stateCovid[i]++;
			if (stateCovid[i] == 6) {
				stateCovid[i] = 0;
			}
			if (i == 5 && stateCovid[i] == 4) {
				stateCovid[i] = 0;
			}
		}
	}

	if (bulletIsActive)
	{
		if (bulletMovement < bulletMaxMovement)
		{
			bulletMovement += 0.15;
		}
		else
		{
			bulletMovement = 0;
			bulletIsActive = false;
		}
	}

	/**********
	 * Update the position with alpha objects
	 */

	 /**********
	  * Sorter with alpha objects
	  */
	std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
	std::map<std::string, glm::vec3>::iterator itblend;
	for (itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end();
		itblend++) {
		float distanceFromView = glm::length(
			camera->getPosition() - itblend->second);
		blendingSorted[distanceFromView] = std::make_pair(itblend->first,
			itblend->second);
	}

	/**********
	 * Render de las transparencias
	 */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	for (std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it =
		blendingSorted.rbegin(); it != blendingSorted.rend(); it++) {
		//if (renderParticles	&& it->second.first.compare("fountain") == 0) {
		//	/**********
		//	 * Init Render particles systems
		//	 */
		//	glm::mat4 modelMatrixParticlesFountain = glm::mat4(1.0);
		//	modelMatrixParticlesFountain = glm::translate(modelMatrixParticlesFountain, it->second.second);
		//	modelMatrixParticlesFountain[3][1] = terrain.getHeightTerrain(modelMatrixParticlesFountain[3][0],
		//		modelMatrixParticlesFountain[3][2]) + 0.36 * 10.0;
		//	modelMatrixParticlesFountain = glm::scale(modelMatrixParticlesFountain, glm::vec3(3.0, 3.0, 3.0));
		//	currTimeParticlesAnimation = TimeManager::Instance().GetTime();
		//	if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 11.0)
		//		lastTimeParticlesAnimation = currTimeParticlesAnimation;
		//	//glDisable(GL_DEPTH_TEST);
		//	glDepthMask(GL_FALSE);
		//	// Set the point size
		//	glPointSize(10.0f);
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
		//	shaderParticlesFountain.turnOn();
		//	shaderParticlesFountain.setFloat("Time",float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
		//	shaderParticlesFountain.setFloat("ParticleLifetime", 1.0f);
		//	shaderParticlesFountain.setInt("ParticleTex", 0);
		//	shaderParticlesFountain.setVectorFloat3("Gravity",glm::value_ptr(glm::vec3(0.0f, -0.9f, 0.0f)));
		//	shaderParticlesFountain.setMatrix4("model", 1, false,glm::value_ptr(modelMatrixParticlesFountain));
		//	glBindVertexArray(VAOParticles);
		//	glDrawArrays(GL_POINTS, 0, nParticles);
		//	glDepthMask(GL_TRUE);
		//	//glEnable(GL_DEPTH_TEST);
		//	shaderParticlesFountain.turnOff();
		//	/**********
		//	 * End Render particles systems
		//	 */
		//}

		for (unsigned int i = 0; i < NUM_COVID; i++)
		{
			if (renderParticles && it->second.first.substr(0, 4) == "Smok" == 0 && renderCovid[i]) {
				//int noSmoke = it->second.first.substr(5, 1)[0] - '0';
				lastTimeParticlesAnimationSmoke = currTimeParticlesAnimationSmoke;
				currTimeParticlesAnimationSmoke = TimeManager::Instance().GetTime();

				shaderParticlesSmoke.setInt("Pass", 1);
				shaderParticlesSmoke.setFloat("Time", currTimeParticlesAnimationSmoke);
				shaderParticlesSmoke.setFloat("DeltaT", currTimeParticlesAnimationSmoke - lastTimeParticlesAnimationSmoke);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_1D, texIdSmoke);
				glEnable(GL_RASTERIZER_DISCARD);
				glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackSmoke[drawBufSmoke]);
				glBeginTransformFeedback(GL_POINTS);
				glBindVertexArray(particleArraySmoke[1 - drawBufSmoke]);
				glVertexAttribDivisor(0, 0);
				glVertexAttribDivisor(1, 0);
				glVertexAttribDivisor(2, 0);
				glDrawArrays(GL_POINTS, 0, nParticlesSmoke);
				glEndTransformFeedback();
				glDisable(GL_RASTERIZER_DISCARD);

				shaderParticlesSmoke.setInt("Pass", 2);
				glm::mat4 modelSmokeParticles = glm::mat4(1.0);
				modelSmokeParticles = glm::translate(modelSmokeParticles, glm::vec3(modelMatrixCovid[i][3].x, modelMatrixCovid[i][3].y, modelMatrixCovid[i][3].z));
				modelSmokeParticles[3][1] = terrain.getHeightTerrain(modelSmokeParticles[3][0], modelSmokeParticles[3][2]) + 2;
				shaderParticlesSmoke.setMatrix4("model", 1, false, glm::value_ptr(modelSmokeParticles));

				shaderParticlesSmoke.turnOn();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureParticleSmokeID);
				glDepthMask(GL_FALSE);
				glBindVertexArray(particleArraySmoke[drawBufSmoke]);
				glVertexAttribDivisor(0, 1);
				glVertexAttribDivisor(1, 1);
				glVertexAttribDivisor(2, 1);
				glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesSmoke);
				glBindVertexArray(0);
				glDepthMask(GL_TRUE);
				drawBufSmoke = 1 - drawBufSmoke;
				shaderParticlesSmoke.turnOff();

				/****************************+
				 * Open AL sound data
				 */
				sourceCovidPos[i][0] = modelSmokeParticles[3].x;
				sourceCovidPos[i][1] = modelSmokeParticles[3].y;
				sourceCovidPos[i][2] = modelSmokeParticles[3].z;
				alSourcefv(source[i], AL_POSITION, sourceCovidPos[i]);
			}
		}

		//if (renderParticles && it->second.first.compare("Smoke1") == 0) {
		//	/**********
		//	 * Init Render particles systems
		//	 */
		//	lastTimeParticlesAnimationSmoke = currTimeParticlesAnimationSmoke;
		//	currTimeParticlesAnimationSmoke = TimeManager::Instance().GetTime();

		//	shaderParticlesSmoke.setInt("Pass", 1);
		//	shaderParticlesSmoke.setFloat("Time",currTimeParticlesAnimationSmoke);
		//	shaderParticlesSmoke.setFloat("DeltaT",currTimeParticlesAnimationSmoke - lastTimeParticlesAnimationSmoke);

		//	glActiveTexture(GL_TEXTURE1);
		//	glBindTexture(GL_TEXTURE_1D, texIdSmoke);
		//	glEnable(GL_RASTERIZER_DISCARD);
		//	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackSmoke[drawBufSmoke]);
		//	glBeginTransformFeedback(GL_POINTS);
		//	glBindVertexArray(particleArraySmoke[1 - drawBufSmoke]);
		//	glVertexAttribDivisor(0, 0);
		//	glVertexAttribDivisor(1, 0);
		//	glVertexAttribDivisor(2, 0);
		//	glDrawArrays(GL_POINTS, 0, nParticlesSmoke);
		//	glEndTransformFeedback();
		//	glDisable(GL_RASTERIZER_DISCARD);

		//	shaderParticlesSmoke.setInt("Pass", 2);
		//	glm::mat4 modelSmokeParticles = glm::mat4(1.0);
		//	modelSmokeParticles = glm::translate(modelSmokeParticles, glm::vec3(modelMatrixCovid[4][3].x, modelMatrixCovid[4][3].y, modelMatrixCovid[4][3].z));
		//	modelSmokeParticles[3][1] = terrain.getHeightTerrain(modelSmokeParticles[3][0], modelSmokeParticles[3][2]) + 2;
		//	shaderParticlesSmoke.setMatrix4("model", 1, false,glm::value_ptr(modelSmokeParticles));

		//	shaderParticlesSmoke.turnOn();
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, textureParticleSmokeID);
		//	glDepthMask(GL_FALSE);
		//	glBindVertexArray(particleArraySmoke[drawBufSmoke]);
		//	glVertexAttribDivisor(0, 1);
		//	glVertexAttribDivisor(1, 1);
		//	glVertexAttribDivisor(2, 1);
		//	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesSmoke);
		//	glBindVertexArray(0);
		//	glDepthMask(GL_TRUE);
		//	drawBufSmoke = 1 - drawBufSmoke;
		//	shaderParticlesSmoke.turnOff();

		//	/****************************+
		//	 * Open AL sound data
		//	 */
		//	source1Pos[0] = modelSmokeParticles[3].x;
		//	source1Pos[1] = modelSmokeParticles[3].y;
		//	source1Pos[2] = modelSmokeParticles[3].z;
		//	alSourcefv(source[1], AL_POSITION, source1Pos);

		//	/**********
		//	 * End Render particles systems
		//	 */
		//}

		//else if (renderParticles && it->second.first.compare("Smoke2") == 0) {
		//	/**********
		//	 * Init Render particles systems
		//	 */
		//	lastTimeParticlesAnimationSmoke = currTimeParticlesAnimationSmoke;
		//	currTimeParticlesAnimationSmoke = TimeManager::Instance().GetTime();

		//	shaderParticlesSmoke.setInt("Pass", 1);
		//	shaderParticlesSmoke.setFloat("Time", currTimeParticlesAnimationSmoke);
		//	shaderParticlesSmoke.setFloat("DeltaT", currTimeParticlesAnimationSmoke - lastTimeParticlesAnimationSmoke);

		//	glActiveTexture(GL_TEXTURE1);
		//	glBindTexture(GL_TEXTURE_1D, texIdSmoke);
		//	glEnable(GL_RASTERIZER_DISCARD);
		//	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackSmoke[drawBufSmoke]);
		//	glBeginTransformFeedback(GL_POINTS);
		//	glBindVertexArray(particleArraySmoke[1 - drawBufSmoke]);
		//	glVertexAttribDivisor(0, 0);
		//	glVertexAttribDivisor(1, 0);
		//	glVertexAttribDivisor(2, 0);
		//	glDrawArrays(GL_POINTS, 0, nParticlesSmoke);
		//	glEndTransformFeedback();
		//	glDisable(GL_RASTERIZER_DISCARD);

		//	shaderParticlesSmoke.setInt("Pass", 2);
		//	glm::mat4 modelSmokeParticles = glm::mat4(1.0);
		//	modelSmokeParticles = glm::translate(modelSmokeParticles, it->second.second);
		//	modelSmokeParticles[3][1] = terrain.getHeightTerrain(modelSmokeParticles[3][0], modelSmokeParticles[3][2]);
		//	shaderParticlesSmoke.setMatrix4("model", 1, false, glm::value_ptr(modelSmokeParticles));

		//	shaderParticlesSmoke.turnOn();
		//	glActiveTexture(GL_TEXTURE0);
		//	glBindTexture(GL_TEXTURE_2D, textureParticleSmokeID);
		//	glDepthMask(GL_FALSE);
		//	glBindVertexArray(particleArraySmoke[drawBufSmoke]);
		//	glVertexAttribDivisor(0, 1);
		//	glVertexAttribDivisor(1, 1);
		//	glVertexAttribDivisor(2, 1);
		//	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesSmoke);
		//	glBindVertexArray(0);
		//	glDepthMask(GL_TRUE);
		//	drawBufSmoke = 1 - drawBufSmoke;
		//	shaderParticlesSmoke.turnOff();

		//	/****************************+
		//	 * Open AL sound data
		//	 */
		//	source1Pos[0] = modelSmokeParticles[3].x;
		//	source1Pos[1] = modelSmokeParticles[3].y;
		//	source1Pos[2] = modelSmokeParticles[3].z;
		//	alSourcefv(source[1], AL_POSITION, source1Pos);

		//	/**********
		//	 * End Render particles systems
		//	 */
		//}

		//else if (renderParticles && it->second.first.compare("Smoke3") == 0) {
		///**********
		// * Init Render particles systems
		// */
		// lastTimeParticlesAnimationSmoke = currTimeParticlesAnimationSmoke;
		// currTimeParticlesAnimationSmoke = TimeManager::Instance().GetTime();

		// shaderParticlesSmoke.setInt("Pass", 1);
		// shaderParticlesSmoke.setFloat("Time", currTimeParticlesAnimationSmoke);
		// shaderParticlesSmoke.setFloat("DeltaT", currTimeParticlesAnimationSmoke - lastTimeParticlesAnimationSmoke);

		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_1D, texIdSmoke);
		// glEnable(GL_RASTERIZER_DISCARD);
		// glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackSmoke[drawBufSmoke]);
		// glBeginTransformFeedback(GL_POINTS);
		// glBindVertexArray(particleArraySmoke[1 - drawBufSmoke]);
		// glVertexAttribDivisor(0, 0);
		// glVertexAttribDivisor(1, 0);
		// glVertexAttribDivisor(2, 0);
		// glDrawArrays(GL_POINTS, 0, nParticlesSmoke);
		// glEndTransformFeedback();
		// glDisable(GL_RASTERIZER_DISCARD);

		// shaderParticlesSmoke.setInt("Pass", 2);
		// glm::mat4 modelSmokeParticles = glm::mat4(1.0);
		// modelSmokeParticles = glm::translate(modelSmokeParticles, it->second.second);
		// modelSmokeParticles[3][1] = terrain.getHeightTerrain(modelSmokeParticles[3][0], modelSmokeParticles[3][2]);
		// shaderParticlesSmoke.setMatrix4("model", 1, false, glm::value_ptr(modelSmokeParticles));

		// shaderParticlesSmoke.turnOn();
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, textureParticleSmokeID);
		// glDepthMask(GL_FALSE);
		// glBindVertexArray(particleArraySmoke[drawBufSmoke]);
		// glVertexAttribDivisor(0, 1);
		// glVertexAttribDivisor(1, 1);
		// glVertexAttribDivisor(2, 1);
		// glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesSmoke);
		// glBindVertexArray(0);
		// glDepthMask(GL_TRUE);
		// drawBufSmoke = 1 - drawBufSmoke;
		// shaderParticlesSmoke.turnOff();

		// /****************************+
		//  * Open AL sound data
		//  */
		// source1Pos[0] = modelSmokeParticles[3].x;
		// source1Pos[1] = modelSmokeParticles[3].y;
		// source1Pos[2] = modelSmokeParticles[3].z;
		// alSourcefv(source[1], AL_POSITION, source1Pos);

		// /**********
		//  * End Render particles systems
		//  */
		//}

		//else if (renderParticles && it->second.first.compare("Smoke4") == 0) {
		///**********
		// * Init Render particles systems
		// */
		// lastTimeParticlesAnimationSmoke = currTimeParticlesAnimationSmoke;
		// currTimeParticlesAnimationSmoke = TimeManager::Instance().GetTime();

		// shaderParticlesSmoke.setInt("Pass", 1);
		// shaderParticlesSmoke.setFloat("Time", currTimeParticlesAnimationSmoke);
		// shaderParticlesSmoke.setFloat("DeltaT", currTimeParticlesAnimationSmoke - lastTimeParticlesAnimationSmoke);

		// glActiveTexture(GL_TEXTURE1);
		// glBindTexture(GL_TEXTURE_1D, texIdSmoke);
		// glEnable(GL_RASTERIZER_DISCARD);
		// glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackSmoke[drawBufSmoke]);
		// glBeginTransformFeedback(GL_POINTS);
		// glBindVertexArray(particleArraySmoke[1 - drawBufSmoke]);
		// glVertexAttribDivisor(0, 0);
		// glVertexAttribDivisor(1, 0);
		// glVertexAttribDivisor(2, 0);
		// glDrawArrays(GL_POINTS, 0, nParticlesSmoke);
		// glEndTransformFeedback();
		// glDisable(GL_RASTERIZER_DISCARD);

		// shaderParticlesSmoke.setInt("Pass", 2);
		// glm::mat4 modelSmokeParticles = glm::mat4(1.0);
		// modelSmokeParticles = glm::translate(modelSmokeParticles, it->second.second);
		// modelSmokeParticles[3][1] = terrain.getHeightTerrain(modelSmokeParticles[3][0], modelSmokeParticles[3][2]);
		// shaderParticlesSmoke.setMatrix4("model", 1, false, glm::value_ptr(modelSmokeParticles));

		// shaderParticlesSmoke.turnOn();
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, textureParticleSmokeID);
		// glDepthMask(GL_FALSE);
		// glBindVertexArray(particleArraySmoke[drawBufSmoke]);
		// glVertexAttribDivisor(0, 1);
		// glVertexAttribDivisor(1, 1);
		// glVertexAttribDivisor(2, 1);
		// glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesSmoke);
		// glBindVertexArray(0);
		// glDepthMask(GL_TRUE);
		// drawBufSmoke = 1 - drawBufSmoke;
		// shaderParticlesSmoke.turnOff();

		// /****************************+
		//  * Open AL sound data
		//  */
		// source1Pos[0] = modelSmokeParticles[3].x;
		// source1Pos[1] = modelSmokeParticles[3].y;
		// source1Pos[2] = modelSmokeParticles[3].z;
		// alSourcefv(source[1], AL_POSITION, source1Pos);

		// /**********
		//  * End Render particles systems
		//  */
		//}

		if (renderParticles && it->second.first.compare("Rain") == 0) {
		/**********
		 * Init Render particles systems
		 */
		 lastTimeParticlesAnimationRain = currTimeParticlesAnimationRain;
		 currTimeParticlesAnimationRain = TimeManager::Instance().GetTime();

		 shaderParticlesRain.setInt("Pass", 1);
		 shaderParticlesRain.setFloat("Time", currTimeParticlesAnimationRain);
		 shaderParticlesRain.setFloat("DeltaT", currTimeParticlesAnimationRain - lastTimeParticlesAnimationRain);

		 glActiveTexture(GL_TEXTURE1);
		 glBindTexture(GL_TEXTURE_1D, texIdRain);
		 glEnable(GL_RASTERIZER_DISCARD);
		 glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedbackRain[drawBufRain]);
		 glBeginTransformFeedback(GL_POINTS);
		 glBindVertexArray(particleArrayRain[1 - drawBufRain]);
		 glVertexAttribDivisor(0, 0);
		 glVertexAttribDivisor(1, 0);
		 glVertexAttribDivisor(2, 0);
		 glDrawArrays(GL_POINTS, 0, nParticlesRain);
		 glEndTransformFeedback();
		 glDisable(GL_RASTERIZER_DISCARD);

		 shaderParticlesRain.setInt("Pass", 2);
		 glm::mat4 modelRainParticles = glm::mat4(1.0);
		 modelRainParticles = glm::translate(modelRainParticles, it->second.second);
		 modelRainParticles[3][1] = terrain.getHeightTerrain(modelRainParticles[3][0], modelRainParticles[3][2]);
		 shaderParticlesRain.setMatrix4("model", 1, false, glm::value_ptr(modelRainParticles));

		 shaderParticlesRain.turnOn();
		 glActiveTexture(GL_TEXTURE0);
		 if (hp <= 1)
			 glBindTexture(GL_TEXTURE_2D, textureParticleSmokeID);
		 else
			 glBindTexture(GL_TEXTURE_2D, textureParticleRainID);
		 glDepthMask(GL_FALSE);
		 glBindVertexArray(particleArrayRain[drawBufRain]);
		 glVertexAttribDivisor(0, 1);
		 glVertexAttribDivisor(1, 1);
		 glVertexAttribDivisor(2, 1);
		 glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesRain);
		 glBindVertexArray(0);
		 glDepthMask(GL_TRUE);
		 drawBufRain = 1 - drawBufRain;
		 shaderParticlesRain.turnOff();
		}

	}
	glEnable(GL_CULL_FACE);
}

int main(int argc, char** argv) {
	init(800, 700, "Osmosis vs Covid", false);
	applicationLoop();
	destroy();
	return 1;
}

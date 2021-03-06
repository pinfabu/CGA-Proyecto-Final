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
#define NUM_MAP_PIECES 35

// Game states
// 0 is main menu, 1 is running, 2 is won, 3 is game over - replaces mainMenu, gameStarted and gameFinished
int gameState = 0;
bool pauseMenu = false;
bool controlScreen = false;
bool isPaused = true;
bool isPausedCovidAudio = false;
// 0 is none, 2 is won, 3 is game over
int endGameAudioOn = 0;
// 0 on start, 1 on controls, 2 on exit
int mainMenuState = 0;
// 0 on restart, 1 on continue, 2 on exit
int pauseMenuState = 0;
// 0 is controller, 1 is keyboard
int controlScreenState = 0;

bool auxKeyBack = true;				// Left (controls) buttons[6]
bool auxKeyStart = true;			// Right (pause) buttons[7]
bool auxKeyJoyA = true;				// buttons[0] (A)
bool auxKeyCrucetaForward = true;	// buttons[12] (down) and buttons[11] (right)
bool auxKeyCrucetaBackward = true;	// buttons[10] (up) and buttons[13] (left)

// Vida personaje
int noHP = 3;
int noBullets = 0;
int noCovids = 4;
int noMasks = 8;
std::string cadena = "";
std::string cadena1 = "";
std::string cadena2 = "";
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
//Shader para las pantallas con texturas
Shader shaderTexture;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distTP = 2.5;
float distFP = -0.5;
float distanceFromTarget = distFP;
bool selectCamera = true;
bool enableCameraChange = false;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;
//Box para las pantallas
Box boxScreens;

ShadowBox* shadowBox;

// Models complex instances
// Lamps
Model modelLamp1;
Model modelLamp2;
// Map
Model mapArray[NUM_MAP_PIECES];
// Bullet
Model modelBullet;

// Model animate instance
// Osmosis
Model modelOsmosisAnimate;
// Covid
Model modelCovid;
// Masks
Model modelMask;

std::string mapDirs[NUM_MAP_PIECES] = {
	"../models/Map/L_HortBackLeft.obj",
	"../models/Map/L_HortBackRight.obj",
	"../models/Map/L_HortFrontLeft.obj",
	"../models/Map/L_HortFrontRight.obj",
	"../models/Map/L_VertBackLeft.obj",
	"../models/Map/L_VertBackRight.obj",
	"../models/Map/L_VertFrontLeft.obj",
	"../models/Map/L_VertFrontRight.obj",
	"../models/Map/Small_BackLeft.obj",
	"../models/Map/Small_BackRight.obj",
	"../models/Map/Small_FrontLeft.obj",
	"../models/Map/Small_FrontRight.obj",
	"../models/Map/Square_FrontLeft.obj",
	"../models/Map/Square_FrontRight.obj",
	"../models/Map/Square_Left.obj",
	"../models/Map/Square_Middle.obj",
	"../models/Map/Square_Right.obj",
	"../models/Map/Wall_InBackLeft.obj",
	"../models/Map/Wall_InBackMiddle.obj",
	"../models/Map/Wall_InBackRight.obj",
	"../models/Map/Wall_InFrontLeft.obj",
	"../models/Map/Wall_InFrontMiddle.obj",
	"../models/Map/Wall_InFrontRight.obj",
	"../models/Map/Wall_OutBackLeft01.obj",
	"../models/Map/Wall_OutBackLeft02.obj",
	"../models/Map/Wall_OutBackRight01.obj",
	"../models/Map/Wall_OutBackRight02.obj",
	"../models/Map/Wall_OutFrontLeft01.obj",
	"../models/Map/Wall_OutFrontLeft02.obj",
	"../models/Map/Wall_OutFrontRight01.obj",
	"../models/Map/Wall_OutFrontRight02.obj",
	"../models/Map/Wall_OutLeftBack.obj",
	"../models/Map/Wall_OutLeftFront.obj",
	"../models/Map/Wall_OutRightBack.obj",
	"../models/Map/Wall_OutRightFront.obj"
};

// Terrain model instance
Terrain terrain(-1, -1, 200, 2, "../Textures/heightmap2.png");

GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID,
textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticleSmokeID, textureParticleRainID, texIDSmoke, texIDRain;
GLuint skyboxTextureID;
GLuint textureActiveID, textureStartID, textureExitID, textureContinueID, textureRestartID, 
textureExit2ID, textureControlsID, textureControls2ID, textureHUDID, textureYouWonID, 
textureGameOverID, textureControlsIniID;

// Modelo para el redener de texto
FontTypeRendering::FontTypeRendering* modelText;
FontTypeRendering::FontTypeRendering* modelText2;
FontTypeRendering::FontTypeRendering* modelText3;
FontTypeRendering::FontTypeRendering* modelText4;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/night/shn_ft.png",
	"../Textures/night/shn_bk.png",
	"../Textures/night/shn_up.png",
	"../Textures/night/shn_dn.png",
	"../Textures/night/shn_rt.png",
	"../Textures/night/shn_lf.png" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 modelMatrixOsmosis = glm::mat4(1.0f);
glm::mat4 modelMatrixCovid[NUM_COVID] = { glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f) };
glm::mat4 modelMatrixMask = glm::mat4(1.0f);
glm::mat4 modelMatrixBullet = glm::mat4(1.0f);
glm::mat4 modelMatrixBulletBody = glm::mat4(1.0f);
glm::mat4 modelMatrixBulletRef = glm::mat4(1.0f);
glm::mat4 modelMatrixMap = glm::mat4(1.0f);

int animationIndex = 1;
int modelSelected = 0;

// Bullet states
bool enableBulletFiring = true;
bool bulletIsActive = false;
float bulletMovement = 0.0;
float bulletMaxMovement = 100.0;

// Rendering flags
bool renderMask[NUM_MASKS] = { true, true, true, true, true, true, true, true };
bool renderCovid[NUM_COVID] = { true, true, true, false, true };
int freeCovid = 3;
bool stopAudioCovid[NUM_COVID] = { false, false, false, false, false };

// Var Covid machines
float stepCountCovid[NUM_COVID] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
int stateCovid[NUM_COVID] = { 0, 0, 0, 0, 0 };

glm::vec3 colorNeb = glm::vec3(0.0, 0.0, 0.0);
glm::vec3 neblinaGris = glm::vec3(0.25, 0.25, 0.25);
glm::vec3 neblinaRoja = glm::vec3(0.9, 0.2, 0.2);
glm::vec3 colorLluvia = glm::vec3(0.5, 0.5, 0.5);
glm::vec3 lluviaGris = glm::vec3(0.25, 0.25, 0.25);
glm::vec3 lluviaRoja = glm::vec3(0.9, 0.2, 0.2);

// Lamps positions
std::vector<glm::vec3> lamp1Position = { glm::vec3(-22.686, 7.12277, -8.18843),
	glm::vec3(22.686, 7.12277, -8.18843), glm::vec3(-21.8281, 7.12277, 11.7985), glm::vec3(21.8281, 7.12277, 11.7985) };
std::vector<float> lamp1Orientation = { 0.0, 0.0, 90.0, -90.0 };
std::vector<glm::vec3> lamp2Position = { glm::vec3(-71.9101, 7.12277, 1.6491),
		glm::vec3(0.0, 7.12277, -35.1904), glm::vec3(0.0, 7.12277, 38.4036), glm::vec3(71.9101, 7.12277, 1.6491) };
std::vector<float> lamp2Orientation = { -90.0, 180, 0.0, 90.0 };

glm::vec3 normalLightsSpot[] = { glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.898, 0.870, 0.643), glm::vec3(0.949, 0.921, 0.694) };
glm::vec3 normalLightsPoint[] = { glm::vec3(0.648, 0.620, 0.393), glm::vec3(0.698, 0.670, 0.443), glm::vec3(0.749, 0.721, 0.494) };
glm::vec3 lowHPLightsSpot[] = { glm::vec3(0.863, 0.224, 0.177), glm::vec3(0.913, 0.274, 0.227), glm::vec3(0.963, 0.324, 0.277) };
glm::vec3 lowHPLightsPoint[] = { glm::vec3(0.663, 0.024, 0.0), glm::vec3(0.713, 0.074, 0.027), glm::vec3(0.763, 0.124, 0.077) };

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
 // One per covid, background, win, lose, covid dying, player losing a life, pick up mask
#define NUM_SOURCES NUM_COVID + 6
#define NUM_BUFFERS NUM_SOURCES
#define NUM_ENVIRONMENTS 1

// Audio variables
// 0 - 4 covid, 5 background, 6 covid dying, 7 pick up mask, 8 player hit, 9 win, 10 game over
float gains[NUM_SOURCES] = { 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 0.20f, 1.5f, 1.0f, 1.5f, 3.0f, 3.0f };
//int bufferIdx[NUM_SOURCES] = { 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6 };
bool loop[NUM_SOURCES] = { true, true, true, true, true, true, false, false, false, false, false };
int distance[NUM_SOURCES] = { 200, 200, 200, 200, 200, 500, 1000, 500, 500, 500, 500 };

// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Sources 
// 0 - 4 covid, 5 background, 6 covid dying, 7 pick up mask, 8 player hit, 9 win, 10 game over
ALfloat sourcesPos[NUM_SOURCES][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } };
ALfloat sourcesVel[NUM_SOURCES][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 },
	{ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } };
// Buffers
// 0 - 4 covid, 5 background, 6 covid dying, 7 pick up mask, 8 player hit, 9 win, 10 game over
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid* data;
int ch;
//ALboolean loop;
// 0 - 4 covid, 5 background, 6 covid dying, 7 pick up mask, 8 player hit, 9 win, 10 game over
std::vector<bool> sourcesPlay = { true, true, true, false, true, true, false, false, false, false, false };

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

void resetGame() {
	noHP = 3;
	noBullets = 0;
	noCovids = 4;
	noMasks = 8;
	cadena = "";
	cadena1 = "";
	cadena2 = "";
	cadena3 = "";

	modelMatrixOsmosis = glm::mat4(1.0f);
	modelMatrixMask = glm::mat4(1.0f);
	modelMatrixBullet = glm::mat4(1.0f);
	modelMatrixBulletBody = glm::mat4(1.0f);
	modelMatrixBulletRef = glm::mat4(1.0f);
	modelMatrixMap = glm::mat4(1.0f);

	enableBulletFiring = true;
	bulletIsActive = false;
	bulletMovement = 0.0;
	bulletMaxMovement = 10.0;

	for (unsigned int i = 0; i < NUM_MASKS; i++) {
		renderMask[i] = true;
	}

	for (unsigned int i = 0; i < NUM_COVID; i++) {
		modelMatrixCovid[i] = glm::mat4(1.0f);
		if (i == 3) {
			renderCovid[i] = false;
		}
		else {
			renderCovid[i] = true;
		}
		stopAudioCovid[i] = false;
		stepCountCovid[i] = 0.0f;
		stateCovid[i] = 0;
	}

	sourcesPlay = { true, true, true, false, true, true, false, false, false, false, false };

	freeCovid = 3;

	// Covid positions
	// Left Down, Left Up, Right Down, Right Up, Middle
	covidPositions = { glm::vec3(-69.0f, 0.1684f, 35.0f), glm::vec3(-69.0f, 0.1684f, -32.0f), glm::vec3(69.0f, 0.1684f, 35.0f),
		glm::vec3(69.0f, 0.1684f, -32.0f), glm::vec3(-30.0f, 0.1684f, -15.0f) };

	// Mask positions
	maskPositions = { glm::vec3(0.0f, 0.0f, -32.0f), glm::vec3(0.0f, 0.0f, 35.0f), glm::vec3(-40.0f, 0.0f, 2.0f),
		glm::vec3(40.0f, 0.0f, 2.0f), glm::vec3(-69.0f, 0.0f, 1.0f), glm::vec3(69.0f, 0.0f, 1.0f), glm::vec3(12.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 18.5f) };

	camera->setPosition(glm::vec3(0.0, 5.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	// Ubicaci??n Osmosis
	modelMatrixOsmosis = glm::translate(modelMatrixOsmosis,
		glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(180.0f),
		glm::vec3(0, 1, 0));

	// Covid
	for (unsigned int i = 0; i < NUM_COVID; i++)
	{
		modelMatrixCovid[i] = glm::translate(modelMatrixCovid[i], covidPositions[i]);
	}
}

// Direction true for forward, false for backward
void navigateMenu(bool direction) {
	// Main menu
	if (gameState == 0) {
		if (controlScreen) {
			if (direction) {
				controlScreenState += 1;
				if (controlScreenState > 1) {
					controlScreenState = 0;
				}
			}
			else {
				controlScreenState -= 1;
				if (controlScreenState < 0) {
					controlScreenState = 1;
				}
			}
		}
		else {
			if (direction) {
				mainMenuState += 1;
				if (mainMenuState > 2) {
					mainMenuState = 0;
				}
			}
			else {
				mainMenuState -= 1;
				if (mainMenuState < 0) {
					mainMenuState = 2;
				}
			}
		}
	}
	// Control screen
	else if (controlScreen) {
		if (direction) {
			controlScreenState += 1;
			if (controlScreenState > 1) {
				controlScreenState = 0;
			}
		}
		else {
			controlScreenState -= 1;
			if (controlScreenState < 0) {
				controlScreenState = 1;
			}
		}
	}
	// Pause menu
	else if (pauseMenu) {
		if (direction) {
			pauseMenuState += 1;
			if (pauseMenuState > 2) {
				pauseMenuState = 0;
			}
		}
		else {
			pauseMenuState -= 1;
			if (pauseMenuState < 0) {
				pauseMenuState = 2;
			}
		}
	}
}

void menuAccept() {
	switch (gameState)
	{
	// Game is on main menu
	case 0:
		// Control screen open from main menu
		if (controlScreen) {
			controlScreen = false;
			controlScreenState = 0;
		}
		// Main menu
		else {
			switch (mainMenuState)
			{
				// Start selected
			case 0:
				gameState = 1;
				isPaused = false;
				break;
				// Controls selected
			case 1:
				controlScreen = true;
				controlScreenState = 0;
				break;
				// Exit selected
			case 2:
				exitApp = true;
				break;
			default:
				break;
			}
		}
		break;
	// Game is running
	case 1:
		// Game is paused
		if (isPaused) {
			// Game is in control screen
			if (controlScreen) {
				// Control screen was opened from pause menu
				if (pauseMenu) {
					controlScreen = false;
					controlScreenState = 0;
				}
				// Control screen was opened with the game running
				else {
					controlScreen = false;
					controlScreenState = 0;
					isPaused = false;
				}
			}
			// Game is on pause menu
			else if (pauseMenu) {
				controlScreen = false;
				controlScreenState = 0;
				switch (pauseMenuState)
				{
					// Restart selected
				case 0:
					resetGame();
					// Continue selected
				case 1:
					isPaused = false;
					break;
					// Exit selected
				case 2:
					exitApp = true;
					break;
				default:
					break;
				}
				pauseMenu = false;
				pauseMenuState = 0;
			}
		}
		break;
	// Game is won and game over
	case 2:
	case 3:
		resetGame();
		gameState = 0;
		break;
	default:
		break;
	}
}

void pauseMenuAction() {
	switch (gameState)
	{
	// Game is running
	case 1:
		if (controlScreen) {
			pauseMenu = true;
		}
		else {
			isPaused = !isPaused;
			pauseMenu = !pauseMenu;
		}
		pauseMenuState = 0;
		controlScreen = false;
		controlScreenState = 0;
		break;
	default:
		break;
	}
}

void controlScreenAction() {
	switch (gameState)
	{
	// Game is running
	case 1:
		// Game is paused
		if (isPaused) {
			// Control screen key was pressed while pause menu was active
			if (pauseMenu) {
				isPaused = true;
				controlScreen = !controlScreen;
				controlScreenState = 0;
			}
			// Control screen key was pressed after opening it directly
			else {
				isPaused = false;
				controlScreen = false;
				controlScreenState = 0;
			}
		}
		// Control screen key was pressed with game running
		else {
			isPaused = true;
			controlScreen = true;
			controlScreenState = 0;
		}
		break;
	default:
		break;
	}
}

void backAction() {
	switch (gameState)
	{
	// Game is in main menu, exists game
	case 0:
		if (controlScreen) {
			controlScreen = false;
			controlScreenState = 0;
		}
		else {
			exitApp = true;
		}
		break;
	// Game is running
	case 1:
		// Game is not paused, enter pause menu
		if (!isPaused) {
			pauseMenuAction();
		}
		// Game is paused and on pause menu, resume game
		else if (isPaused && pauseMenu) {
			pauseMenuAction();
		}
		// Game is paused and on control screen, resume game
		else if (isPaused && controlScreen) {
			isPaused = false;
			controlScreen = false;
			controlScreenState = 0;
		}
		break;
		// Game is won or in game over, back to main menu
	case 2:
	case 3:
		menuAccept();
		break;
	default:
		break;
	}
	pauseMenuState = 0;
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

	// Inicializaci??n de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	shaderParticlesSmoke.initialize("../Shaders/particlesSmoke.vs", "../Shaders/particlesSmoke.fs", { "Position", "Velocity", "Age" });
	shaderParticlesRain.initialize("../Shaders/particlesRain.vs", "../Shaders/particlesRain.fs", { "Position", "Velocity", "Age" });
	shaderViewDepth.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");
	shaderTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");

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

	//Iniciamos el Box de las pantallas
	boxScreens.init();
	boxScreens.setShader(&shaderTexture);
	boxScreens.setScale(glm::vec3(2.0, 2.0, 1.0));

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));

	//Lamp models
	modelLamp1.loadModel("../models/Lamps/PointLamp/PointLamp2.fbx");
	modelLamp1.setShader(&shaderMulLighting);
	modelLamp2.loadModel("../models/Lamps/SpotLamp/SpotLamp3.fbx");
	modelLamp2.setShader(&shaderMulLighting);
	
	//Osmosis
	modelOsmosisAnimate.loadModel("../models/Osmosis/OsmosisPose.fbx");
	modelOsmosisAnimate.setShader(&shaderMulLighting);

	//Map
	for (unsigned int i = 0; i < NUM_MAP_PIECES; i++)
	{
		mapArray[i].loadModel(mapDirs[i]);
		mapArray[i].setShader(&shaderMulLighting);
	}

	// Covid
	modelCovid.loadModel("../models/Covid/covid.fbx");
	modelCovid.setShader(&shaderMulLighting);

	// Masks
	modelMask.loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
	modelMask.setShader(&shaderMulLighting);

	// Bullet
	modelBullet.loadModel("../models/Osmosis/bala.fbx");
	modelBullet.setShader(&shaderMulLighting);

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

	//***************************************************************************************************************************
	// Texturas de pantallas
	Texture textureStart("../Textures/Pantallas/Start.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureStart.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureStart.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureStartID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureStartID);
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
	textureStart.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureControlsIni("../Textures/Pantallas/ControlsIni.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureControlsIni.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureControlsIni.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureControlsIniID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureControlsIniID);
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
	textureControlsIni.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureExit("../Textures/Pantallas/Exit.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureExit.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureExit.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureExitID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureExitID);
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
	textureExit.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureContinue("../Textures/Pantallas/Continue.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureContinue.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureContinue.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureContinueID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureContinueID);
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
	textureContinue.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureRestart("../Textures/Pantallas/Restart.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureRestart.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureRestart.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureRestartID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureRestartID);
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
	textureRestart.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureExit2("../Textures/Pantallas/Exit2.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureExit2.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureExit2.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureExit2ID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureExit2ID);
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
	textureExit2.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureControls("../Textures/Pantallas/Controls.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureControls.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureControls.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureControlsID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureControlsID);
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
	textureControls.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureControls2("../Textures/Pantallas/Controls2.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureControls2.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureControls2.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureControls2ID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureControls2ID);
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
	textureControls2.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureYouWon("../Textures/Pantallas/YouWon.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureYouWon.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureYouWon.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureYouWonID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureYouWonID);
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
	textureYouWon.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureGameOver("../Textures/Pantallas/GameOver.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureGameOver.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureGameOver.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureGameOverID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureGameOverID);
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
	textureGameOver.freeImage(bitmap);

	// Texturas de pantallas
	Texture textureHUD("../Textures/Pantallas/HUD.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureHUD.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureHUD.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureHUDID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureHUDID);
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
	textureHUD.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/snow.jpg");
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Textures/piso.jpg");
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Textures/nieve.jpg");
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Textures/grumos.jpg");
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/blendMap2.png");
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
	std::uniform_real_distribution<float> distr01 = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesSmoke * 2;
	std::vector<GLfloat> randData(size);
	for (int i = 0; i < randData.size(); i++) {
		randData[i] = distr01(generator);
	}
	glGenTextures(1, &texIDSmoke);
	glBindTexture(GL_TEXTURE_1D, texIDSmoke);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT, randData.data());
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
	std::vector<GLfloat> randData2(size2);
	for (int i = 0; i < randData2.size(); i++) {
		randData2[i] = distr02(generator2);
	}
	glGenTextures(1, &texIDRain);
	glBindTexture(GL_TEXTURE_1D, texIDRain);
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
	shaderParticlesSmoke.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));
	shaderParticlesRain.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));

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
	 * almacenar el buffer de profundidad
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
	// 0 covid, 1 background, 2 covid dying, 3 pick up mask, 4 player hit, 5 win, 6 game over
	// 0 - 4 covid, 5 background, 6 covid dying, 7 pick up mask, 8 player hit, 9 win, 10 game over
	buffer[0] = alutCreateBufferFromFile("../sounds/covid.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/covid.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/covid.wav");
	buffer[3] = alutCreateBufferFromFile("../sounds/covid.wav");
	buffer[4] = alutCreateBufferFromFile("../sounds/covid.wav");
	buffer[5] = alutCreateBufferFromFile("../sounds/background.wav");
	buffer[6] = alutCreateBufferFromFile("../sounds/covid_dead.wav");
	buffer[7] = alutCreateBufferFromFile("../sounds/mask.wav");
	buffer[8] = alutCreateBufferFromFile("../sounds/hp_loss.wav");
	buffer[9] = alutCreateBufferFromFile("../sounds/win.wav");
	buffer[10] = alutCreateBufferFromFile("../sounds/game_over.wav");
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

	for (unsigned int i = 0; i < NUM_SOURCES; i++) {
		alSourcef(source[i], AL_PITCH, 1.0f);
		alSourcef(source[i], AL_GAIN, gains[i]);
		alSourcefv(source[i], AL_POSITION, sourcesPos[i]);
		alSourcefv(source[i], AL_VELOCITY, sourcesVel[i]);
		alSourcei(source[i], AL_BUFFER, buffer[i]);
		alSourcei(source[i], AL_LOOPING, loop[i]);
		alSourcef(source[i], AL_MAX_DISTANCE, distance[i]);
	}

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
	modelText4 = new FontTypeRendering::FontTypeRendering(screenWidth,
		screenHeight);
	modelText4->Initialize();
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderSkybox.destroy();
	shaderMulLighting.destroy();
	shaderTerrain.destroy();
	shaderViewDepth.destroy();
	shaderDepth.destroy();
	shaderParticlesSmoke.destroy();
	shaderParticlesRain.destroy();
	shaderTexture.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	boxViewDepth.destroy();
	boxLightViewBox.destroy();
	boxScreens.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	modelLamp1.destroy();
	modelLamp2.destroy();
	modelBullet.destroy();
	for (unsigned int i = 0; i < NUM_MAP_PIECES; i++)
	{
		mapArray[i].destroy();
	}

	// Custom objects animate
	modelOsmosisAnimate.destroy();
	modelCovid.destroy();
	modelMask.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleSmokeID);
	glDeleteTextures(1, &textureParticleRainID);
	glDeleteTextures(1, &texIDSmoke);
	glDeleteTextures(1, &texIDRain);
	glDeleteTextures(1, &skyboxTextureID);
	glDeleteTextures(1, &textureActiveID);
	glDeleteTextures(1, &textureStartID);
	glDeleteTextures(1, &textureExitID);
	glDeleteTextures(1, &textureContinueID);
	glDeleteTextures(1, &textureRestartID);
	glDeleteTextures(1, &textureExit2ID);
	glDeleteTextures(1, &textureControlsID);
	glDeleteTextures(1, &textureControls2ID);
	glDeleteTextures(1, &textureHUDID);
	glDeleteTextures(1, &textureYouWonID);
	glDeleteTextures(1, &textureGameOverID);
	glDeleteTextures(1, &textureControlsIniID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

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
			backAction();
			break;
		case GLFW_KEY_UP:
			if (isPaused) {
				navigateMenu(false);
			}
			break;
		case GLFW_KEY_RIGHT:
			if (isPaused) {
				navigateMenu(true);
			}
			break;
		case GLFW_KEY_DOWN:
			if (isPaused) {
				navigateMenu(true);
			}
			break;
		case GLFW_KEY_LEFT:
			if (isPaused) {
				navigateMenu(false);
			}
			break;
		case GLFW_KEY_ENTER:
			if (isPaused) {
				menuAccept();
			}
			break;
		case GLFW_KEY_P:
			pauseMenuAction();
			break;
		case GLFW_KEY_O:
			controlScreenAction();
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
	/*distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);*/
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			//std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			//std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			//std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			//std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
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

		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1,
			&buttonCount);
		//std::cout << "N??mero de botones disponibles :=>" << buttonCount << std::endl;
		/*if (buttons[0] == GLFW_PRESS)
			std::cout << "Se presiona A" << std::endl;
		if (buttons[1] == GLFW_PRESS)
			std::cout << "Se presiona B" << std::endl;
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
			std::cout << "Cruceta Izquierda" << std::endl;*/

		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		/*std::cout << "N??mero de ejes disponibles :=>" << axesCount << std::endl;
		std::cout << "Left Stick X axis: " << axes[0] << std::endl;
		std::cout << "Left Stick Y axis: " << axes[1] << std::endl;
		std::cout << "Left Trigger/LT: " << axes[4] << std::endl;
		std::cout << "Right Stick X axis: " << axes[3] << std::endl;
		std::cout << "Right Stick Y axis: " << axes[2] << std::endl;
		std::cout << "Right Trigger/RT: " << axes[5] << std::endl;*/

		// Game is running
		if (!isPaused) {
			// Mover Adelante Atras (Left Stick Y axis)
			if (fabs(axes[1]) != 0.0) {
				modelMatrixOsmosis = glm::translate(modelMatrixOsmosis, glm::vec3(0, 0, axes[1] * 0.2));
				animationIndex = 1;
			}

			// Mover Izquierda Derecha (Left Stick X axis)
			if (fabs(axes[0]) != 0.0) {
				modelMatrixOsmosis = glm::translate(modelMatrixOsmosis, glm::vec3(-axes[0] * 0.2, 0, 0));
				animationIndex = 1;
			}

			// Girar Izquierda Derecha (Right Stick Y axis) ??
			if (fabs(axes[2]) != 0.0) {
				modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(-axes[2] * 2), glm::vec3(0, 1, 0));
				//camera->mouseMoveCamera(axes[2], 0, deltaTime);
				animationIndex = 1;
			}

			// Change camera
			if (enableCameraChange && buttons[5] == GLFW_PRESS) {
				selectCamera = !selectCamera;
				if (selectCamera)
					distanceFromTarget = distFP;
				else
					distanceFromTarget = distTP;
				enableCameraChange = false;
			}
			else if (buttons[5] == GLFW_RELEASE)
				enableCameraChange = true;
			
			// Shoot (RT)
			if (enableBulletFiring && axes[5] > 0) {
				if (noBullets > 0) {
					noBullets -= 1;
					enableBulletFiring = false;
					bulletIsActive = true;
					std::cout << "Bullet fired by controller" << std::endl;
				}
				animationIndex = 1;
			}
			else if (axes[5] <= 0) {
				if (!bulletIsActive) {
					enableBulletFiring = true;
				}
			}
		}
		
		// Controls (Back)
		if (auxKeyBack && buttons[6] == GLFW_PRESS && gameState == 1) {
			//std::cout << "BACK PRESS" << std::endl;
			auxKeyBack = false;
			controlScreenAction();
		}
		else if (!auxKeyBack && buttons[6] == GLFW_RELEASE) {
			//std::cout << "BACK RELEASE" << std::endl;
			auxKeyBack = true;
		}

		// Pause menu (Start)
		if (auxKeyStart && buttons[7] == GLFW_PRESS && gameState == 1) {
			//std::cout << "PAUSE PRESS" << std::endl;
			auxKeyStart = false;
			pauseMenuAction();
		}
		else if (!auxKeyStart && buttons[7] == GLFW_RELEASE) {
			//std::cout << "PAUSE RELEASE" << std::endl;
			auxKeyStart = true;
		}

		// Accept (A)
		if (auxKeyJoyA && buttons[0] == GLFW_PRESS) {
			//std::cout << "A PRESS" << std::endl;
			auxKeyJoyA = false;
			menuAccept();
		}
		else if (!auxKeyJoyA && buttons[0] == GLFW_RELEASE) {
			//std::cout << "A RELEASE" << std::endl;
			auxKeyJoyA = true;
		}

		// Forward (Down and Right)
		if (auxKeyCrucetaForward && (buttons[12] == GLFW_PRESS || buttons[11] == GLFW_PRESS)) {
			//std::cout << "FORWARD PRESS" << std::endl;
			auxKeyCrucetaForward = false;
			navigateMenu(true);
		}
		else if (!auxKeyCrucetaForward && buttons[12] == GLFW_RELEASE && buttons[11] == GLFW_RELEASE) {
			//std::cout << "FORWARD RELEASE" << std::endl;
			auxKeyCrucetaForward = true;
		}

		// Backward (Up and Left)
		if (auxKeyCrucetaBackward && (buttons[10] == GLFW_PRESS || buttons[13] == GLFW_PRESS)) {
			//std::cout << "BACKWARD PRESS" << std::endl;
			auxKeyCrucetaBackward = false;
			navigateMenu(false);
		}
		else if (!auxKeyCrucetaBackward && buttons[10] == GLFW_RELEASE && buttons[13] == GLFW_RELEASE) {
			//std::cout << "BACKWARD RELEASE" << std::endl;
			auxKeyCrucetaBackward = true;
		}

		/*if (fabs(axes[3]) > 0.2) {
			camera->mouseMoveCamera(0.0, axes[3], deltaTime);
		}*/
		/*if (fabs(axes[4]) > 0.2) {
			camera->mouseMoveCamera(0.0, axes[4], deltaTime);
		}*/
	}

	if (!isPaused) {

		// Osmosis movement
		if (modelSelected == 0 && (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)) {
			modelMatrixOsmosis = glm::translate(modelMatrixOsmosis, glm::vec3(0, 0, 0.2));
		}
		if (modelSelected == 0 && (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)) {
			modelMatrixOsmosis = glm::translate(modelMatrixOsmosis, glm::vec3(0, 0, -0.2));
		}
		if (modelSelected == 0 && (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)) {
			modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(3.0f), glm::vec3(0, 1, 0));
		}
		if (modelSelected == 0 && (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)) {
			modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(-3.0f), glm::vec3(0, 1, 0));
		}

		// Fire bullet
		if (enableBulletFiring && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			if (noBullets > 0) {
				noBullets -= 1;
				enableBulletFiring = false;
				bulletIsActive = true;
				std::cout << "Bullet fired by keyboard" << std::endl;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
			if (!bulletIsActive) {
				enableBulletFiring = true;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
			if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
			{
				selectCamera = !selectCamera;
				if (selectCamera)
					distanceFromTarget = distFP;
				else
					distanceFromTarget = distTP;
				enableCameraChange = false;
			}
		}

		//Para que no pueda mover la camara el usuario
		/*if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			camera->mouseMoveCamera(0.0, offsetY, deltaTime);*/
		offsetX = 0;
		offsetY = 0;
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

	// Ubicaci??n Osmosis
	modelMatrixOsmosis = glm::translate(modelMatrixOsmosis,
		glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(180.0f),
		glm::vec3(0, 1, 0));

	// Covid
	for (unsigned int i = 0; i < NUM_COVID; i++)
	{
		modelMatrixCovid[i] = glm::translate(modelMatrixCovid[i], covidPositions[i]);
	}

	lastTime = TimeManager::Instance().GetTime();

	// Time for the particles animation
	currTimeParticlesAnimationSmoke = lastTime;
	lastTimeParticlesAnimationSmoke = lastTime;

	currTimeParticlesAnimationRain = lastTime;
	lastTimeParticlesAnimationRain = lastTime;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);

	shadowBox = new ShadowBox(-lightPos, camera.get(), 15.0f, 0.1f, 45.0f);

	while (psi) {
		// GAME STATE
		// Game over
		if (noHP <= 0 || (noCovids > (noMasks + noBullets))) {
			gameState = 3;
			isPaused = true;
			sourcesPlay[10] = true;
		}
		// Game won
		else if (noCovids == 0) {
			gameState = 2;
			isPaused = true;
			sourcesPlay[9] = true;
		}

		//*********************************************************************************************************************
		//Textura de pantalla activa
		switch (gameState)
		{
		// Game in main menu
		case 0:
			if (controlScreen) {
				switch (controlScreenState)
				{
				case 0:
					//std::cout << "GAME STATE 1 is paused control screen 0" << std::endl;
					textureActiveID = textureControlsID;
					break;
				case 1:
					//std::cout << "GAME STATE 1 is paused control screen 1" << std::endl;
					textureActiveID = textureControls2ID;
					break;
				default:
					break;
				}
			}
			else {
				switch (mainMenuState)
				{
				case 0:
					//std::cout << "GAME STATE 0 main menu 0" << std::endl;
					textureActiveID = textureStartID;
					break;
				case 1:
					//std::cout << "GAME STATE 0 main menu 1" << std::endl;
					textureActiveID = textureControlsIniID;
					break;
				case 2:
					//std::cout << "GAME STATE 0 main menu 2" << std::endl;
					textureActiveID = textureExitID;
				default:
					break;
				}
			}
			break;
		// Game has started
		case 1:
			// Game is paused
			if (isPaused) {
				// Game is in control screen
				if (controlScreen) {
					switch (controlScreenState)
					{
					case 0:
						//std::cout << "GAME STATE 1 is paused control screen 0" << std::endl;
						textureActiveID = textureControlsID;
						break;
					case 1:
						//std::cout << "GAME STATE 1 is paused control screen 1" << std::endl;
						textureActiveID = textureControls2ID;
						break;
					default:
						break;
					}
				}
				// Game is in pause menu
				else if (pauseMenu) {
					switch (pauseMenuState)
					{
					case 0:
						//std::cout << "GAME STATE 1 is paused pause menu 0" << std::endl;
						textureActiveID = textureRestartID;
						break;
					case 1:
						//std::cout << "GAME STATE 1 is paused pause menu 1" << std::endl;
						textureActiveID = textureContinueID;
						break;
					case 2:
						//std::cout << "GAME STATE 1 is paused pause menu 2" << std::endl;
						textureActiveID = textureExit2ID;
						break;
					default:
						break;
					}
				}
			}
			// Game is running
			else {
				//std::cout << "GAME STATE 1 is running" << std::endl;
				textureActiveID = textureHUDID;
			}
			break;
		// Game is over
		case 2:
			//std::cout << "GAME STATE 2" << std::endl;
			textureActiveID = textureYouWonID;
			break;
		// Game is won
		case 3:
			textureActiveID = textureGameOverID;
			break;
		default:
			break;
		}

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
		target = glm::vec3(modelMatrixOsmosis[3].x, modelMatrixOsmosis[3].y + 4.25, modelMatrixOsmosis[3].z);

		if (std::isnan(angleTarget))
			angleTarget = 0.0;
		if (axis.y < 0)
			angleTarget = -angleTarget;
		camera->setCameraTarget(target);
		camera->setDistanceFromTarget(distanceFromTarget);
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
		shaderParticlesSmoke.setVectorFloat3("colorSmoke", glm::value_ptr(glm::vec3(1, 1, 1)));

		if (noHP <= 1)
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
		if (noHP <= 1)
			colorNeb = neblinaRoja;
		else
			colorNeb = neblinaGris;
		shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(colorNeb));
		shaderMulLighting.setFloat("density", 0.055);
		shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(colorNeb));
		shaderTerrain.setFloat("density", 0.055);
		shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(colorNeb));
		shaderSkybox.setFloat("density", 0.055);

		/*******************************************
		* Propiedades Luz direccional
		*******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.025, 0.025, 0.025)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.075, 0.075, 0.075)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.025, 0.025, 0.025)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		* Propiedades Luz direccional Terrain
		*******************************************/
		shaderTerrain.setVectorFloat3("viewPos",
			glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.025, 0.025, 0.025)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.075, 0.075, 0.075)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.025, 0.025, 0.025)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		glm::vec3 activeLightsSpot[3];
		glm::vec3 activeLightsPoint[3];

		if (noHP < 2) {
			std::copy(std::begin(lowHPLightsSpot), std::end(lowHPLightsSpot), std::begin(activeLightsSpot));
			std::copy(std::begin(lowHPLightsPoint), std::end(lowHPLightsPoint), std::begin(activeLightsPoint));
		}
		else {
			std::copy(std::begin(normalLightsSpot), std::end(normalLightsSpot), std::begin(activeLightsSpot));
			std::copy(std::begin(normalLightsPoint), std::end(normalLightsPoint), std::begin(activeLightsPoint));
		}

		/*******************************************
		* Propiedades SpotLights
		*******************************************/
		shaderMulLighting.setInt("spotLightCount", lamp2Position.size());
		shaderTerrain.setInt("spotLightCount", lamp2Position.size());
		for (int i = 0; i < lamp2Position.size(); i++) {
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp,	lamp2Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp2Orientation[i]), glm::vec3(0, 1, 0));
			//matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(0.5, 0.5, 0.5));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0.0, -1.45951, -1.41884));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3("spotLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(activeLightsSpot[0])));
			shaderMulLighting.setVectorFloat3("spotLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(activeLightsSpot[1])));
			shaderMulLighting.setVectorFloat3("spotLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(activeLightsSpot[2])));
			shaderMulLighting.setVectorFloat3("spotLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setVectorFloat3("spotLights[" + std::to_string(i) + "].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
			shaderMulLighting.setFloat("spotLights[" + std::to_string(i) + "].constant", 1.0);
			shaderMulLighting.setFloat("spotLights[" + std::to_string(i) + "].linear", 0.04);
			shaderMulLighting.setFloat("spotLights[" + std::to_string(i) + "].quadratic", 0.01);
			shaderMulLighting.setFloat("spotLights[" + std::to_string(i) + "].cutOff", cos(glm::radians(15.5f)));
			shaderMulLighting.setFloat("spotLights[" + std::to_string(i) + "].outerCutOff", cos(glm::radians(25.5f)));
			shaderTerrain.setVectorFloat3("spotLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(activeLightsSpot[0])));
			shaderTerrain.setVectorFloat3("spotLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(activeLightsSpot[1])));
			shaderTerrain.setVectorFloat3("spotLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(activeLightsSpot[2])));
			shaderTerrain.setVectorFloat3("spotLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setVectorFloat3("spotLights[" + std::to_string(i) + "].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
			shaderTerrain.setFloat("spotLights[" + std::to_string(i) + "].constant", 1.0);
			shaderTerrain.setFloat("spotLights[" + std::to_string(i) + "].linear", 0.04);
			shaderTerrain.setFloat("spotLights[" + std::to_string(i) + "].quadratic", 0.01);
			shaderTerrain.setFloat("spotLights[" + std::to_string(i) + "].cutOff", cos(glm::radians(15.5f)));
			shaderTerrain.setFloat("spotLights[" + std::to_string(i) + "].outerCutOff", cos(glm::radians(25.5f)));
		}

		/*******************************************
		* Propiedades PointLights
		*******************************************/
		shaderMulLighting.setInt("pointLightCount", lamp1Position.size());
		shaderTerrain.setInt("pointLightCount", lamp1Position.size());
		for (int i = 0; i < lamp1Position.size(); i++) {
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, lamp1Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
			//matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(0.5, 0.5, 0.5));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0, -1.30273, -2.57012));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(activeLightsPoint[0])));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(activeLightsPoint[1])));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(activeLightsPoint[2])));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.01);
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(activeLightsPoint[0])));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(activeLightsPoint[1])));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(activeLightsPoint[2])));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.02);
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

		//****************************************************************
		//Colocamos HUD
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glViewport(0, 0, screenWidth, screenHeight);
		shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureActiveID);
		shaderTexture.setInt("outTexture", 0);
		boxScreens.render();
		//glfwSwapBuffers(window);

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

		// Covid colliders
		for (unsigned int i = 0; i < NUM_COVID; i++)
		{
			if (renderCovid[i]) {
				AbstractModel::SBB covidCollider;
				glm::mat4 modelMatrixColliderCovid = glm::mat4(modelMatrixCovid[i]);
				modelMatrixColliderCovid = glm::scale(modelMatrixColliderCovid,
					glm::vec3(0.001, 0.001, 0.001));
				modelMatrixColliderCovid = glm::translate(modelMatrixColliderCovid,
					glm::vec3(modelCovid.getSbb().c.x, modelCovid.getSbb().c.y + 25 * 100, modelCovid.getSbb().c.z));
				covidCollider.c = glm::vec3(modelMatrixColliderCovid[3].x, modelMatrixColliderCovid[3].y, modelMatrixColliderCovid[3].z);
				covidCollider.ratio = modelCovid.getSbb().ratio * 0.04;
				addOrUpdateColliders(collidersSBB, "covid-" + std::to_string(i),
					covidCollider, modelMatrixCovid[i]);
			}
			else {
				collidersSBB.erase("covid-" + std::to_string(i));
			}
		}

		// Mask colliders
		for (unsigned int i = 0; i < NUM_MASKS; i++)
		{
			if (renderMask[i]) {
				AbstractModel::OBB maskCollider;
				glm::mat4 modelMatrixColliderMask = glm::mat4(modelMatrixMask);
				modelMatrixColliderMask = glm::translate(modelMatrixColliderMask,
					glm::vec3(maskPositions[i].x - 0.8, maskPositions[i].y + 2.0, maskPositions[i].z - 0.2));
				modelMatrixColliderMask = glm::rotate(modelMatrixColliderMask,
					glm::radians(90.0f), glm::vec3(0, 1, 0));
				addOrUpdateColliders(collidersOBB, "mask-" + std::to_string(i),
					maskCollider, modelMatrixColliderMask);
				maskCollider.u = glm::quat_cast(modelMatrixColliderMask);
				modelMatrixColliderMask = glm::translate(modelMatrixColliderMask,
					glm::vec3(modelMask.getObb().c));
				maskCollider.e = mapArray[i].getObb().e * glm::vec3(0.28, 0.30, 0.18);
				maskCollider.c = glm::vec3(modelMatrixColliderMask[3]);
				std::get<0>(collidersOBB.find("mask-" + std::to_string(i))->second) =
					maskCollider;
			}
			else {
				collidersOBB.erase("mask-" + std::to_string(i));
			}
			
		}

		// Collider Osmosis
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
		osmosisCollider.e = modelOsmosisAnimate.getObb().e * glm::vec3(0.09, 0.09, 0.09) * glm::vec3(0.5, 0.75, 0.5);
		osmosisCollider.c = glm::vec3(modelMatrixColliderOsmosis[3]);
		addOrUpdateColliders(collidersOBB, "Osmosis", osmosisCollider, modelMatrixOsmosis);

		// Map colliders
		for (unsigned int i = 0; i < NUM_MAP_PIECES; i++)
		{
			AbstractModel::OBB mapCollider;
			glm::mat4 modelMatrixColliderMap = glm::mat4(1.0f);
			addOrUpdateColliders(collidersOBB, "map-" + std::to_string(i),
				mapCollider, modelMatrixColliderMap);
			mapCollider.u = glm::quat_cast(modelMatrixColliderMap);
			modelMatrixColliderMap = glm::translate(modelMatrixColliderMap,
				mapArray[i].getObb().c);
			mapCollider.c = glm::vec3(modelMatrixColliderMap[3]);
			mapCollider.e = mapArray[i].getObb().e;
			std::get<0>(collidersOBB.find("map-" + std::to_string(i))->second) =
				mapCollider;
		}

		// Collider Bullet
		if (bulletIsActive) {
			AbstractModel::SBB bulletCollider;
			glm::mat4 modelMatrixColliderBullet;
			glm::vec3 bulletPosition;
			modelMatrixColliderBullet = glm::mat4(modelMatrixBulletBody);
			bulletPosition = glm::vec3(modelBullet.getSbb().c.x,
				modelBullet.getSbb().c.y,
				modelBullet.getSbb().c.z);
			modelMatrixColliderBullet = glm::translate(modelMatrixColliderBullet, bulletPosition);
			bulletCollider.c = glm::vec3(modelMatrixColliderBullet[3]);
			bulletCollider.ratio = modelBullet.getSbb().ratio * 0.25;
			addOrUpdateColliders(collidersSBB, "bullet", bulletCollider, modelMatrixBulletBody);
		}
		else {
			collidersSBB.erase("bullet");
		}

		// Lamp1 colliders
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

		/*******************************************
		 * Render the colliders
		 *******************************************/
		/*for (std::map<std::string,
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
		}*/

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
						std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
						noBullets += 1;
						int noMask = it->first.substr(5, 1)[0] - '0';
						//std::cout << "EXTRACTED " << noMask << " TYPE " << typeid(noMask).name() << std::endl;
						renderMask[noMask] = !renderMask[noMask];
						noMasks -= 1;
						// 7 is mask
						sourcesPlay[7] = true;
						std::cout << "SOUND flagged (mask) " << 7 << " is " << sourcesPlay[7] << std::endl;
					}
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first,
				isCollision);
		}

		// Sphere vs Sphere
		int auxNoCovid = 0;
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
					//std::cout << "Colision " << it->first << " with "
					//	<< jt->first << std::endl;
					isCollision = true;
					if (it->first.substr(0, 5) == "covid" && jt->first == "bullet") {
						std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
						int noCovid = it->first.substr(6, 1)[0] - '0';
						//std::cout << "EXTRACTED " << noCovid << " TYPE " << typeid(noCovid).name() << std::endl;
						auxNoCovid = noCovid;
						bulletIsActive = false;
						bulletMovement = 0.0;
						renderCovid[noCovid] = !renderCovid[noCovid];
						//sourcesPlay[noCovid] = false;
						stopAudioCovid[noCovid] = true;
						std::cout << "SOUND flagged to stop " << noCovid << " is " << sourcesPlay[noCovid] << std::endl;
						noCovids -= 1;
						if (noCovids > 0) {
							// 6 is covid dying
							sourcesPlay[6] = true;
							std::cout << "SOUND flagged (covid dying) " << 6 << " is " << sourcesPlay[6] << std::endl;
						}
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
			for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				if (testSphereOBox(std::get<0>(it->second),
					std::get<0>(jt->second))) {
					if (it->first.substr(0, 5) == "covid" && jt->first == "Osmosis") {
						std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
						isCollision = true;
						addOrUpdateCollisionDetection(collisionDetection, jt->first,
							isCollision);
						int noCovid = it->first.substr(6, 1)[0] - '0';
						//std::cout << "EXTRACTED " << noCovid << " TYPE " << typeid(noCovid).name() << std::endl;
						if (renderCovid[noCovid])
						{
							noHP -= 1;
							renderCovid[noCovid] = !renderCovid[noCovid];
							//sourcesPlay[noCovid] = false;
							//std::cout << "SOUND unflagged " << noCovid << " is " << sourcesPlay[noCovid] << std::endl;
							stopAudioCovid[noCovid] = true;
							if (noHP > 0) {
								renderCovid[freeCovid] = !renderCovid[freeCovid];
								sourcesPlay[freeCovid] = true;
								freeCovid = noCovid;
								std::cout << "SOUND flagged " << freeCovid << " is " << sourcesPlay[freeCovid] << std::endl;
								// 8 is player hit
								sourcesPlay[8] = true;
								std::cout << "SOUND flagged (player hit) " << 8 << " is " << sourcesPlay[8] << std::endl;
							}
						}
					}
					else if (it->first == "bullet" && jt->first.substr(0, 3) == "map") {
						isCollision = true;
						addOrUpdateCollisionDetection(collisionDetection, jt->first,
							isCollision);
						bulletIsActive = false;
						bulletMovement = 0.0;
						std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
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
		cadena = "X" + std::to_string(noHP);
		cadena1 = "X" + std::to_string(noBullets);
		cadena2 = "X" + std::to_string(noCovids);
		cadena3 = "X" + std::to_string(noMasks);
		if (noHP > 0 && !isPaused) {
			modelText->render(cadena, -.75, 0.8, 50, 1.0, 1.0, 1.0);
			modelText2->render(cadena1, -.75, -0.95, 50, 1.0, 1.0, 1.0);
			modelText3->render(cadena2, .9, 0.8, 50, 1.0, 1.0, 1.0);
			modelText4->render(cadena3, .9, -0.95, 50, 1.0, 1.0, 1.0);
			glfwSwapBuffers(window);
		}
		else {
			modelText->render("", -0.55, 0.0, 160, 1.0, 0.0, 0.0);
			glfwSwapBuffers(window);
		}

		/****************************+
		 * Open AL sound data
		 */

		listenerPos[0] = modelMatrixOsmosis[3].x;
		listenerPos[1] = modelMatrixOsmosis[3].y;
		listenerPos[2] = modelMatrixOsmosis[3].z;
		alListenerfv(AL_POSITION, listenerPos);

		glm::vec3 upModel = glm::normalize(modelMatrixOsmosis[1]);
		glm::vec3 frontModel = glm::normalize(modelMatrixOsmosis[2]);

		listenerOri[0] = frontModel.x;
		listenerOri[1] = frontModel.y;
		listenerOri[2] = frontModel.z;
		listenerOri[3] = upModel.x;
		listenerOri[4] = upModel.y;
		listenerOri[5] = upModel.z;

		alListenerfv(AL_ORIENTATION, listenerOri);

		for (unsigned int i = 0; i < sourcesPlay.size(); i++) {
			// If game is paused and hasn't paused Covid audios
			if (isPaused && !isPausedCovidAudio) {
				if (i < NUM_COVID) {
					if (renderCovid[i]) {
						std::cout << "SOUND paused " << i << std::endl;
						//sourcesPlay[i] = true;
						alSourcePause(source[i]);
					}
				}
				else {
					isPausedCovidAudio = true;
				}
			}
			// If game is unpaused and still has paused Covid audios
			else if (!isPaused && isPausedCovidAudio) {
				if (i < NUM_COVID) {
					if (renderCovid[i]) {
						std::cout << "SOUND activated " << i << std::endl;
						sourcesPlay[i] = false;
						alSourcePlay(source[i]);
					}
				}
				else {
					isPausedCovidAudio = false;
				}
			}
			else if (gameState == 2 && !endGameAudioOn && i == 9) {
				endGameAudioOn = 2;
				sourcesPlay[i] = false;
				std::cout << "SOUND activated (state 2) " << i << std::endl;
				alSourcePlay(source[endGameAudioOn + 7]);
			}
			else if (gameState == 3 && !endGameAudioOn && i == 10) {
				endGameAudioOn = 3;
				sourcesPlay[i] = false;
				std::cout << "SOUND activated (state 3) " << i << std::endl;
				alSourcePlay(source[i]);
			}
			else if (gameState != 2 && gameState != 3 && endGameAudioOn) {
				std::cout << "SOUND deactivated " << endGameAudioOn + 7 << std::endl;
				alSourceStop(source[endGameAudioOn + 7]);
				endGameAudioOn = 0;
			}
			else {
				if (i < NUM_COVID) {
					if (!isPaused) {
						if (sourcesPlay[i] && renderCovid[i]) {
							sourcesPlay[i] = false;
							std::cout << "SOUND activated " << i << std::endl;
							alSourcePlay(source[i]);
						}
						else if (!sourcesPlay[i] && !renderCovid[i] && stopAudioCovid[i]) {
							stopAudioCovid[i] = false;
							std::cout << "SOUND deactivated " << i << std::endl;
							alSourceStop(source[i]);
						}
					}
				}
				else if (i < 9) {
					if (sourcesPlay[i] && i != 9 && i != 10) {
						sourcesPos[i][0] = modelMatrixOsmosis[3].x;
						sourcesPos[i][1] = modelMatrixOsmosis[3].y;
						sourcesPos[i][2] = modelMatrixOsmosis[3].z;
						alSourcefv(source[i], AL_POSITION, sourcesPos[i]);
						sourcesPlay[i] = false;
						std::cout << "SOUND activated " << i << std::endl;
						alSourcePlay(source[i]);
					}
				}
			}
		}
	}
}

void prepareScene() {

	skyboxSphere.setShader(&shaderSkybox);

	terrain.setShader(&shaderTerrain);

	//Lamp models
	modelLamp1.setShader(&shaderMulLighting);
	modelLamp2.setShader(&shaderMulLighting);

	//Osmosis
	modelOsmosisAnimate.setShader(&shaderMulLighting);

	// Covid
	modelCovid.setShader(&shaderMulLighting);

	// Masks
	modelMask.setShader(&shaderMulLighting);

	// Bullet
	modelBullet.setShader(&shaderMulLighting);

	//Map
	for (unsigned int i = 0; i < NUM_MAP_PIECES; i++)
	{
		mapArray[i].setShader(&shaderMulLighting);
	}
}

void prepareDepthScene() {

	skyboxSphere.setShader(&shaderDepth);

	terrain.setShader(&shaderDepth);

	//Lamp models
	modelLamp1.setShader(&shaderDepth);
	modelLamp2.setShader(&shaderDepth);

	// Osmosis
	modelOsmosisAnimate.setShader(&shaderDepth);

	// Covid
	modelCovid.setShader(&shaderDepth);

	// Masks
	modelMask.setShader(&shaderDepth);

	// Bullet
	modelBullet.setShader(&shaderDepth);

	// Map
	for (unsigned int i = 0; i < NUM_MAP_PIECES; i++)
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
	glActiveTexture(GL_TEXTURE0);

	// Render the lamps
	for (int i = 0; i < lamp1Position.size(); i++) {
		lamp1Position[i].y = 7.12277 + terrain.getHeightTerrain(lamp1Position[i].x, lamp1Position[i].z);
		modelLamp1.setPosition(lamp1Position[i]);
		modelLamp1.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelLamp1.setOrientation(glm::vec3(0, lamp1Orientation[i], 0));
		modelLamp1.render();
	}

	for (int i = 0; i < lamp2Position.size(); i++) {
		lamp2Position[i].y = 7.12277 + terrain.getHeightTerrain(lamp2Position[i].x, lamp2Position[i].z);
		modelLamp2.setPosition(lamp2Position[i]);
		modelLamp2.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelLamp2.setOrientation(glm::vec3(0, lamp2Orientation[i], 0));
		modelLamp2.render();
	}

	/*******************************************
	* Custom Anim objects obj
	*******************************************/

	modelMatrixOsmosis[3][1] = terrain.getHeightTerrain(modelMatrixOsmosis[3][0], modelMatrixOsmosis[3][2]);
	glm::mat4 modelMatrixOsmosisBody = glm::mat4(modelMatrixOsmosis);
	modelMatrixOsmosisBody = glm::scale(modelMatrixOsmosisBody, glm::vec3(0.004, 0.004, 0.004));
	modelOsmosisAnimate.setAnimationIndex(animationIndex);
	modelOsmosisAnimate.render(modelMatrixOsmosisBody);

	// Map
	for (unsigned int i = 0; i < NUM_MAP_PIECES; i++)
	{
		modelMatrixMap[3][1] = terrain.getHeightTerrain(modelMatrixMap[3][0], modelMatrixMap[3][2]);
		mapArray[i].render(modelMatrixMap);
	}

	// Covid
	for (unsigned int i = 0; i < NUM_COVID; i++) {
		modelMatrixCovid[i][3][1] = terrain.getHeightTerrain(modelMatrixCovid[i][3][0], modelMatrixCovid[i][3][2]) + 1.0f;
		glm::mat4 modelMatrixCovidBody = glm::mat4(modelMatrixCovid[i]);
		modelMatrixCovidBody = glm::scale(modelMatrixCovidBody, glm::vec3(0.015, 0.015, 0.015));
		modelCovid.setAnimationIndex(0);
		if (renderCovid[i])
		{
			modelCovid.render(modelMatrixCovidBody);
		}
	}

	// Masks
	for (unsigned int i = 0; i < NUM_MASKS; i++) {
		maskPositions[i].y = terrain.getHeightTerrain(maskPositions[i].x,
			maskPositions[i].z);
		modelMask.setPosition(maskPositions[i]);
		modelMask.setScale(glm::vec3(0.015, 0.015, 0.015));
		modelMask.setAnimationIndex(0);
		if (renderMask[i])
		{
			modelMask.render();
		}
	}

	// Bullet
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
		modelBullet.render(modelMatrixBulletBody);
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
	if (!isPaused) {
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

	}

	if (bulletIsActive && !isPaused)
	{
		if (bulletMovement < bulletMaxMovement)
		{
			bulletMovement += 0.2;
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

		for (unsigned int i = 0; i < NUM_COVID; i++)
		{
			if (renderParticles && it->second.first.substr(0, 4) == "Smok" == 0 && renderCovid[i]) {
				lastTimeParticlesAnimationSmoke = currTimeParticlesAnimationSmoke;
				currTimeParticlesAnimationSmoke = TimeManager::Instance().GetTime();

				shaderParticlesSmoke.setInt("Pass", 1);
				shaderParticlesSmoke.setFloat("Time", currTimeParticlesAnimationSmoke);
				shaderParticlesSmoke.setFloat("DeltaT", currTimeParticlesAnimationSmoke - lastTimeParticlesAnimationSmoke);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_1D, texIDSmoke);
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
				sourcesPos[i][0] = modelSmokeParticles[3].x;
				sourcesPos[i][1] = modelSmokeParticles[3].y;
				sourcesPos[i][2] = modelSmokeParticles[3].z;
				alSourcefv(source[i], AL_POSITION, sourcesPos[i]);
			}
		}

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
			glBindTexture(GL_TEXTURE_1D, texIDRain);
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
			if (noHP <= 1)
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

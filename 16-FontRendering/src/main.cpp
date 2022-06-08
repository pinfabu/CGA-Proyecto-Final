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
//Vida personaje
int vida = 3;
int balas = 0;
std::string cadena = "";
std::string cadena1 = "";
std::string cadena2 = "Recarga";
std::string cadena3 = "";
int screenWidth;
int screenHeight;
bool recarga = false;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

GLFWwindow* window;
//Desaparecer modelo
bool DisappearModelCubreBocas1 = true;
bool DisappearModelCubreBocas2 = true;
bool DisappearModelCubreBocas3 = true;
bool DisappearModelCubreBocas4 = true;
bool DisappearModelCubreBocas5 = true;
bool DisappearModelCubreBocas6 = true;
bool DisappearModelCubreBocas7 = true;
bool DisappearModelCubreBocas8 = true;

bool DisappearModelCovid1 = true;
bool DisappearModelCovid2 = true;
bool DisappearModelCovid3 = true;
bool DisappearModelCovid4 = true;
bool DisappearModelCovid5 = false;

bool toqueCovid = true;
bool toqueCovid2 = true;
bool toqueCovid3 = true;
bool toqueCovid4 = true;
bool toqueCovid5 = true;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para las particulas de fountain
Shader shaderParticlesFountain;
//Shader para las particulas de fuego
Shader shaderParticlesFire;
//Shader para visualizar el buffer de profundidad
Shader shaderViewDepth;
//Shader para dibujar el buffer de profunidad
Shader shaderDepth;

//std::shared_ptr<FirstPersonCamera> cameraFP(new FirstPersonCamera());
std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 5;
bool banderaDisparo = true;

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
Model OsmosisModelAnimate;
// Covid
Model CovidModelAnimate;
Model CovidModelAnimate2;
Model CovidModelAnimate3;
Model CovidModelAnimate4;
Model CovidModelAnimate5;
//Cubrebocas
Model CubreBocasModelAnimate;
Model CubreBocasModelAnimate2;
Model CubreBocasModelAnimate3;
Model CubreBocasModelAnimate4;
Model CubreBocasModelAnimate5;
Model CubreBocasModelAnimate6;
Model CubreBocasModelAnimate7;
Model CubreBocasModelAnimate8;
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

GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID,
textureLandingPadID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID,
textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticleFountainID, textureParticleFireID, texId;
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
glm::mat4 modelMatrixOsmosis = glm::mat4(1.0f);
glm::mat4 modelMatrixCovid = glm::mat4(1.0f);
glm::mat4 modelMatrixCovid2 = glm::mat4(1.0f);
glm::mat4 modelMatrixCovid3 = glm::mat4(1.0f);
glm::mat4 modelMatrixCovid4 = glm::mat4(1.0f);
glm::mat4 modelMatrixCovid5 = glm::mat4(1.0f);
glm::mat4 modelMatrixCubrebocas = glm::mat4(1.0f);
glm::mat4 modelMatrixCubrebocas2 = glm::mat4(1.0f);
glm::mat4 modelMatrixCubrebocas3 = glm::mat4(1.0f);
glm::mat4 modelMatrixCubrebocas4 = glm::mat4(1.0f);
glm::mat4 modelMatrixCubrebocas5 = glm::mat4(1.0f);
glm::mat4 modelMatrixCubrebocas6 = glm::mat4(1.0f);
glm::mat4 modelMatrixCubrebocas7 = glm::mat4(1.0f);
glm::mat4 modelMatrixCubrebocas8 = glm::mat4(1.0f);
glm::mat4 modelMatrixMapTest = glm::mat4(1.0f);
//glm::mat4 modelMatrixMapRef = glm::mat4(1.0f);
glm::mat4 modelMatrixMap = glm::mat4(1.0f);
glm::mat4 modelMatrixFountain = glm::mat4(1.0f);

int animationIndex = 1;
int modelSelected = 0;
bool enableCountSelected = true;

// Variables to animations keyframes
bool saveFrame = false, availableSave = true;
std::ofstream myfile;
std::string fileName = "";
bool record = false;

// Lamps positions
std::vector<glm::vec3> lamp1Position = { glm::vec3(-7.03, 0, -19.14), glm::vec3(
		24.41, 0, -34.57), glm::vec3(-10.15, 0, -54.10) };
std::vector<float> lamp1Orientation = { -17.0, -82.67, 23.70 };
std::vector<glm::vec3> lamp2Position = { glm::vec3(-36.52, 0, -23.24),
		glm::vec3(-52.73, 0, -3.90) };
std::vector<float> lamp2Orientation = { 21.37 + 90, -65.0 + 90 };

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = {
		{ "fountain", glm::vec3(5.0, 0.0, -40.0) }, { "fire", glm::vec3(0.0,
				0.0, 7.0) } };

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

// Definition for the particle system fire
GLuint initVelFire, startTimeFire;
GLuint VAOParticlesFire;
GLuint nParticlesFire = 2000;
GLuint posBuf[2], velBuf[2], age[2];
GLuint particleArray[2];
GLuint feedback[2];
GLuint drawBuf = 1;
float particleSize = 0.5, particleLifetime = 3.0;
double currTimeParticlesAnimationFire, lastTimeParticlesAnimationFire;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 */

 // OpenAL Defines
#define NUM_BUFFERS 3
#define NUM_SOURCES 3
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };
// Source 2
ALfloat source2Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };
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
std::vector<bool> sourcesPlay = { true, true, true };

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);

void initParticleBuffers() {
	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL,
		GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat* data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f,
			((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(0.6f, 0.8f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initParticleBuffersFire() {
	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesFire * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesFire);
	float rate = particleLifetime / nParticlesFire;
	for (unsigned int i = 0; i < nParticlesFire; i++) {
		int index = i - nParticlesFire;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine{ };
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

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
	shaderMulLighting.initialize(
		"../Shaders/iluminacion_textura_animation_shadow.vs",
		"../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs",
		"../Shaders/terrain_shadow.fs");
	shaderParticlesFountain.initialize("../Shaders/particlesFountain.vs",
		"../Shaders/particlesFountain.fs");
	shaderParticlesFire.initialize("../Shaders/particlesFire.vs",
		"../Shaders/particlesFire.fs", { "Position", "Velocity", "Age" });
	shaderViewDepth.initialize("../Shaders/texturizado.vs",
		"../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs",
		"../Shaders/shadow_mapping_depth.fs");

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
	OsmosisModelAnimate.loadModel("../models/Osmosis/OsmosisDisparo.fbx");
	OsmosisModelAnimate.setShader(&shaderMulLighting);

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

	//Covid
	CovidModelAnimate.loadModel("../models/Covid/Covid.fbx");
	CovidModelAnimate.setShader(&shaderMulLighting);
	CovidModelAnimate2.loadModel("../models/Covid/Covid.fbx");
	CovidModelAnimate2.setShader(&shaderMulLighting);
	CovidModelAnimate3.loadModel("../models/Covid/Covid.fbx");
	CovidModelAnimate3.setShader(&shaderMulLighting);
	CovidModelAnimate4.loadModel("../models/Covid/Covid.fbx");
	CovidModelAnimate4.setShader(&shaderMulLighting);
	CovidModelAnimate5.loadModel("../models/Covid/Covid.fbx");
	CovidModelAnimate5.setShader(&shaderMulLighting);

	//Cubrebocas
	CubreBocasModelAnimate.loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
	CubreBocasModelAnimate.setShader(&shaderMulLighting);
	CubreBocasModelAnimate2.loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
	CubreBocasModelAnimate2.setShader(&shaderMulLighting);
	CubreBocasModelAnimate3.loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
	CubreBocasModelAnimate3.setShader(&shaderMulLighting);
	CubreBocasModelAnimate4.loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
	CubreBocasModelAnimate4.setShader(&shaderMulLighting);
	CubreBocasModelAnimate5.loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
	CubreBocasModelAnimate5.setShader(&shaderMulLighting);
	CubreBocasModelAnimate6.loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
	CubreBocasModelAnimate6.setShader(&shaderMulLighting);
	CubreBocasModelAnimate7.loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
	CubreBocasModelAnimate7.setShader(&shaderMulLighting);
	CubreBocasModelAnimate8.loadModel("../models/Cubrebocas/CubrebocasAnim.fbx");
	CubreBocasModelAnimate8.setShader(&shaderMulLighting);

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
	Texture textureHighway("../Textures/highway.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureHighway.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureHighway.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureHighwayID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureHighwayID);
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
	textureHighway.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureLandingPad("../Textures/landingPad.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureLandingPad.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureLandingPad.convertToData(bitmap, imageWidth, imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureLandingPadID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureLandingPadID);
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
	textureLandingPad.freeImage(bitmap);

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

	Texture textureParticlesFountain("../Textures/bluewater.png");
	bitmap = textureParticlesFountain.loadImage();
	data = textureParticlesFountain.convertToData(bitmap, imageWidth,
		imageHeight);
	glGenTextures(1, &textureParticleFountainID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
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
	textureParticlesFountain.freeImage(bitmap);

	Texture textureParticleFire("../Textures/fire.png");
	bitmap = textureParticleFire.loadImage();
	data = textureParticleFire.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFireID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
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
	textureParticleFire.freeImage(bitmap);

	std::uniform_real_distribution<float> distr01 =
		std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesFire * 2;
	std::vector<GLfloat> randData(size);
	for (int i = 0; i < randData.size(); i++) {
		randData[i] = distr01(generator);
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT,
		randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesFire.setInt("Pass", 1);
	shaderParticlesFire.setInt("ParticleTex", 0);
	shaderParticlesFire.setInt("RandomTex", 1);
	shaderParticlesFire.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesFire.setFloat("ParticleSize", particleSize);
	shaderParticlesFire.setVectorFloat3("Accel",
		glm::value_ptr(glm::vec3(0.0f, 0.1f, 0.0f)));
	shaderParticlesFire.setVectorFloat3("Emitter",
		glm::value_ptr(glm::vec3(0.0f)));

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
	shaderParticlesFire.setMatrix3("EmitterBasis", 1, false,
		glm::value_ptr(basis));

	/*******************************************
	 * Inicializacion de los buffers de la fuente
	 *******************************************/
	initParticleBuffers();

	/*******************************************
	 * Inicializacion de los buffers del fuego
	 *******************************************/
	initParticleBuffersFire();

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
	buffer[0] = alutCreateBufferFromFile("../sounds/fountain.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/fire.wav");
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
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 3.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 3.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 2000);

	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 0.3f);
	alSourcefv(source[2], AL_POSITION, source2Pos);
	alSourcefv(source[2], AL_VELOCITY, source2Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 500);

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
	shaderParticlesFountain.destroy();
	shaderParticlesFire.destroy();

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
	OsmosisModelAnimate.destroy();
	CovidModelAnimate.destroy();
	CovidModelAnimate2.destroy();
	CovidModelAnimate3.destroy();
	CovidModelAnimate4.destroy();
	CovidModelAnimate5.destroy();
	CubreBocasModelAnimate.destroy();
	CubreBocasModelAnimate2.destroy();
	CubreBocasModelAnimate3.destroy();
	CubreBocasModelAnimate4.destroy();
	CubreBocasModelAnimate5.destroy();
	CubreBocasModelAnimate6.destroy();
	CubreBocasModelAnimate7.destroy();
	CubreBocasModelAnimate8.destroy();
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
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleFountainID);
	glDeleteTextures(1, &textureParticleFireID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Remove the buffer of the fountain particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVel);
	glDeleteBuffers(1, &startTime);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticles);

	// Remove the buffer of the fire particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBuf);
	glDeleteBuffers(2, velBuf);
	glDeleteBuffers(2, age);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedback);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesFire);
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
	camera->setDistanceFromTarget(distanceFromTarget);
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

		if (axes[5] > 0) {
			if (banderaDisparo) {
				//aqui lo del disparo
				std::cout << "Disparo" << std::endl;
				if (balas > 0) {
					balas -= 1;
					banderaDisparo = false;
					recarga = true;
				}
			}
			animationIndex = 1;
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
		if (buttons[2] == GLFW_PRESS) {
			std::cout << "Se presiona X" << std::endl;
			std::cout << "Recargas" << std::endl;
			if (balas != 0) {
				banderaDisparo = true;
				recarga = false;
			}
		}
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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (banderaDisparo) {
			//aqui lo del disparo
			std::cout << "Disparo" << std::endl;
			if (balas > 0) {
				balas -= 1;
				banderaDisparo = false;
				recarga = true;
			}
		}
		animationIndex = 1;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		std::cout << "Recargas" << std::endl;
		if (balas != 0) {
			banderaDisparo = true;
			recarga = false;
		}
	}

	//Para que no pueda mover la camara el usuario
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);
	offsetX = 0;
	offsetY = 0;

	// Seleccionar modelo
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		enableCountSelected = false;
		modelSelected++;
		if (modelSelected > 2)
			modelSelected = 0;
		if (modelSelected == 1)

			if (modelSelected == 2)

				std::cout << "modelSelected:" << modelSelected << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		enableCountSelected = true;
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

	float advanceCountCovid1 = 0.0;
	int stateCovid1 = 0;
	int numberAdvanceCovid1 = 0;
	int maxAdvanceCovid1 = 0.0;

	float advanceCountCovid2 = 0.0;
	int stateCovid2 = 0;
	int numberAdvanceCovid2 = 0;
	int maxAdvanceCovid2 = 0.0;

	float advanceCountCovid3 = 0.0;
	int stateCovid3 = 0;
	int numberAdvanceCovid3 = 0;
	int maxAdvanceCovid3 = 0.0;

	float advanceCountCovid4 = 0.0;
	int stateCovid4 = 0;
	int numberAdvanceCovid4 = 0;
	int maxAdvanceCovid4 = 0.0;

	float advanceCountCovid5 = 0.0;
	int stateCovid5 = 0;
	int numberAdvanceCovid5 = 0;
	int maxAdvanceCovid5 = 0.0;

	/*modelMatrixMayow = glm::translate(modelMatrixMayow,
			glm::vec3(13.0f, 0.05f, -5.0f));
	modelMatrixMayow = glm::rotate(modelMatrixMayow, glm::radians(-180.0f),
			glm::vec3(0, 1, 0));*/

	//Ubicación Osmosis
	modelMatrixOsmosis = glm::translate(modelMatrixOsmosis,
		glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrixOsmosis = glm::rotate(modelMatrixOsmosis, glm::radians(180.0f),
		glm::vec3(0, 1, 0));

	//Covid
	modelMatrixCovid = glm::translate(modelMatrixCovid, glm::vec3(-69.0f, 0.1684f, 35.0f));//Covid esquina inferior izquierda
	modelMatrixCovid2 = glm::translate(modelMatrixCovid2, glm::vec3(-69.0f, 0.1684f, -32.0f));//Covid esquina superior izquierda
	modelMatrixCovid3 = glm::translate(modelMatrixCovid3, glm::vec3(69.0f, 0.1684f, 35.0f));//Covid esquina inferior derecha
	modelMatrixCovid4 = glm::translate(modelMatrixCovid4, glm::vec3(69.0f, 0.1684f, -32.0f));//Covid esquina superior derecha
	modelMatrixCovid5 = glm::translate(modelMatrixCovid5, glm::vec3(-30.0f, 0.1684f, -15.0f));//Covid enmedio
	//Cubrebocas
	modelMatrixCubrebocas = glm::translate(modelMatrixCubrebocas, glm::vec3(0.0f, 0.0f, -32.0f));//Enmedio arriba
	modelMatrixCubrebocas2 = glm::translate(modelMatrixCubrebocas2, glm::vec3(0.0f, 0.0f, 35.0f));//Enmedio abajo
	modelMatrixCubrebocas3 = glm::translate(modelMatrixCubrebocas3, glm::vec3(-40.0f, 0.0f, 2.0f));//Segundo lado izquierdo 
	modelMatrixCubrebocas4 = glm::translate(modelMatrixCubrebocas4, glm::vec3(40.0f, 0.0f, 2.0f));//Segundo lado derecho 
	modelMatrixCubrebocas5 = glm::translate(modelMatrixCubrebocas5, glm::vec3(-69.0f, 0.0f, 1.0f));//Primero lado izquierdo 
	modelMatrixCubrebocas6 = glm::translate(modelMatrixCubrebocas6, glm::vec3(69.0f, 0.0f, 1.0f));//Primero lado derecho 
	modelMatrixCubrebocas7 = glm::translate(modelMatrixCubrebocas7, glm::vec3(12.0f, 0.0f, 1.0f));//Enmedio
	modelMatrixCubrebocas8 = glm::translate(modelMatrixCubrebocas8, glm::vec3(0.0f, 0.0f, 18.5f));//Segundo enmedio abajo

	modelMatrixFountain = glm::translate(modelMatrixFountain,
		glm::vec3(5.0, 0.0, -40.0));
	modelMatrixFountain[3][1] = terrain.getHeightTerrain(
		modelMatrixFountain[3][0], modelMatrixFountain[3][2]) + 0.2;
	modelMatrixFountain = glm::scale(modelMatrixFountain,
		glm::vec3(10.0f, 10.0f, 10.0f));

	//Map
	//modelMatrixMapSquareFL = glm::translate(modelMatrixMapSquareFL,
	//	glm::vec3(-15.1655f, 16.93653f, 4.13053f));

	lastTime = TimeManager::Instance().GetTime();

	// Time for the particles animation
	currTimeParticlesAnimation = lastTime;
	lastTimeParticlesAnimation = lastTime;

	currTimeParticlesAnimationFire = lastTime;
	lastTimeParticlesAnimationFire = lastTime;

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

		if (modelSelected == 0) {
			axis = glm::axis(glm::quat_cast(modelMatrixOsmosis));
			angleTarget = glm::angle(glm::quat_cast(modelMatrixOsmosis));
			target = glm::vec3 (modelMatrixOsmosis[3].x, modelMatrixOsmosis[3].y + 4.5, modelMatrixOsmosis[3].z);
		}
		else {
			/*axis = glm::axis(glm::quat_cast(modelMatrixMayow));
			angleTarget = glm::angle(glm::quat_cast(modelMatrixMayow));
			target = modelMatrixMayow[3];*/
		}

		if (std::isnan(angleTarget))
			angleTarget = 0.0;
		if (axis.y < 0)
			angleTarget = -angleTarget;
		if (modelSelected == 1)
			angleTarget -= glm::radians(90.0f);
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
		// Settea la matriz de vista y projection al shader para el fountain
		shaderParticlesFountain.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderParticlesFountain.setMatrix4("view", 1, false,
			glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader para el fuego
		shaderParticlesFire.setInt("Pass", 2);
		shaderParticlesFire.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderParticlesFire.setMatrix4("view", 1, false, glm::value_ptr(view));

		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor",
			glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderTerrain.setVectorFloat3("fogColor",
			glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderSkybox.setVectorFloat3("fogColor",
			glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));

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

		//Collider Covid
		AbstractModel::SBB CovidCollider;
		glm::mat4 modelMatrixColliderCovid = glm::mat4(modelMatrixCovid);
		if (DisappearModelCovid1) {
			modelMatrixColliderCovid = glm::scale(modelMatrixColliderCovid,
				glm::vec3(0.001, 0.001, 0.001));
			modelMatrixColliderCovid = glm::translate(modelMatrixColliderCovid,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate.getSbb().c.x,
					CovidModelAnimate.getSbb().c.y + 31 * 100,
					CovidModelAnimate.getSbb().c.z));
			CovidCollider.c = glm::vec3(modelMatrixColliderCovid[3]);
			CovidCollider.ratio = CovidModelAnimate.getSbb().ratio * 0.03;
		}
		else {
			modelMatrixColliderCovid = glm::scale(modelMatrixColliderCovid,
				glm::vec3(0.000, 0.000, 0.000));
			modelMatrixColliderCovid = glm::translate(modelMatrixColliderCovid,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate.getSbb().c.x,
					CovidModelAnimate.getSbb().c.y + 31 * 100,
					CovidModelAnimate.getSbb().c.z));
			CovidCollider.c = glm::vec3(modelMatrixColliderCovid[3]);
			CovidCollider.ratio = CovidModelAnimate.getSbb().ratio * 0.00;
		}
		addOrUpdateColliders(collidersSBB, "Covid", CovidCollider, modelMatrixCovid);
		

		//Covid 2
		AbstractModel::SBB CovidCollider2;
		glm::mat4 modelMatrixColliderCovid2 = glm::mat4(modelMatrixCovid2);
		if (DisappearModelCovid2) {
			modelMatrixColliderCovid2 = glm::scale(modelMatrixColliderCovid2,
				glm::vec3(0.001, 0.001, 0.001));
			modelMatrixColliderCovid2 = glm::translate(modelMatrixColliderCovid2,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate2.getSbb().c.x,
					CovidModelAnimate2.getSbb().c.y + 31 * 100,
					CovidModelAnimate2.getSbb().c.z));
			CovidCollider2.c = glm::vec3(modelMatrixColliderCovid2[3]);
			CovidCollider2.ratio = CovidModelAnimate2.getSbb().ratio * 0.03;
		}
		else {
			modelMatrixColliderCovid2 = glm::scale(modelMatrixColliderCovid2,
				glm::vec3(0.000, 0.000, 0.000));
			modelMatrixColliderCovid2 = glm::translate(modelMatrixColliderCovid2,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate2.getSbb().c.x,
					CovidModelAnimate2.getSbb().c.y + 31 * 100,
					CovidModelAnimate2.getSbb().c.z));
			CovidCollider2.c = glm::vec3(modelMatrixColliderCovid2[3]);
			CovidCollider2.ratio = CovidModelAnimate2.getSbb().ratio * 0.00;
		}
		addOrUpdateColliders(collidersSBB, "Covid2", CovidCollider2, modelMatrixCovid2);

		//Covid 3
		AbstractModel::SBB CovidCollider3;
		glm::mat4 modelMatrixColliderCovid3 = glm::mat4(modelMatrixCovid3);
		if (DisappearModelCovid3) {
			modelMatrixColliderCovid3 = glm::scale(modelMatrixColliderCovid3,
				glm::vec3(0.001, 0.001, 0.001));
			modelMatrixColliderCovid3 = glm::translate(modelMatrixColliderCovid3,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate3.getSbb().c.x,
					CovidModelAnimate3.getSbb().c.y + 31 * 100,
					CovidModelAnimate3.getSbb().c.z));
			CovidCollider3.c = glm::vec3(modelMatrixColliderCovid3[3]);
			CovidCollider3.ratio = CovidModelAnimate3.getSbb().ratio * 0.03;
		}
		else {
			modelMatrixColliderCovid3 = glm::scale(modelMatrixColliderCovid3,
				glm::vec3(0.000, 0.000, 0.000));
			modelMatrixColliderCovid3 = glm::translate(modelMatrixColliderCovid3,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate3.getSbb().c.x,
					CovidModelAnimate3.getSbb().c.y + 31 * 100,
					CovidModelAnimate3.getSbb().c.z));
			CovidCollider3.c = glm::vec3(modelMatrixColliderCovid3[3]);
			CovidCollider3.ratio = CovidModelAnimate3.getSbb().ratio * 0.00;
		}
		addOrUpdateColliders(collidersSBB, "Covid3", CovidCollider3, modelMatrixCovid3);

		//Covid 4
		AbstractModel::SBB CovidCollider4;
		glm::mat4 modelMatrixColliderCovid4 = glm::mat4(modelMatrixCovid4);
		if (DisappearModelCovid4) {
			modelMatrixColliderCovid4 = glm::scale(modelMatrixColliderCovid4,
				glm::vec3(0.001, 0.001, 0.001));
			modelMatrixColliderCovid4 = glm::translate(modelMatrixColliderCovid4,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate4.getSbb().c.x,
					CovidModelAnimate4.getSbb().c.y + 23 * 100,
					CovidModelAnimate4.getSbb().c.z));
			CovidCollider4.c = glm::vec3(modelMatrixColliderCovid4[3]);
			CovidCollider4.ratio = CovidModelAnimate4.getSbb().ratio * 0.03;
		}
		else {
			modelMatrixColliderCovid4 = glm::scale(modelMatrixColliderCovid4,
				glm::vec3(0.000, 0.000, 0.000));
			modelMatrixColliderCovid4 = glm::translate(modelMatrixColliderCovid4,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate4.getSbb().c.x,
					CovidModelAnimate4.getSbb().c.y + 31 * 100,
					CovidModelAnimate4.getSbb().c.z));
			CovidCollider4.c = glm::vec3(modelMatrixColliderCovid4[3]);
			CovidCollider4.ratio = CovidModelAnimate4.getSbb().ratio * 0.00;
		}
		addOrUpdateColliders(collidersSBB, "Covid4", CovidCollider4, modelMatrixCovid4);

		//Collider Covid5
		AbstractModel::SBB CovidCollider5;
		glm::mat4 modelMatrixColliderCovid5 = glm::mat4(modelMatrixCovid5);
		if (DisappearModelCovid5) {
			modelMatrixColliderCovid5 = glm::scale(modelMatrixColliderCovid5,
				glm::vec3(0.001, 0.001, 0.001));
			modelMatrixColliderCovid5 = glm::translate(modelMatrixColliderCovid5,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate5.getSbb().c.x,
					CovidModelAnimate5.getSbb().c.y + 31 * 100,
					CovidModelAnimate.getSbb().c.z));
			CovidCollider5.c = glm::vec3(modelMatrixColliderCovid5[3]);
			CovidCollider5.ratio = CovidModelAnimate5.getSbb().ratio * 0.03;
		}
		else {
			modelMatrixColliderCovid5 = glm::scale(modelMatrixColliderCovid5,
				glm::vec3(0.000, 0.000, 0.000));
			modelMatrixColliderCovid5 = glm::translate(modelMatrixColliderCovid5,
				//CovidModelAnimate.getSbb().c
				glm::vec3(CovidModelAnimate5.getSbb().c.x,
					CovidModelAnimate5.getSbb().c.y + 31 * 100,
					CovidModelAnimate5.getSbb().c.z));
			CovidCollider5.c = glm::vec3(modelMatrixColliderCovid5[3]);
			CovidCollider5.ratio = CovidModelAnimate5.getSbb().ratio * 0.00;
		}
		addOrUpdateColliders(collidersSBB, "Covid5", CovidCollider5, modelMatrixCovid5);

		//Máquina covid esquina superior izquierda
		switch (stateCovid1)
		{
		case 0:
			if (numberAdvanceCovid1 == 0)//Derecha
			{
				maxAdvanceCovid1 = 24.0f;
			}
			else if (numberAdvanceCovid1 == 1)//Mov Abajo
			{
				maxAdvanceCovid1 = 17.0f;
			}
			else if (numberAdvanceCovid1 == 2)//Mov Izq
			{
				maxAdvanceCovid1 = 7.0f;
			}
			else if (numberAdvanceCovid1 == 3)//Mov Abajo2
			{
				maxAdvanceCovid1 = 13.0f;
			}
			else if (numberAdvanceCovid1 == 4)//Mov Izq2
			{
				maxAdvanceCovid1 = 17.0f;
			}
			else if (numberAdvanceCovid1 == 5)//Mov Arriba
			{
				maxAdvanceCovid1 = 30.0f;
			}

			stateCovid1 = 1;
			break;
		case 1:
			if (numberAdvanceCovid1 == 0) {
				modelMatrixCovid2 = glm::translate(modelMatrixCovid2, glm::vec3(0.08f, 0.0f, 0.0f));//Mov Derecha
				advanceCountCovid1 += 0.08;
			}

			else if (numberAdvanceCovid1 == 1) {
				modelMatrixCovid2 = glm::translate(modelMatrixCovid2, glm::vec3(0.0f, 0.0f, 0.08f));//Mov Abajo
				advanceCountCovid1 += 0.08;
			}

			else if (numberAdvanceCovid1 == 2) {
				modelMatrixCovid2 = glm::translate(modelMatrixCovid2, glm::vec3(-0.08f, 0.0f, 0.0f));//Mov Izq
				advanceCountCovid1 += 0.08;
			}

			else if (numberAdvanceCovid1 == 3) {
				modelMatrixCovid2 = glm::translate(modelMatrixCovid2, glm::vec3(0.0f, 0.0f, 0.08f));//Mov Abajo2
				advanceCountCovid1 += 0.08;
			}
			else if (numberAdvanceCovid1 == 4) {
				modelMatrixCovid2 = glm::translate(modelMatrixCovid2, glm::vec3(-0.08f, 0.0f, 0.0f));//Mov Izq2
				advanceCountCovid1 += 0.08;
			}
			else if (numberAdvanceCovid1 == 5) {
				modelMatrixCovid2 = glm::translate(modelMatrixCovid2, glm::vec3(0.0f, 0.0f, -0.08f));//Mov Arriba
				advanceCountCovid1 += 0.08;
			}

			if (advanceCountCovid1 > maxAdvanceCovid1)
			{
				advanceCountCovid1 = 0;
				numberAdvanceCovid1++;
				stateCovid1 = 0;
				if (numberAdvanceCovid1 == 6) {
					numberAdvanceCovid1 = 0;
				}
			}
			break;
		default:
			break;
		}

		////Máquina covid esquina inferior izquierda
		switch (stateCovid2)
		{
		case 0:
			if (numberAdvanceCovid2 == 0)//Arriba
			{
				maxAdvanceCovid2 = 30.0f;
			}
			else if (numberAdvanceCovid2 == 1)//Der
			{
				maxAdvanceCovid2 = 17.0f;
			}
			else if (numberAdvanceCovid2 == 2)//Abajo
			{
				maxAdvanceCovid2 = 13.0f;
			}
			else if (numberAdvanceCovid2 == 3)//Der
			{
				maxAdvanceCovid2 = 7.0f;
			}
			else if (numberAdvanceCovid2 == 4)//Abajo2
			{
				maxAdvanceCovid2 = 17.0f;
			}
			else if (numberAdvanceCovid2 == 5)//Izq
			{
				maxAdvanceCovid2 = 24.0f;
			}

			stateCovid2 = 1;
			break;
		case 1:
			if (numberAdvanceCovid2 == 0) {
				modelMatrixCovid = glm::translate(modelMatrixCovid, glm::vec3(0.0f, 0.0f, -0.08f));//Mov Arriba
				advanceCountCovid2 += 0.08;
			}

			else if (numberAdvanceCovid2 == 1) {
				modelMatrixCovid = glm::translate(modelMatrixCovid, glm::vec3(0.08f, 0.0f, 0.0f));//Mov Der
				advanceCountCovid2 += 0.08;
			}

			else if (numberAdvanceCovid2 == 2) {
				modelMatrixCovid = glm::translate(modelMatrixCovid, glm::vec3(0.0f, 0.0f, 0.08f));//Mov Abajo
				advanceCountCovid2 += 0.08;
			}

			else if (numberAdvanceCovid2 == 3) {
				modelMatrixCovid = glm::translate(modelMatrixCovid, glm::vec3(0.08f, 0.0f, 0.0f));//Mov Der
				advanceCountCovid2 += 0.08;
			}
			else if (numberAdvanceCovid2 == 4) {
				modelMatrixCovid = glm::translate(modelMatrixCovid, glm::vec3(0.0f, 0.0f, 0.08f));//Mov Abajo2
				advanceCountCovid2 += 0.08;
			}
			else if (numberAdvanceCovid2 == 5) {
				modelMatrixCovid = glm::translate(modelMatrixCovid, glm::vec3(-0.08f, 0.0f, 0.0f));//Mov Izq
				advanceCountCovid2 += 0.08;
			}

			if (advanceCountCovid2 > maxAdvanceCovid2)
			{
				advanceCountCovid2 = 0;
				numberAdvanceCovid2++;
				stateCovid2 = 0;
				if (numberAdvanceCovid2 == 6) {
					numberAdvanceCovid2 = 0;
				}
			}
			break;
		default:
			break;
		}

		//Máquina covid esquina superior derecha
		switch (stateCovid3)
		{
		case 0:
			if (numberAdvanceCovid3 == 0)//Izq
			{
				maxAdvanceCovid3 = 24.0f;
			}
			else if (numberAdvanceCovid3 == 1)//Abajo
			{
				maxAdvanceCovid3 = 17.0f;
			}
			else if (numberAdvanceCovid3 == 2)//Der
			{
				maxAdvanceCovid3 = 7.0f;
			}
			else if (numberAdvanceCovid3 == 3)//Abajo2
			{
				maxAdvanceCovid3 = 13.0f;
			}
			else if (numberAdvanceCovid3 == 4)//Der2
			{
				maxAdvanceCovid3 = 17.0f;
			}
			else if (numberAdvanceCovid3 == 5)//Arriba
			{
				maxAdvanceCovid3 = 30.0f;
			}

			stateCovid3 = 1;
			break;
		case 1:
			if (numberAdvanceCovid3 == 0) {
				modelMatrixCovid4 = glm::translate(modelMatrixCovid4, glm::vec3(-0.08f, 0.0f, 0.0f));//Mov Izq
				advanceCountCovid3 += 0.08;
			}

			else if (numberAdvanceCovid3 == 1) {
				modelMatrixCovid4 = glm::translate(modelMatrixCovid4, glm::vec3(0.0f, 0.0f, 0.08f));//Mov Abajo
				advanceCountCovid3 += 0.08;
			}

			else if (numberAdvanceCovid3 == 2) {
				modelMatrixCovid4 = glm::translate(modelMatrixCovid4, glm::vec3(0.08f, 0.0f, 0.0f));//Mov Der
				advanceCountCovid3 += 0.08;
			}

			else if (numberAdvanceCovid3 == 3) {
				modelMatrixCovid4 = glm::translate(modelMatrixCovid4, glm::vec3(0.0f, 0.0f, 0.08f));//Mov Abajo2
				advanceCountCovid3 += 0.08;
			}
			else if (numberAdvanceCovid3 == 4) {
				modelMatrixCovid4 = glm::translate(modelMatrixCovid4, glm::vec3(0.08f, 0.0f, 0.0f));//Mov Der2
				advanceCountCovid3 += 0.08;
			}
			else if (numberAdvanceCovid3 == 5) {
				modelMatrixCovid4 = glm::translate(modelMatrixCovid4, glm::vec3(0.0f, 0.0f, -0.08f));//Mov Arriba
				advanceCountCovid3 += 0.08;
			}

			if (advanceCountCovid3 > maxAdvanceCovid3)
			{
				advanceCountCovid3 = 0;
				numberAdvanceCovid3++;
				stateCovid3 = 0;
				if (numberAdvanceCovid3 == 6) {
					numberAdvanceCovid3 = 0;
				}
			}
			break;
		default:
			break;
		}

		//Máquina covid esquina inferior derecha
		switch (stateCovid4)
		{
		case 0:
			if (numberAdvanceCovid4 == 0)//Arriba
			{
				maxAdvanceCovid4 = 30.0f;
			}
			else if (numberAdvanceCovid4 == 1)//Izq
			{
				maxAdvanceCovid4 = 17.0f;
			}
			else if (numberAdvanceCovid4 == 2)//Abajo
			{
				maxAdvanceCovid4 = 13.0f;
			}
			else if (numberAdvanceCovid4 == 3)//Izq2
			{
				maxAdvanceCovid4 = 7.0f;
			}
			else if (numberAdvanceCovid4 == 4)//Abajo2
			{
				maxAdvanceCovid4 = 17.0f;
			}
			else if (numberAdvanceCovid4 == 5)//Der
			{
				maxAdvanceCovid4 = 24.0f;
			}

			stateCovid4 = 1;
			break;
		case 1:
			if (numberAdvanceCovid4 == 0) {
				modelMatrixCovid3 = glm::translate(modelMatrixCovid3, glm::vec3(0.0f, 0.0f, -0.08f));//Mov Arriba
				advanceCountCovid4 += 0.08;
			}

			else if (numberAdvanceCovid4 == 1) {
				modelMatrixCovid3 = glm::translate(modelMatrixCovid3, glm::vec3(-0.08f, 0.0f, 0.0f));//Mov Izq
				advanceCountCovid4 += 0.08;
			}

			else if (numberAdvanceCovid4 == 2) {
				modelMatrixCovid3 = glm::translate(modelMatrixCovid3, glm::vec3(0.0f, 0.0f, 0.08f));//Mov Abajo
				advanceCountCovid4 += 0.08;
			}

			else if (numberAdvanceCovid4 == 3) {
				modelMatrixCovid3 = glm::translate(modelMatrixCovid3, glm::vec3(-0.08f, 0.0f, 0.0f));//Mov Izq2
				advanceCountCovid4 += 0.08;
			}
			else if (numberAdvanceCovid4 == 4) {
				modelMatrixCovid3 = glm::translate(modelMatrixCovid3, glm::vec3(0.0f, 0.0f, 0.08f));//Mov Abajo2
				advanceCountCovid4 += 0.08;
			}
			else if (numberAdvanceCovid4 == 5) {
				modelMatrixCovid3 = glm::translate(modelMatrixCovid3, glm::vec3(0.08f, 0.0f, 0.0f));//Mov Der
				advanceCountCovid4 += 0.08;
			}

			if (advanceCountCovid4 > maxAdvanceCovid4)
			{
				advanceCountCovid4 = 0;
				numberAdvanceCovid4++;
				stateCovid4 = 0;
				if (numberAdvanceCovid4 == 6) {
					numberAdvanceCovid4 = 0;
				}
			}
			break;
		default:
			break;
		}

		//Máquina covid en medio
		switch (stateCovid5)
		{
		case 0:
			if (numberAdvanceCovid5 == 0)//Mov Derecha
			{
				maxAdvanceCovid5 = 60.0f;
			}
			else if (numberAdvanceCovid5 == 1)//Mov Abajo
			{
				maxAdvanceCovid5 = 34.0f;
			}
			else if (numberAdvanceCovid5 == 2)//Mov Izq
			{
				maxAdvanceCovid5 = 60.0f;
			}
			else if (numberAdvanceCovid5 == 3)//Mov Arriba
			{
				maxAdvanceCovid5 = 34.0f;
			}
			

			stateCovid5 = 1;
			break;
		case 1:
			if (numberAdvanceCovid5 == 0) {
				modelMatrixCovid5 = glm::translate(modelMatrixCovid5, glm::vec3(0.08f, 0.0f, 0.0f));//Mov Derecha
				advanceCountCovid5 += 0.08;
			}

			else if (numberAdvanceCovid5 == 1) {
				modelMatrixCovid5 = glm::translate(modelMatrixCovid5, glm::vec3(0.0f, 0.0f, 0.08f));//Mov Abajo
				advanceCountCovid5 += 0.08;
			}

			else if (numberAdvanceCovid5 == 2) {
				modelMatrixCovid5 = glm::translate(modelMatrixCovid5, glm::vec3(-0.08f, 0.0f, 0.0f));//Mov Izq
				advanceCountCovid5 += 0.08;
			}

			else if (numberAdvanceCovid5 == 3) {
				modelMatrixCovid5 = glm::translate(modelMatrixCovid5, glm::vec3(0.0f, 0.0f, -0.08f));//Mov Arriba
				advanceCountCovid5 += 0.08;
			}
			

			if (advanceCountCovid5 > maxAdvanceCovid5)
			{
				advanceCountCovid5 = 0;
				numberAdvanceCovid5++;
				stateCovid5 = 0;
				if (numberAdvanceCovid5 == 4) {
					numberAdvanceCovid5 = 0;
				}
			}
			break;
		default:
			break;
		}

		//Collider Cubrebocas
		AbstractModel::OBB CubreCollider;
		if (DisappearModelCubreBocas1) {
			glm::mat4 modelmatrixColliderCubre = glm::mat4(modelMatrixCubrebocas);
			modelmatrixColliderCubre = glm::rotate(modelmatrixColliderCubre, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider.u = glm::quat_cast(modelmatrixColliderCubre);
			modelmatrixColliderCubre = glm::scale(modelmatrixColliderCubre, glm::vec3(0.1, 0.1, 0.1));
			modelmatrixColliderCubre = glm::translate(modelmatrixColliderCubre,
				glm::vec3(CubreBocasModelAnimate.getObb().c.x + 2.5,
					CubreBocasModelAnimate.getObb().c.y + 25,
					CubreBocasModelAnimate.getObb().c.z));
			CubreCollider.e = CubreBocasModelAnimate.getObb().e * glm::vec3(2.25, 2.0, 2.5);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider.c = glm::vec3(modelmatrixColliderCubre[3]);
		}
		else {
			glm::mat4 modelmatrixColliderCubre = glm::mat4(modelMatrixCubrebocas);
			modelmatrixColliderCubre = glm::rotate(modelmatrixColliderCubre, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider.u = glm::quat_cast(modelmatrixColliderCubre);
			modelmatrixColliderCubre = glm::scale(modelmatrixColliderCubre, glm::vec3(0.0, 0.0, 0.0));
			modelmatrixColliderCubre = glm::translate(modelmatrixColliderCubre,
				glm::vec3(CubreBocasModelAnimate.getObb().c.x + 2.5,
					CubreBocasModelAnimate.getObb().c.y + 25,
					CubreBocasModelAnimate.getObb().c.z));
			CubreCollider.e = CubreBocasModelAnimate.getObb().e * glm::vec3(0.0, 0.0, 0.0);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider.c = glm::vec3(modelmatrixColliderCubre[3]);

		}
		addOrUpdateColliders(collidersOBB, "Cubre", CubreCollider, modelMatrixCubrebocas);

		//Collider Cubrebocas2
		AbstractModel::OBB CubreCollider2;
		if (DisappearModelCubreBocas2) {
			glm::mat4 modelmatrixColliderCubre2 = glm::mat4(modelMatrixCubrebocas2);
			modelmatrixColliderCubre2 = glm::rotate(modelmatrixColliderCubre2, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider2.u = glm::quat_cast(modelmatrixColliderCubre2);
			modelmatrixColliderCubre2 = glm::scale(modelmatrixColliderCubre2, glm::vec3(0.1, 0.1, 0.1));
			modelmatrixColliderCubre2 = glm::translate(modelmatrixColliderCubre2,
				glm::vec3(CubreBocasModelAnimate2.getObb().c.x + 2.5,
					CubreBocasModelAnimate2.getObb().c.y + 25,
					CubreBocasModelAnimate2.getObb().c.z));
			CubreCollider2.e = CubreBocasModelAnimate2.getObb().e * glm::vec3(2.25, 2.0, 2.5);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider2.c = glm::vec3(modelmatrixColliderCubre2[3]);
		}
		else {
			glm::mat4 modelmatrixColliderCubre2 = glm::mat4(modelMatrixCubrebocas2);
			modelmatrixColliderCubre2 = glm::rotate(modelmatrixColliderCubre2, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider2.u = glm::quat_cast(modelmatrixColliderCubre2);
			modelmatrixColliderCubre2 = glm::scale(modelmatrixColliderCubre2, glm::vec3(0.0, 0.0, 0.0));
			modelmatrixColliderCubre2 = glm::translate(modelmatrixColliderCubre2,
				glm::vec3(CubreBocasModelAnimate2.getObb().c.x + 2.5,
					CubreBocasModelAnimate2.getObb().c.y + 25,
					CubreBocasModelAnimate2.getObb().c.z));
			CubreCollider2.e = CubreBocasModelAnimate2.getObb().e * glm::vec3(0.0, 0.0, 0.0);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider2.c = glm::vec3(modelmatrixColliderCubre2[3]);

		}
		addOrUpdateColliders(collidersOBB, "Cubre2", CubreCollider2, modelMatrixCubrebocas2);

		//Collider Cubrebocas3
		AbstractModel::OBB CubreCollider3;
		if (DisappearModelCubreBocas3) {
			glm::mat4 modelmatrixColliderCubre3 = glm::mat4(modelMatrixCubrebocas3);
			modelmatrixColliderCubre3 = glm::rotate(modelmatrixColliderCubre3, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider3.u = glm::quat_cast(modelmatrixColliderCubre3);
			modelmatrixColliderCubre3 = glm::scale(modelmatrixColliderCubre3, glm::vec3(0.1, 0.1, 0.1));
			modelmatrixColliderCubre3 = glm::translate(modelmatrixColliderCubre3,
				glm::vec3(CubreBocasModelAnimate3.getObb().c.x + 2.5,
					CubreBocasModelAnimate3.getObb().c.y + 25,
					CubreBocasModelAnimate3.getObb().c.z));
			CubreCollider3.e = CubreBocasModelAnimate3.getObb().e * glm::vec3(2.25, 2.0, 2.5);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider3.c = glm::vec3(modelmatrixColliderCubre3[3]);
		}
		else {
			glm::mat4 modelmatrixColliderCubre3 = glm::mat4(modelMatrixCubrebocas3);
			modelmatrixColliderCubre3 = glm::rotate(modelmatrixColliderCubre3, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider3.u = glm::quat_cast(modelmatrixColliderCubre3);
			modelmatrixColliderCubre3 = glm::scale(modelmatrixColliderCubre3, glm::vec3(0.0, 0.0, 0.0));
			modelmatrixColliderCubre3 = glm::translate(modelmatrixColliderCubre3,
				glm::vec3(CubreBocasModelAnimate3.getObb().c.x + 2.5,
					CubreBocasModelAnimate3.getObb().c.y + 25,
					CubreBocasModelAnimate3.getObb().c.z));
			CubreCollider3.e = CubreBocasModelAnimate3.getObb().e * glm::vec3(0.0, 0.0, 0.0);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider3.c = glm::vec3(modelmatrixColliderCubre3[3]);

		}
		addOrUpdateColliders(collidersOBB, "Cubre3", CubreCollider3, modelMatrixCubrebocas3);

		//Collider Cubrebocas4
		AbstractModel::OBB CubreCollider4;
		if (DisappearModelCubreBocas4) {
			glm::mat4 modelmatrixColliderCubre4 = glm::mat4(modelMatrixCubrebocas4);
			modelmatrixColliderCubre4 = glm::rotate(modelmatrixColliderCubre4, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider4.u = glm::quat_cast(modelmatrixColliderCubre4);
			modelmatrixColliderCubre4 = glm::scale(modelmatrixColliderCubre4, glm::vec3(0.1, 0.1, 0.1));
			modelmatrixColliderCubre4 = glm::translate(modelmatrixColliderCubre4,
				glm::vec3(CubreBocasModelAnimate4.getObb().c.x + 2.5,
					CubreBocasModelAnimate4.getObb().c.y + 25,
					CubreBocasModelAnimate4.getObb().c.z));
			CubreCollider4.e = CubreBocasModelAnimate4.getObb().e * glm::vec3(2.25, 2.0, 2.5);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider4.c = glm::vec3(modelmatrixColliderCubre4[3]);
		}
		else {
			glm::mat4 modelmatrixColliderCubre4 = glm::mat4(modelMatrixCubrebocas4);
			modelmatrixColliderCubre4 = glm::rotate(modelmatrixColliderCubre4, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider4.u = glm::quat_cast(modelmatrixColliderCubre4);
			modelmatrixColliderCubre4 = glm::scale(modelmatrixColliderCubre4, glm::vec3(0.0, 0.0, 0.0));
			modelmatrixColliderCubre4 = glm::translate(modelmatrixColliderCubre4,
				glm::vec3(CubreBocasModelAnimate4.getObb().c.x + 2.5,
					CubreBocasModelAnimate4.getObb().c.y + 25,
					CubreBocasModelAnimate4.getObb().c.z));
			CubreCollider4.e = CubreBocasModelAnimate4.getObb().e * glm::vec3(0.0, 0.0, 0.0);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider4.c = glm::vec3(modelmatrixColliderCubre4[3]);

		}
		addOrUpdateColliders(collidersOBB, "Cubre4", CubreCollider4, modelMatrixCubrebocas4);

		//Collider Cubrebocas5
		AbstractModel::OBB CubreCollider5;
		if (DisappearModelCubreBocas5) {
			glm::mat4 modelmatrixColliderCubre5 = glm::mat4(modelMatrixCubrebocas5);
			modelmatrixColliderCubre5 = glm::rotate(modelmatrixColliderCubre5, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider5.u = glm::quat_cast(modelmatrixColliderCubre5);
			modelmatrixColliderCubre5 = glm::scale(modelmatrixColliderCubre5, glm::vec3(0.1, 0.1, 0.1));
			modelmatrixColliderCubre5 = glm::translate(modelmatrixColliderCubre5,
				glm::vec3(CubreBocasModelAnimate5.getObb().c.x + 2.5,
					CubreBocasModelAnimate5.getObb().c.y + 25,
					CubreBocasModelAnimate5.getObb().c.z));
			CubreCollider5.e = CubreBocasModelAnimate5.getObb().e * glm::vec3(2.25, 2.0, 2.5);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider5.c = glm::vec3(modelmatrixColliderCubre5[3]);
		}
		else {
			glm::mat4 modelmatrixColliderCubre5 = glm::mat4(modelMatrixCubrebocas5);
			modelmatrixColliderCubre5 = glm::rotate(modelmatrixColliderCubre5, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider5.u = glm::quat_cast(modelmatrixColliderCubre5);
			modelmatrixColliderCubre5 = glm::scale(modelmatrixColliderCubre5, glm::vec3(0.0, 0.0, 0.0));
			modelmatrixColliderCubre5 = glm::translate(modelmatrixColliderCubre5,
				glm::vec3(CubreBocasModelAnimate5.getObb().c.x + 2.5,
					CubreBocasModelAnimate5.getObb().c.y + 25,
					CubreBocasModelAnimate5.getObb().c.z));
			CubreCollider5.e = CubreBocasModelAnimate5.getObb().e * glm::vec3(0.0, 0.0, 0.0);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider5.c = glm::vec3(modelmatrixColliderCubre5[3]);

		}
		addOrUpdateColliders(collidersOBB, "Cubre5", CubreCollider5, modelMatrixCubrebocas5);

		//Collider Cubrebocas6
		AbstractModel::OBB CubreCollider6;
		if (DisappearModelCubreBocas6) {
			glm::mat4 modelmatrixColliderCubre6 = glm::mat4(modelMatrixCubrebocas6);
			modelmatrixColliderCubre6 = glm::rotate(modelmatrixColliderCubre6, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider6.u = glm::quat_cast(modelmatrixColliderCubre6);
			modelmatrixColliderCubre6 = glm::scale(modelmatrixColliderCubre6, glm::vec3(0.1, 0.1, 0.1));
			modelmatrixColliderCubre6 = glm::translate(modelmatrixColliderCubre6,
				glm::vec3(CubreBocasModelAnimate6.getObb().c.x + 2.5,
					CubreBocasModelAnimate6.getObb().c.y + 25,
					CubreBocasModelAnimate6.getObb().c.z));
			CubreCollider6.e = CubreBocasModelAnimate6.getObb().e * glm::vec3(2.25, 2.0, 2.5);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider6.c = glm::vec3(modelmatrixColliderCubre6[3]);
		}
		else {
			glm::mat4 modelmatrixColliderCubre6 = glm::mat4(modelMatrixCubrebocas6);
			modelmatrixColliderCubre6 = glm::rotate(modelmatrixColliderCubre6, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider6.u = glm::quat_cast(modelmatrixColliderCubre6);
			modelmatrixColliderCubre6 = glm::scale(modelmatrixColliderCubre6, glm::vec3(0.0, 0.0, 0.0));
			modelmatrixColliderCubre6 = glm::translate(modelmatrixColliderCubre6,
				glm::vec3(CubreBocasModelAnimate6.getObb().c.x + 2.5,
					CubreBocasModelAnimate6.getObb().c.y + 25,
					CubreBocasModelAnimate6.getObb().c.z));
			CubreCollider6.e = CubreBocasModelAnimate6.getObb().e * glm::vec3(0.0, 0.0, 0.0);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider6.c = glm::vec3(modelmatrixColliderCubre6[3]);

		}
		addOrUpdateColliders(collidersOBB, "Cubre6", CubreCollider6, modelMatrixCubrebocas6);

		//Collider Cubrebocas7
		AbstractModel::OBB CubreCollider7;
		if (DisappearModelCubreBocas7) {
			glm::mat4 modelmatrixColliderCubre7 = glm::mat4(modelMatrixCubrebocas7);
			modelmatrixColliderCubre7 = glm::rotate(modelmatrixColliderCubre7, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider7.u = glm::quat_cast(modelmatrixColliderCubre7);
			modelmatrixColliderCubre7 = glm::scale(modelmatrixColliderCubre7, glm::vec3(0.1, 0.1, 0.1));
			modelmatrixColliderCubre7 = glm::translate(modelmatrixColliderCubre7,
				glm::vec3(CubreBocasModelAnimate7.getObb().c.x + 2.5,
					CubreBocasModelAnimate7.getObb().c.y + 25,
					CubreBocasModelAnimate7.getObb().c.z));
			CubreCollider7.e = CubreBocasModelAnimate7.getObb().e * glm::vec3(2.25, 2.0, 2.5);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider7.c = glm::vec3(modelmatrixColliderCubre7[3]);
		}
		else {
			glm::mat4 modelmatrixColliderCubre7 = glm::mat4(modelMatrixCubrebocas7);
			modelmatrixColliderCubre7 = glm::rotate(modelmatrixColliderCubre7, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider7.u = glm::quat_cast(modelmatrixColliderCubre7);
			modelmatrixColliderCubre7 = glm::scale(modelmatrixColliderCubre7, glm::vec3(0.0, 0.0, 0.0));
			modelmatrixColliderCubre7 = glm::translate(modelmatrixColliderCubre7,
				glm::vec3(CubreBocasModelAnimate7.getObb().c.x + 2.5,
					CubreBocasModelAnimate7.getObb().c.y + 25,
					CubreBocasModelAnimate7.getObb().c.z));
			CubreCollider7.e = CubreBocasModelAnimate7.getObb().e * glm::vec3(0.0, 0.0, 0.0);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider7.c = glm::vec3(modelmatrixColliderCubre7[3]);

		}
		addOrUpdateColliders(collidersOBB, "Cubre7", CubreCollider7, modelMatrixCubrebocas7);

		//Collider Cubrebocas8
		AbstractModel::OBB CubreCollider8;
		if (DisappearModelCubreBocas8) {
			glm::mat4 modelmatrixColliderCubre8 = glm::mat4(modelMatrixCubrebocas8);
			modelmatrixColliderCubre8 = glm::rotate(modelmatrixColliderCubre8, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider8.u = glm::quat_cast(modelmatrixColliderCubre8);
			modelmatrixColliderCubre8 = glm::scale(modelmatrixColliderCubre8, glm::vec3(0.1, 0.1, 0.1));
			modelmatrixColliderCubre8 = glm::translate(modelmatrixColliderCubre8,
				glm::vec3(CubreBocasModelAnimate8.getObb().c.x + 2.5,
					CubreBocasModelAnimate8.getObb().c.y + 25,
					CubreBocasModelAnimate8.getObb().c.z));
			CubreCollider8.e = CubreBocasModelAnimate8.getObb().e * glm::vec3(2.25, 2.0, 2.5);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider8.c = glm::vec3(modelmatrixColliderCubre8[3]);
		}
		else {
			glm::mat4 modelmatrixColliderCubre8 = glm::mat4(modelMatrixCubrebocas8);
			modelmatrixColliderCubre8 = glm::rotate(modelmatrixColliderCubre8, glm::radians(90.0f), glm::vec3(0, 1, 0));
			// Set the orientation of collider before doing the scale
			CubreCollider8.u = glm::quat_cast(modelmatrixColliderCubre8);
			modelmatrixColliderCubre8 = glm::scale(modelmatrixColliderCubre8, glm::vec3(0.0, 0.0, 0.0));
			modelmatrixColliderCubre8 = glm::translate(modelmatrixColliderCubre8,
				glm::vec3(CubreBocasModelAnimate8.getObb().c.x + 2.5,
					CubreBocasModelAnimate8.getObb().c.y + 25,
					CubreBocasModelAnimate8.getObb().c.z));
			CubreCollider8.e = CubreBocasModelAnimate8.getObb().e * glm::vec3(0.0, 0.0, 0.0);//* glm::vec3(0.1, 0.1, 0.1) *;
			CubreCollider8.c = glm::vec3(modelmatrixColliderCubre8[3]);

		}
		addOrUpdateColliders(collidersOBB, "Cubre8", CubreCollider8, modelMatrixCubrebocas8);

		//Collider Osmosis
		AbstractModel::OBB OsmosisCollider;
		glm::mat4 modelmatrixColliderOsmosis = glm::mat4(modelMatrixOsmosis);
		modelmatrixColliderOsmosis = glm::rotate(modelmatrixColliderOsmosis, glm::radians(90.0f), glm::vec3(0, 1, 0));
		// Set the orientation of collider before doing the scale
		OsmosisCollider.u = glm::quat_cast(modelmatrixColliderOsmosis);
		modelmatrixColliderOsmosis = glm::scale(modelmatrixColliderOsmosis, glm::vec3(0.1, 0.1, 0.1));
		modelmatrixColliderOsmosis = glm::translate(modelmatrixColliderOsmosis,
			glm::vec3(OsmosisModelAnimate.getObb().c.x - 12.5,
				OsmosisModelAnimate.getObb().c.y + 22,
				OsmosisModelAnimate.getObb().c.z + 2));
		OsmosisCollider.e = OsmosisModelAnimate.getObb().e * glm::vec3(0.1, 0.1, 0.1) * glm::vec3(0.5, 0.75, 0.5);
		OsmosisCollider.c = glm::vec3(modelmatrixColliderOsmosis[3]);
		addOrUpdateColliders(collidersOBB, "Osmosis", OsmosisCollider, modelMatrixOsmosis);

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

		//Box  vs Box
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
					if (it->first == "Cubre") {
						if (jt->first == "Osmosis") {
							balas += 1;
							DisappearModelCubreBocas1 = !DisappearModelCubreBocas1;
							std::cout << "Colision " << it->first << " with "
								<< jt->first << std::endl;
						}
					}
					if (it->first == "Cubre2") {
						if (jt->first == "Osmosis") {
							balas += 1;
							DisappearModelCubreBocas2 = !DisappearModelCubreBocas2;
							std::cout << "Colision " << it->first << " with "
								<< jt->first << std::endl;
						}
					}
					if (it->first == "Cubre3") {
						if (jt->first == "Osmosis") {
							balas += 1;
							DisappearModelCubreBocas3 = !DisappearModelCubreBocas3;
							std::cout << "Colision " << it->first << " with "
								<< jt->first << std::endl;
						}
					}
					if (it->first == "Cubre4") {
						if (jt->first == "Osmosis") {
							balas += 1;
							DisappearModelCubreBocas4 = !DisappearModelCubreBocas4;
							std::cout << "Colision " << it->first << " with "
								<< jt->first << std::endl;
						}
					}
					if (it->first == "Cubre5") {
						if (jt->first == "Osmosis") {
							balas += 1;
							DisappearModelCubreBocas5 = !DisappearModelCubreBocas5;
							std::cout << "Colision " << it->first << " with "
								<< jt->first << std::endl;
						}
					}
					if (it->first == "Cubre6") {
						if (jt->first == "Osmosis") {
							balas += 1;
							DisappearModelCubreBocas6 = !DisappearModelCubreBocas6;
							std::cout << "Colision " << it->first << " with "
								<< jt->first << std::endl;
						}
					}
					if (it->first == "Cubre7") {
						if (jt->first == "Osmosis") {
							balas += 1;
							DisappearModelCubreBocas7 = !DisappearModelCubreBocas7;
							std::cout << "Colision " << it->first << " with "
								<< jt->first << std::endl;
						}
					}
					if (it->first == "Cubre8") {
						if (jt->first == "Osmosis") {
							balas += 1;
							DisappearModelCubreBocas8 = !DisappearModelCubreBocas8;
							std::cout << "Colision " << it->first << " with "
								<< jt->first << std::endl;
						}
					}
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first,
				isCollision);
		}

		//Sphere vs Sphere
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
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first,
				isCollision);
		}
		//Sphere vs Box
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
					isCollision = true;
					addOrUpdateCollisionDetection(collisionDetection, jt->first,
						isCollision);
					if (it->first == "Covid") {
						if (jt->first == "Osmosis") {
							if (toqueCovid){
								vida -= 1;
								DisappearModelCovid1 = !DisappearModelCovid1;
								DisappearModelCovid5 = !DisappearModelCovid5;
								std::cout << "Colision " << it->first << " with "
									<< jt->first << std::endl;
								toqueCovid = false;
							}
						}
					}
					if (it->first == "Covid2") {
						if (jt->first == "Osmosis") {
							if (toqueCovid2) {
								vida -= 1;
								DisappearModelCovid2 = !DisappearModelCovid2;
								DisappearModelCovid5 = !DisappearModelCovid5;
								std::cout << "Colision " << it->first << " with "
									<< jt->first << std::endl;
								toqueCovid2 = false;
							}
						}
					}
					if (it->first == "Covid3") {
						if (jt->first == "Osmosis") {
							if (toqueCovid3) {
								vida -= 1;
								DisappearModelCovid3 = !DisappearModelCovid3;
								DisappearModelCovid5 = !DisappearModelCovid5;
								std::cout << "Colision " << it->first << " with "
									<< jt->first << std::endl;
								toqueCovid3 = false;
							}
						}
					}
					if (it->first == "Covid4") {
						if (jt->first == "Osmosis") {
							if (toqueCovid4) {
								vida -= 1;
								DisappearModelCovid4 = !DisappearModelCovid4;
								DisappearModelCovid5 = !DisappearModelCovid5;
								std::cout << "Colision " << it->first << " with "
									<< jt->first << std::endl;
								toqueCovid4 = false;
							}
						}
					}
					if (it->first == "Covid5") {
						if (jt->first == "Osmosis") {
							if (toqueCovid5) {
								vida -= 1;
								DisappearModelCovid5 = !DisappearModelCovid5;
								std::cout << "Colision " << it->first << " with "
									<< jt->first << std::endl;
								toqueCovid5 = false;
							}
						}
					}
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first,
				isCollision);

		}

		//Impedir que avance el modelo
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
		cadena = "Vidas: " + std::to_string(vida);
		cadena1 = " Balas: " + std::to_string(balas);
		if (vida > 0) {
			modelText->render(cadena, -.95, 0.9, 50, 0.0, 0.63, 0.16);
			modelText2->render(cadena1, -.15, 0.9, 50, 0.9, 0.0, 0.0);
			if (recarga)
				modelText3->render(cadena2, .65, 0.9, 50, 0.0, 0.9, 0.9);
			glfwSwapBuffers(window);
		}
		else {
			modelText->render("GAME OVER!", -0.55, 0.0, 160, 1.0, 0.0, 0.0);
			glfwSwapBuffers(window);
		}

		/****************************+
		 * Open AL sound data
		 */
		source0Pos[0] = modelMatrixFountain[3].x;
		source0Pos[1] = modelMatrixFountain[3].y;
		source0Pos[2] = modelMatrixFountain[3].z;
		alSourcefv(source[0], AL_POSITION, source0Pos);

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
			if (sourcesPlay[i]) {
				sourcesPlay[i] = false;
				alSourcePlay(source[i]);
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
	OsmosisModelAnimate.setShader(&shaderMulLighting);

	//Covid
	CovidModelAnimate.setShader(&shaderMulLighting);
	CovidModelAnimate2.setShader(&shaderMulLighting);
	CovidModelAnimate3.setShader(&shaderMulLighting);
	CovidModelAnimate4.setShader(&shaderMulLighting);
	CovidModelAnimate5.setShader(&shaderMulLighting);

	//Cubrebocas
	CubreBocasModelAnimate.setShader(&shaderMulLighting);
	CubreBocasModelAnimate2.setShader(&shaderMulLighting);
	CubreBocasModelAnimate3.setShader(&shaderMulLighting);
	CubreBocasModelAnimate4.setShader(&shaderMulLighting);
	CubreBocasModelAnimate5.setShader(&shaderMulLighting);
	CubreBocasModelAnimate6.setShader(&shaderMulLighting);
	CubreBocasModelAnimate7.setShader(&shaderMulLighting);
	CubreBocasModelAnimate8.setShader(&shaderMulLighting);

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

	//Osmosis
	OsmosisModelAnimate.setShader(&shaderDepth);

	//Covid
	CovidModelAnimate.setShader(&shaderDepth);
	CovidModelAnimate2.setShader(&shaderDepth);
	CovidModelAnimate3.setShader(&shaderDepth);
	CovidModelAnimate4.setShader(&shaderDepth);
	CovidModelAnimate5.setShader(&shaderDepth);

	//Cubrebocas
	CubreBocasModelAnimate.setShader(&shaderDepth);
	CubreBocasModelAnimate2.setShader(&shaderDepth);
	CubreBocasModelAnimate3.setShader(&shaderDepth);
	CubreBocasModelAnimate4.setShader(&shaderDepth);
	CubreBocasModelAnimate5.setShader(&shaderDepth);
	CubreBocasModelAnimate6.setShader(&shaderDepth);
	CubreBocasModelAnimate7.setShader(&shaderDepth);
	CubreBocasModelAnimate8.setShader(&shaderDepth);

	//Map
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
	modelMatrixBodyLamp1 = glm::translate(modelMatrixBodyLamp1,glm::vec3(69.0f, 10.0f, -13.0f));
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
	OsmosisModelAnimate.setAnimationIndex(animationIndex);
	OsmosisModelAnimate.render(modelMatrixOsmosisBody);


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
	modelMatrixCovid[3][1] = terrain.getHeightTerrain(modelMatrixCovid[3][0], modelMatrixCovid[3][2]);
	glm::mat4 modelMatrixCovidBody = glm::mat4(modelMatrixCovid);
	modelMatrixCovidBody = glm::scale(modelMatrixCovidBody, glm::vec3(0.015, 0.015, 0.015));
	CovidModelAnimate.setAnimationIndex(0);
	if (DisappearModelCovid1) {
		CovidModelAnimate.render(modelMatrixCovidBody);
	}
	//Covid2
	modelMatrixCovid2[3][1] = terrain.getHeightTerrain(modelMatrixCovid2[3][0], modelMatrixCovid2[3][2]);
	glm::mat4 modelMatrixCovid2Body = glm::mat4(modelMatrixCovid2);
	modelMatrixCovid2Body = glm::scale(modelMatrixCovid2Body, glm::vec3(0.015, 0.015, 0.015));
	CovidModelAnimate2.setAnimationIndex(0);
	if (DisappearModelCovid2) {
		CovidModelAnimate2.render(modelMatrixCovid2Body);
	}

	//Covid3
	modelMatrixCovid3[3][1] = terrain.getHeightTerrain(modelMatrixCovid3[3][0], modelMatrixCovid3[3][2]);
	glm::mat4 modelMatrixCovid3Body = glm::mat4(modelMatrixCovid3);
	modelMatrixCovid3Body = glm::scale(modelMatrixCovid3Body, glm::vec3(0.015, 0.015, 0.015));
	CovidModelAnimate3.setAnimationIndex(0);
	if (DisappearModelCovid3) {
		CovidModelAnimate3.render(modelMatrixCovid3Body);
	}

	//Covid4
	modelMatrixCovid4[3][1] = terrain.getHeightTerrain(modelMatrixCovid4[3][0], modelMatrixCovid4[3][2]);
	glm::mat4 modelMatrixCovid4Body = glm::mat4(modelMatrixCovid4);
	modelMatrixCovid4Body = glm::scale(modelMatrixCovid4Body, glm::vec3(0.015, 0.015, 0.015));
	CovidModelAnimate4.setAnimationIndex(0);
	if (DisappearModelCovid4) {
		CovidModelAnimate4.render(modelMatrixCovid4Body);
	}
	// Covid5
	modelMatrixCovid5[3][1] = terrain.getHeightTerrain(modelMatrixCovid5[3][0], modelMatrixCovid5[3][2]);
	glm::mat4 modelMatrixCovidBody5 = glm::mat4(modelMatrixCovid5);
	modelMatrixCovidBody5 = glm::scale(modelMatrixCovidBody5, glm::vec3(0.015, 0.015, 0.015));
	CovidModelAnimate5.setAnimationIndex(0);
	if (DisappearModelCovid5) {
		CovidModelAnimate5.render(modelMatrixCovidBody5);
	}
	//Cubrebocas
	modelMatrixCubrebocas[3][1] = terrain.getHeightTerrain(modelMatrixCubrebocas[3][0], modelMatrixCubrebocas[3][2]);
	glm::mat4 modelMatrixCubreBody = glm::mat4(modelMatrixCubrebocas);
	modelMatrixCubreBody = glm::scale(modelMatrixCubreBody, glm::vec3(0.015, 0.015, 0.015));
	CubreBocasModelAnimate.setAnimationIndex(0);
	if (DisappearModelCubreBocas1) {
		CubreBocasModelAnimate.render(modelMatrixCubreBody);
	}
	//Cubrebocas2
	modelMatrixCubrebocas2[3][1] = terrain.getHeightTerrain(modelMatrixCubrebocas2[3][0], modelMatrixCubrebocas2[3][2]);
	glm::mat4 modelMatrixCubreBody2 = glm::mat4(modelMatrixCubrebocas2);
	modelMatrixCubreBody2 = glm::scale(modelMatrixCubreBody2, glm::vec3(0.015, 0.015, 0.015));
	CubreBocasModelAnimate2.setAnimationIndex(0);
	if (DisappearModelCubreBocas2) {
		CubreBocasModelAnimate2.render(modelMatrixCubreBody2);
	}
	//Cubrebocas3
	modelMatrixCubrebocas3[3][1] = terrain.getHeightTerrain(modelMatrixCubrebocas3[3][0], modelMatrixCubrebocas3[3][2]);
	glm::mat4 modelMatrixCubreBody3 = glm::mat4(modelMatrixCubrebocas3);
	modelMatrixCubreBody3 = glm::scale(modelMatrixCubreBody3, glm::vec3(0.015, 0.015, 0.015));
	CubreBocasModelAnimate3.setAnimationIndex(0);
	if (DisappearModelCubreBocas3) {
		CubreBocasModelAnimate3.render(modelMatrixCubreBody3);
	}
	//Cubrebocas4
	modelMatrixCubrebocas4[3][1] = terrain.getHeightTerrain(modelMatrixCubrebocas4[3][0], modelMatrixCubrebocas4[3][2]);
	glm::mat4 modelMatrixCubreBody4 = glm::mat4(modelMatrixCubrebocas4);
	modelMatrixCubreBody4 = glm::scale(modelMatrixCubreBody4, glm::vec3(0.015, 0.015, 0.015));
	CubreBocasModelAnimate4.setAnimationIndex(0);
	if (DisappearModelCubreBocas4) {
		CubreBocasModelAnimate4.render(modelMatrixCubreBody4);
	}
	//Cubrebocas5
	modelMatrixCubrebocas5[3][1] = terrain.getHeightTerrain(modelMatrixCubrebocas5[3][0], modelMatrixCubrebocas5[3][2]);
	glm::mat4 modelMatrixCubreBody5 = glm::mat4(modelMatrixCubrebocas5);
	modelMatrixCubreBody5 = glm::scale(modelMatrixCubreBody5, glm::vec3(0.015, 0.015, 0.015));
	CubreBocasModelAnimate5.setAnimationIndex(0);
	if (DisappearModelCubreBocas5) {
		CubreBocasModelAnimate5.render(modelMatrixCubreBody5);
	}
	//Cubrebocas6
	modelMatrixCubrebocas6[3][1] = terrain.getHeightTerrain(modelMatrixCubrebocas6[3][0], modelMatrixCubrebocas6[3][2]);
	glm::mat4 modelMatrixCubreBody6 = glm::mat4(modelMatrixCubrebocas6);
	modelMatrixCubreBody6 = glm::scale(modelMatrixCubreBody6, glm::vec3(0.015, 0.015, 0.015));
	CubreBocasModelAnimate6.setAnimationIndex(0);
	if (DisappearModelCubreBocas6) {
		CubreBocasModelAnimate6.render(modelMatrixCubreBody6);
	}
	//Cubrebocas7
	modelMatrixCubrebocas7[3][1] = terrain.getHeightTerrain(modelMatrixCubrebocas7[3][0], modelMatrixCubrebocas7[3][2]);
	glm::mat4 modelMatrixCubreBody7 = glm::mat4(modelMatrixCubrebocas7);
	modelMatrixCubreBody7 = glm::scale(modelMatrixCubreBody7, glm::vec3(0.015, 0.015, 0.015));
	CubreBocasModelAnimate7.setAnimationIndex(0);
	if (DisappearModelCubreBocas7) {
		CubreBocasModelAnimate7.render(modelMatrixCubreBody7);
	}
	//Cubrebocas8
	modelMatrixCubrebocas8[3][1] = terrain.getHeightTerrain(modelMatrixCubrebocas8[3][0], modelMatrixCubrebocas8[3][2]);
	glm::mat4 modelMatrixCubreBody8 = glm::mat4(modelMatrixCubrebocas8);
	modelMatrixCubreBody8 = glm::scale(modelMatrixCubreBody8, glm::vec3(0.015, 0.015, 0.015));
	CubreBocasModelAnimate8.setAnimationIndex(0);
	if (DisappearModelCubreBocas8) {
		CubreBocasModelAnimate8.render(modelMatrixCubreBody8);
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
		if (renderParticles
			&& it->second.first.compare("fountain") == 0) {
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesFountain = glm::mat4(1.0);
			modelMatrixParticlesFountain = glm::translate(
				modelMatrixParticlesFountain, it->second.second);
			modelMatrixParticlesFountain[3][1] = terrain.getHeightTerrain(
				modelMatrixParticlesFountain[3][0],
				modelMatrixParticlesFountain[3][2]) + 0.36 * 10.0;
			modelMatrixParticlesFountain = glm::scale(
				modelMatrixParticlesFountain, glm::vec3(3.0, 3.0, 3.0));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
			shaderParticlesFountain.turnOn();
			shaderParticlesFountain.setFloat("Time",
				float(
					currTimeParticlesAnimation
					- lastTimeParticlesAnimation));
			shaderParticlesFountain.setFloat("ParticleLifetime", 3.5f);
			shaderParticlesFountain.setInt("ParticleTex", 0);
			shaderParticlesFountain.setVectorFloat3("Gravity",
				glm::value_ptr(glm::vec3(0.0f, -0.3f, 0.0f)));
			shaderParticlesFountain.setMatrix4("model", 1, false,
				glm::value_ptr(modelMatrixParticlesFountain));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			shaderParticlesFountain.turnOff();
			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("fire") == 0) {
			/**********
			 * Init Render particles systems
			 */
			lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
			currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

			shaderParticlesFire.setInt("Pass", 1);
			shaderParticlesFire.setFloat("Time",
				currTimeParticlesAnimationFire);
			shaderParticlesFire.setFloat("DeltaT",
				currTimeParticlesAnimationFire
				- lastTimeParticlesAnimationFire);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_1D, texId);
			glEnable(GL_RASTERIZER_DISCARD);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
			glBeginTransformFeedback(GL_POINTS);
			glBindVertexArray(particleArray[1 - drawBuf]);
			glVertexAttribDivisor(0, 0);
			glVertexAttribDivisor(1, 0);
			glVertexAttribDivisor(2, 0);
			glDrawArrays(GL_POINTS, 0, nParticlesFire);
			glEndTransformFeedback();
			glDisable(GL_RASTERIZER_DISCARD);

			shaderParticlesFire.setInt("Pass", 2);
			glm::mat4 modelFireParticles = glm::mat4(1.0);
			modelFireParticles = glm::translate(modelFireParticles,
				it->second.second);
			modelFireParticles[3][1] = terrain.getHeightTerrain(
				modelFireParticles[3][0], modelFireParticles[3][2]);
			shaderParticlesFire.setMatrix4("model", 1, false,
				glm::value_ptr(modelFireParticles));

			shaderParticlesFire.turnOn();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
			glDepthMask(GL_FALSE);
			glBindVertexArray(particleArray[drawBuf]);
			glVertexAttribDivisor(0, 1);
			glVertexAttribDivisor(1, 1);
			glVertexAttribDivisor(2, 1);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
			drawBuf = 1 - drawBuf;
			shaderParticlesFire.turnOff();

			/****************************+
			 * Open AL sound data
			 */
			source1Pos[0] = modelFireParticles[3].x;
			source1Pos[1] = modelFireParticles[3].y;
			source1Pos[2] = modelFireParticles[3].z;
			alSourcefv(source[1], AL_POSITION, source1Pos);

			/**********
			 * End Render particles systems
			 */
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

#include "globalOpenGL_GLFW.h"
#include <iostream>			
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>		
#include <string>
#include <vector>		
#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "sMeshDrawInfo.h"
#include "cShaderManager.h" 
#include "cGameObject.h"
#include "cVAOMeshManager.h"
#include "Physics/cPhysicsWorld.h"
#include "cLightManager.h"
#include "globalGameStuff.h"
#include "cCamera.h"
#include "cFBO.h" 
//#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"
#include "Dalek_Threaded_03.h"
#include "cDalek.h"
//#include "cAnimationState.h"
#include<myInterface.h>
#include<myBulletPhysics.h>
#include "cFrameBuffer.h"




//Vectors Required for Physics Functionality
std::vector<glm::vec3> NewPosition;
std::vector<glm::quat> NewOrientation;
std::vector<int> collision;
myInterface::force wforce;
myInterface::force sforce;
myInterface::force upforce;
myInterface::force leftforce;
myInterface::force rightforce;

float camerafactor = 80.0f;
bool debugrender = 0;
float theta = 180;

bool charswitch = 0;
int selectcharacter = 0;


iDalekManager* g_pDalekManager; 

cFBO g_FBO_Pass1_G_Buffer;
cFBO g_FBO_Pass2_Deferred;


void DrawDebugLightingSpheres(void);


cGameObject* g_pSkyBoxObject = NULL;	


//Global KeyFlags
bool W = 0, A = 0, D = 0, SBAR = 0, J = 0, SS = 0, SW = 0, S=0, up=0, down=0, right=0, left=0;

//Main Graphical object vector
std::vector< cGameObject* >  g_vecGameObjects;

cCamera* g_pTheCamera = NULL;
cVAOMeshManager* g_pVAOManager = 0;		
cShaderManager*		g_pShaderManager = 0;	
cLightManager*		g_pLightManager = 0;
CTextureManager*		g_pTextureManager = 0;
cModelAssetLoader*		g_pModelAssetLoader = NULL;
cDebugRenderer*			g_pDebugRenderer = 0;
cAABBBroadPhase* g_terrainAABBBroadPhase = 0;
cPhysicsWorld*	g_pPhysicsWorld = NULL;	// (theMain.cpp)
cGameObject* g_ExampleTexturedQuad = NULL;
cGameObject* g_Room = NULL;
cGameObject* g_RoomMaskForStencil = NULL;
bool g_IsWindowFullScreen = false;
GLFWwindow* g_pGLFWWindow = NULL;
bool keyflag = 0;
float keytime = 0;
float g_ChromaticAberrationOffset = 0.0f;
float g_CR_Max = 0.1f;
double g_CA_CountDown = 0.0f;
const int RENDER_PASS_0_G_BUFFER_PASS = 0;
const int RENDER_PASS_1_DEFERRED_RENDER_PASS = 1;
const int RENDER_PASS_2_FULL_SCREEN_EFFECT_PASS = 2;


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}


void ReadFileToToken(std::ifstream &file, std::string token)
{
	bool bKeepReading = true;
	std::string garbage;
	do
	{
		file >> garbage;		// Title_End??
		if (garbage == token)
		{
			return;
		}
	} while (bKeepReading);
	return;
}





//Create an object with desired specs
void makeObject(std::string friendlyName, glm::vec3 pos, glm::vec3 or2, float scale, std::string name, std::string text1, float text1v, std::string text2, float text2v)
{
	cGameObject* pTempGO = new cGameObject();
	pTempGO->friendlyName = friendlyName;
	cPhysicalProperties physState;
	physState.position = pos;
	physState.integrationUpdateType = cPhysicalProperties::ePhysicsIntegrationUpdateTypes::EXCLUDED_FROM_INTEGRATION;
	pTempGO->SetPhysState(physState);
	sMeshDrawInfo meshInfo;
	meshInfo.scale = scale;
	meshInfo.name = name;
	meshInfo.globalAlpha = 1.0;
	meshInfo.bUseDebugColour = 0;
	//	meshInfo.bDrawAsWireFrame = 1;
	meshInfo.setMeshOrientationEulerAngles(or2);
	//meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.5f));
	//meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 0.5f));
	meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo(text1, text1v));
	meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo(text2, text2v));

	pTempGO->vecMeshes.push_back(meshInfo);
	//
	::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add
}



//Load the scene from a text file
bool SceneDescriptor(std::string file)
{
	std::string typer;
	glm::vec3 pos; glm::vec3 or ; glm::vec3 or2; float scale;  glm::vec4 color; std::string name; bool phys;
	std::string fname, text1, text2;
	float text1br, text2br;
	std::ifstream plyFile(file.c_str());
	if (!plyFile.is_open())
	{	// Didn't open file, so return
		return false;
	}


	ReadFileToToken(plyFile, "models");
	int count;
	plyFile >> count;

	for (int i = 1; i <= count; i++)
	{

		ReadFileToToken(plyFile, std::to_string(i));
		ReadFileToToken(plyFile, "fname");
		plyFile >> fname;




		ReadFileToToken(plyFile, "position");
		plyFile >> pos.x;
		plyFile >> pos.y;
		plyFile >> pos.z;


		ReadFileToToken(plyFile, "orientation2");
		plyFile >> or2.x;
		plyFile >> or2.y;
		plyFile >> or2.z;

		ReadFileToToken(plyFile, "scale");
		plyFile >> scale;

		ReadFileToToken(plyFile, "name");
		plyFile >> name;

		ReadFileToToken(plyFile, "text1");
		plyFile >> text1;
		ReadFileToToken(plyFile, "text1br");
		plyFile >> text1br;

		ReadFileToToken(plyFile, "text2");
		plyFile >> text2;
		ReadFileToToken(plyFile, "text2br");
		plyFile >> text2br;

		makeObject(fname, pos, or2, scale, name, text1, text1br, text2, text2br );

	}

}





//Main Game stuff

int main(void)
{


	//Load My Scene!
	SceneDescriptor("Scene.txt");

	//Create the bullet INstance with interface
	myInterface* io = new myBulletPhysics();



	std::vector<myInterface::gameObject> ObjectInfoVector = io->InitializePhysics("Scene.txt");


	for (int i = 0; i < ObjectInfoVector.size(); i++)
	{
		NewPosition.push_back(glm::vec3(0, 0, 0));
		NewOrientation.push_back(glm::quat(0, 0, 0, 0));
	}

	for (int i = 0; i < ObjectInfoVector.size(); i++)
	{
		std::cout << i << ObjectInfoVector[i].name << "     " << ObjectInfoVector[i].pos.x << std::endl;
	}








	//GLFWwindow* pGLFWWindow;		// Moved to allow switch from windowed to full-screen
	glfwSetErrorCallback(error_callback);



    if (!glfwInit())
	{
		// exit(EXIT_FAILURE);
		std::cout << "ERROR: Couldn't init GLFW, so we're pretty much stuck; do you have OpenGL??" << std::endl;
		return -1;
	}

	int height = 480;	/* default */
	int width = 640;	// default
	std::string title = "OpenGL Rocks";

	std::ifstream infoFile("config.txt");
	if ( ! infoFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}   
	else
	{	// File DID open, so read it... 
		std::string a;	
		
		infoFile >> a;	// "Game"	//std::cin >> a;
		infoFile >> a;	// "Config"
		infoFile >> a;	// "width"

		infoFile >> width;	// 1080

		infoFile >> a;	// "height"

		infoFile >> height;	// 768

		infoFile >> a;		// Title_Start

		std::stringstream ssTitle;		// Inside "sstream"
		bool bKeepReading = true;
		do
		{
			infoFile >> a;		// Title_End??
			if ( a != "Title_End" )
			{
				ssTitle << a << " ";
			}
			else
			{	// it IS the end! 
				bKeepReading  = false;
				title = ssTitle.str();
			}
		}while( bKeepReading );


	}//if ( ! infoFile.is_open() )




    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// C++ string
    ::g_pGLFWWindow = glfwCreateWindow( width, height, 
							            title.c_str(), 
							            NULL, NULL);
    if ( ! ::g_pGLFWWindow )
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback( ::g_pGLFWWindow, key_callback );
	// For the FBO to resize when the window changes
	glfwSetWindowSizeCallback( ::g_pGLFWWindow, window_size_callback );

    glfwMakeContextCurrent( ::g_pGLFWWindow );
    gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );
    glfwSwapInterval(1);



	std::cout << glGetString(GL_VENDOR) << " " 
		<< glGetString(GL_RENDERER) << ", " 
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// General error string, used for a number of items during start up
	std::string error;

	::g_pShaderManager = new cShaderManager();

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";	
	//fragShader.fileName = "simpleFrag.glsl"; 
	fragShader.fileName = "simpleFragDeferred.glsl"; 
	
	::g_pShaderManager->setBasePath( "assets//shaders//" );

	// Shader objects are passed by reference so that
	//	we can look at the results if we wanted to. 
	if ( ! ::g_pShaderManager->createProgramFromFile(
		        "mySexyShader", vertShader, fragShader ) )
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;	
//		exit(
	}
	std::cout << "The shaders comipled and linked OK" << std::endl;


// Triangle debug renderer test...
	::g_pDebugRenderer = new cDebugRenderer();
	if ( ! ::g_pDebugRenderer->initialize(error) )
	{
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
		std::cout << "\t" << ::g_pDebugRenderer->getLastError() << std::endl;
	}



	// Load models
	::g_pModelAssetLoader = new cModelAssetLoader();
	::g_pModelAssetLoader->setBasePath("assets/models/");

	::g_pVAOManager = new cVAOMeshManager();

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");


	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName("mySexyShader");

	if ( ! Load3DModelsIntoMeshManager(sexyShaderID, ::g_pVAOManager, ::g_pModelAssetLoader, error ) )
	{
		std::cout << "Not all models were loaded..." << std::endl;
		std::cout << error << std::endl;
	}

	LoadModelsIntoScene();
 
	GLint numberOfUniformBlocks = -1;
	glGetProgramiv(currentProgID, GL_ACTIVE_UNIFORM_BLOCKS, &numberOfUniformBlocks);


	const int BUFFERSIZE = 1000;
	int name_length_written = 0;
	char NUBName_0[BUFFERSIZE] = {0};
	char NUBName_1[BUFFERSIZE] = {0};

	glGetActiveUniformBlockName(currentProgID,
								0,
								BUFFERSIZE,
								&name_length_written,
								NUBName_0);

	glGetActiveUniformBlockName(currentProgID,
								1,
								BUFFERSIZE,
								&name_length_written,
								NUBName_1);

	GLuint NUB_Buffer_0_ID = 0;
	GLuint NUB_Buffer_1_ID = 0;

	glGenBuffers(1, &NUB_Buffer_0_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, NUB_Buffer_0_ID );

	glGenBuffers(1, &NUB_Buffer_1_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, NUB_Buffer_1_ID);





	::g_pLightManager = new cLightManager();

	::g_pLightManager->CreateLights(10);	
	//Light 0
	////////////////////////////////////////////////////////////////////////////////////////////////////
	::g_pLightManager->vecLights[0].setLightParamType(cLight::POINT);		
	::g_pLightManager->vecLights[0].position = glm::vec3(500.0f, 1000.0f, 1000.0f);
	::g_pLightManager->vecLights[0].attenuation.y = 0.0002f;		

	//Light 1
	////////////////////////////////////////////////////////////////////////////////////////////////////
	::g_pLightManager->vecLights[1].attenuation.y = 0.000456922280f;		
	::g_pLightManager->vecLights[1].setLightParamType(cLight::POINT);		
	::g_pLightManager->vecLights[1].direction = glm::vec3(0.0f, -1.0f, 0.0f );
	::g_pLightManager->vecLights[1].setLightParamSpotPrenumAngleInner( glm::radians(15.0f) );
	::g_pLightManager->vecLights[1].setLightParamSpotPrenumAngleOuter( glm::radians(45.0f) );
	::g_pLightManager->vecLights[1].position = glm::vec3(0.0f, 3000.0f, 0.0f);	

	//Light 2
	////////////////////////////////////////////////////////////////////////////////////////////////////

	::g_pLightManager->vecLights[2].attenuation.y = 0.000456922280f;		
	::g_pLightManager->vecLights[2].setLightParamType(cLight::POINT);																					
	::g_pLightManager->vecLights[2].direction = glm::vec3(0.0f, -1.0f, 0.0f);
	::g_pLightManager->vecLights[2].setLightParamSpotPrenumAngleInner(glm::radians(15.0f));
	::g_pLightManager->vecLights[2].setLightParamSpotPrenumAngleOuter(glm::radians(45.0f));
	::g_pLightManager->vecLights[2].position = glm::vec3(800.0f, 3500.0f, 0.0f);
	::g_pLightManager->LoadShaderUniformLocations(currentProgID);

	//Light 3
	////////////////////////////////////////////////////////////////////////////////////////////////////

	::g_pLightManager->vecLights[3].attenuation.y = 0.000456922280f;		
	::g_pLightManager->vecLights[3].setLightParamType(cLight::POINT);		
	::g_pLightManager->vecLights[3].direction = glm::vec3(0.0f, -1.0f, 0.0f);
	::g_pLightManager->vecLights[3].setLightParamSpotPrenumAngleInner(glm::radians(15.0f));
	::g_pLightManager->vecLights[3].setLightParamSpotPrenumAngleOuter(glm::radians(45.0f));
	::g_pLightManager->vecLights[3].position = glm::vec3(800.0f, 3500.0f, 800.0f);
	::g_pLightManager->LoadShaderUniformLocations(currentProgID);


	//Light 4
	////////////////////////////////////////////////////////////////////////////////////////////////////
	::g_pLightManager->vecLights[4].attenuation.y = 0.000456922280f;		
	::g_pLightManager->vecLights[4].setLightParamType(cLight::SPOT);		
	::g_pLightManager->vecLights[4].direction = glm::vec3(-1.0f, -1.0f, 0.0f);
	::g_pLightManager->vecLights[4].setLightParamSpotPrenumAngleInner(glm::radians(15.0f));
	::g_pLightManager->vecLights[4].setLightParamSpotPrenumAngleOuter(glm::radians(45.0f));
	::g_pLightManager->vecLights[4].position = glm::vec3(5000.0f, 1000.0f, 0.0f);
	::g_pLightManager->LoadShaderUniformLocations(currentProgID);


	
	// Texture 
	::g_pTextureManager = new CTextureManager();

	std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_TEXTURE_IMAGE_UNITS() << std::endl;
	std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS() << std::endl;

	::g_pTextureManager->setBasePath("assets/textures");
	if ( ! ::g_pTextureManager->Create2DTextureFromBMPFile("Utah_Teapot_xyz_n_uv_Checkerboard.bmp", true) )
	{
		std::cout << "Didn't load the texture. Oh no!" << std::endl;
	}
	else
	{
		std::cout << "Texture is loaded! Hazzah!" << std::endl;
	}



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Load my Sexy Textures!
	::g_pTextureManager->Create2DTextureFromBMPFile("Utah_Teapot_xyz_n_uv_Enterprise.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("GuysOnSharkUnicorn.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("GuysOnSharkUnicorn.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("Seamless_ground_sand_texture.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("barberton_etm_2001121_lrg.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("height_map_norway-height-map-aster-30m.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("image1.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("image2.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("pavement.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("wood.bmp", true);


	//Sky Box
	::g_pTextureManager->setBasePath("assets/textures/skybox");
	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles(
			"space",
			"TropicalSunnyDayRight2048.bmp",
			"TropicalSunnyDayLeft2048.bmp",
			"TropicalSunnyDayUp2048.bmp",
			"TropicalSunnyDayDown2048.bmp",
			"TropicalSunnyDayFront2048.bmp",
			"TropicalSunnyDayBack2048.bmp", true, true))
	{
		std::cout << "Didn't load skybox" << std::endl;
	}


	      
///***********************************************************
	// About the generate the AABB for the terrain
	::g_terrainAABBBroadPhase = new cAABBBroadPhase();
	// Perhaps you'd like something more sophisticated than this...
	::g_terrainAABBBroadPhase->pDebugRenderer = ::g_pDebugRenderer;
	// 
	cMesh terrainMesh; 
	if (::g_pVAOManager->lookupMeshFromName("MeshLabTerrain_xyz_n_uv", terrainMesh))
	{
		std::cout << "Generating the terrain AABB grid. This will take a moment..." << std::endl;
		
		::g_terrainAABBBroadPhase->genAABBGridFromMesh(terrainMesh);

	}//if (::g_pVAOManager->lookupMeshFromName
///***********************************************************


	::g_pTheCamera = new cCamera();

	::g_pTheCamera->setCameraMode(cCamera::MODE_FLY_USING_LOOK_AT);
	::g_pTheCamera->FlyCamLA->setEyePosition(glm::vec3(0.0f, 10.0f, 650.0f));
	::g_pTheCamera->FlyCamLA->setTargetInWorld(glm::vec3(0.0f, 20.0f, 0.0f));
	::g_pTheCamera->FlyCamLA->setUpVector(glm::vec3(0.0f, 1.0f, 0.0f));

	::g_pPhysicsWorld = new cPhysicsWorld();


	// All loaded!
	std::cout << "And we're good to go! Staring the main loop..." << std::endl;

	glEnable( GL_DEPTH );


	// Create an FBO
	if ( ! ::g_FBO_Pass1_G_Buffer.init(1920, 1080, error) )
	{
		std::cout << "::g_FBO_Pass2_Deferred error: " << error << std::endl;
	}
	else
	{
		std::cout << "::g_FBO_Pass2_Deferred is good." << std::endl;
		std::cout << "\t::g_FBO_Pass1_G_Buffer ID = " << ::g_FBO_Pass1_G_Buffer.ID << std::endl;
		std::cout << "\tcolour texture ID = " << ::g_FBO_Pass1_G_Buffer.colourTexture_0_ID << std::endl;
		std::cout << "\tnormal texture ID = " << ::g_FBO_Pass1_G_Buffer.normalTexture_1_ID << std::endl;

		std::cout << "GL_MAX_COLOR_ATTACHMENTS = " << ::g_FBO_Pass1_G_Buffer.getMaxColourAttachments() << std::endl;
		std::cout << "GL_MAX_DRAW_BUFFERS = " << ::g_FBO_Pass1_G_Buffer.getMaxDrawBuffers() << std::endl;

	}//if ( ! ::g_FBO_Pass1_G_Buffer.init

	if ( ! ::g_FBO_Pass2_Deferred.init(1920, 1080, error) )
	{
		std::cout << "::g_FBO_Pass2_Deferred error: " << error << std::endl;
	}
	else
	{
		std::cout << "FBO is good." << std::endl;
		std::cout << "\t::g_FBO_Pass2_Deferred ID = " << ::g_FBO_Pass2_Deferred.ID << std::endl;
		std::cout << "\tcolour texture ID = " << ::g_FBO_Pass2_Deferred.colourTexture_0_ID << std::endl;
		std::cout << "\tnormal texture ID = " << ::g_FBO_Pass2_Deferred.normalTexture_1_ID << std::endl;

		std::cout << "GL_MAX_COLOR_ATTACHMENTS = " << ::g_FBO_Pass2_Deferred.getMaxColourAttachments() << std::endl;
		std::cout << "GL_MAX_DRAW_BUFFERS = " << ::g_FBO_Pass2_Deferred.getMaxDrawBuffers() << std::endl;

	}//if ( ! ::g_FBO_Pass2_Deferred.init




	setWindowFullScreenOrWindowed( ::g_pGLFWWindow, ::g_IsWindowFullScreen );


	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();



	::g_pTheCamera->setCameraMode(cCamera::MODE_FLY_USING_LOOK_AT);
	// Main game or application loop


	while ( ! glfwWindowShouldClose(::g_pGLFWWindow) )
    {
		// Essentially the "frame time"
		// Now many seconds that have elapsed since we last checked
		double curTime = glfwGetTime();
		double deltaTime =  curTime - lastTimeStep;
		lastTimeStep = curTime;


		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////
		//Main STUFF 
		::g_pPhysicsWorld->IntegrationStep(deltaTime);

		::g_pTheCamera->updateTick(deltaTime);

                                                   
		cPhysicalProperties skyBoxPP;
		::g_pSkyBoxObject->GetPhysState(skyBoxPP);
		skyBoxPP.position = ::g_pTheCamera->getEyePosition();
		::g_pSkyBoxObject->SetPhysState(skyBoxPP);



		::g_pShaderManager->useShaderProgram("mySexyShader");

		// Direct everything to the FBO
		GLint renderPassNumber_LocID = glGetUniformLocation(sexyShaderID, "renderPassNumber");
		glUniform1i( renderPassNumber_LocID, RENDER_PASS_0_G_BUFFER_PASS );

		glBindFramebuffer(GL_FRAMEBUFFER, g_FBO_Pass1_G_Buffer.ID );
		// Clear colour AND depth buffer
		g_FBO_Pass1_G_Buffer.clearBuffers();

//glDisable(GL_DEPTH_TEST);		// Check what's already in the depth buffer
//glDepthMask(GL_FALSE);			// Written to the depth buffer

		// 1. Drawing the "mask" object (that "stencil")

		// Note the addition to the stencil buffer clear
		// BIG NOTE: This clears the FINAL frame buffer! 
		// (Keep in mind that when the stencil is enabled, it's enabled for ALL frame buffers)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );


		//glm::vec3 g_cameraXYZ = { ::g_vecGameObjects[1]->position.x + camerafactor * cosf(theta), ::g_vecGameObjects[1]->position.y + 0.5*camerafactor, ::g_vecGameObjects[1]->position.z + camerafactor * sin(theta) };	// 5 units "down" z
		//glm::vec3 g_cameraTarget_XYZ = ::g_vecGameObjects[1]->position;
		

		
		g_pTheCamera->FlyCamLA->setEyePosition(glm::vec3 (::g_vecGameObjects[0]->getPosition().x + camerafactor * cosf(theta), ::g_vecGameObjects[0]->getPosition().y + 0.5*camerafactor, ::g_vecGameObjects[0]->getPosition().z + camerafactor * sin(theta)) );
		g_pTheCamera->FlyCamLA->setTargetInWorld(::g_vecGameObjects[0]->getPosition());
		
		upforce.direction = glm::vec3(0, 1, 0);
		wforce.direction = glm::normalize ( ( ::g_vecGameObjects[0]->getPosition() - g_pTheCamera->getEyePosition()) );
		sforce.direction = - glm::normalize( (::g_vecGameObjects[0]->getPosition() - g_pTheCamera->getEyePosition()) );
		leftforce.direction = glm::normalize ( ( glm::cross(glm::vec3(0, 1, 0), ::g_vecGameObjects[0]->getPosition() - g_pTheCamera->getEyePosition()))  );
		rightforce.direction = -leftforce.direction; 
		

		wforce.strength = 1.0f;
		sforce.strength = 1.0f;
		upforce.strength = 1.0f;
		rightforce.strength = 1.0f;
		leftforce.strength = 1.0f;

		if (::left == 1)
		{
			theta += (float)deltaTime;
		}
		if (::right == 1)
		{
			theta -= (float)deltaTime;
		}
		if (::up == 1)
		{
			camerafactor += 50*(float)deltaTime;
		}
		if (::down == 1)
		{
			camerafactor -= 50 * (float)deltaTime;
		}
		if (::W == 1)
		{
			wforce.on=1;
		}
		else
		{
			wforce.on = 0;
		}
		if (::S == 1)
		{
			sforce.on = 1;
		}
		else
		{
			sforce.on = 0;
		}
		if (::A == 1)
		{
			leftforce.on = 1;
		}
		else
		{
			leftforce.on = 0;
		}
		if (::D == 1)
		{
			rightforce.on = 1;
		}
		else
		{
			rightforce.on = 0;
		}



		std::cout << "X: " << ::g_vecGameObjects[0]->getPosition().x << " Z: " << ::g_vecGameObjects[0]->getPosition().z << std::endl;


		io->PhysicsStep(deltaTime, wforce, sforce, leftforce, rightforce, upforce, NewPosition, NewOrientation, ObjectInfoVector, collision);
		for (int i = 0; i < ObjectInfoVector.size()-1; i++)
		{
			::g_vecGameObjects[i]->overwritePotition(NewPosition[i]);
			::g_vecGameObjects[i]->overwriteQOrientation (NewOrientation[i] );
		}
		
		
	//	::g_vecGameObjects[1]->adjQOrientationEuler(glm::vec3(deltaTime,0,0));





		// 3. Draw the rest of the scene.
		// Clear the depth buffer, too (where the door mask was)
		glClear( GL_DEPTH_BUFFER_BIT );
//		glDisable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0x00);		// Make sure we can't write to the stencil buffer
		// BUT, we DON'T want to change what's already on the stencil buffer
		glStencilOp(GL_KEEP,		// Stencil fail
					GL_KEEP,		// Depth fail
					GL_KEEP);		// Stencil AND Depth PASS

		RenderScene( ::g_vecGameObjects, ::g_pGLFWWindow, deltaTime );

		/*for (int i=0 ; i <= 1; i++)
		{
			if (g_vecGameObjects[i]->getPosition().z >= 800)
			{
				g_vecGameObjects[i]->overwritePotition(glm::vec3(g_vecGameObjects[i]->getPosition().x, g_vecGameObjects[i]->getPosition().y - 150*deltaTime, g_vecGameObjects[i]->getPosition().z));
			}
		}
		*/




		// Render it again, but point the the FBO texture... 
//		glBindFramebuffer(GL_FRAMEBUFFER, g_FBO_Pass2_Deferred.ID );
//		g_FBO_Pass2_Deferred.clearBuffers();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
		glDisable(GL_STENCIL_TEST);

		::g_pShaderManager->useShaderProgram("mySexyShader");

		glUniform1i( renderPassNumber_LocID, RENDER_PASS_1_DEFERRED_RENDER_PASS );
		
		//uniform sampler2D texFBONormal2D;
		//uniform sampler2D texFBOVertexWorldPos2D;

		GLint texFBOColour2DTextureUnitID = 10;
		GLint texFBOColour2DLocID = glGetUniformLocation(sexyShaderID, "texFBOColour2D");
		GLint texFBONormal2DTextureUnitID = 11;
		GLint texFBONormal2DLocID = glGetUniformLocation(sexyShaderID, "texFBONormal2D");
		GLint texFBOWorldPosition2DTextureUnitID = 12;
		GLint texFBOWorldPosition2DLocID = glGetUniformLocation(sexyShaderID, "texFBOVertexWorldPos2D");

		// Pick a texture unit... 
		glActiveTexture(GL_TEXTURE0 + texFBOColour2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.colourTexture_0_ID);
		glUniform1i(texFBOColour2DLocID, texFBOColour2DTextureUnitID);

		glActiveTexture(GL_TEXTURE0 + texFBONormal2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.normalTexture_1_ID);
		glUniform1i(texFBONormal2DLocID, texFBONormal2DTextureUnitID);
		
		glActiveTexture(GL_TEXTURE0 + texFBOWorldPosition2DTextureUnitID);
		glBindTexture(GL_TEXTURE_2D, g_FBO_Pass1_G_Buffer.vertexWorldPos_2_ID);
		glUniform1i(texFBOWorldPosition2DLocID, texFBOWorldPosition2DTextureUnitID);
		
		// Set the sampler in the shader to the same texture unit (20)

		glfwGetFramebufferSize(::g_pGLFWWindow, &width, &height);

		GLint screenWidthLocID = glGetUniformLocation(sexyShaderID, "screenWidth");
		GLint screenHeightLocID = glGetUniformLocation(sexyShaderID, "screenHeight");
		glUniform1f(screenWidthLocID, (float)width);
		glUniform1f(screenHeightLocID, (float)height);

		// Adjust the CA:
		//float g_ChromaticAberrationOffset = 0.0f;
		// 0.1
		if ( ::g_CA_CountDown > 0.0 )
		{
			float CRChange = getRandInRange( (-g_CR_Max/10.0f), (::g_CR_Max/10.0f) );
			::g_ChromaticAberrationOffset += CRChange;
			::g_CA_CountDown -= deltaTime;
		}
		else
		{
			::g_ChromaticAberrationOffset = 0.0f;
		}

		GLint CROffset_LocID = glGetUniformLocation(sexyShaderID, "CAoffset" );
		glUniform1f( CROffset_LocID, g_ChromaticAberrationOffset);

		std::vector< cGameObject* >  vecCopy2ndPass;
		// Push back a SINGLE quad or GIANT triangle that fills the entire screen
		// Here we will use the skybox (as it fills the entire screen)
		vecCopy2ndPass.push_back( ::g_pSkyBoxObject );

//		cGameObject* pBunny = findObjectByFriendlyName("bugs", ::g_vecGameObjects);
//		vecCopy2ndPass.push_back(pBunny);

		RenderScene(vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );




		// Set the scissor buffer
//// Example to render only INSIDE the scissor buffer (square) area
//		glScissor(500, 500, 1080, 600);
//		glEnable(GL_SCISSOR_TEST );
//		RenderScene(vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );
//		glDisable(GL_SCISSOR_TEST );
//// 
//
//// Example to render only OUTSIDE of the scissor area:
//		RenderScene(vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );
//		glScissor(500, 500, 1080, 600);
//		glEnable(GL_SCISSOR_TEST );
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glDisable(GL_SCISSOR_TEST );
//// 


//    ___  _              _   ___  ___    ___               
//   | __|(_) _ _   __ _ | | |_  )|   \  | _ \ __ _  ___ ___
//   | _| | || ' \ / _` || |  / / | |) | |  _// _` |(_-<(_-<
//   |_|  |_||_||_|\__,_||_| /___||___/  |_|  \__,_|/__//__/
//                                                          	
// Here, we read from the off screen buffer, the one that 
// has all the lighting, etc. 
// This is where we can do the "2nd pass effects", so the 
// full-screen 2D effects.
//
// NOTE: In this example, we are only using this to render to an offscreen object

		// Now the final pass (in this case, only rendering to a quad)
		//RENDER_PASS_2_FULL_SCREEN_EFFECT_PASS
///////	glBindFramebuffer(GL_FRAMEBUFFER, 0);
///////
///////	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
///////
///////	::g_pShaderManager->useShaderProgram("mySexyShader");
///////
///////	glUniform1i(renderPassNumber_LocID, RENDER_PASS_2_FULL_SCREEN_EFFECT_PASS );
///////
///////	/// The "deferred pass" FBO has a colour texture with the entire rendered scene
///////	/// (including lighting, etc.)
///////	GLint fullRenderedImage2D_LocID = glGetUniformLocation(sexyShaderID, "fullRenderedImage2D");
///////
///////	/// Pick a texture unit... 
///////	unsigned int pass2unit = 50;
///////	glActiveTexture( GL_TEXTURE0 + pass2unit);
///////	glBindTexture(GL_TEXTURE_2D, ::g_FBO_Pass2_Deferred.colourTexture_0_ID);
///////	glUniform1i(fullRenderedImage2D_LocID, pass2unit);
///////
///////
///////	std::vector< cGameObject* >  vecCopySingleLonelyQuad;
///////	/// Push back a SINGLE quad or GIANT triangle that fills the entire screen
///////	vecCopySingleLonelyQuad.push_back( ::g_ExampleTexturedQuad );
///////
///////	cGameObject* pTheShip = findObjectByFriendlyName( "NCC-1701", ::g_vecGameObjects );
///////	vecCopySingleLonelyQuad.push_back( pTheShip );
///////
///////	RenderScene(vecCopySingleLonelyQuad, ::g_pGLFWWindow, deltaTime);





//		RenderScene(::g_vecTransparentObjectsOnly, ::g_pGLFWWindow, deltaTime);

		std::stringstream ssTitle;
//		ssTitle << "Camera (xyz): " 
//			<< g_cameraXYZ.x << ", " 
//			<< g_cameraXYZ.y << ", " 
//			<< g_cameraXYZ.z;
		// 
//		glm::vec4 EulerAngle;
		//g_pTheCamera->qOrientation.eularAngles(EulerAngle);
		//ssTitle << "Camera (xyz): "
		//	<< EulerAngle.x << ", "
		//	<< EulerAngle.y << ", "
		//	<< EulerAngle.z;

		glfwSetWindowTitle( ::g_pGLFWWindow, ssTitle.str().c_str() );

		// "Presents" what we've drawn
		// Done once per scene 
        glfwSwapBuffers(::g_pGLFWWindow);
        glfwPollEvents();


    }// while ( ! glfwWindowShouldClose(window) )


    glfwDestroyWindow(::g_pGLFWWindow);
    glfwTerminate();

	// 
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;


	

//    exit(EXIT_SUCCESS);
	return 0;
}


void DrawDebugLightingSpheres(void)
{
	//DEBUG sphere
	::g_pDebugRenderer->addDebugSphere( glm::vec3( 0.0f, 0.0f, 0.0f ), 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), 1.0f );
	// Light at 95% 
	float scaleAt99 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.99f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt99 );

	// Light at 50% 
	float scaleAt50 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.5f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt50 );

	// Light at 25% 
	float scaleAt25 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.25f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt25 );

	// Light at 1% 
	float scaleAt01 = ::g_pLightManager->vecLights[0].calcApproxDistFromAtten( 0.01f );	
	::g_pDebugRenderer->addDebugSphere( ::g_pLightManager->vecLights[0].position, 
										glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f ), scaleAt01 );
	return;
}



void setWindowFullScreenOrWindowed( GLFWwindow* pTheWindow, bool IsFullScreen )
{
	// Set full-screen or windowed
	if ( ::g_IsWindowFullScreen )
	{	
		// Find the size of the primary monitor
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode( pPrimaryScreen );
		// Set this window to full screen, matching the size of the monitor:
		glfwSetWindowMonitor( pTheWindow, pPrimaryScreen, 
							  0, 0,				// left, top corner 
							  pPrimMonVidMode->width, pPrimMonVidMode->height, 
							  GLFW_DONT_CARE );	// refresh rate

		std::cout << "Window now fullscreen at ( " 
			<< pPrimMonVidMode->width << " x " 
			<< pPrimMonVidMode->height << " )" << std::endl;
	}
	else
	{
		// Make the screen windowed. (i.e. It's CURRENTLY full-screen)
		// NOTE: We aren't saving the "old" windowed size - you might want to do that...
		// HACK: Instead, we are taking the old size and mutiplying it by 75% 
		// (the thinking is: the full-screen switch always assumes we want the maximum
		//	resolution - see code above - but when we make that maximum size windowed,
		//  it's going to be 'too big' for the screen)
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode( pPrimaryScreen );

		// Put the top, left corner 10% of the size of the full-screen
		int topCornerTop = (int)( (float)pPrimMonVidMode->height * 0.1f );
		int topCornerLeft = (int)( (float)pPrimMonVidMode->width * 0.1f );
		// Make the width and height 75% of the full-screen resolution
		int height = (int)( (float)pPrimMonVidMode->height * 0.75f );
		int width = (int)( (float)pPrimMonVidMode->width * 0.75f );

		glfwSetWindowMonitor( pTheWindow, NULL,		// This NULL makes the screen windowed
							  topCornerLeft, topCornerTop,				// left, top corner 
							  width, height, 
							  GLFW_DONT_CARE );	// refresh rate

		std::cout << "Window now windowed at ( " 
			<< width << " x " << height << " )" 
			<< " and offset to ( "
			<< topCornerLeft << ", " << topCornerTop << " )"
			<< std::endl;
	}
	return;
}
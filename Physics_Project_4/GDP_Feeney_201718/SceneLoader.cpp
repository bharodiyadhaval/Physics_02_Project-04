// This file is used to laod the models
#include "cGameObject.h"
#include <vector>
#include "Utilities.h"		// getRandInRange()
#include <glm/glm.hpp>
#include "globalGameStuff.h"

// For the cSimpleAssimpSkinnedMeshLoader class
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

#include "cAnimationState.h"

extern std::vector< cGameObject* >  g_vecGameObjects;
extern cGameObject* g_pTheDebugSphere;

extern cGameObject* g_ExampleTexturedQuad;


//const float SURFACEOFGROUND = -10.0f;
//const float RIGHTSIDEWALL = 15.0f;
//const float LEFTSIDEWALL = -15.0f;



void LoadModelsIntoScene(void)
{


	//{	// Teapot #2
	//	cGameObject* pTempGO = new cGameObject();
	//	pTempGO->friendlyName = "myplane";
	//	cPhysicalProperties physState;
	//	physState.position.x = 0.0f;
	//	physState.integrationUpdateType = cPhysicalProperties::ePhysicsIntegrationUpdateTypes::EXCLUDED_FROM_INTEGRATION;
	//	pTempGO->SetPhysState(physState);
	//	sMeshDrawInfo meshInfo;
	//	meshInfo.scale = 60.0f;
	//	meshInfo.name = "planexy";
	//	meshInfo.globalAlpha = 0.75f;
	//	meshInfo.bUseDebugColour = 0;
	//	//	meshInfo.bDrawAsWireFrame = 1;
	//	meshInfo.setMeshOrientationEulerAngles(glm::vec3(-1.56f, 0.0f, 0.0f));
	//	meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.5f));
	//	meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 0.5f));
	//	pTempGO->vecMeshes.push_back(meshInfo);
	//	//
	//	::g_vecGameObjects.push_back(pTempGO);		// Fastest way to add

	//}





	{// 2 sided quad
		::g_ExampleTexturedQuad = new cGameObject();
		::g_ExampleTexturedQuad->friendlyName = "quad";
		cPhysicalProperties physState;
		physState.position = glm::vec3(100.0f, 100.0, -250.0f);
		physState.setOrientationEulerAngles(glm::vec3(90.0f, 0.0, 0.0f));
		::g_ExampleTexturedQuad->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 75;
		meshInfo.setMeshOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 0.0f));
		//		meshInfo.debugDiffuseColour = glm::vec4( 0.0f, 1.0f, 0.0f, 1.0f );
		//		meshInfo.bDrawAsWireFrame = true;
		meshInfo.name = "2SidedQuad";
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("GuysOnSharkUnicorn.bmp", 0.0f));
		meshInfo.vecMehs2DTextures.push_back(sTextureBindBlendInfo("Utah_Teapot_xyz_n_uv_Enterprise.bmp", 1.0f));
		::g_ExampleTexturedQuad->vecMeshes.push_back(meshInfo);
		//::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}



	
	

	// Our skybox object
	{
		//cGameObject* pTempGO = new cGameObject();
		::g_pSkyBoxObject = new cGameObject();
		cPhysicalProperties physState;
		::g_pSkyBoxObject->SetPhysState(physState);
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 10000.0f;
		meshInfo.debugDiffuseColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		meshInfo.name = "SmoothSphereRadius1InvertedNormals";			
		meshInfo.vecMeshCubeMaps.push_back(sTextureBindBlendInfo("space", 1.0f));
		meshInfo.bIsSkyBoxObject = true;
		::g_pSkyBoxObject->vecMeshes.push_back(meshInfo);
		// IS SKYBOX
		::g_vecGameObjects.push_back(::g_pSkyBoxObject);		// Fastest way to add
	}

	
	return;
}

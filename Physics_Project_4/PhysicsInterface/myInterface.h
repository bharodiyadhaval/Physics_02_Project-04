#pragma once
//#include"structs.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>			
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>		
#include <string>
#include <vector>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include<iostream>
#include<vector>

class myInterface
{
public: 

	virtual ~myInterface() {}
	struct force
	{
		int on;
		float strength;
		glm::vec3 direction;

	};

	enum eTypeOfObject
	{	
		SPHERE = 0,		
		PLANE = 1,		
		CAPSULE = 2,    
		AABB = 3,		
		UNKNOWN = 99	
	};

	struct gameObject
	{
		std::string typer;
		glm::vec3 pos;
		glm::vec3 ori;
		glm::vec3 or2;
		float scale;
		glm::vec4 color;
		std::string name;
		eTypeOfObject type;
		bool phys;
		glm::vec3 vel = glm::vec3(0.0f, 0.0f, 0.0f);
	};

	
	virtual void ReadFileToToken(std::ifstream &file, std::string token) = 0;
	virtual std::vector<gameObject> InitializePhysics(std::string file) = 0;
	virtual void PhysicsStep(double deltaTime, force &wforce, force &sforce, force &leftforce, force &rightforce, force &upforce, std::vector<glm::vec3> &NewPosition, std::vector<glm::quat> &NewOrientation, std::vector<gameObject> &ObjectInfoVector, std::vector<int> &collision) = 0;
};
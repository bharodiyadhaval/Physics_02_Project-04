#include"myBulletPhysics.h"



int slideforce = 15;
int slideforce2 = 10;

void myBulletPhysics::ReadFileToToken(std::ifstream &file, std::string token)
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



std::vector<myBulletPhysics::gameObject> myBulletPhysics::InitializePhysics(std::string file)
{

	//Physics Bullet Initializer
	//Adds the objects to the world, 
	//Takes all the information required from a text file

	//Also, Adds the constraints
	
	dynamicsWorld->setGravity(btVector3(0, -100, 0));


	//Ground will automatically be defined using the textfile which has got a plane in it used for ground

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//GROUND SHAPE DEFINITION
	//btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	//btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	//btRigidBody::btRigidBodyConstructionInfo  groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	//btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	//dynamicsWorld->addRigidBody(groundRigidBody);


	//BACKWALL SHAPE DEFINITION
	btCollisionShape* BackWallShape = new btStaticPlaneShape(btVector3(0, 0, 1), 0);
	btDefaultMotionState* BackWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -500)));
	btRigidBody::btRigidBodyConstructionInfo  BackWallRigidBodyCI(0, BackWallMotionState, BackWallShape, btVector3(0, 0, 0));
	btRigidBody* BackWallRigidBody = new btRigidBody(BackWallRigidBodyCI);
	dynamicsWorld->addRigidBody(BackWallRigidBody);

	//LeftWall SHAPE DEFINITION
	btCollisionShape* LeftWallShape = new btStaticPlaneShape(btVector3(1, 0, 0), 0);
	btDefaultMotionState* LeftWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-500, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo  LeftWallRigidBodyCI(0, LeftWallMotionState, LeftWallShape, btVector3(0, 0, 0));
	btRigidBody* LeftWallRigidBody = new btRigidBody(LeftWallRigidBodyCI);
	dynamicsWorld->addRigidBody(LeftWallRigidBody);

	//RightWall SHAPE DEFINITION
	btCollisionShape* RightWallShape = new btStaticPlaneShape(btVector3(-1, 0, 0), 0);
	btDefaultMotionState* RightWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(500, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo  RightWallRigidBodyCI(0, RightWallMotionState, RightWallShape, btVector3(0, 0, 0));
	btRigidBody* RightWallRigidBody = new btRigidBody(RightWallRigidBodyCI);
	dynamicsWorld->addRigidBody(RightWallRigidBody);

	//FRONTWALL SHAPE DEFINITION
	btCollisionShape* FrontWallShape = new btStaticPlaneShape(btVector3(0, 0, -1), 0);
	btDefaultMotionState* FrontWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 500)));
	btRigidBody::btRigidBodyConstructionInfo  FrontWallRigidBodyCI(0, FrontWallMotionState, FrontWallShape, btVector3(0, 0, 0));
	btRigidBody* FrontWallRigidBody = new btRigidBody(FrontWallRigidBodyCI);
	dynamicsWorld->addRigidBody(FrontWallRigidBody);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	std::vector <gameObject> BulletPhysicsVector;
	gameObject myobj;
	std::ifstream plyFile(file.c_str());

	if (!plyFile.is_open())
	{	// Didn't open file, so return

	}




	//Load everything from the file
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ReadFileToToken(plyFile, "models");
	int count;
	plyFile >> count;

	for (int i = 1; i <= count; i++)
	{

		ReadFileToToken(plyFile, std::to_string(i));
		ReadFileToToken(plyFile, "position");
		plyFile >> myobj.pos.x;
		plyFile >> myobj.pos.y;
		plyFile >> myobj.pos.z;

		
		ReadFileToToken(plyFile, "orientation2");
		plyFile >> myobj.or2.x;
		plyFile >> myobj.or2.y;
		plyFile >> myobj.or2.z;

		ReadFileToToken(plyFile, "scale");
		plyFile >> myobj.scale;


		ReadFileToToken(plyFile, "name");
		plyFile >> myobj.name;

		ReadFileToToken(plyFile, "type");
		plyFile >> myobj.typer;
		if (myobj.typer == "sphere")
		{
			myobj.type = eTypeOfObject::SPHERE;
		}
		else
		{
			myobj.type = eTypeOfObject::PLANE;
		}

		ReadFileToToken(plyFile, "physics");
		plyFile >> myobj.phys;


		if (myobj.typer == "plane")
		{
			masses.push_back(0);
			PhysicsObjectShapes.push_back(new btStaticPlaneShape(btVector3(0, 2, 0), 1));
			
		}
		else if(myobj.typer == "sphere")
		{
			masses.push_back(myobj.scale);
			PhysicsObjectShapes.push_back(new btSphereShape(myobj.scale));
		}
		else if (myobj.typer == "cone")
		{
			masses.push_back(myobj.scale);
			PhysicsObjectShapes.push_back(new btConeShape(20, 25));
		}
		else
		{
			masses.push_back(myobj.scale);
			//PhysicsObjectShapes.push_back(new btSphereShape(myobj.scale + 10));
			PhysicsObjectShapes.push_back(new btBoxShape(btVector3(myobj.scale , myobj.scale , myobj.scale )));

		}
	
		
		motionstates.push_back(new btDefaultMotionState((btTransform(btQuaternion(0, 0, 0, 1), btVector3(myobj.pos.x, myobj.pos.y, myobj.pos.z)))));
		ballInertias.push_back(btVector3(0,0,0));
		
		BulletPhysicsVector.push_back(myobj);

	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//Create Rigid Bodies

	for (int i = 0; i < BulletPhysicsVector.size(); i++)
	{

		PhysicsObjectShapes[i]->calculateLocalInertia(masses[i], ballInertias[i]);
		rigidbodies.push_back(new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(masses[i], motionstates[i], PhysicsObjectShapes[i], ballInertias[i])));
	}

	for (int i = 0; i < rigidbodies.size(); i++)
	{
		dynamicsWorld->addRigidBody(rigidbodies[i]);

	}




	//ADD THE CONSTRAINTS
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	btTransform Identitytrans;
	Identitytrans.setIdentity();
	btVector3 nodeinertia;
	PhysicsObjectShapes[1]->calculateLocalInertia(0, nodeinertia);
	rigidbodies[1]->setMassProps(0, nodeinertia);
	//rigidbodies[2]->getMotionState()->getWorldTransform(pos);


	//1
	//Ball SOCKET
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	btPoint2PointConstraint* ballchain1 = new btPoint2PointConstraint(*rigidbodies[1], *rigidbodies[2], btVector3(0, 0, 0), btVector3(10, 10, 0));
	btPoint2PointConstraint* ballchain2 = new btPoint2PointConstraint(*rigidbodies[2], *rigidbodies[3], btVector3(0, 0, 0), btVector3(10, 10, 0));
	btPoint2PointConstraint* ballchain3 = new btPoint2PointConstraint(*rigidbodies[3], *rigidbodies[4], btVector3(0, 0, 0), btVector3(10, 10, 0));
	

	//2
	//HINGE
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	btHingeConstraint* door1 = new btHingeConstraint(*rigidbodies[8], btVector3(-20, 20, -20), btVector3(0, 1, 0), false);
	

	//3
	//Slider
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	btTransform sliderT;
	sliderT = btTransform::getIdentity();

	//3 Part 2
	//Slider no. 2
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	btTransform sliderT2;
	sliderT2 = btTransform::getIdentity();
	sliderT2.getBasis().setEulerZYX(0, 3.14 / 2, 0);
	btSliderConstraint* SlideTheBlock = new btSliderConstraint(*rigidbodies[7], sliderT, false);
	btSliderConstraint* SlideTheBlock2 = new btSliderConstraint(*rigidbodies[6], sliderT2, false);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//4
	//Six degrees of freedom, I've limited the cone into a virtual cube which has got the dimension of 100 x 100  and the height is 200 units
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	btGeneric6DofConstraint* sixDOF = new btGeneric6DofConstraint(*rigidbodies[5], sliderT, false);
	sixDOF->setLinearLowerLimit(btVector3(-50, 0, -50));
	sixDOF->setLinearUpperLimit(btVector3(50, 200, 50));
	
	//ENABLE ALL THE CONSTRAINTS
	//////////////////////////////////////////////////
	dynamicsWorld->addConstraint(ballchain1, false);
	dynamicsWorld->addConstraint(ballchain2, false);
	dynamicsWorld->addConstraint(ballchain3, false);
	dynamicsWorld->addConstraint(door1, false);
	dynamicsWorld->addConstraint(SlideTheBlock, false);
	dynamicsWorld->addConstraint(SlideTheBlock2, false);
	dynamicsWorld->addConstraint(sixDOF, false);

	



	return BulletPhysicsVector;

	




	
}






void myBulletPhysics::PhysicsStep(double deltaTime, force &wforce, force &sforce, force &leftforce, force &rightforce, force &upforce, std::vector<glm::vec3> &disp, std::vector<glm::quat> &NewOrientation, std::vector<gameObject> &ObjectInfoVector, std::vector<int> &collision)
{

	//Start the physics simulation
	dynamicsWorld->stepSimulation((float)deltaTime, 100);


	//Transforms and Orientation Variables
	btTransform trans;
	btQuaternion orient;
	btTransform trans2;
	float xr, yr, zy;


	

	//Apply the physics on all the objects in the vector
	for (int i = 0; i < rigidbodies.size(); i++)
	{
		
		rigidbodies[i]->getMotionState()->getWorldTransform(trans);
		orient = trans.getRotation();
		
		

		

		disp[i] = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
		ObjectInfoVector[i].pos = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

		


		//Keep the Sliders Sliding!!
		///////////////////////////////////////////////////////////////////////////////
		if (i == 7)
		{
			rigidbodies[7]->applyCentralImpulse(btVector3(slideforce, 0, 0));
			if (disp[7].x > 200.0f )
			{
				slideforce = -15;

			}
			if (disp[7].x < -100.0f)
			{
				slideforce = 15;
			}
		
		}

		if (i == 6)
		{
			rigidbodies[6]->applyCentralImpulse(btVector3(0, 0, slideforce2));
			if (disp[6].z > 200.0f)
			{
				slideforce2 = -10;

			}
			if (disp[6].z < -200.0f)
			{
				slideforce2 = 10;
			}

		}
		///////////////////////////////////////////////////////////////////////////////



		//No rotation for the Cone
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (ObjectInfoVector[i].typer != "plane" && ObjectInfoVector[i].typer != "cone")
		{
			
			NewOrientation[i] = glm::quat( (float)trans.getRotation().getW(), (float)trans.getRotation().getX(), (float)trans.getRotation().getY(), (float)trans.getRotation().getZ() ) ;
			//ObjectInfoVector[i].or2 = glm::vec3(orient.x(), orient.y(), orient.z());
			//NewOrientation[i] = (glm::cross(glm::vec3(0,1,0), disp[i]))/8.0f;
		}
		else
		{
			//Do something Something Just in case
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		

		//Move the Ball with the Key presses, Smooth and sturdy
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (wforce.on == 1)
		{
			
			rigidbodies[0]->applyCentralImpulse( btVector3(wforce.strength * wforce.direction.x,0, wforce.strength * wforce.direction.z));
		}
		if (rightforce.on == 1)
		{

			rigidbodies[0]->applyCentralImpulse(btVector3(rightforce.strength *rightforce.direction.x, 0, rightforce.strength *rightforce.direction.z) );
		}
		if (leftforce.on == 1)
		{

			rigidbodies[0]->applyCentralImpulse(btVector3(leftforce.strength *leftforce.direction.x, 0, leftforce.strength * leftforce.direction.z) );
		}
		if (sforce.on == 1)
		{

			rigidbodies[0]->applyCentralImpulse(btVector3(sforce.strength *sforce.direction.x, 0, sforce.strength *sforce.direction.z) );
		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		std::cout << ObjectInfoVector[i].pos.y<<std::endl;

	}

	int i = 1;
	
	
	//Sleep(50);





	return;
}




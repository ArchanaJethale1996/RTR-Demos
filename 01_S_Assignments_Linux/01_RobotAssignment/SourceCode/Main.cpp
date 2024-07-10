#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<cstring>
#include<X11/Xlib.h>			//smilar to windows.h
#include<X11/Xutil.h>			//XVisualInfo
#include<X11/XKBlib.h>		//keyboard utilization header
#include<X11/keysym.h>		//key symbol


//openGL
#include<GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>

//OpenAL
#include <AL/al.h>
#include <AL/alc.h>
#include <audio/wave.h>

//Timer
#include <time.h>    
#include <math.h>    

#include"vmath.h"
using namespace std;
using namespace vmath;


enum
{
	AMC_ATTRIBUTE_POSITION=0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0
};

//Our headers
#include"Mesh.h"
#include"PrimitiveShape.h"
#include"List.h"
#include"3DShape.h"

bool bFullScreen=false;
Display *gpDisplay=NULL;
XVisualInfo *gpXVisualInfo=NULL;

Colormap gColormap;
Window gWindow;
int giWindowWidth=800;
int giWindowHeight=600;
GLXContext gGLXContext;
typedef GLXContext (*GLXCreateContextAttribsARBProc)(Display *,GLXFBConfig,GLXContext,Bool,const int *);
GLXCreateContextAttribsARBProc gGLXCreateContextAttribsARB=NULL;
GLXFBConfig gGlXFBConfig;
GLXContext gGlXContext;
FILE *gpFile = NULL;


//Shapes
PrimitiveShape *cylinder,*cube,*cone,*cudoid,*pyramid,*frustrum,*sphere,*disc;
Mesh *cylinderMesh, *cubeMesh, *coneMesh, *cudoidMesh, *pyramidMesh, *frustrumMesh, *sphereMesh, *discMesh;

int slices = 30, stacks = 30;
float radius = 0.5f;

int slicesCylinder = 30, stacksCylinder = 30;
float radiusCylinder = 0.5f,radius2Cylinder=0.5f,heightCylinder =0.6f;

int slicesCone = 30, stacksCone = 30;
float radiusCone = 0.5f, radius2Cone = 0.5f, heightCone = 0.6f;


int slicesFrustrum = 30, stacksFrustrum = 30;
float radiusFrustrum = 0.5f, radius2Frustrum = 0.2f, heightFrustrum = 0.6f;

//Shaders
GLuint gVertexShaderObjectPerVertex;
GLuint gFragmentShaderObjectPerVertex;
GLuint gShaderProgramObjectPerVertex;

GLuint gVertexShaderObjectPerFragment;
GLuint gFragmentShaderObjectPerFragment;
GLuint gShaderProgramObjectPerFragment;


GLuint mUniformPerVertex, vUniformPerVertex, pUniformPerVertex, LDUniformPerVertex, KDUniformPerVertex, LAUniformPerVertex, KAUniformPerVertex, KSUniformPerVertex, LSUniformPerVertex, LightPositionUniformPerVertex, MaterialShininessUniformPerVertex, LKeyIsPressedUniformPerVertex;
GLuint mUniformPerFragment, vUniformPerFragment, pUniformPerFragment, LDUniformPerFragment, KDUniformPerFragment, LAUniformPerFragment, KAUniformPerFragment, KSUniformPerFragment, LSUniformPerFragment, LightPositionUniformPerFragment, MaterialShininessUniformPerFragment, LKeyIsPressedUniformPerFragment;
mat4 perspectiveProjectionMatrix;


//uniform values
float lightAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
float lightDiffused[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightPosition[4] = { 100.0f,100.0f,100.0f,1.0f };



//animated 
int gLighting = 0;

bool gFragment = true;

//Martix Stack
list_t *modelStack,*fingerStack; 

//Robot Variables
float neckX = 0.0f, neckY = 0.0f, neckZ = 0.0f;
float shoulderRightX = 0.0f, shoulderRightY = 0.0f, shoulderRightZ = 0.0f, elbowRightX = 0.0f, elbowRightY = 0.0f, elbowRightZ = 0.0f;
float shoulderLeftX = 0.0f, shoulderLeftY = 0.0f, shoulderLeftZ = 0.0f, elbowLeftX = 0.0f, elbowLeftY = 0.0f, elbowLeftZ = 0.0f;
float palmLeftRotateX = 0.0f, palmLeftRotateY = 0.0f, palmLeftRotateZ = 0.0f;
float palmRightRotateX = 0.0f, palmRightRotateY = 0.0f, palmRightRotateZ = 0.0f;
float finger1LeftRotatePart1 = 0.0f, finger2LeftRotatePart1 = 0.0f, finger3LeftRotatePart1 = 0.0f, finger4LeftRotatePart1 = 0.0f, finger5LeftRotatePart1 = 0.0f;
float finger1LeftRotateY = 0.0f, finger2LeftRotateY = 0.0f, finger3LeftRotateY = 0.0f, finger4LeftRotateY = 0.0f, finger5LeftRotateY = 0.0f;
float finger1LeftRotatePart2 = 0.0f, finger2LeftRotatePart2 = 0.0f, finger3LeftRotatePart2 = 0.0f, finger4LeftRotatePart2 = 0.0f, finger5LeftRotatePart2 = 0.0f;

float thaiRightX = 0.0f, thaiRightY = 0.0f, thaiRightZ = 0.0f, legRightX = 0.0f, legRightY = 0.0f, legRightZ = 0.0f;
float thaiLeftX = 0.0f, thaiLeftY = 0.0f, thaiLeftZ = 0.0f, legLeftX = 0.0f, legLeftY = 0.0f, legLeftZ = 0.0f;
float legFingerLeftRotateX = 0.0f, legFingerLeftRotateY = 0.0f, legFingerLeftRotateZ = 0.0f;
float legFingerRightRotateX = 0.0f, legFingerRightRotateY = 0.0f, legFingerRightRotateZ = 0.0f;


float bodyRotationX = 0.0f, bodyRotationX1 = 0.0f, bodyRotationY = 0.0f, bodyRotationZ = 0.0f, bodyRotationZ1 = 0.0f;
float bodyTranslationX = 0.0f, bodyTranslationY = 0.0f, bodyTranslationZ = 0.0f;
float bodySwimTranslationX=0.0f, bodySwimTranslationY=0.0f, bodySwimTranslationZ =0.0f;
bool showDiamond = false;
int Action = 0;
//Walking
bool leg1Part1Over = false, leg1Over = false;
bool leg2Part1Over = false, leg2Over = false;

float walkingDirectionX=0.1f;
float walkingDirectionZ = 0.1f;

//skipping
bool skippingStartJumping = false,skip1Over=false,skippingToggleJumping=false;

//squatting
bool sitting = false;

//Swimming
bool hand1Over = false;
bool hand2Over = false;
bool start1 = false;

//assemble/disassemble
bool assemble = false;

float leftHandPos = 0.0f, rightHandPos = 0.0f;
float chestPos = 0.0f;
float leftLegPos = 0.0f, rightLegPos = 0.0f;
float headPos = 0.0f;
float diamondPos = 0.0f;
//Mode
bool interupptedMode = false;
clock_t uninterupptedMode_StartTime, uninterupptedMode_CurrentTime;
bool startedUninterupptedMode = false;
float extremeTime = 0.0f;
bool doneStep1 = false, doneStep2 = false, doneStep3 = false, doneStep4 = false, doneStep5 = false, doneStep6 = false,doneStep7 = false;
bool gAnimateChest = false,gAnimateHead = false,gAnimateHands = false,gAnimateLegs = false;
bool gAnimateChestDetach = false,gAnimateHeadDetach = false,gAnimateHandsDetach = false,gAnimateLegsDetach = false;

//OpenAL 
ALCdevice *device;
ALCcontext *context;
ALuint source;
ALuint buffer;

ALuint sourceInterupptedMode;
ALuint bufferInterupptedMode;

//clock
clock_t currentTime,previousTime;

//Const values
float diamondPosConst = 12.0f;
float chestPosConst = -15.0f;
float leftLegPosConst = -15.0f, rightLegPosConst = -15.0f;
float leftHandPosConst = -22.0f, rightHandPosConst = 22.0f;
float headPosConst = 15.0f;
bool gEyesLight=false;
bool gChestDisplay=false;
float walkingSpeed=0.5f;
bool MildActionStart=false;

/// Stack Using Linked List
list_t* CreateStack(void)
{
	return CreateList();
}

int PushMatrix(list_t *pList, data_t new_data)
{
	return InsertAtBegining(pList, new_data);
}

int PopMatrix(list_t *pList, data_t *pData)
{
	return ExamineAndDeleteBegining(pList, pData);
}

int DeleteStack(list_t **ppList)
{
	return DestroyList(ppList);
}

//Our Function
void InitializeInteruuptedMode()
{	
	//if(interupptedMode == true)
	{
		alSourcePlay(sourceInterupptedMode);
		alSourceStop(source);
		gLighting=1;
	}	
}
void walkingCalc()
{
	if(Action!=4)
	{
		if (bodyRotationY >= 0.0 && bodyRotationY < 90.0f)
		{
			walkingDirectionZ = 1.0f * fabs(cos(bodyRotationY)*walkingSpeed);
			walkingDirectionX = 1.0f * fabs(sin(bodyRotationY)*walkingSpeed);
		}
		else if (bodyRotationY >= 90.0 && bodyRotationY < 180.0f)
		{
			walkingDirectionZ = -1.0f*fabs(cos(bodyRotationY)*walkingSpeed);
			walkingDirectionX = 1.0f * fabs(sin(bodyRotationY)*walkingSpeed);
		}
		else if (bodyRotationY >= 180.0 && bodyRotationY < 270.0f)
		{
			walkingDirectionZ = -1.0f *fabs(cos(bodyRotationY)*walkingSpeed);
			walkingDirectionX = -1.0f*fabs(sin(bodyRotationY)*walkingSpeed);
		}
		else if (bodyRotationY >= 270.0f && bodyRotationY <= 360.0f)
		{
			walkingDirectionZ = 1.0f*fabs(cos(bodyRotationY)*walkingSpeed);
			walkingDirectionX = -1.0f*fabs(sin(bodyRotationY)*walkingSpeed);
		}

	}
	else
	{
		if (bodyRotationZ >= 0.0 && bodyRotationZ < 90.0f)
		{
			walkingDirectionZ = -1.0f*fabs(sin(bodyRotationZ)*walkingSpeed);
			walkingDirectionX = -1.0f*fabs(cos(bodyRotationZ)*walkingSpeed);
		}
		else if (bodyRotationZ >= 90.0 && bodyRotationZ < 180.0f)
		{
			walkingDirectionZ = -1.0f*fabs(sin(bodyRotationZ)*walkingSpeed);
			walkingDirectionX = 1.0f * fabs(cos(bodyRotationZ)*walkingSpeed);
		}
		else if (bodyRotationZ >= 180.0 && bodyRotationZ < 270.0f)
		{
			walkingDirectionZ = 1.0f *fabs(sin(bodyRotationZ)*walkingSpeed);
			walkingDirectionX = +1.00f*fabs(cos(bodyRotationZ)*walkingSpeed);;
		}
		else if (bodyRotationZ >= 270.0f && bodyRotationZ < 360.0f)
		{
			walkingDirectionZ = 1.0f*fabs(sin(bodyRotationZ)*walkingSpeed);
			walkingDirectionX = -1.0f*fabs(cos(bodyRotationZ)*walkingSpeed);
		}
		
		
	}
}
float prevAction=0;
void InitializeDefaultValueForRobotParts()
{
	neckX = 0.0f; neckY = 0.0f; neckZ = 0.0f;
	shoulderRightX = 0.0f; shoulderRightY = 0.0f; shoulderRightZ = 0.0f; elbowRightX = 0.0f; elbowRightY = 0.0f; elbowRightZ = 0.0f;
	shoulderLeftX = 0.0f; shoulderLeftY = 0.0f; shoulderLeftZ = 0.0f; elbowLeftX = 0.0f; elbowLeftY = 0.0f; elbowLeftZ = 0.0f;
	palmLeftRotateX = 0.0f; palmLeftRotateY = 0.0f; palmLeftRotateZ = 0.0f;
	palmRightRotateX = 0.0f; palmRightRotateY = 0.0f, palmRightRotateZ = 0.0f;
	finger1LeftRotatePart1 = 0.0f; finger2LeftRotatePart1 = 0.0f; finger3LeftRotatePart1 = 0.0f; finger4LeftRotatePart1 = 0.0f; finger5LeftRotatePart1 = 0.0f;
	finger1LeftRotateY = 0.0f; finger2LeftRotateY = 0.0f; finger3LeftRotateY = 0.0f; finger4LeftRotateY = 0.0f; finger5LeftRotateY = 0.0f;
	finger1LeftRotatePart2 = 0.0f; finger2LeftRotatePart2 = 0.0f; finger3LeftRotatePart2 = 0.0f; finger4LeftRotatePart2 = 0.0f; finger5LeftRotatePart2 = 0.0f;

	thaiRightX = 0.0f; thaiRightY = 0.0f; thaiRightZ = 0.0f; legRightX = 0.0f; legRightY = 0.0f; legRightZ = 0.0f;
	thaiLeftX = 0.0f; thaiLeftY = 0.0f; thaiLeftZ = 0.0f; legLeftX = 0.0f; legLeftY = 0.0f; legLeftZ = 0.0f;
	legFingerLeftRotateX = 0.0f; legFingerLeftRotateY = 0.0f; legFingerLeftRotateZ = 0.0f;
	legFingerRightRotateX = 0.0f; legFingerRightRotateY = 0.0f; legFingerRightRotateZ = 0.0f;
	if(Action==4){
		bodyRotationX = 0.0f, bodyRotationX1 = 0.0f, bodyRotationY = 0.0f, bodyRotationZ = 0.0f, bodyRotationZ1 = 0.0f;
	}
	if(prevAction!=Action){	
		bodyRotationX = 0.0f, bodyRotationX1 = 0.0f, bodyRotationY = 0.0f, bodyRotationZ = 0.0f, bodyRotationZ1 = 0.0f;
		bodyTranslationX = 0.0f, bodyTranslationY = 0.0f, bodyTranslationZ = 0.0f;
		prevAction=Action;
	}
	leftHandPos = 0.0f; rightHandPos = 0.0f;
	chestPos = 0.0f;
	leftLegPos = 0.0f; rightLegPos = 0.0f;
	headPos = 0.0f;
	diamondPos = 0.0f;

	leg1Part1Over = false, leg1Over = false;
	leg2Part1Over = false, leg2Over = false;

	//skipping
	skippingStartJumping = false, skip1Over = false, skippingToggleJumping = false;

	//Swimming
	hand1Over = false;
	hand2Over = false;
	start1 = false;
	bodySwimTranslationX=0.0f, bodySwimTranslationY=0.0f, bodySwimTranslationZ =0.0f;
	
	//Swimming
	if (Action == 4)
	{
		shoulderLeftY = -180.0f;
		shoulderRightY = -180.0f;	
	}
	if (Action == 5)
	{
		shoulderLeftY = -90.0f;
		shoulderRightY = -90.0f;
	}
	if (Action == 6)
	{
		if (assemble == true)
		{
			diamondPos = diamondPosConst;
			chestPos = chestPosConst;
			leftLegPos = leftLegPosConst, rightLegPos = rightLegPosConst;
			leftHandPos = leftHandPosConst, rightHandPos = rightHandPosConst;
			headPos = headPosConst;
		}
			
	}
	if(interupptedMode==true)
		{
			gAnimateChest = true;
			gAnimateHead = true;
			gAnimateHands = true;
			gAnimateLegs = true;
			gAnimateChestDetach = true;
			gAnimateHeadDetach = true;
			gAnimateHandsDetach = true;
			gAnimateLegsDetach = true;
			gEyesLight=true;
			gChestDisplay=true;
			showDiamond = true;
		}	
	walkingCalc();
}

void update(void)
{
	//interupptedMode
	if (interupptedMode == false)
	{
		if (startedUninterupptedMode == false)
		{
			uninterupptedMode_StartTime = clock();
			startedUninterupptedMode = true;
		}
		uninterupptedMode_CurrentTime = clock();
		clock_t timeSec = (float)(uninterupptedMode_CurrentTime - uninterupptedMode_StartTime) / CLOCKS_PER_SEC;
		if (timeSec>=0.0f && timeSec<50.0f)
		{
			assemble = true;
			Action = 6;
			if (doneStep1==false)
			{
				InitializeDefaultValueForRobotParts();
				doneStep1 = true;
				extremeTime = 15.0f;
			}
			if(timeSec==9.0f)
			{
				gChestDisplay=true;
				gAnimateChest = true;	
			}
			if(timeSec==12.0f)
			{
				gAnimateLegs = true;
				gAnimateHands = true;
			}
			if(timeSec==18.0f)
			{
				gAnimateHead = true;
			}
			if(timeSec==24.0f)
			{
				gLighting = 1;
			}
			
			if(timeSec==33.0f)
			{
				gEyesLight=true;
				showDiamond=true;	
			}
			
			
		}
		else if (timeSec >= 48.0f && timeSec<63.0f)
		{
						
			Action =1;
			if (doneStep2 == false)
			{
				InitializeDefaultValueForRobotParts();
				doneStep2 = true;
			}
			if (timeSec- 48.0f < (1 * (15.0f / 4)))
			{
				bodyRotationY = 0.0f;
				walkingSpeed=0.5f;
			}
			else if (timeSec- 48.0f < (2 * (15.0f / 4)))
			{
				bodyRotationY = 95.0f;
				walkingSpeed=0.8f;
			}
			else if (timeSec- 48.0f < (3 * (15.0f / 4)))
			{
				bodyRotationY = 185.0f;
				walkingSpeed=1.0f;
			}
			else if (timeSec- 48.0f < (4 * (15.0f / 4)))
			{
				bodyRotationY = 275.0f;
				walkingSpeed=1.0f;
			}
			

		}
		else if (timeSec >= 63.0f && timeSec<80.0f)
		{
			Action = 2;
			if (doneStep3 == false)
			{
				InitializeDefaultValueForRobotParts();
				doneStep3 = true;
			}
			if (timeSec - 63.0f < (1 * (15.0f / 4)))
			{
				bodyRotationY = 0.0f;
			}
			else if (timeSec - 63.0f  < (2 * (15.0f / 4)))
			{
				bodyRotationY = 95.0f;
			}
			else if (timeSec - 63.0f  < (3 * (15.0f / 4)))
			{
				bodyRotationY = 185.0f;
			}
			else if (timeSec- 63.0f < (4 * (15.0f / 4)))
			{
				bodyRotationY = 275.0f;
			}

		}
		
		else if (timeSec >= 78.0f && timeSec<110.0f)
		{
			Action = 3;
			if (doneStep4 == false)
			{
				InitializeDefaultValueForRobotParts();
				doneStep4 = true;
				extremeTime = 60.0f;
			}

			if (timeSec - 78.0f < (1 * (32.0f / 4)))
			{
				bodyRotationY = 0.0f;
			}
			else if (timeSec- 78.0f < (2 * (32.0f / 4)))
			{
				bodyRotationY = 95.0f;
			}
			else if (timeSec - 78.0f < (3 * (32.0f / 4)))
			{
				bodyRotationY = 185.0f;
			}
			else if (timeSec - 78.0f < (4 * (32.0f / 4)))
			{
				bodyRotationY = 275.0f;
			}
			walkingSpeed=1.5f;

		}
		else if (timeSec >= 110.0f && timeSec<125.0f)
		{
			Action = 4;
			if (doneStep5 == false)
			{
				InitializeDefaultValueForRobotParts();
				doneStep5 = true;
			}
			if (timeSec - 110.0f < (1 * (15.0f / 4)))
			{
				bodyRotationZ = 0.0f;
			}
			else if (timeSec - 110.0f < (2 * (15.0f / 4)))
			{	
				bodyRotationZ = 90.0f;
			}
			else if (timeSec - 110.0f < (3 * (15.0f / 4)))
			{				
				
				bodyRotationZ = 180.0f;
			}
			else if (timeSec- 110.0f < (4 * (15.0f / 4)))
			{
				
				bodyRotationZ = 275.0f;
			}

		}
		else if (timeSec >= 125.0f && timeSec<140.0f)
		{
			Action = 5;
			if (doneStep6 == false)
			{
				InitializeDefaultValueForRobotParts();
				doneStep6 = true;
			}
			if (timeSec - 125.0f < (1 * (15.0f / 4)))
			{
				bodyRotationY = 0.0f;
			}
			else if (timeSec - 125.0f < (2 * (15.0f / 4)))
			{
				bodyRotationY = 95.0f;
			}
			else if (timeSec - 125.0f < (3 * (15.0f / 4)))
			{
				bodyRotationY = 185.0f;
			}
			else if (timeSec- 125.0f < (4 * (15.0f / 4)))
			{
				bodyRotationY = 275.0f;
			}

		}
		else if (timeSec >= 140.0f && timeSec<170.0f)
		{
			if (timeSec == 140.0f)
			{
				assemble = false;
				Action = 6;
				if (doneStep7 == false)
				{
					InitializeDefaultValueForRobotParts();
					doneStep7 = true;
				}
				
			}
			if(timeSec==152.0f)
			{
				gEyesLight=false;
				showDiamond=false;
			}
			if(timeSec==153.0f)
			{
				gAnimateLegsDetach = true;
				gAnimateHandsDetach = true;
			}
			if(timeSec==155.0f)
			{
				gAnimateHeadDetach = true;
			}
			if(timeSec==157.0f)
			{
				gAnimateChestDetach = true;	
			}	
		}
		if(timeSec>170)
		{
			/*interupptedMode = true;
			assemble = true;
			Action = 6;
			InitializeInteruuptedMode();*/
			exit(0);
		}	
		walkingCalc();
	}
	
	
	//walking
	if (Action == 1)
	{
		//shoulderRightZ = (int)(shoulderRightZ + 3) % 360;
		//shoulderRightZ = (int)(shoulderRightZ - 3) % 360;
		if (leg1Part1Over == false)
		{
			thaiLeftY = (int)(thaiLeftY - 1) % 360;
			legLeftY = (int)(legLeftY + 2) % 360;
			shoulderLeftY = (int)(shoulderLeftY - 1) % 360;
			elbowLeftY = (int)(elbowLeftY - 1) % 360;
			if (thaiLeftY < -60.0f &&legLeftY > 120.0f && shoulderLeftY < -60.0f &&elbowLeftY < -60.0f &&leg1Part1Over == false)
			{
				leg1Part1Over = true;
			}
		}

		if (leg1Part1Over == true && leg1Over == false)
		{
			thaiLeftY = (int)(thaiLeftY + 2) % 360;
			legLeftY = (int)(legLeftY - 4) % 360;
			shoulderLeftY = (int)(shoulderLeftY + 2) % 360;
			elbowLeftY = (int)(elbowLeftY + 2) % 360;
			if (thaiLeftY > 0.0f && legLeftY < 1.0f && shoulderLeftY > 0.0f && elbowLeftY > 0.0f)
			{
				leg1Over = true;
				bodyTranslationX = bodyTranslationX + walkingDirectionX;
				bodyTranslationZ = bodyTranslationZ + walkingDirectionZ;
			}
		}


		if (leg1Over == true)
		{
			if (leg2Part1Over == false)
			{
				thaiRightY = (int)(thaiRightY - 1) % 360;
				legRightY = (int)(legRightY + 2) % 360;
				shoulderRightY = (int)(shoulderRightY - 1) % 360;
				elbowRightY = (int)(elbowRightY - 1) % 360;
				if (thaiRightY < -60.0f && legRightY > 120.0f &&  shoulderRightY < -60.0f && elbowRightY < -60.0f   &&leg2Part1Over == false)
				{
					leg2Part1Over = true;

				}
			}

			if (leg2Part1Over == true && leg2Over == false)
			{
				thaiRightY = (int)(thaiRightY + 2) % 360;
				legRightY = (int)(legRightY - 4) % 360;
				shoulderRightY = (int)(shoulderRightY + 2) % 360;
				elbowRightY = (int)(elbowRightY + 2) % 360;
				if (thaiRightY > 0.0f && legRightY < 1.0f && shoulderRightY > 0.0f && elbowRightY > 0.0f)
				{
					leg2Over = false;
					leg1Over = false;
					leg1Part1Over = false;
					leg2Part1Over = false;

					bodyTranslationX = bodyTranslationX + walkingDirectionX;
					bodyTranslationZ = bodyTranslationZ + walkingDirectionZ;
				}
			}
		}

	}
	//skipping
	else if (Action == 2)
	{
		finger1LeftRotatePart1 = -90.0f;
		finger2LeftRotatePart1 = -90.0f;
		finger3LeftRotatePart1 = -90.0f;
		finger4LeftRotatePart1 = -90.0f;
		finger5LeftRotatePart1 = -90.0f;

		finger1LeftRotatePart2 = -90.0f;
		finger2LeftRotatePart2 = -90.0f;
		finger3LeftRotatePart2 = -90.0f;
		finger4LeftRotatePart2 = -90.0f;
		finger5LeftRotatePart2 = -90.0f;

		shoulderLeftZ = -10.0f;
		shoulderLeftY = 0.0f;
	
		shoulderRightZ = 10.0f;
		shoulderRightY = 0.0f;
		elbowLeftZ = -130.0f;
		elbowRightZ = 130.0f;
		elbowLeftY = (int)(elbowLeftY + 1) % 360;
		elbowRightY = (int)(elbowRightY + 1) % 360;
		
		palmLeftRotateX = 180.0f;
		palmRightRotateX = 180.0f;
		if (elbowRightY  > 170.0f && elbowRightY < 175.0f)
		{
			skippingStartJumping = true;
		}
		
		if (skippingStartJumping == true)
		{
			if (skippingToggleJumping == false)
			{
				thaiLeftY = (int)(thaiLeftY - 1) % 360;
				legLeftY = (int)(legLeftY + 6) % 360;

				thaiRightY = (int)(thaiRightY - 1) % 360;
				legRightY = (int)(legLeftY + 6) % 360;
				
				bodyTranslationY = bodyTranslationY + 0.03f;
				
				if (thaiLeftY < 20.0f && legLeftY > 60.0f && thaiRightY < 20.0f && legRightY > 60.0f)
				{
					skippingToggleJumping = true;
				}
			}

			if (skippingToggleJumping == true)
			{
				thaiLeftY = (int)(thaiLeftY + 1) % 360;
				legLeftY = (int)(legLeftY - 6) % 360;

				thaiRightY = (int)(thaiRightY + 1) % 360;
				legRightY = (int)(legRightY - 6) % 360;
				
				bodyTranslationY = bodyTranslationY - 0.03f;
				
				if (thaiLeftY >= 0.0f && legLeftY <= 0.0f && thaiRightY >= 0.0f && legRightY <= 0.0f)
				{
					thaiLeftY = 0.0f; legLeftY = 0.0f; thaiRightY = 0.0f; legRightY = 0.0f;
					skippingToggleJumping = false;
					skippingStartJumping = false;
				}
			}

		}
	}
	//running
	else if (Action == 3) {
		//if (leg1Part1Over == false)
		//{
		//	if (thaiRightY >= 0.0f && legRightY <= 0.0f)
		//	{
		//		thaiRightY = 0.0f;
		//		legRightY = 0.0f;
		//	}
		//	else
		//	{
		//		thaiRightY = (int)(thaiRightY + 1) % 360;
		//		legRightY = (int)(legRightY - 1) % 360;
		//	}


		//	thaiLeftY = (int)(thaiLeftY - 2) % 360;
		//	legLeftY = (int)(legLeftY + 4) % 360;
		//	shoulderLeftY = (int)(shoulderLeftY - 2) % 360;
		//	elbowLeftY = -110.0f;
		//	if (thaiLeftY <= -60.0f && legLeftY >= 150.0f && shoulderLeftY <= -60.0f && leg1Part1Over == false)
		//	{
		//		thaiLeftY = -60.0f;
		//		legLeftY = 150.0f;
		//		shoulderLeftY = -60.0f;
		//		leg1Part1Over = true;
		//	}
		//}

		//if (leg1Part1Over == true && leg1Over == false)
		//{
		//	thaiLeftY = (int)(thaiLeftY + 1) % 360; //-60
		//	legLeftY = (int)(legLeftY - 4) % 360;   //120
		//	shoulderLeftY = (int)(shoulderLeftY + 2) % 360; //60
		//	elbowLeftY = -110.0f;


		//	if (thaiLeftY >= -30.0f && legLeftY <= 30.0f && shoulderLeftY >= 0.0f)
		//	{

		//		bodyTranslationX = bodyTranslationX + walkingDirectionX;
		//		bodyTranslationZ = bodyTranslationZ + walkingDirectionZ;
		//		thaiLeftY = -30.0f;
		//		legLeftY = 30.0f;
		//		shoulderLeftY = 0.0f;
		//		leg1Over = true;
		//	}
		//}


		//if (leg1Over == true)
		//{
		//	if (leg2Part1Over == false)
		//	{
		//		
		//		if (thaiLeftY >= 0.0f && legLeftY <= 0.0f)
		//		{
		//			thaiLeftY = 0.0f;
		//			legLeftY = 0.0f;
		//		}
		//		else
		//		{
		//			thaiLeftY = (int)(thaiLeftY + 1) % 360;
		//			legLeftY = (int)(legLeftY - 1) % 360;
		//			
		//		}
		//		thaiRightY = (int)(thaiRightY - 2) % 360;
		//		legRightY = (int)(legRightY + 4) % 360;
		//		shoulderRightY = (int)(shoulderRightY - 2) % 360;
		//		elbowRightY = -110.0f;
		//	
		//		if (thaiRightY <= -60.0f && legRightY >= 150.0f && shoulderRightY <= -60.0f && leg2Part1Over == false)
		//		{
		//			thaiRightY = -60.0f;
		//			legRightY = 150.0f;
		//			shoulderRightY = -60.0f;
		//			leg2Part1Over = true;


		//		}
		//	}

		//	if (leg2Part1Over == true && leg2Over == false)
		//	{
		//		thaiRightY = (int)(thaiRightY + 1) % 360;
		//		legRightY = (int)(legRightY - 4) % 360;
		//		
		//		shoulderRightY = (int)(shoulderRightY + 2) % 360;
		//		elbowRightY = -110.0f;
		//		if (thaiRightY >= -30.0f && legRightY <= 30.0f && shoulderRightY >= 0.0f)
		//		{
		//			
		//			thaiRightY = -30.0f;
		//			legRightY = 30.0f;
		//			shoulderRightY = 0.0f;
		//	
		//			bodyTranslationX = bodyTranslationX + walkingDirectionX;
		//			bodyTranslationZ = bodyTranslationZ + walkingDirectionZ;

		//			leg2Over = false;
		//			leg1Over = false;
		//			leg1Part1Over = false;
		//			leg2Part1Over = false;

		//		}
		//	}
		//}

		if (leg1Part1Over == false)
		{
			if (thaiRightY >= 0.0f && legRightY <= 0.0f)
			{
				thaiRightY = 0.0f;
				legRightY = 0.0f;
			}
			else
			{
				thaiRightY = (int)(thaiRightY + 1) % 360;
				legRightY = (int)(legRightY - 1) % 360;
			}

			if (start1 == false)
			{
				thaiLeftY = (int)(thaiLeftY - 2) % 360;
				legLeftY = (int)(legLeftY + 3) % 360;
				shoulderLeftY = (int)(shoulderLeftY - 2) % 360;
			}else
			{ 

				thaiLeftY = (int)(thaiLeftY - 3) % 360;
				legLeftY = (int)(legLeftY + 1) % 360;
				shoulderLeftY = (int)(shoulderLeftY - 2) % 360;
			}
			elbowLeftY = -110.0f;
			if (thaiLeftY <= -60.0f && legLeftY >= 90.0f && shoulderLeftY <= -60.0f && leg1Part1Over == false)
			{
				thaiLeftY = -60.0f;
				legLeftY = 90.0f;
				shoulderLeftY = -60.0f;
				leg1Part1Over = true;
				start1 = true;
			}
		}

		if (leg1Part1Over == true && leg1Over == false)
		{
		
			if (thaiRightY < 30.0f && legRightY < 60.0f)
			{
				thaiRightY = (int)(thaiRightY + 1) % 360;
				legRightY = (int)(legRightY + 2) % 360;
			}

			bodyTranslationX = bodyTranslationX + (walkingDirectionX)/50;
			bodyTranslationZ = bodyTranslationZ + (walkingDirectionZ)/50;

			thaiLeftY = (int)(thaiLeftY + 1) % 360; //-60
			legLeftY = (int)(legLeftY - 2) % 360;   //90
			shoulderLeftY = (int)(shoulderLeftY + 2) % 360; //60
			elbowLeftY = -110.0f;
			if (thaiLeftY >= -30.0f && legLeftY <= 30.0f && shoulderLeftY >= 0.0f)
			{
				thaiLeftY = -30.0f;
				legLeftY = 30.0f;
				shoulderLeftY = 0.0f;
				leg1Over = true;
			}
		}


		if (leg1Over == true)
		{
			if (leg2Part1Over == false)
			{
				
				if (thaiLeftY >= 0.0f && legLeftY <= 0.0f)
				{
					thaiLeftY = 0.0f;
					legLeftY = 0.0f;
				}
				else
				{
					thaiLeftY = (int)(thaiLeftY + 1) % 360;
					legLeftY = (int)(legLeftY - 1) % 360;
					
				}
				thaiRightY = (int)(thaiRightY - 3) % 360;
				legRightY = (int)(legRightY + 1) % 360;
				shoulderRightY = (int)(shoulderRightY - 2) % 360;
				elbowRightY = -110.0f;
			
				if (thaiRightY <= -60.0f && legRightY >= 90.0f && shoulderRightY <= -60.0f && leg2Part1Over == false)
				{
					thaiRightY = -60.0f;
					legRightY = 90.0f;
					shoulderRightY = -60.0f;
					leg2Part1Over = true;


				}
			}

			if (leg2Part1Over == true && leg2Over == false)
			{
				if (thaiLeftY < 30.0f && legLeftY < 60.0f)
				{
					thaiLeftY = (int)(thaiLeftY + 1) % 360;
					legLeftY = (int)(legLeftY + 2) % 360;
				}

				thaiRightY = (int)(thaiRightY + 1) % 360;
				legRightY = (int)(legRightY - 2) % 360;
				
				shoulderRightY = (int)(shoulderRightY + 2) % 360;
				elbowRightY = -110.0f;
				bodyTranslationX = bodyTranslationX + (walkingDirectionX) / 50;
				bodyTranslationZ = bodyTranslationZ + (walkingDirectionZ) / 50;
				if (thaiRightY >= -30.0f && legRightY <= 30.0f && shoulderRightY >= 0.0f)
				{
					
					thaiRightY = -30.0f;
					legRightY = 30.0f;
					shoulderRightY = 0.0f;
			
					leg2Over = false;
					leg1Over = false;
					leg1Part1Over = false;
					leg2Part1Over = false;

				}
			}
		}
	}
	//swimming
	else if (Action == 4)
	{
		walkingCalc();
		bodyRotationZ1 = 90.0f;
		bodyRotationX1 = 90.0f;
		elbowLeftZ = 20.0f;
		elbowRightZ = -20.0f;
		palmLeftRotateX = 90.0f;
		palmRightRotateX = 90.0f;
		shoulderLeftZ = -20.0f;
		shoulderRightZ = 20.0f;
		
		bodySwimTranslationX=bodySwimTranslationX+walkingDirectionX/100;
		bodySwimTranslationZ=bodySwimTranslationZ+walkingDirectionZ/100;
		
		if (hand1Over == false)
		{
			shoulderLeftY = (int)(shoulderLeftY + 2) % 360;
			if (shoulderLeftY > 0.0f)
			{
				shoulderLeftY = -180.0f;
				hand1Over = true;
			}
			
		}
		if (hand1Over == true)
		{
			shoulderRightY = (int)(shoulderRightY + 2) % 360;
			if (shoulderRightY > 0.0f)
			{
				shoulderRightY = -180.0f;
				hand1Over = false;
			}
		}


		if (leg1Part1Over == false)
		{
			thaiLeftY = (int)(thaiLeftY + 1) % 360;
			legLeftY = (int)(legLeftY + 1) % 360;
			if (thaiLeftY > 20.0f &&legLeftY > 10.0f &&leg1Part1Over == false)
			{
				leg1Part1Over = true;
			}
			
		}

		if (leg1Part1Over == true && leg1Over == false)
		{
			thaiLeftY = (int)(thaiLeftY - 1) % 360;
			legLeftY = (int)(legLeftY - 1) % 360;
			if (thaiLeftY < 0.0f && legLeftY < 1.0f )
			{
				leg1Over = true;
			}
		}

		if (leg1Over == true)
		{
			if (leg2Part1Over == false)
			{
				thaiRightY = (int)(thaiRightY + 1) % 360;
				legRightY = (int)(legRightY + 1) % 360;
				if (thaiRightY > 20.0f && legRightY >10.0f &&leg2Part1Over == false)
				{
					leg2Part1Over = true;
				}
			}

			if (leg2Part1Over == true && leg2Over == false)
			{
				thaiRightY = (int)(thaiRightY - 1) % 360;
				legRightY = (int)(legRightY - 1) % 360;
				if (thaiRightY < 0.0f && legRightY < 1.0f )
				{
					leg2Over = false;
					leg1Over = false;
					leg1Part1Over = false;
					leg2Part1Over = false;
				}
			}
		}


	}
	//squating
	else if (Action == 5) {
		if (sitting == true)
		{
			if (thaiLeftY < -90.0f)
				thaiLeftY = -90.0f;
			else
				thaiLeftY = (int)(thaiLeftY - 1) % 360;
			if (thaiRightY < -90.0f)
				thaiRightY = -90.0f;
			else
				thaiRightY = (int)(thaiRightY - 1) % 360;
			
			//legLeftY = 90.0f;
			//legRightY = 90.0f;
			if (legLeftY > 90.0f)
				legLeftY = 90.0f;
			else
				legLeftY = (int)(legLeftY + 1) % 360;
			if (legRightY > 90.0f)
				legRightY = 90.0f;
			else
				legRightY = (int)(legRightY + 1) % 360;
			
			bodyTranslationY = bodyTranslationY - 0.05f/2.0f;
			if(bodyRotationY >= 0.0f && bodyRotationY < 90.0f)
				bodyTranslationZ = bodyTranslationZ - 0.045f / 2.0f;
			if (bodyRotationY >= 90.0f &&  bodyRotationY < 180.0f)
				bodyTranslationX = bodyTranslationX - 0.045f / 2.0f;
			if (bodyRotationY >= 180.0f && bodyRotationY < 270.0f)
				bodyTranslationZ = bodyTranslationZ + 0.045f / 2.0f;
			if (bodyRotationY >= 270.0f &&  bodyRotationY < 360.0f)
				bodyTranslationX = bodyTranslationX + 0.045f / 2.0f;
			if (thaiLeftY <= -90.0f && thaiRightY <= -90.0f && bodyTranslationY <= -2.0f)
			{
				sitting = false;
			}
		
		}
		else
		{

			if (thaiLeftY > 0.0f)
				thaiLeftY = 0.0f;
			else
				thaiLeftY = (int)(thaiLeftY + 2) % 360;
			if (thaiRightY > 0.0f)
				thaiRightY = 0.0f;
			else
				thaiRightY = (int)(thaiRightY + 2) % 360;

			//legLeftY = 90.0f;
			//legRightY = 90.0f;
			if (legLeftY < 0.0f)
				legLeftY = 0.0f;
			else
				legLeftY = (int)(legLeftY - 2) % 360;
			if (legRightY < 0.0f)
				legRightY = 0.0f;
			else
				legRightY = (int)(legRightY - 2) % 360;

			bodyTranslationY = bodyTranslationY + 0.05f;
			if (bodyRotationY >= 0.0f && bodyRotationY < 90.0f)
				bodyTranslationZ = bodyTranslationZ + 0.045f;
			if (bodyRotationY >= 90.0f &&  bodyRotationY < 180.0f)
				bodyTranslationX = bodyTranslationX + 0.045f;
			if (bodyRotationY >= 180.0f && bodyRotationY < 270.0f)
				bodyTranslationZ = bodyTranslationZ - 0.045f;
			if (bodyRotationY >= 270.0f &&  bodyRotationY < 360.0f)
				bodyTranslationX = bodyTranslationX - 0.045f;
			if (thaiLeftY >= 0.0f && thaiRightY >= 0.0f && bodyTranslationY >= 0.0f)
			{
				sitting = true;
			}

		}
	}
	//assemble/Disassemble
	else if (Action == 6)
	{
		if (assemble == true)
		{

			if(gAnimateLegs==true)
			{
				if (leftLegPos <= 0.0f && rightLegPos <= 0.0f)
				{
					leftLegPos = leftLegPos + 0.1f;
					rightLegPos = rightLegPos + 0.1f;
				}
			}


			if(gAnimateHands==true)
			{
				if (leftHandPos <= 0.0f)
				{
					leftHandPos = leftHandPos + 0.1f;
				}

				if (rightHandPos >= 0.0f)
				{
					rightHandPos = rightHandPos - 0.1f;
				}
			}
			if(gAnimateHead==true)
			{
				if (headPos >= 0.0f)
				{
					headPos = headPos - 0.1f;
				}	
			}
			if(gAnimateChest==true)
			{
				if (diamondPos >= 0.0f)
				{
					diamondPos = diamondPos - 0.1f;
				}
				if (chestPos <= 0.0f)
				{
					chestPos = chestPos + 0.1f;
				}
			}
		}
		else
		{
			if(gAnimateLegsDetach==true)
			{
				if (leftLegPos >= leftLegPosConst && rightLegPos >= rightLegPosConst)
				{
					leftLegPos = leftLegPos - 0.1f;
					rightLegPos = rightLegPos - 0.1f;
				}
			}
			if(gAnimateHandsDetach==true)
			{
				if (leftHandPos >= leftHandPosConst)
				{
					leftHandPos = leftHandPos - 0.1f;
				}
				
				if (rightHandPos <= rightHandPosConst)
				{
					rightHandPos = rightHandPos + 0.1f;
				}
			}
			if(gAnimateHeadDetach==true)
			{
				if (headPos <= headPosConst)
				{
					headPos = headPos + 0.1f;
				}
			}
			if(gAnimateChestDetach==true)
			{
				if (diamondPos <= diamondPosConst)
				{
					diamondPos = diamondPos + 0.1f;
				}

				if (chestPos >= chestPosConst)
				{
					chestPos = chestPos - 0.1f;
				}
				else
					gChestDisplay=false;
			}
		}
	}
}

//Draw Robot

void setLightForDiamondUniformsPerFragment()
{
	float materialAmbient[4] = {  0.135f,0.18725f,0.1745f,1.0f };
	float materialDiffused[4] = { 0.396f,0.074151f,0.69102f,1.0f };
	float materialSpecular[4] = { 0.297254f,0.30829f, 0.306678f,1.0f };

	float materialShininess = 0.1f * 128;

	if (gLighting == 1)
	{
		glUniform1i(LKeyIsPressedUniformPerFragment,
			gLighting);

		glUniform4fv(LightPositionUniformPerFragment, 1, (GLfloat*)lightPosition);
		glUniform3fv(LAUniformPerFragment, 1, (GLfloat*)lightAmbient);
		glUniform3fv(KAUniformPerFragment, 1, (GLfloat*)materialAmbient);
		glUniform3fv(LDUniformPerFragment, 1, (GLfloat*)lightDiffused);
		glUniform3fv(KDUniformPerFragment, 1, (GLfloat*)materialDiffused);
		glUniform3fv(LSUniformPerFragment, 1, (GLfloat*)lightSpecular);
		glUniform3fv(KSUniformPerFragment, 1, (GLfloat*)materialSpecular);
		glUniform1f(MaterialShininessUniformPerFragment, materialShininess);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerFragment, gLighting);
	}

}
void setLightForDiamondUniformsPerVertex()
{
	float materialAmbient[4] = { 0.135f,0.18725f,0.1745f,1.0f };
	float materialDiffused[4] = { 0.396f,0.074151f,0.69102f,1.0f };
	float materialSpecular[4] = { 0.297254f,0.30829f, 0.306678f,1.0f };
	float materialShininess = 0.1f * 128;

	if (gLighting == 1)
	{
		glUniform1i(LKeyIsPressedUniformPerVertex,
			gLighting);

		glUniform4fv(LightPositionUniformPerVertex, 1, (GLfloat*)lightPosition);
		glUniform3fv(LAUniformPerVertex, 1, (GLfloat*)lightAmbient);
		glUniform3fv(KAUniformPerVertex, 1, (GLfloat*)materialAmbient);
		glUniform3fv(LDUniformPerVertex, 1, (GLfloat*)lightDiffused);
		glUniform3fv(KDUniformPerVertex, 1, (GLfloat*)materialDiffused);
		glUniform3fv(LSUniformPerVertex, 1, (GLfloat*)lightSpecular);
		glUniform3fv(KSUniformPerVertex, 1, (GLfloat*)materialSpecular);
		glUniform1f(MaterialShininessUniformPerVertex, materialShininess);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerVertex, gLighting);
	}

}
void setLightForDiamondUniform()
{
	if (gFragment == true)
		setLightForDiamondUniformsPerFragment();
	else
		setLightForDiamondUniformsPerVertex();
}


void setLightForEyesUniformsPerFragment()
{
	float materialAmbient[4] = {  0.135f,0.18725f,0.1745f,1.0f };
	float materialDiffused[4] = { 0.396f,0.074151f,0.69102f,1.0f };
	float materialSpecular[4] = { 0.297254f,0.30829f, 0.306678f,1.0f };
	float materialShininess = 0.1f * 128;

	if (gLighting == 1)
	{
		glUniform1i(LKeyIsPressedUniformPerFragment,
			gLighting);

		glUniform4fv(LightPositionUniformPerFragment, 1, (GLfloat*)lightPosition);
		glUniform3fv(LAUniformPerFragment, 1, (GLfloat*)lightAmbient);
		glUniform3fv(KAUniformPerFragment, 1, (GLfloat*)materialAmbient);
		glUniform3fv(LDUniformPerFragment, 1, (GLfloat*)lightDiffused);
		glUniform3fv(KDUniformPerFragment, 1, (GLfloat*)materialDiffused);
		glUniform3fv(LSUniformPerFragment, 1, (GLfloat*)lightSpecular);
		glUniform3fv(KSUniformPerFragment, 1, (GLfloat*)materialSpecular);
		glUniform1f(MaterialShininessUniformPerFragment, materialShininess);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerFragment, gLighting);
	}

}
void setLightForEyesUniformsPerVertex()
{
	float materialAmbient[4] = { 0.135f,0.18725f,0.1745f,1.0f };
	float materialDiffused[4] = { 0.396f,0.074151f,0.69102f,1.0f };
	float materialSpecular[4] = { 0.297254f,0.30829f, 0.306678f,1.0f };
	float materialShininess = 0.1f * 128;

	if (gLighting == 1)
	{
		glUniform1i(LKeyIsPressedUniformPerVertex,
			gLighting);

		glUniform4fv(LightPositionUniformPerVertex, 1, (GLfloat*)lightPosition);
		glUniform3fv(LAUniformPerVertex, 1, (GLfloat*)lightAmbient);
		glUniform3fv(KAUniformPerVertex, 1, (GLfloat*)materialAmbient);
		glUniform3fv(LDUniformPerVertex, 1, (GLfloat*)lightDiffused);
		glUniform3fv(KDUniformPerVertex, 1, (GLfloat*)materialDiffused);
		glUniform3fv(LSUniformPerVertex, 1, (GLfloat*)lightSpecular);
		glUniform3fv(KSUniformPerVertex, 1, (GLfloat*)materialSpecular);
		glUniform1f(MaterialShininessUniformPerVertex, materialShininess);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerVertex, gLighting);
	}

}
void setLightForEyesUniform()
{
	if (gFragment == true)
		setLightForEyesUniformsPerFragment();
	else
		setLightForEyesUniformsPerVertex();
}



void setLightUniformsPerFragment()
{
	float materialAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialDiffused[4] = { 1.0f,1.0f,1.0f,1.0f };
	float materialSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
	float materialShininess = 250.0f;

	if (gLighting == 1)
	{
		glUniform1i(LKeyIsPressedUniformPerFragment,
			gLighting);

		glUniform4fv(LightPositionUniformPerFragment, 1, (GLfloat*)lightPosition);
		glUniform3fv(LAUniformPerFragment, 1, (GLfloat*)lightAmbient);
		glUniform3fv(KAUniformPerFragment, 1, (GLfloat*)materialAmbient);
		glUniform3fv(LDUniformPerFragment, 1, (GLfloat*)lightDiffused);
		glUniform3fv(KDUniformPerFragment, 1, (GLfloat*)materialDiffused);
		glUniform3fv(LSUniformPerFragment, 1, (GLfloat*)lightSpecular);
		glUniform3fv(KSUniformPerFragment, 1, (GLfloat*)materialSpecular);
		glUniform1f(MaterialShininessUniformPerFragment, materialShininess);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerFragment, gLighting);
	}

}
void setLightUniformsPerVertex()
{
	float materialAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialDiffused[4] = { 1.0f,1.0f,1.0f,1.0f };
	float materialSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
	float materialShininess = 250.0f;

	if (gLighting == 1)
	{
		glUniform1i(LKeyIsPressedUniformPerVertex,
			gLighting);

		glUniform4fv(LightPositionUniformPerVertex, 1, (GLfloat*)lightPosition);
		glUniform3fv(LAUniformPerVertex, 1, (GLfloat*)lightAmbient);
		glUniform3fv(KAUniformPerVertex, 1, (GLfloat*)materialAmbient);
		glUniform3fv(LDUniformPerVertex, 1, (GLfloat*)lightDiffused);
		glUniform3fv(KDUniformPerVertex, 1, (GLfloat*)materialDiffused);
		glUniform3fv(LSUniformPerVertex, 1, (GLfloat*)lightSpecular);
		glUniform3fv(KSUniformPerVertex, 1, (GLfloat*)materialSpecular);
		glUniform1f(MaterialShininessUniformPerVertex, materialShininess);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerVertex, gLighting);
	}

}
void setLightUniforms()
{
	if (gFragment == true)
		setLightUniformsPerFragment();
	else
		setLightUniformsPerVertex();
}
void setModelMatrix(mat4 modelMatrix)
{
	if (gFragment == true)
	{
		glUniformMatrix4fv(mUniformPerFragment,
			1,
			GL_FALSE,
			modelMatrix);
	}
	else
	{
		glUniformMatrix4fv(mUniformPerVertex,
			1,
			GL_FALSE,
			modelMatrix);
	}

}
void drawCapsule(mat4 modelMatrix) {
	mat4 modelMatrix1 = modelMatrix;
	
	mat4 scaleMatrix = scale(0.5f, 0.5f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;

	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);
	
	modelMatrix1 = modelMatrix;
	mat4 rotationMatrix = rotate(90.0f, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;

	scaleMatrix = scale(0.5f, 0.5f, 1.0f);
	modelMatrix1 = modelMatrix1*scaleMatrix;

	setModelMatrix(modelMatrix1);
	cylinderMesh->RenderMesh(0);

	modelMatrix1 = modelMatrix;
	mat4 translateMatrix = translate(0.6f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*translateMatrix;
	scaleMatrix = scale(0.5f, 0.5f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;

	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);

}

void drawRoboChest(mat4 modelMatrix)
{		
	if(interupptedMode == true && Action!=6)
		gChestDisplay=true;
	if(gChestDisplay==true)
	{
		mat4 modelMatrixLocal = modelMatrix;
		mat4 translationMatrix = translate(0.0f, 0.7f, 0.0f);
		modelMatrixLocal = modelMatrixLocal*translationMatrix;

		mat4 scaleMatrix = scale(1.5f, 0.5f, 0.7f);
		modelMatrixLocal = modelMatrixLocal*scaleMatrix;

		setModelMatrix(modelMatrixLocal);

		cudoidMesh->RenderMeshByArray(0);
		
		modelMatrixLocal = modelMatrix;
		translationMatrix = translate(0.0f, 0.0f, 0.0f);
		modelMatrixLocal = modelMatrixLocal*translationMatrix;

		scaleMatrix = scale(0.75f, 0.75f, 0.75f);
		modelMatrixLocal = modelMatrixLocal*scaleMatrix;

		scaleMatrix = scale(1.5f, 0.5f, 0.7f);
		modelMatrixLocal = modelMatrixLocal*scaleMatrix;

		setModelMatrix(modelMatrixLocal);
		cubeMesh->RenderMeshByArray(0);

		
		//Tier
		modelMatrixLocal = modelMatrix;
		translationMatrix = translate(0.0f, -0.5f, 0.0f);
		modelMatrixLocal = modelMatrixLocal*translationMatrix;

		scaleMatrix = scale(1.70f, 0.9f, 1.0f);
		modelMatrixLocal = modelMatrixLocal*scaleMatrix;

		setModelMatrix(modelMatrixLocal);
		sphereMesh->RenderMesh(0);
		
		modelMatrixLocal = modelMatrix;
		translationMatrix = translate(0.0f, -0.7f, 0.0f);
		modelMatrixLocal = modelMatrixLocal*translationMatrix;

		scaleMatrix = scale(1.70f, 0.9f, 1.0f);
		modelMatrixLocal = modelMatrixLocal*scaleMatrix;

		setModelMatrix(modelMatrixLocal);
		sphereMesh->RenderMesh(0);

		modelMatrixLocal = modelMatrix;
		translationMatrix = translate(0.0f, -0.9f, 0.0f);
		modelMatrixLocal = modelMatrixLocal*translationMatrix;

		scaleMatrix = scale(1.70f, 0.9f, 1.0f);
		modelMatrixLocal = modelMatrixLocal*scaleMatrix;

		setModelMatrix(modelMatrixLocal);
		sphereMesh->RenderMesh(0);

		modelMatrixLocal = modelMatrix;
		translationMatrix = translate(0.0f, -1.1f, 0.0f);
		modelMatrixLocal = modelMatrixLocal*translationMatrix;

		scaleMatrix = scale(1.70f, 1.5f, 1.0f);
		modelMatrixLocal = modelMatrixLocal*scaleMatrix;

		setModelMatrix(modelMatrixLocal);
		sphereMesh->RenderMesh(0);

		//circle
		if (showDiamond == true)
		{	
			setLightForDiamondUniform();
		}			
			modelMatrixLocal = modelMatrix;

			translationMatrix = translate(0.0f, 0.0f, diamondPos);
			modelMatrix = modelMatrix*translationMatrix;

			translationMatrix = translate(0.0f, 0.5f, 0.5f);
			modelMatrixLocal = modelMatrixLocal*translationMatrix;

			scaleMatrix = scale(0.75f, 0.75f, 0.75f);
			modelMatrixLocal = modelMatrixLocal*scaleMatrix;

			setModelMatrix(modelMatrixLocal);
			sphereMesh->RenderMesh(0);
		
		setLightUniforms();
	}

}

void drawRoboHandShoulder(mat4 modelMatrix)
{
	//Draw Sphere
	mat4 modelMatrix1 = modelMatrix;
	modelMatrix1 = modelMatrix1*scale(1.2f, 1.2f, 1.2f);
	drawCapsule(modelMatrix1);
	modelMatrix1 = modelMatrix;
	modelMatrix1 = modelMatrix1*translate(1.0f, 0.0f, 0.0f);
	drawCapsule(modelMatrix1);
}

void drawRoboHandElbow(mat4 modelMatrix)
{
	//Draw Sphere
	mat4 modelMatrix1 = modelMatrix;
	modelMatrix1 = modelMatrix1*scale(0.9f, 0.9f, 0.9f);
	drawCapsule(modelMatrix1);
	modelMatrix1 = modelMatrix;
	modelMatrix1 = modelMatrix1*translate(0.7f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*scale(0.8f, 0.8f, 0.8f);
	drawCapsule(modelMatrix1);
}

void drawRoboLegThei(mat4 modelMatrix)
{
	mat4 modelMatrix1 = modelMatrix;
	modelMatrix1 = modelMatrix1*scale(1.5f, 1.5f, 1.5f);
	drawCapsule(modelMatrix1);
	modelMatrix1 = modelMatrix;
	modelMatrix1 = modelMatrix1*translate(1.3f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*scale(1.2f, 1.2f, 1.2f);
	drawCapsule(modelMatrix1);
}

void drawRoboLeg(mat4 modelMatrix)
{
	mat4 modelMatrix1 = modelMatrix;
	modelMatrix1 = modelMatrix1*scale(1.0f, 1.0f, 1.0f);
	drawCapsule(modelMatrix1);
	modelMatrix1 = modelMatrix;
	modelMatrix1 = modelMatrix1*translate(0.8f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*scale(0.9f, 0.9f, 0.9f);
	drawCapsule(modelMatrix1);
}

void drawRoboLegFingers(mat4 modelMatrix)
{
	mat4 modelMatrix1 = modelMatrix;
	mat4 translateMatrix = translate(0.0f,0.0f, 0.3f);
	modelMatrix1 = modelMatrix1*translateMatrix;
	mat4 scaleMatrix = scale(0.3f, 0.5f, 1.0f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);
}

void drawRoboFinger(mat4 modelMatrix,float part1Angle,float part2Angle)
{
	mat4 modelMatrix1 = modelMatrix;
	mat4 translateMatrix = translate(0.3f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*translateMatrix;
	PushMatrix(fingerStack, modelMatrix1);
	mat4 scaleMatrix = scale(0.3f, 0.3f, 0.2f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	drawCapsule(modelMatrix1);


	PopMatrix(fingerStack, &modelMatrix1);
	translateMatrix = translate(0.2f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*translateMatrix;

	mat4 rotationMatrix = rotate(part1Angle, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;
	
	PushMatrix(fingerStack, modelMatrix1);
	scaleMatrix = scale(0.2f, 0.25f, 0.2f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	drawCapsule(modelMatrix1);

	PopMatrix(fingerStack, &modelMatrix1);
	translateMatrix = translate(0.2f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*translateMatrix;

	rotationMatrix = rotate(part2Angle, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;

	scaleMatrix = scale(0.2f, 0.2f, 0.2f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	drawCapsule(modelMatrix1);
}

void drawRoboHandFingers(mat4 modelMatrix)
{
	mat4 modelMatrix1 = modelMatrix;
	mat4 scaleMatrix = scale(0.9f, 0.8f, 0.2f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);

	modelMatrix1 = modelMatrix;

	mat4 translationMatrix = translate(0.0f, -0.15f, 0.0f);
	modelMatrix1 = modelMatrix1*translationMatrix;

	mat4 rotationMatrix = rotate((GLfloat)finger1LeftRotateY, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;
	
    	drawRoboFinger(modelMatrix1, finger1LeftRotatePart1, finger1LeftRotatePart2);
 
	//finger2
	modelMatrix1 = modelMatrix;
	translationMatrix = translate(-0.1f, -0.3f, 0.0f);
	modelMatrix1 = modelMatrix1*translationMatrix;

	rotationMatrix = rotate((GLfloat)finger2LeftRotateY, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;
	
	drawRoboFinger(modelMatrix1, finger2LeftRotatePart1, finger2LeftRotatePart2);

	//middle
	modelMatrix1 = modelMatrix;
	translationMatrix = translate(0.15f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*translationMatrix;

	rotationMatrix = rotate((GLfloat)finger3LeftRotateY, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;

	drawRoboFinger(modelMatrix1, finger3LeftRotatePart1, finger3LeftRotatePart2);

	//finger3
	modelMatrix1 = modelMatrix;
	rotationMatrix = rotate((GLfloat)finger4LeftRotateY, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;
	translationMatrix = translate(0.0f, 0.15f, 0.0f);
	modelMatrix1 = modelMatrix1*translationMatrix;

	drawRoboFinger(modelMatrix1, finger4LeftRotatePart1, finger4LeftRotatePart2);

	//finger4
	modelMatrix1 = modelMatrix;
	rotationMatrix = rotate((GLfloat)finger5LeftRotateY, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;
	translationMatrix = translate(-0.1f, 0.3f, 0.0f);
	modelMatrix1 = modelMatrix1*translationMatrix;

	drawRoboFinger(modelMatrix1, finger5LeftRotatePart1, finger5LeftRotatePart2);
}

void drawRoboNeck(mat4 modelMatrix)
{
	mat4 modelMatrix1 = modelMatrix;

	mat4 scaleMatrix = scale(0.5f, 0.5f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;

	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);

	modelMatrix1 = modelMatrix;

	mat4 translateMatrix = translate(0.0f, 0.1f, 0.0f);
	modelMatrix1 = modelMatrix1*translateMatrix;

	scaleMatrix = scale(0.5f, 0.5f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;

	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);

	modelMatrix1 = modelMatrix;

	translateMatrix = translate(0.0f, 0.2f, 0.0f);
	modelMatrix1 = modelMatrix1*translateMatrix;

	scaleMatrix = scale(0.5f, 0.5f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;

	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);
}

void drawRoboHead(mat4 modelMatrix)
{
	mat4 modelMatrix1 = modelMatrix;
	mat4 translateMatrix = translate(0.0f, 1.2f, 0.1f);
	modelMatrix1 = modelMatrix1*translateMatrix;
	mat4 scaleMatrix = scale(1.5f, 2.1f, 1.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);

	//Eyes
	if(gEyesLight == true)
	{
		setLightForEyesUniform();
	}
	modelMatrix1 = modelMatrix;
	translateMatrix = translate(0.25f, 1.2f, 0.65f);
	modelMatrix1 = modelMatrix1*translateMatrix;
	scaleMatrix = scale(0.5f, 0.5f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);

	modelMatrix1 = modelMatrix;
	translateMatrix = translate(-0.25f, 1.2f, 0.65f);
	modelMatrix1 = modelMatrix1*translateMatrix;
	scaleMatrix = scale(0.5f, 0.5f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);
		
	
	setLightUniforms();
	

	//Ears
	modelMatrix1 = modelMatrix;
	translateMatrix = translate(-0.6f, 1.2f, 0.1f);
	modelMatrix1 = modelMatrix1*translateMatrix;
	scaleMatrix = scale(0.5f, 0.7f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);

	modelMatrix1 = modelMatrix;
	translateMatrix = translate(0.6f, 1.2f, 0.1f);
	modelMatrix1 = modelMatrix1*translateMatrix;
	scaleMatrix = scale(0.5f, 0.7f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);

	//mouth
	modelMatrix1 = modelMatrix;
	translateMatrix = translate(0.0f, 0.6f, 0.4f);
	modelMatrix1 = modelMatrix1*translateMatrix;

	scaleMatrix = scale(0.5f, 0.2f, 0.7f);
	modelMatrix1 = modelMatrix1*scaleMatrix;

	setModelMatrix(modelMatrix1);
	sphereMesh->RenderMesh(0);
}


int main(void)
{
	//function prototyppe	
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void unInitialize();
	void Initialize(void);
	void resize(int,int);
	void display(void);	
	void update(void);
	//variable declaration
	int winWidth=giWindowWidth;
	int winHeight=giWindowHeight;
	bool bDone=false;	

	//Code
	CreateWindow();
	Initialize();

	//Message Loop
	XEvent event;
	KeySym keysym;
	currentTime=clock();
	previousTime=clock();
	while(bDone==false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay,&event);
			switch(event.type)
			{
				case MapNotify:
					XEvent eventCreate;					
					unsigned int keyCode;
					keyCode=XKeysymToKeycode(gpDisplay,XK_F);
					eventCreate.type=KeyPress;
					eventCreate.xkey.state=ShiftMask;
					eventCreate.xkey.keycode=keyCode;
					eventCreate.xkey.same_screen=true;
					XSendEvent(gpDisplay,gWindow,true,KeyPressMask,&eventCreate);
					break;
				case KeyPress:
					keysym=XkbKeycodeToKeysym(gpDisplay,event.xkey.keycode,0,0);
					switch(keysym)
					{
						case XK_Escape:
							bDone=true;
							unInitialize();
							exit(0);
						
						case XK_F:
						case XK_f:
							if(bFullScreen==false)
							{
								ToggleFullscreen();
								bFullScreen=true;
							}
							else
							{
								ToggleFullscreen();
								bFullScreen=false;
							}
							break;
						case XK_T:		
							if(Action!=4)
							{	
								bodyRotationY = (int)(bodyRotationY + 3) % 360;
								if (bodyRotationY < 0.0f)
								{
									bodyRotationY = 360.0f - fabs(bodyRotationY);
								}
								//if (Action == 1)
								walkingCalc();
								interupptedMode = true;
							}
							else
							{
								bodyRotationZ = (int)(bodyRotationZ + 3) % 360;
								if (bodyRotationZ < 0.0f)
								{
									bodyRotationZ = 360.0f - fabs(bodyRotationZ);
								}
								//if (Action == 1)
								walkingCalc();
								interupptedMode = true;
							}
							break;
						
						case XK_t:
							if(Action!=4)
							{
								bodyRotationY = (int)(bodyRotationY - 3) % 360;
								if (bodyRotationY < 0.0f)
								{
									bodyRotationY = 360.0f - fabs(bodyRotationY);
								}
								//if (Action == 1)
								walkingCalc();
								interupptedMode = true;
							}
							else
							{
							
								bodyRotationZ = (int)(bodyRotationZ - 3) % 360;
								if (bodyRotationZ < 0.0f)
								{
									bodyRotationZ = 360.0f - fabs(bodyRotationZ);
								}
								//if (Action == 1)
								walkingCalc();
								interupptedMode = true;
							}	
							break;
						case XK_R:
						case XK_r:
							Action = 3;
							InitializeDefaultValueForRobotParts();
							if(interupptedMode==false)
							{
								interupptedMode = true;
			InitializeInteruuptedMode();							
							}
							walkingSpeed=1.5f;
							break;
						case XK_S:
						case XK_s:
							Action = 5;
							sitting = !sitting;
							InitializeDefaultValueForRobotParts();
							if(interupptedMode==false)
							{
								interupptedMode = true;
			InitializeInteruuptedMode();							
							}

							break;
						case XK_M:
						case XK_m:
							Action = 4;
							InitializeDefaultValueForRobotParts();
							if(interupptedMode==false)
							{
								interupptedMode = true;
			InitializeInteruuptedMode();							
							}
							walkingSpeed=1.0f;
							walkingCalc();
							
							break;
						case XK_K:
						case XK_k:
							Action = 2;
							InitializeDefaultValueForRobotParts();
							if(interupptedMode==false)
							{
								interupptedMode = true;
			InitializeInteruuptedMode();							
							}
							break;
						case XK_W:
						case XK_w:
							Action = 1;
							InitializeDefaultValueForRobotParts();
							if(interupptedMode==false)
							{
								interupptedMode = true;
			InitializeInteruuptedMode();							
							}
							walkingSpeed=1.0f;
							break;
						case XK_I:
						case XK_i:
							Action = 0;
							InitializeDefaultValueForRobotParts();
							if(interupptedMode==false)
							{
								interupptedMode = true;
			InitializeInteruuptedMode();							
							}
							break;
						case XK_A:
						case XK_a:
							Action = 6;
							assemble = !assemble;
							InitializeDefaultValueForRobotParts();
							if(interupptedMode==false)
							{
								interupptedMode = true;
			InitializeInteruuptedMode();							
							}
							break;
						case XK_V:
						case XK_v:
							if (gFragment == true)
								gFragment = false;
							else
								gFragment = true;			
							break;	
						case XK_L:
						case XK_l:
							if (gLighting == 0)
								gLighting = 1;
							else
								gLighting = 0;
							break;

						default:
							break;				
					}
					break;
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:
							break;
						case 2:
							break;
						case 3:
							break;
						default:
							break;
					}
					break;
				case MotionNotify:
					break;
				case ConfigureNotify:
					winWidth=event.xconfigure.width;
					winHeight=event.xconfigure.height;
					resize(winWidth,winHeight);
					break;
				case Expose:
					break;
				case 33:
					bDone=true;
					unInitialize();
					exit(0);
				default:
					break;
			}		
			
		}
		currentTime=clock();
		if(((float)(currentTime-previousTime)/CLOCKS_PER_SEC) >1.0f/120.0f)
		{
			update();
			previousTime=currentTime;				
		}
		display();
	}
	unInitialize();
	return(0);
}
void CreateWindow(){

	//function prototype
	void unInitialize();

	int styleMask;
	XSetWindowAttributes winAttribs;


	GLXFBConfig *pGLXFBConfigs=NULL;
	GLXFBConfig bestGLXFBConfig;
	XVisualInfo *pTempXVisualInfo=NULL;
	int iNumberOfFBConfigs=0;
	
	static int frameBufferAttribs[]={GLX_X_RENDERABLE,True,
				GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,
				GLX_RENDER_TYPE,GLX_RGBA_BIT,
				GLX_X_VISUAL_TYPE,GLX_TRUE_COLOR,
				GLX_RED_SIZE,8,
				GLX_GREEN_SIZE,8,
				GLX_BLUE_SIZE,8,
				GLX_ALPHA_SIZE,8,
				GLX_DEPTH_SIZE,24,
				GLX_STENCIL_SIZE,8,
				GLX_DOUBLEBUFFER,True,				
				None};
	//code
	gpDisplay=XOpenDisplay(NULL);
	if(gpDisplay==NULL)
	{
			printf("ERROR: Uable to Open X Display");
			unInitialize();
			exit(1);
	}

	int defaultScreen=XDefaultScreen(gpDisplay);

	//Retrive All FBConfig that the driver has
	pGLXFBConfigs=glXChooseFBConfig(gpDisplay,defaultScreen,frameBufferAttribs,&iNumberOfFBConfigs);
	printf("There are %d FBConfigs",iNumberOfFBConfigs);

	int bestFrameBufferConfig=-1;
	int bestNumberOfSamples=-1;
	int worstFrameBufferConfig=-1;
	int worstNumberOfSampes=999;

	for(int i=0;i<iNumberOfFBConfigs;i++)
	{
		//for each obtained FBConfig getTempVisual
		pTempXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,pGLXFBConfigs[i]);			
		if(pTempXVisualInfo)
		{
			//Get Number of samples buffers from rspective fb COnfig
			int sampleBuffers,samples;
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLE_BUFFERS,&sampleBuffers);
			glXGetFBConfigAttrib(gpDisplay,pGLXFBConfigs[i],GLX_SAMPLES,&samples);

			//calculae its best
			if(bestFrameBufferConfig<0 || sampleBuffers&&sampleBuffers>bestNumberOfSamples)
			{
				bestFrameBufferConfig=i;
				bestNumberOfSamples=samples;			
			}

			//calculae its worst
			if(worstFrameBufferConfig<0 || sampleBuffers&&sampleBuffers<worstNumberOfSampes)
			{
				worstFrameBufferConfig=i;
				worstNumberOfSampes=samples;			
			}
						
		}
		XFree(pTempXVisualInfo);
	}
	//Now Assign Found Best One
	bestGLXFBConfig=pGLXFBConfigs[bestFrameBufferConfig];

	//Now Assign this to global one
	gGlXFBConfig=bestGLXFBConfig;
	
	XFree(pGLXFBConfigs);
	gpXVisualInfo=glXGetVisualFromFBConfig(gpDisplay,bestGLXFBConfig);
	if(gpXVisualInfo==NULL)
	{
		printf("ERROR: glXGetVisualFromFBConfig failed");
		unInitialize();
		exit(1);
	}	
	
	winAttribs.border_pixel=0;
	winAttribs.background_pixmap=0;
	winAttribs.colormap=XCreateColormap(gpDisplay,RootWindow(gpDisplay,gpXVisualInfo->screen),gpXVisualInfo->visual,AllocNone);
	gColormap=winAttribs.colormap;
	winAttribs.background_pixel=BlackPixel(gpDisplay,defaultScreen);

	winAttribs.event_mask=ExposureMask|VisibilityChangeMask|ButtonPressMask|KeyPressMask|PointerMotionMask|StructureNotifyMask;
	styleMask=CWBorderPixel|CWBackPixel|CWEventMask|CWColormap;

	gWindow=XCreateWindow(gpDisplay,
			RootWindow(gpDisplay,gpXVisualInfo->screen),
			0,
			0,
			giWindowWidth,
			giWindowHeight,
			0,
			gpXVisualInfo->depth,
			InputOutput,
			gpXVisualInfo->visual,
			styleMask,
			&winAttribs);
	if(!gWindow)
	{
		printf("ERROR: Create Window Failed");
		unInitialize();
		exit(1);
	}
	XStoreName(gpDisplay,gWindow,"First XWindow");
	Atom windowManagerDelete=XInternAtom(gpDisplay,"WM_DELETE_ATOM",True);
	XSetWMProtocols(gpDisplay,gWindow,&windowManagerDelete,1);
	
	XMapWindow(gpDisplay,gWindow);
}

void ToggleFullscreen(void)
{
	//variable declaration
	Atom wm_state;
	Atom fullscreen;
	XEvent xev={0};

	//code
	wm_state=XInternAtom(gpDisplay,"_NET_WM_STATE",False);
	memset(&xev,0,sizeof(xev));
	xev.type=ClientMessage;
	xev.xclient.window=gWindow;
	xev.xclient.message_type=wm_state;
	xev.xclient.format=32;
	xev.xclient.data.l[0]=bFullScreen?0:1;

	fullscreen=XInternAtom(gpDisplay,"_NET_WM_STATE_FULLSCREEN",False);
	xev.xclient.data.l[1]=fullscreen;
	XSendEvent(gpDisplay,
		RootWindow(gpDisplay,gpXVisualInfo->screen),False,StructureNotifyMask,&xev);
}


void resize(int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	perspectiveProjectionMatrix = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);

}

void display(void)
{
	
	//functions
	void drawRoboActions();

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (gFragment == true)
	{
		glUseProgram(gShaderProgramObjectPerFragment);

		mat4 modelMatrix;
		mat4 viewMatrix;
		mat4 translationMatrix;
		mat4 rotationMatrix;
		mat4 scaleMatrix;

		viewMatrix = mat4::identity();

		glUniformMatrix4fv(vUniformPerFragment,
			1,
			GL_FALSE,
			viewMatrix);
		glUniformMatrix4fv(pUniformPerFragment,
			1,
			GL_FALSE,
			perspectiveProjectionMatrix);
		setLightUniforms();

		drawRoboActions();
		glUseProgram(0);

	}
	else
	{
		glUseProgram(gShaderProgramObjectPerVertex);

		mat4 modelMatrix;
		mat4 viewMatrix;
		mat4 translationMatrix;
		mat4 rotationMatrix;
		mat4 scaleMatrix;

		viewMatrix = mat4::identity();

		glUniformMatrix4fv(vUniformPerVertex,
			1,
			GL_FALSE,
			viewMatrix);
		glUniformMatrix4fv(pUniformPerVertex,
			1,
			GL_FALSE,
			perspectiveProjectionMatrix);
		setLightUniforms();

		drawRoboActions();
		glUseProgram(0);
	}
	glXSwapBuffers(gpDisplay,gWindow);
}


void drawRoboActions()
{

	mat4 modelMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();

	translationMatrix = translate(bodyTranslationX, bodyTranslationY, bodyTranslationZ - 25.0f);
	modelMatrix = modelMatrix*translationMatrix;

	translationMatrix = translate(bodySwimTranslationX, bodySwimTranslationY, bodySwimTranslationZ);
	modelMatrix = modelMatrix*translationMatrix;

	//Initial state pushed	
	rotationMatrix = rotate((GLfloat)bodyRotationX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)bodyRotationX1, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)bodyRotationY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)bodyRotationZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)bodyRotationZ1, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	PushMatrix(modelStack, modelMatrix);

	/////////////////////////////////////////////////////////////
	//Draw Chest
	PushMatrix(modelStack, modelMatrix);
	
	translationMatrix = translate(0.0f, 0.0f, chestPos);
	modelMatrix = modelMatrix*translationMatrix;

	drawRoboChest(modelMatrix);

	//////////////////////////////////////////////////////////////////////////////////////
	////Right shoulder
	PopMatrix(modelStack, &modelMatrix);
	
	translationMatrix = translate(rightHandPos, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	translationMatrix = translate(1.4f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	//For Initial Pos
	rotationMatrix = rotate(-80.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	//--------

	//Draw here
	rotationMatrix = rotate((GLfloat)shoulderRightX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)shoulderRightY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)shoulderRightZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	PushMatrix(modelStack, modelMatrix);
	//For Assemble & Dissassemble

	drawRoboHandShoulder(modelMatrix);

	/////////////////////////////////////////////////////////
	//Elbow
	PopMatrix(modelStack, &modelMatrix);

	translationMatrix = translate(1.7f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	rotationMatrix = rotate((GLfloat)elbowRightX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)elbowRightY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)elbowRightZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	PushMatrix(modelStack, modelMatrix);
	drawRoboHandElbow(modelMatrix);

	//////////////////////////////////////////////////////////
	//Draw Fingers
	PopMatrix(modelStack, &modelMatrix);


	translationMatrix = translate(1.5f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	//Initial Pos
	rotationMatrix = rotate(90.0f, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	//////////

	rotationMatrix = rotate(palmLeftRotateX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate(palmLeftRotateY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate(palmLeftRotateZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	scaleMatrix = scale(0.7f, 0.7f, 0.7f);
	modelMatrix = modelMatrix*scaleMatrix;


	drawRoboHandFingers(modelMatrix);

	/////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////
	//2nd Hand
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);

	translationMatrix = translate(leftHandPos, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	translationMatrix = translate(-1.4f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	//Draw here	
	//For Initial Pos
	rotationMatrix = rotate(-105.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	//--------

	rotationMatrix = rotate((GLfloat)shoulderLeftX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)shoulderLeftY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)shoulderLeftZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	PushMatrix(modelStack, modelMatrix);
	drawRoboHandShoulder(modelMatrix);

	/////////////////////////////////////////////////////////
	//Elbow
	PopMatrix(modelStack, &modelMatrix);

	translationMatrix = translate(1.7f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	rotationMatrix = rotate((GLfloat)elbowLeftX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)elbowLeftY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)elbowLeftZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	PushMatrix(modelStack, modelMatrix);
	rotationMatrix = rotate(180.0f, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	drawRoboHandElbow(modelMatrix);

	//Draw Fingers
	//.............
	PopMatrix(modelStack, &modelMatrix);
	translationMatrix = translate(1.5f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	//Initial Pos
	rotationMatrix = rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	//////////

	rotationMatrix = rotate(palmRightRotateX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate(palmRightRotateY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate(palmRightRotateZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;


	scaleMatrix = scale(0.7f, 0.7f, 0.7f);
	modelMatrix = modelMatrix*scaleMatrix;

	drawRoboHandFingers(modelMatrix);
	///////////////////////////////////////////////
	//Leg 1
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);

	translationMatrix = translate(0.0f, leftLegPos, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	translationMatrix = translate(0.5f, -1.5f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	//Draw here
	//Initial Pos
	rotationMatrix = rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	//---
	rotationMatrix = rotate((GLfloat)thaiLeftX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)thaiLeftY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)thaiLeftZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	PushMatrix(modelStack, modelMatrix);
	drawRoboLegThei(modelMatrix);


	/////////////////////
	//Leg Part2
	PopMatrix(modelStack, &modelMatrix);
	translationMatrix = mat4::identity();
	rotationMatrix = mat4::identity();

	translationMatrix = translate(2.2f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	rotationMatrix = rotate((GLfloat)legLeftX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)legLeftY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)legLeftZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	PushMatrix(modelStack, modelMatrix);
	drawRoboLeg(modelMatrix);

	//Draw Fingers
	PopMatrix(modelStack, &modelMatrix);

	translationMatrix = translate(1.7f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	rotationMatrix = rotate((GLfloat)legFingerLeftRotateX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)legFingerLeftRotateY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)legFingerLeftRotateZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	drawRoboLegFingers(modelMatrix);

	/////////////////////////////////////////
	//Leg2
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);

	translationMatrix = translate(0.0f, leftLegPos, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	translationMatrix = translate(-0.5f, -1.5f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	//Draw here
	//Initial Pos
	rotationMatrix = rotate(-90.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	//---
	rotationMatrix = rotate((GLfloat)thaiRightX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)thaiRightY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)thaiRightZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	PushMatrix(modelStack, modelMatrix);
	drawRoboLegThei(modelMatrix);

	/////////////////////
	//Leg Part2
	PopMatrix(modelStack, &modelMatrix);
	translationMatrix = translate(2.2f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	rotationMatrix = rotate((GLfloat)legRightX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)legRightY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)legRightZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	PushMatrix(modelStack, modelMatrix);
	drawRoboLeg(modelMatrix);

	//Draw Fingers
	PopMatrix(modelStack, &modelMatrix);
	translationMatrix = translate(1.7f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	rotationMatrix = rotate((GLfloat)legFingerRightRotateX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)legFingerRightRotateY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)legFingerRightRotateZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	drawRoboLegFingers(modelMatrix);

	//Head
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);

	translationMatrix = translate(0.0f, headPos, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	translationMatrix = translate(0.0f, 1.3f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;

	PushMatrix(modelStack, modelMatrix);
	drawRoboNeck(modelMatrix);

	PopMatrix(modelStack, &modelMatrix);
	rotationMatrix = rotate((GLfloat)neckX, 1.0f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)neckY, 0.0f, 1.0f, 0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate((GLfloat)neckZ, 0.0f, 0.0f, 1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	drawRoboHead(modelMatrix);
	PopMatrix(modelStack, &modelMatrix);

}


ALenum to_al_format(short channels, short samples)
{
        bool stereo = (channels > 1);

        switch (samples) {
        case 16:
	     if (stereo)
	             return AL_FORMAT_STEREO16;
	     else
	             return AL_FORMAT_MONO16;
        case 8:
	     if (stereo)
	             return AL_FORMAT_STEREO8;
	     else
	             return AL_FORMAT_MONO8;
        default:
	     return -1;
        }
}
void list_audio_devices(const ALCchar *devices)
{
        const ALCchar *device = devices, *next = devices + 1;
        size_t len = 0;

        printf("Devices list:\n");
        printf("----------\n");
        while (device && *device != '\0' && next && *next != '\0') {
                printf("%s\n", device);
                len = strlen(device);
                device += (len + 1);
                next += (len + 2);
        }
        printf("----------\n");
}
void InitAudio()
{

	//Open AL
	 device = alcOpenDevice(NULL);
	if (!device)
		printf("OpenAL : Error in Handling Devices\n");	
	//Check enumeration is suppoted or not
	ALboolean enumeration;

	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	if (enumeration == AL_FALSE)
	        printf("OpenAL : enumeration not supported\n");
	else
	        printf("OpenAl : enumeration supported\n");
	
	//check the list of devices
	list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	
	ALCenum error;
	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context))
	        printf("OpenAL : failed to make context current\n");
	
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	error = alGetError();
	if (error != AL_NO_ERROR)
	       printf("OpenAL : alListener3f(AL_POSITION) Error Occurred\n");

	
	alGenSources((ALuint)1, &source);
	error = alGetError();
	if (error != AL_NO_ERROR)
	       printf("OpenAL : Create Context Error Occurred\n");

	alSourcei(source, AL_LOOPING, AL_FALSE);
	error = alGetError();
	if (error != AL_NO_ERROR)
	       printf("OpenAL : alSourcei(AL_LOOPING) Error Occurred\n");

	//Generate buffer
	alGenBuffers((ALuint)1, &buffer);
	error = alGetError();
	if (error != AL_NO_ERROR)
	       printf("OpenAL : alGenBuffers() Error Occurred\n");

		
	WaveInfo *wave;
	char *bufferData;
	int ret;

	wave = WaveOpenFileForReading("FinalTest.wav");
	if (!wave) {
	        printf("RTR : Failed to read wave file.\n");
	}

	ret = WaveSeekFile(0, wave);
	if (ret) {
	        printf("RTR : Failed to seek wave file\n");
	}

	bufferData = (char *)malloc(wave->dataSize);
	if (!bufferData) {
	         printf("RTR : malloc Failed\n");
	}

	ret = WaveReadFile(bufferData, wave->dataSize, wave);
	if (ret != wave->dataSize) {
	        printf("RTR : Short read: %d, want: %d\n", ret, wave->dataSize);
	}

	

	alBufferData(buffer, to_al_format(wave->channels, wave->bitsPerSample),
		     bufferData, wave->dataSize, wave->sampleRate);

	
	alSourcei(source, AL_BUFFER, buffer);
	delete(bufferData);
	delete(wave);
	
	//Interupted Mode Song
	
	alGenSources((ALuint)1, &sourceInterupptedMode);
	error = alGetError();
	if (error != AL_NO_ERROR)
	       printf("OpenAL : Create Context Error Occurred\n");

	alSourcei(sourceInterupptedMode, AL_LOOPING, AL_TRUE);
	error = alGetError();
	if (error != AL_NO_ERROR)
	       printf("OpenAL : alSourcei(AL_LOOPING) Error Occurred\n");

	//Generate buffer
	alGenBuffers((ALuint)1, &bufferInterupptedMode);
	error = alGetError();
	if (error != AL_NO_ERROR)
	       printf("OpenAL : alGenBuffers() Error Occurred\n");

	
	wave = WaveOpenFileForReading("test.wav");
	if (!wave) {
	        printf("RTR : Failed to read wave file.\n");
	}

	ret = WaveSeekFile(0, wave);
	if (ret) {
	        printf("RTR : Failed to seek wave file\n");
	}

	bufferData = (char *)malloc(wave->dataSize);
	if (!bufferData) {
	         printf("RTR : malloc Failed\n");
	}

	ret = WaveReadFile(bufferData, wave->dataSize, wave);
	if (ret != wave->dataSize) {
	        printf("RTR : Short read: %d, want: %d\n", ret, wave->dataSize);
	}

	

	alBufferData(bufferInterupptedMode, to_al_format(wave->channels, wave->bitsPerSample),
		     bufferData, wave->dataSize, wave->sampleRate);

	delete(bufferData);
	delete(wave);

	alSourcei(sourceInterupptedMode, AL_BUFFER, bufferInterupptedMode);

}

void Initialize(void)
{
	//Function Prototype
	void resize(int, int);
	void unInitialize();	
	GLenum result1;

	//code
	gGLXCreateContextAttribsARB=(GLXCreateContextAttribsARBProc)
			glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
	if(gGLXCreateContextAttribsARB==NULL)
	{
		printf("glXGetProcAddressARB failed\n");
		unInitialize();
	}
	const GLint glAttribs[]={GLX_CONTEXT_MAJOR_VERSION_ARB,4,
				GLX_CONTEXT_MINOR_VERSION_ARB,2,
				GLX_CONTEXT_PROFILE_MASK_ARB,GLX_CONTEXT_CORE_PROFILE_BIT_ARB,None};

	gGLXContext=gGLXCreateContextAttribsARB(gpDisplay,gGlXFBConfig,0,true,glAttribs);
	if(gGLXContext)
	{
		const GLint Attribs[]={
				GLX_CONTEXT_MAJOR_VERSION_ARB,1,
				GLX_CONTEXT_MINOR_VERSION_ARB,0,
				None		
				};		
	}	
	if(!glXIsDirect(gpDisplay,gGLXContext))
	{
		printf("No hardware Rendering context\n");
	}
	else
	{
		printf("Obtained Context for hardware Rendering\n");
	}
	glXMakeCurrent(gpDisplay,gWindow,gGLXContext);
	
	result1 = glewInit();
	if (result1 != GLEW_OK)
	{
		printf("GLEW not done\n");
	}

	InitAudio();

	modelStack = CreateStack();
	fingerStack = CreateStack();
	
	////////////////////////////////////////////////////
	//Per Vertex


	//Vertex Shader
	gVertexShaderObjectPerVertex = glCreateShader(GL_VERTEX_SHADER);

	//Write Vertex Shader Object
	const GLchar *vertexShaderSourceCodePerVertex =
		"#version 420 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_Ks;" \
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ka;" \
		"uniform vec4 u_Light_Position;" \
		"uniform float u_MaterialShininess;" \
		"uniform int u_LKeyIsPressed;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyIsPressed==1)" \
		"{" \
		"vec4 eye_coordinates=u_v_matrix*u_m_matrix*vPosition;" \

		"vec3 tNorm=normalize(mat3(u_v_matrix*u_m_matrix)*vNormal);" \
		"vec3 lightDirection=normalize(vec3(u_Light_Position-eye_coordinates));" \
		"float tndotld=max(dot(lightDirection,tNorm),0.0);" \
		"vec3 ReflectionVector=reflect(-lightDirection,tNorm);" \
		"vec3 viewerVector=normalize(vec3(-eye_coordinates.xyz));" \
		"vec3 ambient=u_La*u_Ka;" \
		"vec3 diffused=u_Ld*u_Kd*tndotld;" \
		"vec3 specular=u_Ls*u_Ks*pow(max(dot(ReflectionVector,viewerVector),0.0),u_MaterialShininess);" \
		"phong_ads_light = ambient+diffused+specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_light=vec3(1.0,1.0,1.0);" \
		"}" \
		"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix*vPosition;" \
		"}";

	glShaderSource(gVertexShaderObjectPerVertex, 1, (const GLchar **)&vertexShaderSourceCodePerVertex, NULL);
	glCompileShader(gVertexShaderObjectPerVertex);
	//Error Check
	GLint iShaderComileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar* szLogInfo = NULL;

	glGetShaderiv(gVertexShaderObjectPerVertex, GL_COMPILE_STATUS, &iShaderComileStatus);
	if (iShaderComileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szLogInfo = (GLchar *)malloc(iInfoLogLength);
			if (szLogInfo != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectPerVertex, iInfoLogLength, &written, szLogInfo);
				printf("%s", szLogInfo);
				free(szLogInfo);
				unInitialize();
				exit(0);
			}
		}
	}


	//Fragment Shader
	gFragmentShaderObjectPerVertex = glCreateShader(GL_FRAGMENT_SHADER);

	//Write Vertex Shader Object
	const GLchar *fragmentShaderSourceCodePerVertex =
		"#version 420 core" \
		"\n" \
		"in vec3 phong_ads_light;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor=vec4(phong_ads_light,1.0);" \
		"}";


	glShaderSource(gFragmentShaderObjectPerVertex, 1, (const GLchar **)&fragmentShaderSourceCodePerVertex, NULL);
	glCompileShader(gFragmentShaderObjectPerVertex);
	//Error Check
	iShaderComileStatus = 0;
	iInfoLogLength = 0;
	szLogInfo = NULL;

	glGetShaderiv(gFragmentShaderObjectPerVertex, GL_COMPILE_STATUS, &iShaderComileStatus);
	if (iShaderComileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szLogInfo = (GLchar *)malloc(iInfoLogLength);
			if (szLogInfo != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectPerVertex, iInfoLogLength, &written, szLogInfo);
				printf("%s", szLogInfo);
				free(szLogInfo);
				unInitialize();
				exit(0);
			}
		}
	}

	//Create Program
	gShaderProgramObjectPerVertex = glCreateProgram();
	glAttachShader(gShaderProgramObjectPerVertex, gVertexShaderObjectPerVertex);
	glAttachShader(gShaderProgramObjectPerVertex, gFragmentShaderObjectPerVertex);

	glBindAttribLocation(gShaderProgramObjectPerVertex, AMC_ATTRIBUTE_POSITION, "vPosition");

	glBindAttribLocation(gShaderProgramObjectPerVertex, AMC_ATTRIBUTE_NORMAL, "vNormal");

	glLinkProgram(gShaderProgramObjectPerVertex);
	GLint iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	szLogInfo = NULL;

	glGetProgramiv(gShaderProgramObjectPerVertex, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerVertex, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szLogInfo = (GLchar *)malloc(iInfoLogLength);
			if (szLogInfo != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPerVertex, iInfoLogLength, &written, szLogInfo);
				printf("%s", szLogInfo);
				free(szLogInfo);
				unInitialize();
				exit(0);
			}
		}
	}

	mUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_m_matrix");
	vUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_v_matrix");
	pUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_p_matrix");
	LDUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_Ld");
	KDUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_Kd");
	LAUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_La");
	KAUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_Ka");
	LSUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_Ls");
	KSUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_Ks");
	MaterialShininessUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_MaterialShininess");
	LightPositionUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_Light_Position");
	LKeyIsPressedUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_LKeyIsPressed");


	///////////////////////////////////////////////////
	//Per Fragment

	//Vertex Shader
	gVertexShaderObjectPerFragment = glCreateShader(GL_VERTEX_SHADER);

	//Write Vertex Shader Object
	const GLchar *vertexShaderSourceCodePerFragment =
		"#version 420 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_LKeyIsPressed;" \
		"out vec3 tNormVertexShader;" \
		"out vec4 eye_coordinatesVertexShader;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyIsPressed==1)" \
		"{" \
		"eye_coordinatesVertexShader=u_v_matrix*u_m_matrix*vPosition;" \
		"tNormVertexShader=mat3(u_v_matrix*u_m_matrix)*vNormal;" \
		"}" \
		"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix*vPosition;" \
		"}";

	glShaderSource(gVertexShaderObjectPerFragment, 1, (const GLchar **)&vertexShaderSourceCodePerFragment, NULL);
	glCompileShader(gVertexShaderObjectPerFragment);
	//Error Check
	iShaderComileStatus = 0;
	iInfoLogLength = 0;
	szLogInfo = NULL;

	glGetShaderiv(gVertexShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderComileStatus);
	if (iShaderComileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szLogInfo = (GLchar *)malloc(iInfoLogLength);
			if (szLogInfo != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectPerFragment, iInfoLogLength, &written, szLogInfo);
				printf("%s", szLogInfo);
				free(szLogInfo);
				unInitialize();
				exit(0);
			}
		}
	}


	//Fragment Shader
	gFragmentShaderObjectPerFragment = glCreateShader(GL_FRAGMENT_SHADER);

	//Write Vertex Shader Object
	const GLchar *fragmentShaderSourceCodePerFragment =
		"#version 420 core" \
		"\n" \
		"in vec3 tNormVertexShader;" \
		"in vec4 eye_coordinatesVertexShader;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ls;" \
		"uniform vec3 u_Ks;" \
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ka;" \
		"uniform vec4 u_Light_Position;" \
		"uniform float u_MaterialShininess;" \
		"uniform int u_LKeyIsPressed;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"if(u_LKeyIsPressed==1)" \
		"{" \
		"vec3 tNorm=normalize(tNormVertexShader);"
		"vec3 lightDirection=normalize(vec3(u_Light_Position-eye_coordinatesVertexShader));" \
		"float tndotld=max(dot(lightDirection,tNorm),0.0);" \
		"vec3 ReflectionVector=reflect(-lightDirection,tNorm);" \
		"vec3 viewerVector=normalize(vec3(-eye_coordinatesVertexShader.xyz));" \
		"vec3 ambient=u_La*u_Ka;" \
		"vec3 diffused=u_Ld*u_Kd*tndotld;" \
		"vec3 specular=u_Ls*u_Ks*pow(max(dot(ReflectionVector,viewerVector),0.0),u_MaterialShininess);" \
		"vec3 phong_ads_light = ambient+diffused+specular;" \
		"FragColor=vec4(phong_ads_light,1.0);" \
		"}" \
		"else" \
		"{" \
		"FragColor=vec4(1.0,1.0,1.0,1.0);"
		"}" \

		"}";


	glShaderSource(gFragmentShaderObjectPerFragment, 1, (const GLchar **)&fragmentShaderSourceCodePerFragment, NULL);
	glCompileShader(gFragmentShaderObjectPerFragment);
	//Error Check
	iShaderComileStatus = 0;
	iInfoLogLength = 0;
	szLogInfo = NULL;

	glGetShaderiv(gFragmentShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderComileStatus);
	if (iShaderComileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szLogInfo = (GLchar *)malloc(iInfoLogLength);
			if (szLogInfo != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectPerFragment, iInfoLogLength, &written, szLogInfo);
				printf("%s", szLogInfo);
				free(szLogInfo);
				unInitialize();
				exit(0);
			}
		}
	}

	//Create Program
	gShaderProgramObjectPerFragment = glCreateProgram();
	glAttachShader(gShaderProgramObjectPerFragment, gVertexShaderObjectPerFragment);
	glAttachShader(gShaderProgramObjectPerFragment, gFragmentShaderObjectPerFragment);

	glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_NORMAL, "vNormal");

	glLinkProgram(gShaderProgramObjectPerFragment);
	iShaderLinkStatus = 0;
	iInfoLogLength = 0;
	szLogInfo = NULL;

	glGetProgramiv(gShaderProgramObjectPerFragment, GL_LINK_STATUS, &iShaderLinkStatus);
	if (iShaderLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szLogInfo = (GLchar *)malloc(iInfoLogLength);
			if (szLogInfo != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPerFragment, iInfoLogLength, &written, szLogInfo);
				printf("%s", szLogInfo);
				free(szLogInfo);
				unInitialize();
				exit(0);
			}
		}
	}

	mUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_m_matrix");
	vUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_v_matrix");
	pUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_p_matrix");
	LDUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_Ld");
	KDUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_Kd");
	LAUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_La");
	KAUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_Ka");
	LSUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_Ls");
	KSUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_Ks");
	MaterialShininessUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_MaterialShininess");
	LightPositionUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_Light_Position");
	LKeyIsPressedUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_LKeyIsPressed");

	//Cylinder
	cylinder = new PrimitiveShape(slicesCylinder, stacksCylinder);
	drawCylinder(radiusCylinder, radius2Cylinder, heightCylinder, slicesCylinder, stacksCylinder, cylinder->vertices, cylinder->normals,cylinder->texCoords,cylinder->indices);
	cylinderMesh = new Mesh();
	cylinderMesh->CreateMesh(cylinder->vertices, cylinder->normals, cylinder->texCoords, cylinder->indices, cylinder->verticesSize, cylinder->normalsSize, cylinder->texCoordsSize, cylinder->indicesSize);
	cylinder->ClearPrimitiveShape();
	delete(cylinder);
	cylinder = NULL;

	//Cone
	cone = new PrimitiveShape(slicesCone, stacksCone);
	drawCylinder(radiusCone, 0.0, heightCone, slicesCone, stacksCone, cone->vertices, cone->normals, cone->texCoords, cone->indices);
	coneMesh = new Mesh();
	coneMesh->CreateMesh(cone->vertices, cone->normals, cone->texCoords, cone->indices, cone->verticesSize, cone->normalsSize, cone->texCoordsSize, cone->indicesSize);
	cone->ClearPrimitiveShape();
	delete(cone);
	cone = NULL;

	//Frustrum
	frustrum = new PrimitiveShape(slicesFrustrum, stacksFrustrum);
	drawCylinder(radiusFrustrum, 0.0, heightFrustrum, slicesFrustrum, stacksFrustrum, frustrum->vertices, frustrum->normals, frustrum->texCoords, frustrum->indices);
	frustrumMesh = new Mesh();
	frustrumMesh->CreateMesh(frustrum->vertices, frustrum->normals, frustrum->texCoords, frustrum->indices, frustrum->verticesSize, frustrum->normalsSize, frustrum->texCoordsSize, frustrum->indicesSize);
	frustrum->ClearPrimitiveShape();
	delete(frustrum);
	frustrum = NULL;

	//sphere
	sphere = new PrimitiveShape(slices, stacks);
	drawSphere(radius, slices, stacks, sphere->vertices, sphere->normals, sphere->texCoords, sphere->indices);
	sphereMesh = new Mesh();
	sphereMesh->CreateMesh(sphere->vertices, sphere->normals, sphere->texCoords, sphere->indices, sphere->verticesSize, sphere->normalsSize, sphere->texCoordsSize, sphere->indicesSize);
	sphere->ClearPrimitiveShape();
	delete(sphere);
	sphere = NULL;

	//cudoid
	cudoid = new PrimitiveShape(24*3,24*3,24*2,36);
	drawTrapezoidalCube(radius, slices, stacks, cudoid->vertices, cudoid->normals, cudoid->texCoords, cudoid->indices);
	cudoidMesh = new Mesh();
	cudoidMesh->CreateMesh(cudoid->vertices, cudoid->normals, cudoid->texCoords, cudoid->indices, cudoid->verticesSize, cudoid->normalsSize, cudoid->texCoordsSize, cudoid->indicesSize);
	cudoid->ClearPrimitiveShape();
	delete(cudoid);
	cudoid = NULL;


	//cube
	cube = new PrimitiveShape(24 * 3, 24 * 3, 24 * 2, 36);
	drawCube(radius, slices, stacks, cube->vertices, cube->normals, cube->texCoords, cube->indices);
	cubeMesh = new Mesh();
	cubeMesh->CreateMesh(cube->vertices, cube->normals, cube->texCoords, cube->indices, cube->verticesSize, cube->normalsSize, cube->texCoordsSize, cube->indicesSize);
	cube->ClearPrimitiveShape();
	delete(cube);
	cube = NULL;


	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	walkingCalc();
	perspectiveProjectionMatrix = mat4::identity();

	alSourcePlay(source);

	resize(giWindowWidth,giWindowHeight);
}
void unInitialize(void)
{
	
	if(cylinderMesh)
	{ 
		cylinderMesh->ClearMesh();
		delete(cylinderMesh);
		cylinderMesh = NULL;
	}
	
	if (coneMesh)
	{
		coneMesh->ClearMesh();
		delete(coneMesh);
		coneMesh = NULL;
	}
	if (frustrumMesh)
	{
		frustrumMesh->ClearMesh();
		delete(frustrumMesh);
		frustrumMesh = NULL;
	}
	if (sphereMesh)
	{
		sphereMesh->ClearMesh();
		delete(sphereMesh);
		sphereMesh = NULL;
	}
	if (cudoidMesh)
	{
		cudoidMesh->ClearMesh();
		delete(cudoidMesh);
		cudoidMesh = NULL;
	}
	if (cubeMesh)
	{
		cubeMesh->ClearMesh();
		delete(cubeMesh);
		cubeMesh = NULL;
	}
	if (pyramidMesh)
	{
		pyramidMesh->ClearMesh();
		delete(pyramidMesh);
		pyramidMesh = NULL;
	}


	GLsizei shaderCount;
	GLsizei shaderNumber;
	if (gShaderProgramObjectPerFragment)
	{
		glUseProgram(gShaderProgramObjectPerFragment);

		glGetProgramiv(gShaderProgramObjectPerFragment, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint)* shaderCount);
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObjectPerFragment, shaderCount, &shaderCount, pShaders);
			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				glDetachShader(gShaderProgramObjectPerFragment, pShaders[shaderNumber]);
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObjectPerFragment);
		gShaderProgramObjectPerFragment = 0;
		glUseProgram(0);
	}

	if (gShaderProgramObjectPerVertex)
	{
		glUseProgram(gShaderProgramObjectPerVertex);

		glGetProgramiv(gShaderProgramObjectPerVertex, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint *pShaders = (GLuint *)malloc(sizeof(GLuint)* shaderCount);
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObjectPerVertex, shaderCount, &shaderCount, pShaders);
			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				glDetachShader(gShaderProgramObjectPerVertex, pShaders[shaderNumber]);
				glDeleteShader(pShaders[shaderNumber]);
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObjectPerVertex);
		gShaderProgramObjectPerVertex = 0;
		glUseProgram(0);


	}

	DeleteStack(&modelStack);
	DeleteStack(&fingerStack);
	GLXContext currentGLXContext;
	currentGLXContext=glXGetCurrentContext();

	if(currentGLXContext!=NULL && currentGLXContext==gGLXContext)
	{
		glXMakeCurrent(gpDisplay,0,0);
	}
	
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay,gGLXContext);
	}
	if(gWindow)
	{
		XDestroyWindow(gpDisplay,gWindow);
	}
	if(gColormap)
	{
		XFreeColormap(gpDisplay,gColormap);;		
	}
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo=NULL;
	}
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay=NULL;
	}
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);
	alDeleteSources(1, &sourceInterupptedMode);
	alDeleteBuffers(1, &bufferInterupptedMode);
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}
	



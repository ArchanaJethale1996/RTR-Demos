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

int slices = 40, stacks = 40;
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


GLuint mUniformPerVertex, vUniformPerVertex, pUniformPerVertex, LDUniformPerVertex, KDUniformPerVertex, LAUniformPerVertex, KAUniformPerVertex, KSUniformPerVertex, LSUniformPerVertex, LightPositionUniformPerVertex, MaterialShininessUniformPerVertex, LKeyIsPressedUniformPerVertex,colorUniformPerVertex;
GLuint mUniformPerFragment, vUniformPerFragment, pUniformPerFragment, LDUniformPerFragment, KDUniformPerFragment, LAUniformPerFragment, KAUniformPerFragment, KSUniformPerFragment, LSUniformPerFragment, LightPositionUniformPerFragment, MaterialShininessUniformPerFragment, LKeyIsPressedUniformPerFragment,colorUniformPerFragment;
mat4 perspectiveProjectionMatrix;


//uniform values
float lightAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
float lightDiffused[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightSpecular[4] = { 1.0f,1.0f,1.0f,1.0f };
float lightPosition[4] = { 0.0f,0.0f,-2.0f,1.0f };



//animated 
int gLighting = 1;
bool gAnimate=false;
bool gFragment = true;

//Martix Stack
list_t *modelStack,*fingerStack; 

//OpenAL 
ALCdevice *device;
ALCcontext *context;
ALuint source;
ALuint buffer;

//clock
clock_t currentTime,previousTime;
float RotateX,RotateY,RotateZ;


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
void update(void)
{
	if(gAnimate==true)
	{
		RotateY=RotateY+0.2f;
		if(RotateX>=360.0f)
		{
			RotateY=0.0f;
			gAnimate=false;		
		}
	}
}

//Draw Bird

void setLightUniformsPerFragment()
{
	float materialAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialDiffused[4] = { 0.0f,0.0f,1.0f,1.0f };
	float materialSpecular[4] = { 0.0f,0.0f,1.0f,1.0f };
	float materialShininess = 250.0f;
	float color[4] = { 1.0f,1.0f,1.0f,1.0f };
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
		glUniform3fv(colorUniformPerFragment, 1, (GLfloat*)color);
		glUniform1f(MaterialShininessUniformPerFragment, materialShininess);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerFragment, gLighting);
		glUniform3fv(colorUniformPerFragment, 1, (GLfloat*)color);
	}

}
void setLightUniformsPerVertex()
{
	float materialAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialDiffused[4] = { 0.0f,0.0f,1.0f,1.0f };
	float materialSpecular[4] = { 0.0f,0.0f,1.0f,1.0f };
	float materialShininess = 250.0f;
	float color[4] = { 1.0f,1.0f,1.0f,1.0f };
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
		glUniform3fv(colorUniformPerVertex, 1, (GLfloat*)color);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerVertex, gLighting);
		glUniform3fv(colorUniformPerFragment, 1, (GLfloat*)color);
	}

}
void setLightUniforms()
{
	if (gFragment == true)
		setLightUniformsPerFragment();
	else
		setLightUniformsPerVertex();
}


void setLightUniformsPerFragmentBlack()
{
	float materialAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialDiffused[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialSpecular[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialShininess = 250.0f;
	float color[4] = { 1.0f,1.0f,1.0f,1.0f };
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
		glUniform3fv(colorUniformPerFragment, 1, (GLfloat*)color);
		glUniform1f(MaterialShininessUniformPerFragment, materialShininess);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerFragment, gLighting);
		glUniform3fv(colorUniformPerFragment, 1, (GLfloat*)color);
	}

}
void setLightUniformsPerVertexBlack()
{
	float materialAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialDiffused[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialSpecular[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialShininess = 250.0f;
	float color[4] = { 1.0f,1.0f,1.0f,1.0f };
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
		glUniform3fv(colorUniformPerVertex, 1, (GLfloat*)color);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerVertex, gLighting);
		glUniform3fv(colorUniformPerFragment, 1, (GLfloat*)color);
	}

}
void setLightUniformsBlack()
{
	if (gFragment == true)
		setLightUniformsPerFragmentBlack();
	else
		setLightUniformsPerVertexBlack();
}



void setLightUniformsPerFragmentBeak()
{
	float materialAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialDiffused[4] = { 1.0f,0.0f,0.0f,1.0f };
	float materialSpecular[4] = { 1.0f,0.0f,0.0f,1.0f };
	float materialShininess = 250.0f;
	float color[4] = { 1.0f,1.0f,1.0f,1.0f };
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
		glUniform3fv(colorUniformPerFragment, 1, (GLfloat*)color);
		glUniform1f(MaterialShininessUniformPerFragment, materialShininess);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerFragment, gLighting);
		glUniform3fv(colorUniformPerFragment, 1, (GLfloat*)color);
	}

}
void setLightUniformsPerVertexBeak()
{
	float materialAmbient[4] = { 0.0f,0.0f,0.0f,1.0f };
	float materialDiffused[4] = { 1.0f,0.0f,0.0f,1.0f };
	float materialSpecular[4] = { 1.0f,0.0f,0.0f,1.0f };
	float materialShininess = 250.0f;
	float color[4] = { 1.0f,1.0f,1.0f,1.0f };
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
		glUniform3fv(colorUniformPerVertex, 1, (GLfloat*)color);
	}
	else
	{
		glUniform1i(LKeyIsPressedUniformPerVertex, gLighting);
		glUniform3fv(colorUniformPerFragment, 1, (GLfloat*)color);
	}

}
void setLightUniformsBeak()
{
	if (gFragment == true)
		setLightUniformsPerFragmentBeak();
	else
		setLightUniformsPerVertexBeak();
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

void drawFinger(mat4 modelMatrix)
{
	mat4 modelMatrix1 = modelMatrix;
	mat4 translateMatrix = translate(0.3f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*translateMatrix;
	PushMatrix(fingerStack, modelMatrix1);
	mat4 scaleMatrix = scale(0.5f, 0.5f, 0.5f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	drawCapsule(modelMatrix1);


	PopMatrix(fingerStack, &modelMatrix1);
	translateMatrix = translate(0.2f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*translateMatrix;

	
	PushMatrix(fingerStack, modelMatrix1);
	scaleMatrix = scale(0.4f, 0.4f, 0.4f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	drawCapsule(modelMatrix1);

	PopMatrix(fingerStack, &modelMatrix1);
	translateMatrix = translate(0.2f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*translateMatrix;

	scaleMatrix = scale(0.3f, 0.3f, 0.3f);
	modelMatrix1 = modelMatrix1*scaleMatrix;
	drawCapsule(modelMatrix1);
}

void drawHandFingers(mat4 modelMatrix)
{
	mat4 modelMatrix1 = modelMatrix;
	drawCapsule(modelMatrix);

	modelMatrix1 = modelMatrix;
	mat4 translationMatrix = translate(0.5f, -0.15f, 0.0f);
	modelMatrix1 = modelMatrix1*translationMatrix;
	mat4 rotationMatrix = rotate((GLfloat)50.0f, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;
    	drawFinger(modelMatrix1);
 
	//finger2
	modelMatrix1 = modelMatrix;
	translationMatrix = translate(0.5f, -0.3f, 0.0f);
	modelMatrix1 = modelMatrix1*translationMatrix;
	rotationMatrix = rotate((GLfloat)50.0f, 0.0f, 0.0f, 1.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;
	drawFinger(modelMatrix1);

	//middle
	modelMatrix1 = modelMatrix;
	translationMatrix = translate(0.5f, 0.0f, 0.0f);
	modelMatrix1 = modelMatrix1*translationMatrix;
	rotationMatrix = rotate((GLfloat)-50.0f, 0.0f, 1.0f, 0.0f);
	modelMatrix1 = modelMatrix1*rotationMatrix;
	drawFinger(modelMatrix1);
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
						case XK_X:
						case XK_x:
							RotateX=RotateX+1.0f;
							break;
						case XK_Y:
						case XK_y:
							RotateY=RotateY+1.0f;
							break;
						case XK_Z:
						case XK_z:
							RotateZ=RotateZ+1.0f;
							break;
						case XK_A:
						case XK_a:
							gAnimate=!gAnimate;
							break;
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
		if(((float)(currentTime-previousTime)/CLOCKS_PER_SEC) >1.0f/60.0f)
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
	void drawBird();

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

		drawBird();
		glUseProgram(0);

	}
	else
	{
		glUseProgram(gShaderProgramObjectPerVertex);

		mat4 viewMatrix;
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

		drawBird();
		glUseProgram(0);
	}
	glXSwapBuffers(gpDisplay,gWindow);
}


void drawBird()
{
	mat4 modelMatrix = mat4::identity();
	mat4 translationMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();

	//Draw body
	translationMatrix = translate(0.0f, 0.0f, -5.0f);
	modelMatrix = modelMatrix*translationMatrix;

	rotationMatrix = rotate(RotateX,1.0f, 0.0f,0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate(RotateY,0.0f, 1.0f,0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate(RotateZ,0.0f, 0.0f,1.0f);
	modelMatrix = modelMatrix*rotationMatrix;	


	rotationMatrix = rotate(45.0f,0.0f, 0.0f,1.0f);
	modelMatrix = modelMatrix*rotationMatrix;	

	PushMatrix(modelStack, modelMatrix);
	scaleMatrix=scale(1.2f,0.8f,0.8f);
	modelMatrix = modelMatrix*scaleMatrix;

	setModelMatrix(modelMatrix);
	sphereMesh->RenderMesh(0);
	
	//Wing1
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);
	
	translationMatrix = translate(0.1f, 0.0f, -0.3f);
	modelMatrix = modelMatrix*translationMatrix;

	rotationMatrix = rotate(30.0f,1.0f, 0.0f,0.0f);
	modelMatrix = modelMatrix*rotationMatrix;	

	scaleMatrix=scale(0.8f,0.5f,0.2f);
	modelMatrix = modelMatrix*scaleMatrix;

	setModelMatrix(modelMatrix);
	sphereMesh->RenderMesh(0);

	//Wing2
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);
	
	translationMatrix = translate(0.1f, 0.0f, 0.3f);
	modelMatrix = modelMatrix*translationMatrix;

	rotationMatrix = rotate(-30.0f,1.0f, 0.0f,0.0f);
	modelMatrix = modelMatrix*rotationMatrix;	

	scaleMatrix=scale(0.8f,0.5f,0.2f);
	modelMatrix = modelMatrix*scaleMatrix;

	setModelMatrix(modelMatrix);
	sphereMesh->RenderMesh(0);

	//Draw Face
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);
	translationMatrix = translate(0.7f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;
	PushMatrix(modelStack, modelMatrix);
	scaleMatrix=scale(0.6f,0.6f,0.6f);
	modelMatrix = modelMatrix*scaleMatrix;
	setModelMatrix(modelMatrix);
	sphereMesh->RenderMesh(0);

	//beak
	setLightUniformsBeak();
	PopMatrix(modelStack, &modelMatrix);
	translationMatrix = translate(0.05f,-0.2f,0.0f);
	modelMatrix = modelMatrix*translationMatrix;
	PushMatrix(modelStack, modelMatrix);
	rotationMatrix = rotate(90.0f,0.0f,1.0f,0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	rotationMatrix = rotate(90.0f,1.0f,0.0f,0.0f);
	modelMatrix = modelMatrix*rotationMatrix;		
	scaleMatrix=scale(0.2f,0.2f,0.5f);
	modelMatrix = modelMatrix*scaleMatrix;
	setModelMatrix(modelMatrix);
	coneMesh->RenderMesh(0);
	coneMesh->RenderMesh(0);
	
	//Eyes
	setLightUniformsBlack();
	PopMatrix(modelStack, &modelMatrix);
	mat4 localMat=modelMatrix;
	translationMatrix = translate(0.1f,0.1f,0.2f);
	modelMatrix = modelMatrix*translationMatrix;
	scaleMatrix=scale(0.1f,0.1f,0.1f);
	modelMatrix = modelMatrix*scaleMatrix;
	setModelMatrix(modelMatrix);
	sphereMesh->RenderMesh(0);
	modelMatrix=localMat;
	translationMatrix = translate(0.1f,0.1f,-0.2f);
	modelMatrix = modelMatrix*translationMatrix;
	scaleMatrix=scale(0.1f,0.1f,0.1f);
	modelMatrix = modelMatrix*scaleMatrix;
	setModelMatrix(modelMatrix);
	sphereMesh->RenderMesh(0);

	setLightUniforms();
	//Draw Tail
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);
	translationMatrix = translate(-0.4f, 0.0f, 0.0f);
	modelMatrix = modelMatrix*translationMatrix;
	
	rotationMatrix = rotate(90.0f,0.0f, 0.0f,1.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	rotationMatrix = rotate(-90.0f,0.0f, 1.0f,0.0f);
	modelMatrix = modelMatrix*rotationMatrix;

	rotationMatrix = rotate(-10.0f,0.0f, 1.0f,0.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	setModelMatrix(modelMatrix);
	//coneMesh->RenderMesh(0);
	discMesh->RenderMesh(0);


	//Draw Legs
	setLightUniformsBlack();
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);
	translationMatrix = translate(-0.2f, -0.3f, 0.1f);
	modelMatrix = modelMatrix*translationMatrix;
	localMat=modelMatrix;
	scaleMatrix=scale(0.09f,0.3f,0.09f);
	modelMatrix = modelMatrix*scaleMatrix;
	rotationMatrix = rotate(-120.0f,0.0f, 0.0f,1.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	drawHandFingers(modelMatrix);
	
	
	PopMatrix(modelStack, &modelMatrix);
	PushMatrix(modelStack, modelMatrix);
	translationMatrix = translate(-0.2f, -0.3f, -0.1f);
	modelMatrix = modelMatrix*translationMatrix;
	scaleMatrix=scale(0.09f,0.3f,0.09f);
	modelMatrix = modelMatrix*scaleMatrix;
	rotationMatrix = rotate(-120.0f,0.0f, 0.0f,1.0f);
	modelMatrix = modelMatrix*rotationMatrix;
	drawHandFingers(modelMatrix);

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
	alSourcei(source, AL_LOOPING, AL_TRUE);
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
	alBufferData(buffer, to_al_format(wave->channels, wave->bitsPerSample),
		     bufferData, wave->dataSize, wave->sampleRate);
	
	alSourcei(source, AL_BUFFER, buffer);
	delete(bufferData);
	delete(wave);
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
		"uniform vec3 color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor=vec4(phong_ads_light,1.0)*vec4(color,1.0);" \
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
	colorUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "color");

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
		"uniform vec3 color;" \
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
		"FragColor=vec4(phong_ads_light,1.0)*vec4(color,1.0);" \
		"}" \
		"else" \
		"{" \
		"FragColor=vec4(1.0,1.0,1.0,1.0)*vec4(color,1.0);"
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
	colorUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "color");


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

	
	//sphere
	sphere = new PrimitiveShape(slices, stacks);
	drawSphere(radius, slices, stacks, sphere->vertices, sphere->normals, sphere->texCoords, sphere->indices);
	sphereMesh = new Mesh();
	sphereMesh->CreateMesh(sphere->vertices, sphere->normals, sphere->texCoords, sphere->indices, sphere->verticesSize, sphere->normalsSize, sphere->texCoordsSize, sphere->indicesSize);
	sphere->ClearPrimitiveShape();
	delete(sphere);
	sphere = NULL;

	//disc
	disc = new PrimitiveShape(32,32);
	drawPartialDisk(0.0f, 1.2f, 30, 30, 0.0, 10.0,disc->vertices, disc->normals, disc->texCoords, disc->indices);
	discMesh = new Mesh();
	discMesh->CreateMesh(disc->vertices, disc->normals, disc->texCoords, disc->indices, disc->verticesSize, disc->normalsSize, disc->texCoordsSize, disc->indicesSize);
	disc->ClearPrimitiveShape();
	delete(disc);
	disc = NULL;

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	
	perspectiveProjectionMatrix = mat4::identity();

	alSourcePlay(source);

	resize(giWindowWidth,giWindowHeight);
}
void unInitialize(void)
{

	if(discMesh)
	{ 
		discMesh->ClearMesh();
		delete(discMesh);
		discMesh = NULL;
	}
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
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}
	



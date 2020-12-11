// include OpenGL and GLFW libraries
#include <GL/glew.h>                    // include GLEW to get our OpenGL 3.0+ bindings
#include <GLFW/glfw3.h>			        // include GLFW framework header

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// include C/C++ libraries
#include <cstdio>				        // for printf functionality
#include <cstdlib>				        // for exit functionality
#include <vector>                       // for vector functionality

// include our class CSCI441 libraries
#include <stb_image.h>
#include <CSCI441/OpenGLUtils.hpp>      // to print info about OpenGL
#include <CSCI441/objects.hpp>          // to render our 3D primitives
#include <CSCI441/ShaderProgram.hpp>    // a wrapper class for Shader Programs
#include <ctime>

#include "kirb.h"
#include "Asteroid.h"
#include "AsteroidSystem.h"

#include "ParticleSystem.h"

#include "Ship.h"
#include "Skybox.h"

//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience
const GLint WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 960;

GLboolean leftMouseDown;    	 		// status ff the left mouse button is pressed
glm::vec2 mousePosition;				// last known X and Y of the mouse

//glm::vec3 cameraDirection;
//GLfloat cameraSensivity = 0.02f;
//GLfloat cameraTheta, cameraPhi, cameraRadius = 20;

// keep track of all our camera information
struct CameraParameters {
    glm::vec3 cameraAngles;             // cameraAngles --> x = theta, y = phi, z = radius
    glm::vec3 camDir;                   // direction to the camera
    glm::vec3 eyePos;                   // camera position
    glm::vec2 cameraSpeed;              // cameraSpeed --> FREECAM: x = forward/backward delta, y = rotational delta, ARCBALL: x = zoom, y = rotational delta
    glm::vec3 lookAtPoint;              // location of our object of interest to view
    glm::vec3 upVector;                 // the upVector of our camera
    //GLfloat camRadius;
} Camera;

//Bools for different cam types
bool arcballCam = false;
bool freeCam = true;

GLfloat lastTime;
GLfloat deltaTime;                      // Frametime number, updated every frame

glm::vec3 skyColor = glm::vec3(0.07, 0.15, 0.25);
glm::vec3 lightColor;
glm::vec3 lightDirection;
glm::vec3 pointPosition;

GLint controlKey;                       // Keeps track of left control's press status

GLboolean keys[256] = {0};              // keep track of our key states
GLboolean showCage, showCurve;

// Global variables for our heroes and the skybox
kirb kirbcopter;
Ship spaceship;
Skybox skybox;
Asteroid asteroid1;
ParticleSystem particleSystem;

AsteroidSystem asteroidSystem;

float sparkRenderTime = -3.0f;

GLuint billboardTexHandle;

CSCI441::ShaderProgram *billboardShaderProgram = nullptr;
struct BillboardShaderProgramUniforms {
    GLint mvMatrix;                     // the ModelView Matrix to apply
    GLint projMatrix;                   // the Projection Matrix to apply
    GLint image;                        // the texture to bind
    GLint skyColor;                        // the texture to bind

} billboardShaderProgramUniforms;
struct BillboardShaderProgramAttributes {
    GLint vPos;                         // the vertex position
} billboardShaderProgramAttributes;


CSCI441::ShaderProgram *lightingShader = nullptr;   // the wrapper for our shader program
struct LightingShaderUniforms {         // stores the locations of all of our shader uniforms
    GLint mvpMatrix;
    GLint normalMatrix;
    GLint objectMatrix;

    GLint materialColor;
    GLint lightDirection;
    GLint lightColor;

    GLint pointLight4Color;
    GLint pointLight4Pos;

}lightingShaderUniforms;

struct LightingShaderAttributes {       // stores the locations of all of our shader attributes
    GLint vPos;
    GLint vNormal;
} lightingShaderAttributes;

char* skyboxPathways[] = {"data/right.png", "data/left.png", "data/up.png", "data/down.png", "data/front.png", "data/back.png"};
char* shipTexturePathway = "data/ship.png";
char* billboardTexPath = "data/spark.jpg";

//*************************************************************************************
//
// Helper Functions

// setupSkybox()
//
// this function registers the VAO and sets up uv/coordinates for the skybox
void setupSkybox()
{

}


// updateCameraDirection() /////////////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraTheta, cameraPhi, or cameraRadius is updated.
//
////////////////////////////////////////////////////////////////////////////////
void updateCameraDirection() {
    // ensure phi doesn't flip our camera
    if (Camera.cameraAngles.y <= 0) Camera.cameraAngles.y = 0.0f + 0.001f;
    if (Camera.cameraAngles.y >= M_PI) Camera.cameraAngles.y = M_PI - 0.001f;


    if(arcballCam) {
        // do not let our camera get too close or too far away
        if (Camera.cameraAngles.z <= 2.0f) Camera.cameraAngles.z = 2.0f;
        if (Camera.cameraAngles.z >= 30.0f) Camera.cameraAngles.z = 30.0f;
    }

    // update the new direction to the camera
    Camera.camDir.x = Camera.cameraAngles.z * sinf( Camera.cameraAngles.x ) * sinf( Camera.cameraAngles.y );
    Camera.camDir.y = Camera.cameraAngles.z * -cosf( Camera.cameraAngles.y );
    Camera.camDir.z = Camera.cameraAngles.z * -cosf( Camera.cameraAngles.x ) * sinf( Camera.cameraAngles.y );

    // normalize this direction
    Camera.camDir = glm::normalize(Camera.camDir);

    if(arcballCam){
        Camera.lookAtPoint = spaceship.getPosition();
        Camera.eyePos = Camera.lookAtPoint + Camera.camDir * Camera.cameraAngles.z;
    } else if (freeCam) {
        Camera.lookAtPoint = Camera.eyePos + Camera.camDir;
    }
}


// computeAndSendMatrixUniforms() ///////////////////////////////////////////////
//
// This function precomputes the matrix uniforms CPU-side and then sends them
// to the GPU to be used in the shader for each vertex.  It is more efficient
// to calculate these once and then use the resultant product in the shader.
//
////////////////////////////////////////////////////////////////////////////////

//*************************************************************************************
//
// Event Callbacks

// error_callback() /////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
////////////////////////////////////////////////////////////////////////////////
static void error_callback( int error, const char* description ) {
    fprintf( stderr, "[ERROR]: (%d) %s\n", error, description );
}

// keyboard_callback() /////////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's keyboard callback.
//	We only log if a key was pressed/released and will handle the actual event
//  updates later in updateScene().
//      Pressing Q or ESC does close the window and quit the program.
//
////////////////////////////////////////////////////////////////////////////////
static void keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
    if( action == GLFW_PRESS ) {
        // store that a key had been pressed
        keys[key] = GL_TRUE;

        switch( key ) {
            // close our window and quit our program
            case GLFW_KEY_ESCAPE:
            case GLFW_KEY_Q:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_LEFT_CONTROL:
                controlKey = action;
                break;
            case GLFW_KEY_1:
                showCage = !showCage;
                freeCam = true;
                arcballCam = false;
                Camera.cameraSpeed = glm::vec2(0.25f, 0.02f);
                Camera.cameraAngles = glm::vec3(M_PI/2.0f, -M_PI/2.0f, 20.0f);
                updateCameraDirection();
                break;
            case GLFW_KEY_2:
                showCurve = !showCurve;
                freeCam = false;
                arcballCam = true;
                // TODO RESET ARCBALL with respect to ship pos
                // ship is initially draw downwards for some reason
                Camera.cameraSpeed = glm::vec2(0.01f, 0.03f);
                Camera.cameraAngles = glm::vec3(-M_PI/2.0f, M_PI/4.6f, 20.0f);
                updateCameraDirection();
                break;
            default:
                break;
        }
    } else if( action == GLFW_RELEASE ) {
        // store that a key is no longer being pressed
        keys[key] = GL_FALSE;
        // make control key no longer pressed
        switch(key) {
            case GLFW_KEY_LEFT_CONTROL:
                controlKey = action;
                break;
            default: break;
        }
    }
}

// cursor_callback() ///////////////////////////////////////////////////////////
//
//		We will register this function as GLFW's cursor movement callback.
//	Responds to mouse movement.
//
////////////////////////////////////////////////////////////////////////////////
static void cursor_callback( GLFWwindow* window, double xPos, double yPos) {
    // make sure movement is in bounds of the window
    // glfw captures mouse movement on entire screen
    if( xPos > 0 && xPos < WINDOW_WIDTH ) {
        if( yPos > 0 && yPos < WINDOW_HEIGHT ) {
            // active motion
            if( leftMouseDown && arcballCam) {
                // ensure we've moved at least one pixel to register a movement from the click
                if( !((mousePosition.x - -9999.0f) < 0.001f) && controlKey != GLFW_PRESS) {
                    Camera.cameraAngles.x += (xPos - mousePosition.x) * Camera.cameraSpeed.x;
                    Camera.cameraAngles.y += (mousePosition.y - yPos) * Camera.cameraSpeed.x;
                    updateCameraDirection();
                }
                // If the mouse is pressed and left control is pressed, zoom the camera with mouse cursor y position
                if(controlKey == GLFW_PRESS && !((mousePosition.x - -9999.0f) < 0.001f)) {
                    Camera.cameraAngles.z += (yPos - mousePosition.y) * Camera.cameraSpeed.x;
                    updateCameraDirection();
                }
            }
        }
    }
    mousePosition = glm::vec2( xPos, yPos);
//    cameraPhi = glm::clamp(cameraPhi, 0.001f, 3.1415f);
//    updateCameraDirection();
}

// mouse_button_callback() /////////////////////////////////////////////////////
//
//		We will register this function as GLFW's mouse button callback.
//	Responds to mouse button presses and mouse button releases.
//
////////////////////////////////////////////////////////////////////////////////
static void mouse_button_callback( GLFWwindow *window, int button, int action, int mods ) {
    if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leftMouseDown = GL_TRUE;
    } else {
        leftMouseDown = GL_FALSE;
        mousePosition = glm::vec2(-9999.0f, -9999.0f);
    }
}

//*************************************************************************************
//
// Drawing Funcs

// renderScene() ///////////////////////////////////////////////////////////////
//
//  Responsible for drawing all of our objects that make up our world.  Must
//      use the corresponding Shaders, VAOs, and set uniforms as appropriate.
//
////////////////////////////////////////////////////////////////////////////////
void renderScene( glm::mat4 viewMtx, glm::mat4 projMtx )  {
    glm::mat4 rotationMtx = projMtx * glm::mat4(glm::mat3(viewMtx));
    skybox.draw(rotationMtx);

    glm::mat4 modelMtx(1.0f);
    if(freeCam) modelMtx = glm::translate(modelMtx, Camera.eyePos + Camera.camDir * Camera.cameraAngles.z);
    spaceship.draw(modelMtx, viewMtx, projMtx);

    lightingShader->useProgram();
    lastTime = glfwGetTime();
    kirbcopter.draw(viewMtx, projMtx, glm::mat4(1));
    asteroidSystem.draw(viewMtx, projMtx);

    billboardShaderProgram->useProgram();
    glBindTexture(GL_TEXTURE_2D, billboardTexHandle);
    particleSystem.draw(viewMtx, projMtx, Camera.eyePos, Camera.camDir);

//    asteroid1.draw(viewMtx, projMtx, glm::vec3(10, 10, 10), asteroid1.rotationAngle+0.01 );

}



// updateScene() ////////////////////////////////////////////////////////////////
//
//  Responsible for updating any of the attributes/parameters for any of our
//      world objects.  This includes any objects that should be animated AND
//      updating our camera.
//
////////////////////////////////////////////////////////////////////////////////
void updateScene() {
    GLfloat horizontalInput = 0, verticalInput = 0, flyInput = 0;
    deltaTime = glfwGetTime() - lastTime;
    // turn right
    if (keys[GLFW_KEY_D]) {
        horizontalInput += 2;
        if(freeCam) {
            Camera.cameraAngles.x += Camera.cameraSpeed.y;
            updateCameraDirection();
        }
    }
    // turn left
    if (keys[GLFW_KEY_A]) {
        horizontalInput -= 2;
        if(freeCam) {
            Camera.cameraAngles.x -= Camera.cameraSpeed.y;
            updateCameraDirection();
        }
    }
    // pitch up
    if (keys[GLFW_KEY_W]) {
        verticalInput += 2;
        if(freeCam) {
            Camera.cameraAngles.y += Camera.cameraSpeed.y;
            updateCameraDirection();
        }
    }
    // pitch down
    if (keys[GLFW_KEY_S]) {
        verticalInput -= 2;
        if(freeCam) {
            Camera.cameraAngles.y -= Camera.cameraSpeed.y;
            updateCameraDirection();
        }
    }
    // go forward
    if (keys[GLFW_KEY_SPACE]) {
        flyInput += 1;
        if(freeCam) {
            Camera.eyePos += Camera.camDir * Camera.cameraSpeed.x;
            updateCameraDirection();
        }
    }
    // go backward
    if (keys[GLFW_KEY_X]) {
        if(freeCam) {
            Camera.eyePos -= Camera.camDir * Camera.cameraSpeed.x;
            updateCameraDirection();
        }
    }


    deltaTime = glfwGetTime() - lastTime;
    if(arcballCam) spaceship.rotate(verticalInput, horizontalInput, deltaTime);
    if(freeCam) {
        spaceship.rotatex(-Camera.cameraAngles.x + M_PI/2.0f, 1.0f);
        spaceship.rotatey(Camera.cameraAngles.y - M_PI/3.8f, 1.0f); // ADJUST DENOMINATOR FOR starting ship angle
    }
    kirbcopter.heightmapCollision(0);
    kirbcopter.boundaryCollision(glm::vec3(-100, -100, -100), glm::vec3(100, 100, 100));

    if (sparkRenderTime < -0.0f) {
        particleSystem.reset();
    } else if (sparkRenderTime < 3.01f) {
        sparkRenderTime += deltaTime;
    }

    if (sparkRenderTime < 3.0f) {
    } else {
        sparkRenderTime = -1.0f;
    }

    // TODO: Check all asteroids for collisions with projectiles

    bool particleCollision = false; // Set to true if there was a collision

    Asteroid temp = asteroidSystem.asteroids[0]; // Change asteroid with collided asteroid

    if (particleCollision) { // Place particle system and collided asteroid and draw for 3 seconds
        sparkRenderTime = 0;
        particleSystem = ParticleSystem(temp.getPosition(), 100, billboardShaderProgramAttributes.vPos, billboardTexHandle, billboardShaderProgramUniforms.projMatrix, billboardShaderProgramUniforms.mvMatrix);
        particleSystem.update(deltaTime, temp.getPosition(), glm::vec3(20,20,20));
    }

}

//*************************************************************************************
//
// Setup Functions

// setupGLFW() //////////////////////////////////////////////////////////////////
//
//      Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
////////////////////////////////////////////////////////////////////////////////
GLFWwindow* setupGLFW() {
    // set what function to use when registering errors
    // this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
    // all other GLFW calls must be performed after GLFW has been initialized
    glfwSetErrorCallback( error_callback );

    // initialize GLFW
    if( !glfwInit() ) {
        fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
        exit( EXIT_FAILURE );
    } else {
        fprintf( stdout, "[INFO]: GLFW initialized\n" );
    }

    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );					// request forward compatible OpenGL context
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );	    // request OpenGL Core Profile context
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );	                // request OpenGL v4.X
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );	                // request OpenGL vX.1
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );		                    // do not allow our window to be able to be resized
    glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );                         // request double buffering

    // create a window for a given size, with a given title
    GLFWwindow *window = glfwCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, "A06 - Kirb doin Kirb things", nullptr, nullptr );
    if( !window ) {						// if the window could not be created, NULL is returned
        fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
        glfwTerminate();
        exit( EXIT_FAILURE );
    } else {
        fprintf( stdout, "[INFO]: GLFW Window created\n" );
    }

    glfwMakeContextCurrent(window);		                                    // make the created window the current window
    glfwSwapInterval(1);				     	                    // update our screen after at least 1 screen refresh

    glfwSetKeyCallback( window, keyboard_callback );						// set our keyboard callback function
    glfwSetCursorPosCallback( window, cursor_callback );					// set our cursor position callback function
    glfwSetMouseButtonCallback( window, mouse_button_callback );	        // set our mouse button callback function

    return window;						                                    // return the window that was created
}

// setupOpenGL() ///////////////////////////////////////////////////////////////
//
//      Used to setup everything OpenGL related.
//
////////////////////////////////////////////////////////////////////////////////
void setupOpenGL() {
    glEnable( GL_DEPTH_TEST );					                            // enable depth testing
    glDepthFunc( GL_LESS );							                        // use less than depth test

    glEnable(GL_BLEND);									                    // enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	                    // use one minus blending equation

    glClearColor( skyColor.r, skyColor.g, skyColor.b, 1.0f );	        // clear the frame buffer to black
}

// setupGLEW() /////////////////////////////////////////////////////////////////
//
//      Used to initialize GLEW
//
////////////////////////////////////////////////////////////////////////////////
void setupGLEW() {
    glewExperimental = GL_TRUE;
    GLenum glewResult = glewInit();

    // check for an error
    if( glewResult != GLEW_OK ) {
        printf( "[ERROR]: Error initializing GLEW\n");
        // Problem: glewInit failed, something is seriously wrong.
        fprintf( stderr, "[ERROR]: %s\n", glewGetErrorString(glewResult) );
        exit(EXIT_FAILURE);
    } else {
        fprintf( stdout, "[INFO]: GLEW initialized\n" );
    }
}

// setupShaders() //////////////////////////////////////////////////////////////
//
//      Create our shaders.  Send GLSL code to GPU to be compiled.  Also get
//  handles to our uniform and attribute locations.
//
////////////////////////////////////////////////////////////////////////////////
void setupShaders() {

    lightingShader = new CSCI441::ShaderProgram( "shaders/vertexshader.v.glsl", "shaders/vertexshader.f.glsl" );
    lightingShaderUniforms.mvpMatrix        = lightingShader->getUniformLocation("mvpMatrix");
    lightingShaderUniforms.normalMatrix     = lightingShader->getUniformLocation("normalMatrix");
    lightingShaderUniforms.objectMatrix     = lightingShader->getUniformLocation("objectMatrix");

    lightingShaderUniforms.materialColor    = lightingShader->getUniformLocation("materialColor");
    lightingShaderUniforms.lightDirection   = lightingShader->getUniformLocation("lightDirection");
    lightingShaderUniforms.lightColor       = lightingShader->getUniformLocation("lightColor");

    lightingShaderUniforms.pointLight4Color = lightingShader->getUniformLocation("pointLight4Color");
    lightingShaderUniforms.pointLight4Pos   = lightingShader->getUniformLocation("pointLight4Pos");

    lightingShaderAttributes.vPos           = lightingShader->getAttributeLocation("vPos");
    lightingShaderAttributes.vNormal        = lightingShader->getAttributeLocation("vNormal");

    billboardShaderProgram = new CSCI441::ShaderProgram( "shaders/billboardQuadShader.v.glsl", "shaders/billboardQuadShader.g.glsl", "shaders/billboardQuadShader.f.glsl" );
    billboardShaderProgramUniforms.mvMatrix            = billboardShaderProgram->getUniformLocation( "mvMatrix");
    billboardShaderProgramUniforms.projMatrix          = billboardShaderProgram->getUniformLocation( "projMatrix");
    billboardShaderProgramUniforms.image               = billboardShaderProgram->getUniformLocation( "image");
    billboardShaderProgramUniforms.skyColor            = billboardShaderProgram->getUniformLocation( "skyColor");

    billboardShaderProgramAttributes.vPos              = billboardShaderProgram->getAttributeLocation( "vPos");


    skybox = Skybox(skyboxPathways);
    spaceship = Ship(shipTexturePathway);
    asteroidSystem = AsteroidSystem("textures/asteroid.jpg");

//    asteroid1 = Asteroid("textures/asteroid.jpg", glm::vec3(10, 10, 10), glm::vec3(1, 1, 1), glm::vec3(10, 10, 10));

    //updateCameraDirection();
}


// setupScene() /////////////////////////////////////////////////////////////////
//
//	Setup everything specific to our scene.  in this case,
//	position our camera
//
////////////////////////////////////////////////////////////////////////////////
void setupScene() {

    setupSkybox();

    leftMouseDown = false;
    mousePosition = glm::vec2(-9999.0f, -9999.0f);

    // give the camera a scenic starting point.
    // ADJUST STARTING DIRECTION/EYEPOS HERE
    Camera.eyePos = glm::vec3(10.0f, 10.0f, 5.0f);
    Camera.cameraAngles = glm::vec3(M_PI/2.0f, -M_PI/1.45f, 20.0f);
    Camera.cameraSpeed = glm::vec2(0.25f, 0.02f);
    updateCameraDirection();

    lightingShader->useProgram();               // use our lighting shader program so when
    // assign uniforms, they get sent to this shader

    lightDirection = glm::vec3(0, -1, 1);
    lightColor = glm::vec3(1.0, 1.0, 0.80);

    glUniform3fv(lightingShaderUniforms.lightDirection, 1, &lightDirection[0]);
    glUniform3fv(lightingShaderUniforms.lightColor, 1, &lightColor[0]);

    billboardShaderProgram->useProgram();
    glUniform1i(billboardShaderProgramUniforms.image, 0);
    glUniform3fv(billboardShaderProgramUniforms.skyColor, 1, &skyColor[0]);

    glm::vec3 kirbLightColor = 5.0f * glm::vec3(1.5, 0.25, 0.35);
    glm::vec3 kirbPos = kirbcopter.getPosition();
    glUniform3fv(lightingShaderUniforms.pointLight4Color, 1, &kirbLightColor[0]);
    glUniform3fv(lightingShaderUniforms.pointLight4Pos, 1, &kirbPos[0]);
}


void setupTextures(){
    glGenTextures(1, &billboardTexHandle);
    glBindTexture(GL_TEXTURE_2D, billboardTexHandle);

    int width, height, channels;
    unsigned char *data = stbi_load( billboardTexPath, &width, &height, &channels, 0);
    if(data) {
        fprintf(stdout, "Loaded billboard texture [%s]\n", billboardTexPath);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
        );
    }
    else {
        fprintf(stdout, "texture failed to load [%s]\n", billboardTexPath );
    }
    stbi_image_free(data);


}


///*************************************************************************************
//
// Our main function

// main() //////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
int main() {
    // GLFW sets up our OpenGL context so must be done first
    GLFWwindow *window = setupGLFW();	                    // initialize all of the GLFW specific information related to OpenGL and our window
    setupOpenGL();										    // initialize all of the OpenGL specific information
    setupGLEW();											// initialize all of the GLEW specific information


    CSCI441::OpenGLUtils::printOpenGLInfo();

    setupShaders();                                         // load our shader program into memory
    setupTextures();
    setupScene();

    // needed to connect our 3D Object Library to our shader
    CSCI441::setVertexAttributeLocations( lightingShaderAttributes.vPos,  lightingShaderAttributes.vNormal);

    updateScene();

    //  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
    //	until the user decides to close the window and quit the program.  Without a loop, the
    //	window will display once and then the program exits.
    while( !glfwWindowShouldClose(window) ) {	// check if the window was instructed to be closed
        glDrawBuffer( GL_BACK );				// work with our back frame buffer
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window


        // update the projection matrix based on the window size
        // the GL_PROJECTION matrix governs properties of the view coordinates;
        // i.e. what gets seen - use a perspective projection that ranges
        // with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
        glm::mat4 projMtx = glm::perspective( 45.0f, (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 1000.0f );


        // Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
        // when using a Retina display the actual window can be larger than the requested window.  Therefore
        // query what the actual size of the window we are rendering to is.
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize( window, &framebufferWidth, &framebufferHeight );

        // update the viewport - tell OpenGL we want to render to the whole window
        glViewport( 0, 0, framebufferWidth, framebufferHeight );

        // set up our look at matrix to position our camera
        glm::mat4 viewMtx = glm::lookAt(Camera.eyePos, Camera.lookAtPoint, glm::vec3(0, 1, 0));

        renderScene( viewMtx, projMtx);					// draw everything to the window

        glfwSwapBuffers(window);                            // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                    // check for any events and signal to redraw screen

        updateScene();
    }

    fprintf( stdout, "[INFO]: Shutting down.......\n" );
    fprintf( stdout, "[INFO]: ...freeing memory...\n" );

    delete lightingShader;                                  // delete our shader program
    CSCI441::deleteObjectVBOs();                            // delete our library VBOs
    CSCI441::deleteObjectVAOs();                            // delete our library VAOs

    fprintf( stdout, "[INFO]: ...closing GLFW.....\n" );

    glfwDestroyWindow( window );                            // clean up and close our window
    glfwTerminate();						                // shut down GLFW to clean up our context

    fprintf( stdout, "[INFO]: ............complete\n" );
    fprintf( stdout, "[INFO]: Goodbye\n" );

    return EXIT_SUCCESS;				                    // exit our program successfully!
}

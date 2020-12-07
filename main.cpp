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
//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience
const GLint WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 960;

bool noDecorations = false;

GLboolean leftMouseDown;    	 		// status ff the left mouse button is pressed
glm::vec2 mousePosition;				// last known X and Y of the mouse


glm::vec3 cameraDirection;
GLfloat cameraSensivity = 0.02f;
GLfloat cameraTheta, cameraPhi, cameraRadius = 20;


GLfloat lastTime;
GLfloat deltaTime;                      // Frametime number, updated every frame

glm::vec3 skyColor = glm::vec3(0.07, 0.15, 0.25);
glm::vec3 lightColor;
glm::vec3 lightDirection;
glm::vec3 pointPosition;

GLint controlKey;                       // Keeps track of left control's press status

GLboolean keys[256] = {0};              // keep track of our key states
GLboolean showCage, showCurve;

// Global variables for our heroes and the landscape
kirb kirbcopter;

struct BuildingData {                   // stores the information unique to a single building
    glm::mat4 modelMatrix;                  // the translation/scale of each building
    glm::vec3 color;                        // the color of each building
};
std::vector<BuildingData> buildings;    // information for all of our buildings

GLuint groundVAO;                       // the VAO descriptor for our ground plane

// Shader Program information

CSCI441::ShaderProgram *lineShader = nullptr;   // the wrapper for our shader program


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


CSCI441::ShaderProgram *waterShader = nullptr;   // the wrapper for our shader program
struct waterShaderUniforms {         // stores the locations of all of our shader uniforms
    GLint mvpMatrix;
    GLint objectMatrix;
    GLint cameraPos;
    GLint skybox;                 // Skybox texture sampler


    GLint lightDirection;
    GLint lightColor;
    GLint skyColor;

    GLint pointLight4Color;
    GLint pointLight4Pos;

    GLint time;

}waterShaderUniforms;

struct waterShaderAttributes {       // stores the locations of all of our shader attributes
    GLint vPos;
} waterShaderAttributes;





CSCI441::ShaderProgram *skyboxShader = nullptr;   // the wrapper for our shader program
struct skyboxShaderUniforms {         // stores the locations of all of our shader uniforms
    GLint mvpMatrix;
    GLint skybox;                 // Skybox texture sampler
}skyboxShaderUniforms;

struct skyboxShaderAttributes {       // stores the locations of all of our shader attributes
    GLint vPos;
} skyboxShaderAttributes;

GLuint skyboxVAO, skyboxVBO, skyboxIBO;


char* bezierPathway = "data/controlPoints.csv";

GLuint skyboxTextureHandle;
//char* skyboxPathways[] = {"data/right.png", "data/left.png", "data/up.png", "data/down.png", "data/front.png", "data/back.png"};
char* skyboxPathways[] = {"data/right.png", "data/left.png", "data/down.png", "data/up.png", "data/front.png", "data/back.png"};

//*************************************************************************************
//
// Helper Functions

// setupSkybox()
//
// this function registers the VAO and sets up uv/coordinates for the skybox
void setupSkybox()
{
    fprintf(stdout, "Begining setting up skybox\n");
    glGenTextures(1, &skyboxTextureHandle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureHandle);
    int width, height, channels;
    for(int i = 0; i < 6; i ++ ) {
        unsigned char *data = stbi_load( skyboxPathways[i], &width, &height, &channels, 0);
        if(data) {
            fprintf(stdout, "Loaded cubemap texture [%s]\n", skyboxPathways[i] );
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else {
            fprintf(stdout, "Cubemap texture failed to load [%s]\n", skyboxPathways[i] );
        }
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    // create our platform
    glm::vec3 cubeVertices[8] = {
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f,  -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f, 1.0f),
            glm::vec3(1.0f,  -1.0f, 1.0f),
            glm::vec3(-1.0f, 1.0f, -1.0f),
            glm::vec3(1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f,  1.0f, 1.0f)
    };
    unsigned short cubeIndices[36] = {0, 1, 2, 1, 2, 3,
                                      4, 5, 6, 5, 6, 7,
                                      0, 2, 4, 2, 4, 6,
                                      1, 3, 5, 3, 5, 7,
                                      0, 1, 4, 1, 4, 5,
                                      2, 3, 6, 3, 6, 7};

    glGenVertexArrays( 1, &skyboxVAO );
    glBindVertexArray( skyboxVAO );

    glGenBuffers( 1, &skyboxVBO );
    glBindBuffer( GL_ARRAY_BUFFER, skyboxVBO );
    glBufferData( GL_ARRAY_BUFFER, 8 * sizeof(glm::vec3), cubeVertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( skyboxShaderAttributes.vPos );
    glVertexAttribPointer( skyboxShaderAttributes.vPos, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0 );

    glGenBuffers(1, &skyboxIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxIBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 36, &cubeIndices[0], GL_STATIC_DRAW);

    fprintf( stdout, "[INFO]: Skybox read in with VAO %d\n", skyboxVAO );
}


// updateCameraDirection() /////////////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraTheta, cameraPhi, or cameraRadius is updated.
//
////////////////////////////////////////////////////////////////////////////////
void updateCameraDirection() {
    cameraDirection = glm::vec3(cos(cameraTheta) * sin(cameraPhi), cos(cameraPhi), sin(cameraTheta) * sin(cameraPhi));
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
                break;
            case GLFW_KEY_2:
                showCurve = !showCurve;
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
            if( leftMouseDown ) {
                // ensure we've moved at least one pixel to register a movement from the click
                if( !((mousePosition.x - -9999.0f) < 0.001f) && controlKey != GLFW_PRESS) {
                    cameraTheta += (xPos - mousePosition.x) * cameraSensivity;
                    cameraPhi -= (yPos - mousePosition.y) * cameraSensivity;
                }
                // If the mouse is pressed and left control is pressed, zoom the camera with mouse cursor y position
                if(controlKey == GLFW_PRESS && !((mousePosition.x - -9999.0f) < 0.001f)) {
                    cameraRadius += (yPos - mousePosition.y) * cameraSensivity;
                    updateCameraDirection();
                }


            }
        }
    }
    mousePosition = glm::vec2( xPos, yPos);
    cameraPhi = glm::clamp(cameraPhi, 0.001f, 3.1415f);
    updateCameraDirection();
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

// drawSkybox()
//
// draws the skybox before everything else is drawing, and then re-clears the depth buffer.
void drawSkybox(glm::mat4 rotationMtx){
    skyboxShader->useProgram();

    glDepthMask(GL_FALSE);

    glUniformMatrix4fv(skyboxShaderUniforms.mvpMatrix, 1, GL_FALSE, &rotationMtx[0][0] );

    glBindVertexArray( skyboxVAO );
    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0 );

    glDepthMask(GL_TRUE);
}

// renderScene() ///////////////////////////////////////////////////////////////
//
//  Responsible for drawing all of our objects that make up our world.  Must
//      use the corresponding Shaders, VAOs, and set uniforms as appropriate.
//
////////////////////////////////////////////////////////////////////////////////
void renderScene( glm::mat4 viewMtx, glm::mat4 projMtx )  {
    glm::mat4 rotationMtx = projMtx * glm::mat4(glm::mat3(viewMtx));
    drawSkybox(rotationMtx);

    lightingShader->useProgram();
    lastTime = glfwGetTime();
    kirbcopter.draw(viewMtx, projMtx, glm::mat4(1));

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
    // turn right
    if( keys[GLFW_KEY_D] ) {
        horizontalInput += 1;
    }
    // turn left
    if( keys[GLFW_KEY_A] ) {
        horizontalInput -= 1;
    }
    // pitch up
    if( keys[GLFW_KEY_W] ) {
        verticalInput += 1;
    }
    // pitch down
    if( keys[GLFW_KEY_S] ) {
        verticalInput -= 1;
    }
    // go forward
    if( keys[GLFW_KEY_SPACE] ) {
        flyInput += 1;
    }
    kirbcopter.move(verticalInput, horizontalInput, flyInput);

    deltaTime = glfwGetTime() - lastTime;
    kirbcopter.animate(deltaTime);
    kirbcopter.heightmapCollision(0);
    kirbcopter.boundaryCollision(glm::vec3(-100, -100, -100), glm::vec3(100, 100, 100));
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
    lineShader = new CSCI441::ShaderProgram( "shaders/flat.v.glsl", "shaders/flat.f.glsl" );

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



    waterShader = new CSCI441::ShaderProgram( "shaders/water.v.glsl", "shaders/water.f.glsl" );
    waterShaderUniforms.mvpMatrix        = waterShader->getUniformLocation("mvpMatrix");
    waterShaderUniforms.lightDirection   = waterShader->getUniformLocation("lightDirection");
    waterShaderUniforms.objectMatrix     = waterShader->getUniformLocation("objectMatrix");
    waterShaderUniforms.lightColor       = waterShader->getUniformLocation("lightColor");

    waterShaderUniforms.skybox           = waterShader->getUniformLocation("skybox");
    waterShaderUniforms.cameraPos        = waterShader->getUniformLocation("cameraPos");

    waterShaderUniforms.pointLight4Color = waterShader->getUniformLocation("pointLight4Color");
    waterShaderUniforms.pointLight4Pos   = waterShader->getUniformLocation("pointLight4Pos");
    waterShaderUniforms.time   = waterShader->getUniformLocation("time");



    waterShaderAttributes.vPos           = waterShader->getAttributeLocation("vPos");


    skyboxShader = new CSCI441::ShaderProgram( "shaders/skybox.v.glsl", "shaders/skybox.f.glsl" );
    skyboxShaderUniforms.mvpMatrix      = skyboxShader->getUniformLocation("mvpMatrix");
    skyboxShaderUniforms.skybox = skyboxShader->getUniformLocation("skybox");

    skyboxShaderAttributes.vPos         = skyboxShader->getAttributeLocation("vPos");
}

// setupBuffers() //////////////////////////////////////////////////////////////
//
//      Create our VAOs.  Send vertex data to the GPU to be stored.
//
////////////////////////////////////////////////////////////////////////////////
void setupBuffers() {

    CSCI441::drawSolidSphere( 0.5, 16, 16 );
    CSCI441::drawSolidCube( 1.0 );
    CSCI441::drawSolidTorus(1.0f, 5.0f, 10, 10);
    CSCI441::drawSolidCylinder( 1.0f, 1.0f, 1, 2, 16 );

    lastTime = glfwGetTime();

    kirbcopter = kirb(glm::vec3(0, 0 + 4, 0), 0, lightingShaderUniforms.mvpMatrix, lightingShaderUniforms.normalMatrix, lightingShaderUniforms.objectMatrix, lightingShaderUniforms.materialColor );

    updateCameraDirection();
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
    updateCameraDirection();

    lightingShader->useProgram();               // use our lighting shader program so when
    // assign uniforms, they get sent to this shader

    lightDirection = glm::vec3(0, -1, 1);
    lightColor = glm::vec3(1.0, 1.0, 0.80);

    glUniform3fv(lightingShaderUniforms.lightDirection, 1, &lightDirection[0]);
    glUniform3fv(lightingShaderUniforms.lightColor, 1, &lightColor[0]);

    glm::vec3 kirbLightColor = 5.0f * glm::vec3(1.5, 0.25, 0.35);
    glm::vec3 kirbPos = kirbcopter.getPosition();
    glUniform3fv(lightingShaderUniforms.pointLight4Color, 1, &kirbLightColor[0]);
    glUniform3fv(lightingShaderUniforms.pointLight4Pos, 1, &kirbPos[0]);


    waterShader->useProgram();

    glUniform3fv(waterShaderUniforms.lightDirection, 1, &lightDirection[0]);
    glUniform3fv(waterShaderUniforms.lightColor, 1, &lightColor[0]);

    glUniform3fv(waterShaderUniforms.pointLight4Color, 1, &kirbLightColor[0]);
    glUniform3fv(waterShaderUniforms.pointLight4Pos, 1, &kirbPos[0]);
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
    setupBuffers();
    setupScene();

    // needed to connect our 3D Object Library to our shader
    CSCI441::setVertexAttributeLocations( lightingShaderAttributes.vPos,  lightingShaderAttributes.vNormal);


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
        glm::vec3 cameraWorldPos = kirbcopter.getPosition() + cameraRadius * cameraDirection;
        glm::mat4 viewMtx = glm::lookAt( cameraWorldPos, kirbcopter.getPosition(), glm::vec3(0, 1, 0));


        waterShader->useProgram();
        glUniform3fv(waterShaderUniforms.cameraPos, 1, &cameraWorldPos[0]);

        renderScene( viewMtx, projMtx);					// draw everything to the window

        glfwSwapBuffers(window);                            // flush the OpenGL commands and make sure they get rendered!
        glfwPollEvents();				                    // check for any events and signal to redraw screen

        updateScene();
    }

    fprintf( stdout, "[INFO]: Shutting down.......\n" );
    fprintf( stdout, "[INFO]: ...freeing memory...\n" );

    delete lightingShader;                                  // delete our shader program
    glDeleteBuffers(1, &groundVAO);                         // delete our ground VAO
    CSCI441::deleteObjectVBOs();                            // delete our library VBOs
    CSCI441::deleteObjectVAOs();                            // delete our library VAOs

    fprintf( stdout, "[INFO]: ...closing GLFW.....\n" );

    glfwDestroyWindow( window );                            // clean up and close our window
    glfwTerminate();						                // shut down GLFW to clean up our context

    fprintf( stdout, "[INFO]: ............complete\n" );
    fprintf( stdout, "[INFO]: Goodbye\n" );

    return EXIT_SUCCESS;				                    // exit our program successfully!
}
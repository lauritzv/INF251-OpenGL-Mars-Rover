#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <gl/GL.h>

#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>

#include "model_obj.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "lodepng.h"
#include "create_matrix.h"
#include "Camera.h"
#include <algorithm>
#include <ctime>

using namespace std;

/*
 * CONTROLS:
 *
 * Left mouse rotates
 * Middle mouse changes fov
 * Right mouse moves camera
 *
 * r: resets camera
 * w: toggle wireframe-mote
 * t: toggle rotation on model
 * l: reload shaders
 * q: quit program
 */

// --- OpenGL callbacks ---------------------------------------------------------------------------
void display();
void idle();
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void motion(int, int);
float clamp(float, float, float);


// --- Other methods ------------------------------------------------------------------------------
bool initBuffers();
bool initShaders();
bool initTextures();
string readTextFile(const string&);
bool initTexture(const char* pathfilename, GLuint &TObject, unsigned char* &TextureData);

// --- Global variables ---------------------------------------------------------------------------

// 3D model
ModelOBJ Model;		///< A 3D model
GLuint VBO = 0;		///< A vertex buffer object
GLuint IBO = 0;		///< An index buffer object

// Shaders
GLuint ShaderProgram = 0;	///< A shader program

// Textures
GLuint TObjectDiffuse0 = 0, TObjectNormal0 = 0, TObjectSpecular0 = 0;
unsigned int TWidth = 0, THeight = 0;
unsigned char *TextureDataDiffuse = nullptr, *TextureDataNorm = nullptr, *TextureDataSpec = nullptr;


// Vertex transformations
Vector3f Translation;	///< Translation
float RotationX;
float RotationY;
float Scaling;			///< Scaling
Matrix4<float> projection;
Matrix4<float> transformation;
float aspect_ratio; // Set from initial WindowSize

// Mouse interactions
int MouseX, MouseY;		///< The last position of the mouse
int MouseButton;		///< The last mouse button pressed or released

// Toggled Mode
bool wireframe = false;
bool orthographic = false;
clock_t Timer;
bool animateScene = true;

Camera Cam;
create_matrix cm;

// --- main() -------------------------------------------------------------------------------------
/// The entry point of the application
int main(int argc, char **argv) {

	// Initialize glut and create a simple window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //added glut_depth
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("OpenGL Tutorial");

	aspect_ratio = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) / static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	// Initialize OpenGL callbacks
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	// Initialize glew (must be done after glut is initialized!)
	GLenum res = glewInit();
	if (res != GLEW_OK) {
		cerr << "Error initializing glew: \n"
			<< reinterpret_cast<const char*>(glewGetErrorString(res)) << endl;
		return -1;
	}

	// Initialize program variables
	// OpenGL
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);	        // enable depth ordering
	glEnable(GL_CULL_FACE);		        // enable back-face culling
	glFrontFace(GL_CCW);		        // vertex order for the front face
	glCullFace(GL_BACK);		        // back-faces should be removed

	// Transformation
	Translation.set(0.0f, -2.0f, -10.0f);
	RotationX = 0.0;
	RotationY = 0.0;
	Scaling = 1.0f;
	transformation = cm.create_transformation_matrix(Translation, RotationX, RotationY, Scaling);

	// Camera init
	Cam.init();
	Cam.ar = aspect_ratio;

	// Shaders, Textures & buffers
	if (!initShaders() || !initTextures() || !initBuffers())
	{
		cout << "Press Enter to exit..." << endl;
		getchar();
		return -1;
	}

	glutSetCursor(GLUT_CURSOR_CROSSHAIR); // hide the cursor

	//Start timer
	Timer = clock();

	// Start the main event loop
	glutMainLoop();

	return 0;
}

// ************************************************************************************************
// *** OpenGL callbacks implementation ************************************************************
/// Called whenever the scene has to be drawn
void display() {
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable the shader program
	assert(ShaderProgram != 0);
	glUseProgram(ShaderProgram);

	// *** New stuff ******************************************************************************

	// Set transformations
	GLint trULocation = glGetUniformLocation(ShaderProgram, "transformation");
	assert(trULocation != -1);
	glUniformMatrix4fv(trULocation, 1, false, transformation.get());

	// Set projection
	GLint prULocation = glGetUniformLocation(ShaderProgram, "projection");
	assert(prULocation != -1);
	//glUniformMatrix4fv(prULocation, 1, false, projection.get());
	glUniformMatrix4fv(prULocation, 1, false, Cam.computeCameraTransform().get());

	// Set Eye position
	GLint eyeULocation = glGetUniformLocation(ShaderProgram, "camera_position");
	assert(eyeULocation != -1);
	glUniform3fv(eyeULocation, 1, Cam.position.get());

	// tell the shader which T.U. to use
	GLint const diffSamplerULocation = glGetUniformLocation(ShaderProgram, "diffSampler");
	glUniform1i(diffSamplerULocation, 0);
	GLint const normSamplerULocation = glGetUniformLocation(ShaderProgram, "normSampler");
	glUniform1i(normSamplerULocation, 1);
	GLint const specSamplerULocation = glGetUniformLocation(ShaderProgram, "specSampler");
	glUniform1i(specSamplerULocation, 2);

	//// Set normalMatrix
	//Matrix4f modelView; // = ????
	//Matrix4f normalMatrix = modelView.getInverse().getTransposed();
	//GLint nmULocation = glGetUniformLocation(ShaderProgram, "normalMatrix");
	//assert(nmULocation != -1);
	//glUniformMatrix4fv(nmULocation, 1, true, normalMatrix.get());

	// ********************************************************************************************


	// Enable the vertex attributes and set their format
	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,											//location
		3,											//datacount per
		GL_FLOAT,									//datatype
		GL_FALSE,									//normalize?
		sizeof(ModelOBJ::Vertex),					//stride
		reinterpret_cast<const GLvoid*>(0));		//

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(3* sizeof(float)));

	//normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>( 5* sizeof(float))
	);

	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// set the active texture units

	// Bind our diffuse texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TObjectDiffuse0);

	// Bind our normal texture in Texture Unit 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TObjectNormal0);

	// Bind our specular texture in Texture Unit 2
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, TObjectSpecular0);

	// ...

	// Draw the elements on the GPU
	glDrawElements(
		GL_TRIANGLES,
		Model.getNumberOfIndices(),
		GL_UNSIGNED_INT,
		0);

	// Disable the vertex attributes (not necessary but recommended)
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

/// Called at regular intervals (can be used for animations)
void idle() {
	clock_t now = clock(); //get the current “time”
	if (animateScene) {
		const float rotationSpeed = 100.f;
		RotationX += rotationSpeed * (now - Timer) / CLOCKS_PER_SEC;
		transformation = cm.create_transformation_matrix(Translation, RotationX, RotationY, Scaling);
	}
	Timer = now; //store the current “time”
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	Vector3f right;

	switch (tolower(key)) {
		// --- camera movements ---
	case 'w':
		Cam.position += Cam.target * 0.1f;
		break;
	case 'a':
		right = Cam.target.cross(Cam.up);
		Cam.position -= right * 0.1f;
		break;
	case 's':
		Cam.position -= Cam.target * 0.1f;
		break;
	case 'd':
		right = Cam.target.cross(Cam.up);
		Cam.position += right * 0.1f;
		break;
	case 'c':
		Cam.position -= Cam.up * 0.1f;
		break;
	case ' ':
		Cam.position += Cam.up * 0.1f;
		break;
	case 'r': // Reset camera status
		Cam.init();
		Cam.ar = aspect_ratio;
		break;
	case 't':
		animateScene = !animateScene;
		break;
		// --- utilities ---
	case 'p': // toggle wireframe mode
		wireframe = !wireframe;
		if (wireframe)
			glPolygonMode(GL_FRONT, GL_LINE);   // draw polygons as wireframe
		else
			glPolygonMode(GL_FRONT, GL_FILL); // draw polygon surfaces
		glutPostRedisplay();
		break;
	case 'g': // show the current OpenGL version
		cout << "OpenGL version " << glGetString(GL_VERSION) << endl;
		break;
	case 'l': // reload shaders
		cout << "Re-loading shaders..." << endl;
		if (initShaders()) {
			cout << "> done." << endl;
			glutPostRedisplay();
		}
		break;
	case 'q':  // terminate the application
		exit(0);
		break;
	}
	// redraw
	glutPostRedisplay();
}

// Called whenever a special keyboard button is pressed
void special(int key, int x, int y) {
	Vector3f right;

	switch (key) {
		// --- camera movements ---
	case GLUT_KEY_PAGE_UP:	// Increase field of view
		Cam.fov = min(Cam.fov + 1.f, 179.f);
		break;
	case GLUT_KEY_PAGE_DOWN:	// Decrease field of view
		Cam.fov = max(Cam.fov - 1.f, 1.f);
		break;

		// --- utilities ---
	case GLUT_KEY_F5:	// Reload shaders
		cout << "Re-loading shaders..." << endl;
		if (initShaders()) {
			cout << "> done." << endl;
		}
		break;
	}
	// redraw
	glutPostRedisplay();
}

/// Called whenever a mouse event occur (press or release)
void mouse(int button, int state, int x, int y) {
	// Store the current mouse status
	MouseButton = button;

	// Instead of updating the mouse position, lock it at the center of the screen
	MouseX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	MouseY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	//glutWarpPointer(MouseX, MouseY);
}

/// Called whenever the mouse is moving while a button is pressed
void motion(int x, int y) {

	if (MouseButton == GLUT_RIGHT_BUTTON) {
		Cam.position += Cam.target * 0.003f * (MouseY - y);
		Cam.position += Cam.target.cross(Cam.up) * 0.003f * (x - MouseX);
	}
	if (MouseButton == GLUT_MIDDLE_BUTTON) {
		Cam.zoom = max(0.001f, Cam.zoom + 0.003f * (y - MouseY));
	}
	if (MouseButton == GLUT_LEFT_BUTTON) {
		Matrix4f ry, rr;

		// "horizontal" rotation
		ry.rotate(0.1f * (MouseX - x), Vector3f(0, 1, 0));
		Cam.target = ry * Cam.target;
		Cam.up = ry * Cam.up;

		// "vertical" rotation
		rr.rotate(0.1f * (MouseY - y), Cam.target.cross(Cam.up));
		Cam.up = rr * Cam.up;
		Cam.target = rr * Cam.target;
	}

	// Lock the mouse at the center of the screen
	glutWarpPointer(MouseX, MouseY);

	// Redraw the scene
	glutPostRedisplay();
}

/*
/// Called whenever a keyboard button is pressed (only ASCII characters)
void keyboard(const unsigned char key, int x, int y) {
	switch (tolower(key)) {

	case 'g': // show the current OpenGL version
		cout << "OpenGL version " << glGetString(GL_VERSION) << endl;
		break;

	case 'q':  // terminate the application
		exit(0);

	case 'r':
		cout << "Re-loading shaders..." << endl;
		if (initShaders()) {
			cout << "> done." << endl;
			glutPostRedisplay();
		}
		break;

	case 'w': // toggle wireframe mode
		wireframe = !wireframe;
		if (wireframe)
			glPolygonMode(GL_FRONT, GL_LINE);   // draw polygons as wireframe
		else
			glPolygonMode(GL_FRONT, GL_FILL); // draw polygon surfaces
		glutPostRedisplay();
		break;

	case 'p': // toggle perspective-orthographic
		orthographic = !orthographic;
		projection = cm.create_projection(orthographic, aspect_ratio);
		glutPostRedisplay();
		break;

	default:
		break;
	}
}

/// Called whenever a mouse event occur (press or release)
void mouse(int button, int state, int x, int y) {
	// Store the current mouse status
	MouseButton = button;
	MouseX = x;
	MouseY = y;
}

/// Called whenever the mouse is moving while a button is pressed
void motion(int x, int y) {
	if (MouseButton == GLUT_RIGHT_BUTTON) {
		Translation.x() += 0.003f * (x - MouseX); // Accumulate translation amount
		Translation.y() += 0.003f * (MouseY - y);
		MouseX = x; // Store the current mouse position
		MouseY = y;
		transformation = cm.create_transformation_matrix(Translation, RotationX, RotationY, Scaling);
	}
	if (MouseButton == GLUT_MIDDLE_BUTTON) {
		Scaling = clamp(Scaling + 0.003f * (MouseY - y), 0.1f, 100.0f); // Accumulate scaling amount
		MouseX = x; // Store the current mouse position
		MouseY = y;
		transformation = cm.create_transformation_matrix(Translation, RotationX, RotationY, Scaling);
	}
	if (MouseButton == GLUT_LEFT_BUTTON) {
		RotationX += 0.3f * (x - MouseX);
		RotationY += 0.3f * (MouseY - y);
		MouseX = x;
		MouseY = y;
		transformation = cm.create_transformation_matrix(Translation, RotationX, RotationY, Scaling);
	}

	glutPostRedisplay(); // Specify that the scene needs to be updated
}
*/

// ************************************************************************************************
// *** Other methods implementation ***************************************************************
/// Initialize buffer objects
bool initBuffers() {
	// Load the OBJ model
	if (!Model.import(
		//"models\\capsule\\capsule.obj"
		"models\\crystalpot\\crystalpot.obj"
		//"models\\rover\\rover.obj"
		)) {
		cerr << "Error: cannot load model." << endl;
		return false;
	}

	// Notice that normals may not be stored in the model
	// This issue will be dealt with in the next lecture

	// VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
		Model.getNumberOfVertices() * sizeof(ModelOBJ::Vertex),
		Model.getVertexBuffer(),
		GL_STATIC_DRAW);

	// IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3 * Model.getNumberOfTriangles() * sizeof(int),
		Model.getIndexBuffer(),
		GL_STATIC_DRAW);



	return true;
} /* initBuffers() */

bool initTextures()
{
	return initTexture("models\\crystalpot\\crystalshell_diff.png", ::TObjectDiffuse0, TextureDataDiffuse)
		&& initTexture("models\\crystalpot\\crystalshell_norm.png", TObjectNormal0, TextureDataNorm)
		&& initTexture("models\\crystalpot\\crystalshell_ao.png", TObjectSpecular0, TextureDataSpec);
}

bool initTexture(const char* pathfilename, GLuint &TObject, unsigned char* &TextureData)
{
	unsigned int fail = lodepng_decode_file(
		&TextureData, // the texture will be stored here
		&TWidth, &THeight, // width and height of the texture will be stored here
		pathfilename, // path and file name
		LCT_RGBA, // format of the image
		8); // bits for each color channel (bit depth / num. of channels)
	if (fail != 0)
		return false;

	glGenTextures(1, &TObject); // Create the texture object
	glBindTexture(GL_TEXTURE_2D, TObject); // Bind it as a 2D texture

	// Set the texture data
	glTexImage2D(GL_TEXTURE_2D, // type of texture
		0,						// level of detail (used for mip-mapping only)
		GL_RGBA,				// color components (how the data should be interpreted)
		::TWidth, ::THeight,		// texture width (must be a power of 2 on some systems)
		0,						// border thickness (just set this to 0)
		GL_RGBA,				// data format (how the data is supplied)
		GL_UNSIGNED_BYTE,		// the basic type of the data array
		TextureData);			// pointer to the data

	// Set texture parameters for minification and magnification
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

/// Initialize shaders. Return false if initialization fail
bool initShaders() {
	// Create the shader program and check for errors
	if (ShaderProgram != 0)
		glDeleteProgram(ShaderProgram);
	ShaderProgram = glCreateProgram();
	if (ShaderProgram == 0) {
		cerr << "Error: cannot create shader program." << endl;
		return false;
	}

	// Create the shader objects and check for errors
	const GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	const GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (vertShader == 0 || fragShader == 0) {
		cerr << "Error: cannot create shader objects." << endl;
		return false;
	}

	// Read and set the source code for the vertex shader
	string text = readTextFile("shaders\\shader.ass3.v.glsl");
	const char* code = text.c_str();
	int length = static_cast<int>(text.length());
	if (length == 0)
		return false;
	glShaderSource(vertShader, 1, &code, &length);

	// Read and set the source code for the fragment shader
	string text2 = readTextFile("shaders\\shader.ass3.f.glsl");
	const char *code2 = text2.c_str();
	length = static_cast<int>(text2.length());
	if (length == 0)
		return false;
	glShaderSource(fragShader, 1, &code2, &length);

	// Compile the shaders
	glCompileShader(vertShader);
	glCompileShader(fragShader);

	// Check for compilation error
	GLint success;
	GLchar errorLog[1024];
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile vertex shader.\nError log:\n" << errorLog << endl;
		getchar();
		return false;
	}
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragShader, 1024, nullptr, errorLog);
		cerr << "Error: cannot compile fragment shader.\nError log:\n" << errorLog << endl;
		getchar();
		return false;
	}

	// Attach the shader to the program and link it
	glAttachShader(ShaderProgram, vertShader);
	glAttachShader(ShaderProgram, fragShader);
	glLinkProgram(ShaderProgram);

	// Check for linking error
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, errorLog);
		cerr << "Error: cannot link shader program.\nError log:\n" << errorLog << endl;
		getchar();
		return false;
	}

	// Make sure that the shader program can run
	glValidateProgram(ShaderProgram);

	// Check for validation error
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 1024, nullptr, errorLog);
		cerr << "Error: cannot validate shader program.\nError log:\n" << errorLog << endl;
		getchar();
		return false;
	}

	// Shaders can be deleted now
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return true;
} /* initShaders() */



/// Read the specified file and return its content
string readTextFile(const string& pathAndFileName) {
	// Try to open the file
	ifstream fileIn(pathAndFileName);
	if (!fileIn.is_open()) {
		cerr << "Error: cannot open file " << pathAndFileName.c_str();
		return "";
	}

	// Read the file
	string text = "";
	string line;
	while (!fileIn.eof()) {
		getline(fileIn, line);
		text += line + "\n";
		bool bad = fileIn.bad();
		bool fail = fileIn.fail();
		if (fileIn.bad() || (fileIn.fail() && !fileIn.eof())) {
			cerr << "Warning: problems reading file " << pathAndFileName.c_str()
				<< "\nBad flag: " << bad << "\tFail flag: " << fail
				<< "\nText read: \n" << text.c_str();
			fileIn.close();
			return text;
		}
	}
	// finalize
	fileIn.close();

	return text;
} /* readTextFile() */

///clamp that float
float clamp(const float in, const float min, const float max)
{
	float out;
	if (in < min) out = min;
	else if (in > max) out = max;
	else out = in;
	return out;
}



/* --- eof main.cpp --- */
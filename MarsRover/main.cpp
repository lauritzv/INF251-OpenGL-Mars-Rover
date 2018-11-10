#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <gl/GL.h>

#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <ctime>

#include "Vector3.h"
#include "Matrix4.h"
#include "create_matrix.h"
#include "Camera.h"
#include "MeshObject.h"
#include "MaterialObject.h"

using namespace std;

// --- OpenGL callbacks ---------------------------------------------------------------------------
void display();
void idle();
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void motion(int, int);

// --- Other methods ------------------------------------------------------------------------------
bool initModels();
bool initShaders();
bool initTextures();
void setCommonUniforms();
string readTextFile(const string&);
float clamp(float, float, float);

// --- Global variables ---------------------------------------------------------------------------

// Shaders
GLuint ShaderProgram0 = 0;	///< A shader program

// Materials and textures
vector<MaterialObject> material_objects;

// Models
vector<MeshObject> mesh_objects;

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

int viewMode = 0;

Camera Cam;
create_matrix cm;

// --- main() -------------------------------------------------------------------------------------
/// The entry point of the application
int main(int argc, char **argv) {

	cout <<
'\n' << "CONTROLS:"<<
'\n' <<
'\n' << "Left mouse rotates"<<
'\n' << "Middle mouse changes fov"<<
'\n' << "Right mouse moves camera"<<
'\n' << "wasd, space and c: first-person movement"<<
'\n' <<
'\n' << "r: resets camera"<<
'\n' << "p: toggle wireframe-mode"<<
'\n' << "t: toggle model rotation"<<
'\n' << "v: cycle between different fragment output modes"<<
'\n' << "l: reload shaders"<<
'\n' << "q: quit program" << endl;


	// Initialize glut and create a simple window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //added glut_depth
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("OpenGL Tutorial");

	aspect_ratio = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) / static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));

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
	Cam.ar = aspect_ratio;

	// Shaders, Textures & buffers
	if (!initShaders() || !initTextures() || !initModels())
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
	assert(ShaderProgram0 != 0);
	glUseProgram(ShaderProgram0);

	setCommonUniforms();

	// Enable the vertex attributes and set their format
	glEnableVertexAttribArray(0); //pos
	glEnableVertexAttribArray(1); //uv-coord
	glEnableVertexAttribArray(2); //normals

	//draw objects:
	for (auto& el : mesh_objects)
		el.DrawObject();

	//cleanup VAOs
	glDisableVertexAttribArray(0); // pos
	glDisableVertexAttribArray(1); // uv
	glDisableVertexAttribArray(2); // normal

	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

/// Called at regular intervals (can be used for animations)
void idle() {
	clock_t now = clock(); //get the current “time”
	if (animateScene) {
		const float rotationSpeed = 60.f;
		RotationX += rotationSpeed * (now - Timer) / CLOCKS_PER_SEC;
		transformation = cm.create_transformation_matrix(Translation, RotationX, RotationY, Scaling);
	}
	Timer = now; //store the current “time”
	glutPostRedisplay();
}

void setCommonUniforms()
{
	// Set transformations
	const GLint trULocation = glGetUniformLocation(ShaderProgram0, "transformation");
	assert(trULocation != -1);
	glUniformMatrix4fv(trULocation, 1, false, transformation.get());

	// Set projection
	const GLint prULocation = glGetUniformLocation(ShaderProgram0, "projection");
	assert(prULocation != -1);
	glUniformMatrix4fv(prULocation, 1, false, projection.get());
	glUniformMatrix4fv(prULocation, 1, false, Cam.computeCameraTransform().get());

	// Set lightPositionMatrix
	const GLint lpULocation = glGetUniformLocation(ShaderProgram0, "lightPositionMat");
	glUniformMatrix4fv(lpULocation, 1, false, Matrix4f().get()); //identity matrix

	// Set normalMatrix
	Matrix4f normalMatrix = transformation.getInverse().getTransposed();
	const GLint nmaULocation = glGetUniformLocation(ShaderProgram0, "normal_matrix");
	glUniformMatrix4fv(nmaULocation, 1, false, normalMatrix.get()); // <-- this bool caused a lot of headache!!!
																	// Made the lightsource rotate with the model!
	const GLint vmULocation = glGetUniformLocation(ShaderProgram0, "viewMode");
	assert(vmULocation != -1);
	glUniform1i(vmULocation, viewMode);

	// tell the shader which T.U. to use
	GLint const diffSamplerULocation = glGetUniformLocation(ShaderProgram0, "diffSampler");
	glUniform1i(diffSamplerULocation, 0);
	GLint const normSamplerULocation = glGetUniformLocation(ShaderProgram0, "normSampler");
	glUniform1i(normSamplerULocation, 1);
	GLint const specSamplerULocation = glGetUniformLocation(ShaderProgram0, "specSampler");
	glUniform1i(specSamplerULocation, 2);
}

// ************************************************************************************************
// *** Other methods implementation ***************************************************************
/// Initialize buffer objects
bool initModels() {

	mesh_objects.emplace_back("models\\rover\\rover_body_shell.obj", material_objects[0]);		// green shell-textured rover bodyparts
	mesh_objects.emplace_back("models\\rover\\rover_static_metal.obj", material_objects[1]);	// non-arm metal parts
	mesh_objects.emplace_back("models\\rover\\rover_temp_parts.obj", material_objects[1]);		// not yet correctly textured parts
	mesh_objects.emplace_back("models\\rover\\rover_arm0.obj", material_objects[1]);			// lowest robotic arm joint
	mesh_objects.emplace_back("models\\rover\\rover_arm1.obj", material_objects[1]);
	mesh_objects.emplace_back("models\\rover\\rover_arm2.obj", material_objects[1]);
	mesh_objects.emplace_back("models\\rover\\rover_arm3.obj", material_objects[1]);			// outermost robotic arm joint
	mesh_objects.emplace_back("models\\rover\\rover_gratings.obj", material_objects[1]);		// hopefully to be shaded with cutout material
	mesh_objects.emplace_back("models\\rover\\rover_body_flooring.obj", material_objects[2]);	// floor-textured rover bodyparts
	mesh_objects.emplace_back("models\\rover\\rover_wings.obj", material_objects[3]);			// wing/solar panel parts
	mesh_objects.emplace_back("models\\rover\\rover_static_hoses.obj", material_objects[4]);	// non-arm hoses
	mesh_objects.emplace_back("models\\rover\\rover_arm1_hose.obj", material_objects[4]);		// hose connected to arm1
	mesh_objects.emplace_back("models\\rover\\rover_arm2_hose.obj", material_objects[4]);		// hose connected to arm2

	for (auto& el : mesh_objects)
		if (!el.successfullyImported)
			return false;
	return true;
}

bool initTextures()
{
	// [0] grimy green rover body shell textures
	material_objects.emplace_back(
		"models\\rover\\shell_body_diff.png",
		"models\\rover\\crystalshell_norm.png",
		"models\\rover\\crystalshell_ao.png");
	// [1] rust textures
	material_objects.emplace_back(
		"models\\rover\\rust_comb.png",
		"models\\rover\\rust_norm.png",
		"models\\rover\\rust_spec.png");
	// [2] walkable grating textures
	material_objects.emplace_back(
		"models\\rover\\grating_comb.png",
		"models\\rover\\grating_norm.png",
		"models\\rover\\grating_ao.png");
	// [3] wing (solar panel) textures
	material_objects.emplace_back(
		"models\\rover\\wings_diff.png",
		"models\\rover\\wings_norm.png",
		"models\\rover\\wings_spec.png");
	// [4] grimy red hose textures
	material_objects.emplace_back(
		"models\\rover\\hose_diff.png",
		"models\\rover\\rust_spec.png",
		"models\\rover\\rust_spec.png");



	for (auto& el : material_objects)
		if (!el.successfullyImported)
			return false;
	return true;
}

/// Initialize shaders. Return false if initialization fail
bool initShaders() {
	// Create the shader program and check for errors
	if (ShaderProgram0 != 0)
		glDeleteProgram(ShaderProgram0);
	ShaderProgram0 = glCreateProgram();
	if (ShaderProgram0 == 0) {
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
	string text = readTextFile("shaders\\vshader.glsl");
	const char* code = text.c_str();
	int length = static_cast<int>(text.length());
	if (length == 0)
		return false;
	glShaderSource(vertShader, 1, &code, &length);

	// Read and set the source code for the fragment shader
	string text2 = readTextFile("shaders\\fshader.glsl");
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
	glAttachShader(ShaderProgram0, vertShader);
	glAttachShader(ShaderProgram0, fragShader);
	glLinkProgram(ShaderProgram0);

	// Check for linking error
	glGetProgramiv(ShaderProgram0, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram0, 1024, nullptr, errorLog);
		cerr << "Error: cannot link shader program.\nError log:\n" << errorLog << endl;
		getchar();
		return false;
	}

	// Make sure that the shader program can run
	glValidateProgram(ShaderProgram0);

	// Check for validation error
	glGetProgramiv(ShaderProgram0, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram0, 1024, nullptr, errorLog);
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
	string text;
	string line;
	while (!fileIn.eof()) {
		getline(fileIn, line);
		text += line + "\n";
		const bool bad = fileIn.bad();
		const bool fail = fileIn.fail();
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
	case 'v':
		viewMode = (viewMode++) % 6;
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

/* --- eof main.cpp --- */
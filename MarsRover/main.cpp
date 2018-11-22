#include <GL/glew.h>
#include <GL/glut.h>
#include <gl/GL.h>

#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <ctime>
#include <chrono>

#include "create_matrix.h"
#include "Camera.h"

#include "spline/BSpline.h"
#include "MeshObjectDiffNormalSpec.h"
#include "MeshObjectUnlitDiffuse.h"
#include "SceneNode.h"
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
bool initShader(GLuint &shader_program, const string& vshaderpath, const string& fshaderpath);
void initSpline(bool loop);
shared_ptr<SceneNode> CreateNode(const char * model_path,MaterialObject &material,
	const Matrix4f &transf_local,GLuint &shader_program,const shared_ptr<SceneNode> &parent);
string readTextFile(const string&);

// --- Global variables ---------------------------------------------------------------------------

// Shaders
GLuint ShaderProgram0 = 0;	///< A shader program: Phong diff/norm/specmapped
GLuint ShaderProgram1 = 0;	///< A shader program: Unlit diff

// Materials and textures
vector<MaterialObject> material_objects;

// Vertex transformations
Vector3f Translation;	///< Translation
float RotationX;
float RotationY;
float Scaling;			///< Scaling
Matrix4<float> projection;
Matrix4<float> transformation;
create_matrix cm;

// Mouse interactions
int MouseX, MouseY;		///< The last position of the mouse
int MouseButton;		///< The last mouse button pressed or released

// Toggled Mode
bool wireframe = false;
bool orthographic = false;
int viewMode = 0;

// Animation:
bool animateScene = false;
bool animateAlongPath = false;
chrono::high_resolution_clock::time_point previousFrameTime;
Camera Cam;

// Curve stuff
Curve* curve;
double position_along_path = 0.;
int node_number = 0;
Vector3f PositionAlongPath(const double &delta_time, const double &movespeed);

// Scene graph nodes:
vector<shared_ptr<SceneNode>> scene_nodes;
const auto root_node = make_shared<SceneNode>(nullptr);
shared_ptr<SceneNode> rover_body_node;
shared_ptr<SceneNode> rover_arm0;
shared_ptr<SceneNode> rover_arm1;
shared_ptr<SceneNode> rover_arm2;
shared_ptr<SceneNode> rover_arm3;

// --- main() -------------------------------------------------------------------------------------
/// The entry point of the application
int main(int argc, char **argv) {

	cout <<
		'\n' << "CONTROLS:" <<
		'\n' <<
		'\n' << "Left mouse rotates" <<
		'\n' << "Middle mouse changes fov" <<
		'\n' << "Right mouse moves camera" <<
		'\n' << "wasd, space and c: first-person movement" <<
		'\n' <<
		'\n' << "r: resets camera" <<
		'\n' << "p: toggle wireframe-mode" <<
		'\n' << "t: toggle model rotation" <<
		'\n' << "v: cycle between different fragment output modes" <<
		'\n' << "l: reload shaders" <<
		'\n' << "q: quit program" << endl;


	// Initialize glut and create a simple window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //added glut_depth
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(300, 50);
	glutCreateWindow("OpenGL Tutorial");

	// Initialize OpenGL callbacks
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	// Initialize glew (must be done after glut is initialized!)
	const GLenum res = glewInit();
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

	// init spline loop
	initSpline(true);

	// Shaders, Textures & buffers
	if (!initShaders() || !initTextures() || !initModels())
	{
		cout << "Press Enter to exit..." << endl;
		getchar();
		return -1;
	}

	glutSetCursor(GLUT_CURSOR_CROSSHAIR); // hide the cursor

	//Record current time
	previousFrameTime = chrono::high_resolution_clock::now();

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

	// Calculate camera projection and set window aspect ratio
	Cam.ar = static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) / static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT));
	projection = Cam.computeCameraTransform();

	// Enable the vertex attributes and set their format
	glEnableVertexAttribArray(0); //pos
	glEnableVertexAttribArray(1); //uv-coord
	glEnableVertexAttribArray(2); //normals

	//Trigger drawing of scene from scenegraph root
	root_node->Draw(projection);

	//cleanup VAOs
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

/// Called at regular intervals (can be used for animations)      (note: actually kind of irregular intervals...)
void idle()
{
	// Record current time and elapsed time since last frame as double with higher precision
	const auto time_now = chrono::high_resolution_clock::now();
	const auto delta_time = (time_now - previousFrameTime).count() * .000001;

	if (animateScene) {

		// Model rotation
		const double rotationSpeed = 0.1;
		RotationX += rotationSpeed * delta_time;

		// Movement along BSpline
		if (animateAlongPath)
			Translation = PositionAlongPath(delta_time, .005);

		transformation = cm.create_transformation_matrix(Translation, RotationX, RotationY, Scaling);
		rover_body_node->SetTransform(transformation);
	}
	previousFrameTime = time_now;

	glutPostRedisplay();
}

Vector3f PositionAlongPath(const double &delta_time, const double &movespeed)
{
	const int nodeCount = curve->node_count();
	const double deltaMovement = delta_time * movespeed;
	position_along_path += deltaMovement;

	if (curve->loop){
		if (position_along_path > curve->total_length())
			position_along_path -= curve->total_length();
	}
	else
	{
		position_along_path = min(position_along_path, curve->total_length()-1);
		animateAlongPath = false;
	}

	const auto position_rescaled = position_along_path / curve->total_length() * curve->node_count();
	const auto node_number = static_cast<int>(position_rescaled);

	// interpolate the position between the pre-interpolated positions:
	const auto node_number_a_influence = position_rescaled - node_number;
	const auto interpolatedPos = curve->getInterpolatedPosition(node_number_a_influence, node_number);

	// from double Vector to Vector3f:
	return Vector3f(interpolatedPos.x, interpolatedPos.y, interpolatedPos.z);
}

// ************************************************************************************************
// *** Other methods implementation ***************************************************************

void initSpline(const bool loop)
{
	curve = new BSpline();
	curve->set_steps(100); // generate 100 interpolate points between the last 4 way points
	curve->loop = loop;

	//control points:
	curve->add_way_point(Vector(-6, -2, -16));
	curve->add_way_point(Vector(-6, -2, -8));
	curve->add_way_point(Vector(6, -2, -8));
	curve->add_way_point(Vector(6, -2, -16));
	if (loop)
	{
		//first 3 points added again for looping BSpline
		for (auto i = 0; i < 3; i++)
			curve->add_way_point(curve->_way_points[i]);
	}

	//cout << "nodes: " << curve->node_count() << endl;
	//cout << "total length: " << curve->total_length() << endl;
}

/// creates a SceneNode, attach mesh, program and textures to it and add it to scene_nodes
shared_ptr<SceneNode> CreateNode(
	const char * model_path,
	MaterialObject &material,
	const Matrix4f &transf_local,
	GLuint &shader_program,
	const shared_ptr<SceneNode> &parent)
{
	unique_ptr<MeshObject> new_object;

	if (shader_program == ShaderProgram0)
		new_object = make_unique<MeshObjectDiffNormalSpec>(model_path, material, shader_program);
	else if (shader_program == ShaderProgram1)
		new_object = make_unique<MeshObjectUnlitDiffuse>(model_path, material, shader_program);
	else new_object = nullptr;

	const auto new_node = make_shared<SceneNode>(std::move(new_object));
	new_node->SetTransform(transf_local);
	parent->AddChild(new_node);
	scene_nodes.push_back(new_node);
	return new_node;
}
/// Initialize buffer objects
bool initModels() {
	const Matrix4f identitymatrix;

	root_node->SetTransform(identitymatrix);
	root_node->Update();
	scene_nodes.push_back(root_node);

	// diff / norm / spec mapped objects:

	rover_body_node = CreateNode("models\\rover\\rover_body_shell.obj", material_objects[0], identitymatrix, ShaderProgram0, root_node);	// green shell - textured rover bodyparts

	rover_arm0 = CreateNode("models\\rover\\rover_arm0.obj", material_objects[1], identitymatrix, ShaderProgram0, rover_body_node);		// lowest robotic arm joint
	rover_arm1 = CreateNode("models\\rover\\rover_arm1.obj", material_objects[1], identitymatrix, ShaderProgram0, rover_arm0);			//
	rover_arm2 = CreateNode("models\\rover\\rover_arm2.obj", material_objects[1], identitymatrix, ShaderProgram0, rover_arm1);			//
	rover_arm3 = CreateNode("models\\rover\\rover_arm3.obj", material_objects[1], identitymatrix, ShaderProgram0, rover_arm2);			// outermost robotic arm joint
	CreateNode("models\\rover\\rover_arm1_hose.obj", material_objects[4], identitymatrix, ShaderProgram0, rover_arm1);					// hose connected to arm1
	CreateNode("models\\rover\\rover_arm2_hose.obj", material_objects[4], identitymatrix, ShaderProgram0, rover_arm2);					// hose connected to arm2

	CreateNode("models\\rover\\rover_static_metal.obj", material_objects[1], identitymatrix, ShaderProgram0, rover_body_node);			// non-arm metal parts
	CreateNode("models\\rover\\rover_temp_parts.obj", material_objects[1], identitymatrix, ShaderProgram0, rover_body_node);			// not yet correctly textured parts
	CreateNode("models\\rover\\rover_gratings.obj", material_objects[1], identitymatrix, ShaderProgram0, rover_body_node);				// hopefully to be shaded with cutout material
	CreateNode("models\\rover\\rover_body_flooring.obj", material_objects[2], identitymatrix, ShaderProgram0, rover_body_node);			// floor-textured rover bodyparts
	CreateNode("models\\rover\\rover_wings.obj", material_objects[3], identitymatrix, ShaderProgram0, rover_body_node);					// wing/solar panel parts
	CreateNode("models\\rover\\rover_static_hoses.obj", material_objects[4], identitymatrix, ShaderProgram0, rover_body_node);			// non-arm hoses

	// environment:

	const auto environment_surface_node = CreateNode("models\\aquarium\\terrain_resculpt.obj", material_objects[8], identitymatrix, ShaderProgram0, root_node);	// terrain surface
	environment_surface_node->SetTransform(cm.create_transformation_matrix(Vector3f(0.f, -20.f, 0.f), 0.f, 0.f, 1.f));
	// unlit objects:
	CreateNode("models\\aquarium\\skalle_03_preplaced.obj", material_objects[6], identitymatrix, ShaderProgram1, environment_surface_node);
	CreateNode("models\\aquarium\\environment_sphere.obj", material_objects[7], identitymatrix, ShaderProgram1, environment_surface_node);
	CreateNode("models\\aquarium\\terrain_resculpt-sides.obj", material_objects[9], identitymatrix, ShaderProgram1, environment_surface_node);
	//CreateNode("models\\aquarium\\akvariemesh.obj", material_objects[10], identitymatrix, ShaderProgram1, environment_surface_node);

	// double-check if all objects has been sucessfully imported
	for (auto& el : scene_nodes)
	{
		if (!el->SuccessfullyImported())
			return false;
	}

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
	// [5] unlit texture
	material_objects.emplace_back("models\\rover\\shell_body_diff.png");
	// [6] skull
	material_objects.emplace_back("models\\aquarium\\skalle03_DiffM.png");
	// [7] environment sphere
	material_objects.emplace_back("models\\aquarium\\grad_pink.png");

	// [8] terrain surface
	material_objects.emplace_back(
		"models\\aquarium\\tunneled_terrain02-DiffM.png",
		"models\\aquarium\\tunneled_terrain02-NM.png",
		"models\\aquarium\\tunneled_terrain02-DM.png");
	// [9] terrain sides
	material_objects.emplace_back("models\\aquarium\\terrain_sides-DiffM.png");
	// [10] akvariesider
	material_objects.emplace_back("models\\aquarium\\akvarie_opac.png");


	for (auto& el : material_objects)
		if (!el.successfullyImported)
			return false;
	return true;
}

bool initShaders()
{
	return
		initShader(ShaderProgram0, "shaders\\vshader.glsl", "shaders\\fshader.glsl") &&								// diff/norm/spec mapped
		initShader(ShaderProgram1, "shaders\\shader.unlitdiffuse.v.glsl", "shaders\\shader.unlitdiffuse.f.glsl");	// unlit diffusemapped
}

/// Initialize shaders. Return false if initialization fail
bool initShader(GLuint &shader_program, const string& vshaderpath, const string& fshaderpath) {
	// Create the shader program and check for errors
	if (shader_program != 0)
		glDeleteProgram(shader_program);
	shader_program = glCreateProgram();
	if (shader_program == 0) {
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
	string text = readTextFile(vshaderpath);
	const char* code = text.c_str();
	int length = static_cast<int>(text.length());
	if (length == 0)
		return false;
	glShaderSource(vertShader, 1, &code, &length);

	// Read and set the source code for the fragment shader
	string text2 = readTextFile(fshaderpath);
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
	glAttachShader(shader_program, vertShader);
	glAttachShader(shader_program, fragShader);
	glLinkProgram(shader_program);

	// Check for linking error
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram0, 1024, nullptr, errorLog);
		cerr << "Error: cannot link shader program.\nError log:\n" << errorLog << endl;
		getchar();
		return false;
	}

	// Make sure that the shader program can run
	glValidateProgram(shader_program);

	// Check for validation error
	glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 1024, nullptr, errorLog);
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

void keyboard(unsigned char key, int x, int y) {
	Vector3f right;

	// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
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
		break;
	case 't':
		animateScene = !animateScene;
		break;
	case 'y':
		animateAlongPath = !animateAlongPath;
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
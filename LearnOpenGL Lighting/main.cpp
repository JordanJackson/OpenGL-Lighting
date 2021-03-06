#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW\glfw3.h>

// Other libs
#include <SOIL.h>

// GLM Math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Other includes
#include "Light.h"
#include "Material.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Transform.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

const GLuint NUM_POINT_LIGHTS = 4;

const glm::vec3 horrorDefault = glm::vec3(0.1f, 0.1f, 0.1f);
const glm::vec3 horrorRed = glm::vec3(0.3f, 0.1f, 0.1f);

Material mat = Material(glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f);
DirLight dirLight = DirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.2f, 0.2f, 0.2f));
PointLight pointLights[NUM_POINT_LIGHTS] =
{
	PointLight(glm::vec3(0.7f,  0.2f,  2.0f), horrorDefault, horrorDefault, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f),
	PointLight(glm::vec3(2.3f, -3.3f, -4.0f), horrorDefault, horrorDefault, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f),
	PointLight(glm::vec3(-4.0f,  2.0f, -12.0f), horrorDefault, horrorDefault, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f),
	PointLight(glm::vec3(0.0f,  0.0f, -3.0f), horrorRed, horrorRed, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f)
};
SpotLight spotLight = SpotLight(camera.Position(), camera.Front(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)));

GLfloat x = 0.0f;
GLfloat y = 90.0f;
GLfloat z = 0.0f;

Transform t = Transform(glm::vec3(0.0f, -1.75f, 0.0f), glm::quat(), glm::vec3(0.02f, 0.02f, 0.02f));

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Build and compile our shader program
	Shader lightingShader("shaders/lighting.vert", "shaders/lighting.frag");
	Shader lampShader("shaders/lamp.vert", "shaders/lamp.frag");
	Shader modelShader("shaders/model_loading.vert", "shaders/model_loading.frag");

	// Load models
	//Model ourModel("models/Nanosuit/nanosuit.obj");
	Model ourModel("models/Paladin/Paladin_J_Nordstrom.dae");
	Model animModel("models/Paladin_Idle/idle.dae");


	Transform* t2 = new Transform(glm::vec3(100.0f, 0.0f, -5.0f), glm::quat(), glm::vec3(1.0f));
	
	t2->setParent(t);

	// Draw in wireframe
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions           // Normals           // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// First, set the container's VAO (and VBO)
	GLuint VBO, containerVAO;
	glGenVertexArrays(1, &containerVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(containerVAO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TextureCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// Load textures
	int width, height;
	unsigned char* image;
	// Diffuse map
	image = SOIL_load_image("textures/container2.png", &width, &height, 0, SOIL_LOAD_RGB);
	mat.InitializeDiffuseMap(image, width, height);
	SOIL_free_image_data(image);

	// Specular map
	image = SOIL_load_image("textures/container2_specular.png", &width, &height, 0, SOIL_LOAD_RGB);
	mat.InitializeSpecularMap(image, width, height);
	SOIL_free_image_data(image);

	// Emission map
	image = SOIL_load_image("textures/matrix.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	mat.InitializeEmissionMap(image, width, height);
	SOIL_free_image_data(image);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), mat.DiffuseMap());
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), mat.SpecularMap());
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.emission"), mat.EmissionMap());

	GLfloat timer = 0.0f;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		timer += deltaTime;

		x += 5.0f * deltaTime;
		y += 5.0f * deltaTime;
		z += 5.0f * deltaTime;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.Position().x, camera.Position().y, camera.Position().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), mat.Shininess());

		// Set directional light uniforms
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), dirLight.direction.x, dirLight.direction.y, dirLight.direction.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), dirLight.ambient.x, dirLight.ambient.y, dirLight.ambient.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), dirLight.diffuse.x, dirLight.diffuse.y, dirLight.diffuse.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), dirLight.specular.x, dirLight.specular.y, dirLight.specular.z);

		// Set point light uniforms
		glUniform1i(glGetUniformLocation(lightingShader.Program, "pointLightCount"), NUM_POINT_LIGHTS);

		for (GLuint i = 0; i < NUM_POINT_LIGHTS; i++)
		{
			std::string idx = std::to_string(i);

			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + idx + "].position").c_str()), pointLights[i].position.x, pointLights[i].position.y, pointLights[i].position.z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + idx + "].ambient").c_str()), pointLights[i].ambient.x, pointLights[i].ambient.y, pointLights[i].ambient.z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + idx + "].diffuse").c_str()), pointLights[i].diffuse.x, pointLights[i].diffuse.y, pointLights[i].diffuse.z);
			glUniform3f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + idx + "].specular").c_str()), pointLights[i].specular.x, pointLights[i].specular.y, pointLights[i].specular.z);

			glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + idx + "].constant").c_str()), pointLights[i].constant);
			glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + idx + "].linear").c_str()), pointLights[i].linear);
			glUniform1f(glGetUniformLocation(lightingShader.Program, ("pointLights[" + idx + "].quadratic").c_str()), pointLights[i].quadratic);
		}
		spotLight.position = camera.Position();
		spotLight.direction = camera.Front();
		// Set spot light uniforms
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), spotLight.position.x, spotLight.position.y, spotLight.position.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), spotLight.direction.x, spotLight.direction.y, spotLight.direction.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), spotLight.ambient.x, spotLight.ambient.y, spotLight.ambient.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), spotLight.diffuse.x, spotLight.diffuse.y, spotLight.diffuse.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), spotLight.specular.x, spotLight.specular.y, spotLight.specular.z);

		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), spotLight.constant);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), spotLight.linear);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), spotLight.quadratic);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), spotLight.cutOff);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), spotLight.outerCutOff);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Zoom(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Bind diffuse map
		glActiveTexture(GL_TEXTURE0 + mat.DiffuseMap());
		glBindTexture(GL_TEXTURE_2D, mat.DiffuseMap());
		// Bind specular map
		glActiveTexture(GL_TEXTURE0 + mat.SpecularMap());
		glBindTexture(GL_TEXTURE_2D, mat.SpecularMap());
		// Bind emission map
		glActiveTexture(GL_TEXTURE0 + mat.EmissionMap());
		glBindTexture(GL_TEXTURE_2D, mat.EmissionMap());

		/*
		// Draw the container (using container's vertex attributes)
		glBindVertexArray(containerVAO);
		glm::mat4 model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		*/

		// Draw 10 containers with the same VAO and VBO info
		glm::mat4 model;
		glBindVertexArray(containerVAO);
		for (GLuint i = 0; i < 10; i++)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// draw lamps
		for (GLuint i = 0; i < NUM_POINT_LIGHTS; i++)
		{
			glUniform4f(glGetUniformLocation(lampShader.Program, "Color"), pointLights[i].diffuse.x, pointLights[i].diffuse.y, pointLights[i].diffuse.z, 1.0f);
			model = glm::mat4();
			model = glm::translate(model, pointLights[i].position);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			// Draw the light object (using light's vertex attributes)
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);
		
		// Draw nanosuit
		modelShader.Use();
		projection = glm::perspective(camera.Zoom(), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		model = glm::mat4();

		t.computeMatrix(glm::mat4());
		model = t.getMatrix();
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		ourModel.Draw(modelShader);

		model = t2->getMatrix();
		glUniformMatrix4fv(glGetUniformLocation(modelShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		ourModel.Draw(modelShader);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;

}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (keys[GLFW_KEY_LEFT])
	{
		t.translate(-t.right() * 0.1f);
	}
	if (keys[GLFW_KEY_RIGHT])
	{
		t.translate(t.right() * 0.1f);
	}
	if (keys[GLFW_KEY_UP])
	{
		t.translate(t.forward() * 0.1f);
	}
	if (keys[GLFW_KEY_DOWN])
	{
		t.translate(-t.forward() * 0.1f);
	}
	if (keys[GLFW_KEY_Q])
	{
		t.rotate(0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (keys[GLFW_KEY_E])
	{
		t.rotate(-0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
	}
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/freeglut.h>
#include <FreeImage.h>
#include "Shader.h"
#include "Mesh.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();
void loadTextures();
enum class movement { UP, Back, Front, Down };

// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 1000;

//Texture
const int numTextures = 7;
GLuint texID[numTextures];
const char* textureFileNames[numTextures];

// Camera
float camUpDown = 0.0;
glm::vec3 cameraPos = glm::vec3(-5.0f, -1.5f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
GLfloat cameraSpeed;

// Deltatime
GLfloat deltaTime = 0.0f; 
GLfloat lastFrame = 0.0f;

//Tiles Texture 
float tileX = 0;
float tileZ = 0;

//Rotation Angle for tree
float angle = 0.0;

//Light Coordinates 
float x = 15.0f, y = 10.0f, z = 10.0f;

//Main rendering loop starts here 
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(WIDTH, HEIGHT, "Car Garage", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, HEIGHT, HEIGHT);

	glEnable(GL_DEPTH_TEST);

	//For shader that render texture over the object 
	GLuint shaderProgram = initShader("vert.glsl", "texFragBlinn.glsl");
	GLuint shaderProgram2 = initShader("vert.glsl", "texFrag.glsl");
	//For shader that add light effects 
	GLuint lampshader = initShader("lampvert.glsl", "lampfrag.glsl");

	// Load meshes and textures
	const int numModels = 5;
	Mesh mesh[numModels];
	mesh[0].loadOBJ("models/InnerBlackGrage.obj");
	mesh[1].loadOBJ("models/body.obj");
	mesh[2].loadOBJ("models/cooler.obj");
	mesh[3].loadOBJ("models/Walls.obj");
	mesh[4].loadOBJ("models/Roof.obj");

	Mesh frame;
	frame.loadOBJ("models/Jeep/Jeep_Renegade_2016.obj");
	
	// Model positions
	glm::vec3 modelPosition = glm::vec3(-4.5f, -1.9f, 10.5f);
	glm::vec3 garagePosition = glm::vec3(-4.5f, -1.3f, 10.5f);
	// Model scale
	glm::vec3 modelScaling = glm::vec3(0.6f, 0.6f, 0.6f);

	//Vertices	Normal		Textures
	GLfloat Surface[] =
	{
		-100.0,-2.0,  100.5, 1.0f, 0.0f, 0.0f, 0,0,
		 -90.0,-2.0,  100.5, 1.0f, 0.0f, 0.0f, 0,1,
		 -90.0,-2.0,  90.0,1.0f, 0.0f, 0.0f, 1,1,

		 -90.0,-2.0,  90.0, 1.0f, 0.0f, 0.0f, 1,1,
		-100.0,-2.0,  90.0, 1.0f, 0.0f, 0.0f, 1,0,
		-100.0,-2.0,  100.5, 1.0f, 0.0f, 0.0f, 0,0,
	};

	GLuint VBO[3], VAO[3];
	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	//Surface 
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Surface), Surface, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	//Light Object vertices
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the normal vectors
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//Setting texture Images 
	textureFileNames[0] = "textures/black.jpg";
	textureFileNames[1] = "textures/grey.png";
	textureFileNames[2] = "textures/tableTop.jpeg";
	textureFileNames[3] = "textures/brick.jpg";
	textureFileNames[4] = "textures/white.jpg";
	textureFileNames[5] = "models/Jeep/Jeep_Renegade_2016/car_jeep_ren.jpg";
	textureFileNames[6] = "textures/tiles.jpg";
	loadTextures();

	// Light attributes
	glm::vec3 lightPos2(x, y, z);

	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		do_movement();

		glClearColor(0.0, 0.8, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLint objectColorLoc;
		GLint lightColorLoc;
		GLint lightPosLoc;
		GLint viewPosLoc;

		// Create camera transformations
		glm::mat4 view;
		glm::mat4 projection;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		// Get the uniform locations
		GLint modelLoc;
		GLint viewLoc;
		GLint projLoc;
		GLint blinn;
		GLint texture1;
		
		//Setting Up Shader Program 
		glUseProgram(shaderProgram);

		objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
		
		// Get the uniform locations
		modelLoc = glGetUniformLocation(shaderProgram, "model");
		viewLoc = glGetUniformLocation(shaderProgram, "view");
		projLoc = glGetUniformLocation(shaderProgram, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//Tiles Surface
		for (int i = 0; i < 50; i++) {
			tileX = 0;
			for (int j = 0; j < 50; j++) {
				glm::mat4 grassPos;
				glBindVertexArray(VAO[1]);
				glm::vec3 grassVec(tileX, 0, tileZ);
				grassPos = glm::translate(grassPos, grassVec);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texID[6]);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(grassPos));
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(0);
				tileX += 10;
			}
			tileZ -= 10;
		}
		tileZ = 0;
		
		//Loading Garage Models
		glm::mat4 model;
		for (int i = 0; i < numModels; i++) {
			model = glm::translate(glm::mat4(1.0), garagePosition) * glm::scale(glm::mat4(1.0), modelScaling);
			model = glm::rotate(model, angle, glm::vec3(0, 1, 0));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texID[i]);
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			mesh[i].draw();
			glBindVertexArray(0);
		}

		//Shader for Car Model
		glUseProgram(shaderProgram2);

		objectColorLoc = glGetUniformLocation(shaderProgram2, "objectColor");
		lightColorLoc = glGetUniformLocation(shaderProgram2, "lightColor");
		lightPosLoc = glGetUniformLocation(shaderProgram2, "lightPos");
		viewPosLoc = glGetUniformLocation(shaderProgram2, "viewPos");
		viewPosLoc = glGetUniformLocation(shaderProgram2, "viewPos");

		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);

		// Get the uniform locations
		modelLoc = glGetUniformLocation(shaderProgram2, "model");
		viewLoc = glGetUniformLocation(shaderProgram2, "view");
		projLoc = glGetUniformLocation(shaderProgram2, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		glm::mat4 model2;
		model2 = glm::translate(glm::mat4(1.0), modelPosition) * glm::scale(glm::mat4(1.0), modelScaling);
		model2 = glm::rotate(model2, angle, glm::vec3(0, 1, 0));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID[5]);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
		frame.draw();
		glBindVertexArray(0);
		

		//Shader for Light
		glUseProgram(lampshader);

		modelLoc = glGetUniformLocation(lampshader, "model");
		viewLoc = glGetUniformLocation(lampshader, "view");
		projLoc = glGetUniformLocation(lampshader, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4();
		model = glm::translate(model, lightPos2);
		model = glm::scale(model, glm::vec3(0.2f));

		glPushMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glutSolidSphere(1.0, 30, 30);
		glPopMatrix();

		glfwSwapBuffers(window);
	}

	return 0;
}
float speed = 5.0f;
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
	cameraSpeed = speed * deltaTime;
	if (keys[GLFW_KEY_X]) {
		x += 0.01;
	}
	if (keys[GLFW_KEY_Y]) {
		y += 0.01;
	}
	if (keys[GLFW_KEY_Z]) {
		z += 0.01;
	}

	// Camera controls
	cameraSpeed = speed * deltaTime;
	if (keys[GLFW_KEY_UP]) {
		cameraPos.x += (cameraSpeed) * (cameraFront.x);
		cameraPos.z += (cameraSpeed) * (cameraFront.z);
	}
	if (keys[GLFW_KEY_DOWN]) {
		cameraPos.x -= (cameraSpeed) * (cameraFront.x);
		cameraPos.z -= (cameraSpeed) * (cameraFront.z);
	}
	if (keys[GLFW_KEY_LEFT])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_RIGHT])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
		firstMouse = false;
	}

	GLfloat xoffset = (GLfloat)xpos - lastX;
	GLfloat yoffset = lastY - (GLfloat)ypos; // Reversed since y-coordinates go from bottom to left
	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;

	GLfloat sensitivity = (GLfloat)0.05; // Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);


}


void loadTextures()
{
	int i;
	glGenTextures(numTextures, texID);
	for (i = 0; i < numTextures; i++) {
		void* imgData;  
		int imgWidth;   
		int imgHeight;  
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(textureFileNames[i]);
		if (format == FIF_UNKNOWN) {
			printf("Unknown file type for texture image file %s\n", textureFileNames);
			continue;
		}
		FIBITMAP* bitmap = FreeImage_Load(format, textureFileNames[i], 0);
		if (!bitmap) {
			printf("Failed to load image %s\n", textureFileNames[i]);
			continue;
		}
		FIBITMAP* bitmap2 = FreeImage_ConvertTo24Bits(bitmap);
		FreeImage_Unload(bitmap);
		imgData = FreeImage_GetBits(bitmap2);
		imgWidth = FreeImage_GetWidth(bitmap2);
		imgHeight = FreeImage_GetHeight(bitmap2);
		if (imgData) {
			printf("Texture image loaded from file %s, size %dx%d\n",
				textureFileNames[i], imgWidth, imgHeight);
			int format;
			if (FI_RGBA_RED == 0)
				format = GL_RGB;
			else
				format = GL_BGR;
			glBindTexture(GL_TEXTURE_2D, texID[i]);  // Will load image data into texture object #i
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, format,
				GL_UNSIGNED_BYTE, imgData);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Required since there are no mipmaps.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // (Actually, this one is the default)
			//stbi_image_free(imgData);
		}
		else {
			printf("Failed to get texture data from %s\n", textureFileNames[i]);
		}

	}
}



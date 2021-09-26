#include "Demo.h"



Demo::Demo() {

}


Demo::~Demo() {
}



void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	shaderProgram = BuildShader("vertexShader.vert", "fragmentShader.frag", nullptr);

	BuildColoredCube();

	BuildColoredPlane();

	InitCamera();
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.0001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.0001f;
		}
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		StrafeCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		StrafeCamera(CAMERA_SPEED);
	}

	// update camera rotation
	// ----------------------
	double mouseX, mouseY;
	double midX = screenWidth / 2;
	double midY = screenHeight / 2;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	// Get mouse position
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if ((mouseX == midX) && (mouseY == midY)) {
		return;
	}

	// Set mouse position
	glfwSetCursorPos(window, midX, midY);

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angleY = (float)((midX - mouseX)) / 1000;
	angleZ = (float)((midY - mouseY)) / 1000;

	// The higher the value is the faster the camera looks around.
	viewCamY += angleZ * 2;

	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}
	RotateCamera(-angleY);




}

void Demo::Update(double deltaTime) {

}

void Demo::Render() {
	glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(fovy, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::mat4 view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(upCamX, upCamY, upCamZ));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	DrawColoredCube();

	DrawColoredPlane();

	glDisable(GL_DEPTH_TEST);
}

void Demo::BuildColoredCube() {
	// load image into texture memory
	// ------------------------------
	// Load and create a texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height;
	unsigned char* image = SOIL_load_image("crate.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, tex coords
		// dudukan kursi
		//depan
		-1.8, 3.0, 1.6, 0, 0,	// 0
		1.8, 3.0, 1.6, 1, 0,	// 1
		1.8, 3.4, 1.6, 1, 1,	// 2
		-1.8, 3.4, 1.6, 0, 1,   // 3 
		//kanan
		1.8, 3.0, -1.6, 0, 0,	// 4
		1.8, 3.4, -1.6, 1, 0,	// 5
		1.8, 3.4, 1.6, 1, 1,	// 6
		1.8, 3.0, 1.6, 0, 1,	// 7
		//belakang
		-1.8, 3.0, -1.6, 0, 0, // 8
		-1.8, 3.4, -1.6, 1, 0, // 9
		1.8, 3.4, -1.6, 1, 1,	// 10
		1.8, 3.0, -1.6, 0, 1,	// 11
		//kiri
		-1.8, 3.0, -1.6, 0, 0,	// 12
		-1.8, 3.0, 1.6, 1, 0,	// 13
		-1.8, 3.4, 1.6, 1, 1,	// 14
		-1.8, 3.4, -1.6, 0, 1,	// 15
		//atas
		1.8, 3.4, 1.6, 0, 0,	// 16
		-1.8, 3.4, 1.6, 1, 0,	// 17
		-1.8, 3.4, -1.6, 1, 1,	// 18
		1.8, 3.4, -1.6, 0, 1,	// 19
		//bawah
		1.8, 3.0, 1.6, 0, 0,	// 20
		-1.8, 3.0, 1.6, 1, 0,	// 21
		-1.8, 3.0, -1.6, 1, 1,	// 22
		1.8, 3.0, -1.6, 0, 1,	// 23

		//kaki kanan depan
		//depan
		 1.4, 0.0, 1.6, 0, 0,	// 24
		 1.8, 0.0, 1.6, 1, 0,	// 25
		 1.8, 3.0, 1.6, 1, 1,	// 26
		 1.4, 3.0, 1.6, 0, 1,	// 27

		//belakang
		 1.4, 0.0, 1.2, 0, 0,	// 28
		 1.8, 0.0, 1.2, 1, 0,	// 29
		 1.8, 3.0, 1.2, 1, 1,	// 30
		 1.4, 3.0, 1.2, 0, 1,	// 31

		//kiri
		 1.4, 0.0, 1.6, 0, 0,	// 32
		 1.4, 0.0, 1.2, 1, 0,	// 33
		 1.4, 3.0, 1.2, 1, 1,	// 34
		 1.4, 3.0, 1.6, 0, 1,	// 35

		//kanan
		 1.8, 0.0, 1.6, 0, 0,	// 36
		 1.8, 0.0, 1.2, 1, 0,	// 37
		 1.8, 3.0, 1.2, 1, 1,	// 38
		 1.8, 3.0, 1.6, 0, 1,	// 39

		//kaki kanan belakang
		//depan
		 1.4, 0.0, -1.2, 0, 0,	// 40
		 1.8, 0.0, -1.2, 1, 0,	// 41
		 1.8, 3.0, -1.2, 1, 1,	// 42
		 1.4, 3.0, -1.2, 0, 1,	// 43

		//belakang
		 1.4, 0.0, -1.6, 0, 0,	// 44
		 1.8, 0.0, -1.6, 1, 0,	// 45
		 1.8, 3.0, -1.6, 1, 1,	// 46
		 1.4, 3.0, -1.6, 0, 1,	// 47

		//kiri
		 1.4, 0.0, -1.2, 0, 0,	// 48
		 1.4, 0.0, -1.6, 1, 0,	// 49
		 1.4, 3.0, -1.6, 1, 1,	// 50
		 1.4, 3.0, -1.2, 0, 1,	// 51

		//kanan
		 1.8, 0.0, -1.2, 0, 0,	// 52
		 1.8, 0.0, -1.6, 1, 0,	// 53
		 1.8, 3.0, -1.6, 1, 1,	// 54
		 1.8, 3.0, -1.2, 0, 1,	// 55

		//kaki kiri depan
		//depan
		 -1.4, 0.0, 1.6, 0, 0,	// 56
		 - 1.8, 0.0, 1.6, 1, 0,// 57
		 - 1.8, 3.0, 1.6, 1, 1,// 58
		 - 1.4, 3.0, 1.6, 0, 1,// 59

		//belakang
		 - 1.4, 0.0, 1.2, 0, 0,// 60
		 - 1.8, 0.0, 1.2, 1, 0,// 61
		 - 1.8, 3.0, 1.2, 1, 1,// 62
		 - 1.4, 3.0, 1.2, 0, 1,// 63

		//kiri
		 - 1.8, 0.0, 1.6, 0, 0,// 60
		 - 1.8, 0.0, 1.2, 1, 0,// 61
		 - 1.8, 3.0, 1.2, 1, 1,// 62
		 - 1.8, 3.0, 1.6, 0, 0,// 63

		//kanan
		 - 1.4, 0.0, 1.6, 0, 0,// 64
		 - 1.4, 0.0, 1.2, 1, 0,// 65
		 - 1.4, 3.0, 1.2, 1, 1,// 66
		 - 1.4, 3.0, 1.6, 0, 1,// 67

		//kaki kiri belakang
		//depan
		 -1.8, 0.0, -1.2, 0, 0,// 68
		 - 1.4, 0.0, -1.2, 1, 0,// 69
		 - 1.4, 3.0, -1.2, 1, 1,// 70
		 - 1.8, 3.0, -1.2, 0, 1,// 71

		//belakang
		 - 1.8, 0.0, -1.6, 0, 0,// 72
		 - 1.4, 0.0, -1.6, 1, 0,// 73
		 - 1.4, 3.0, -1.6, 1, 1,// 74
		 - 1.8, 3.0, -1.6, 0, 1,// 75

		//kiri
		 - 1.4, 0.0, -1.6, 0, 0,// 76
		 - 1.4, 0.0, -1.2, 1, 0,// 77
		 - 1.4, 3.0, -1.2, 1, 1,// 78
		 - 1.4, 3.0, -1.6, 0, 1,// 79

		//kanan
		 - 1.8, 0.0, -1.6, 0, 0,// 80
		 - 1.8, 0.0, -1.2, 1, 0,// 81
		 - 1.8, 3.0, -1.2, 1, 1,// 82
		 - 1.8, 3.0, -1.6, 0, 1,// 83

		//sandaran kursi
		//depan
		 -1.8, 3.4, -1.2, 0, 0,	 // 84
		 1.8, 3.4, -1.2, 1, 0, 	 // 85
		 1.8, 8.4, -1.2, 1, 1,	 // 86
		 -1.8, 8.4, -1.2, 0, 1, // 87

		//belakang
		 -1.8, 3.4, -1.6, 0, 0,	// 88
		 1.8, 3.4, -1.6, 1, 0,		// 89
		 1.8, 8.4, -1.6, 1, 1,		// 90
		 -1.8, 8.4, -1.6, 0, 1,	// 91

		//kiri
		 - 1.8, 3.4, -1.6, 0, 0,	// 92
		 - 1.8, 8.4, -1.6, 1, 0,	// 93
		 - 1.8, 8.4, -1.2, 1, 1,	// 94
		 - 1.8, 3.4, -1.2, 0, 1,	// 95

		//kanan
		 1.8, 3.4, -1.6, 0, 0,		// 96
		 1.8, 8.4, -1.6, 1, 0,		// 97
		 1.8, 8.4, -1.2, 1, 1,		// 98
		 1.8, 3.4, -1.2, 0, 1,		// 99

		//atas
		 - 1.8, 8.4, -1.6, 0, 0,	// 100
		 - 1.8, 8.4, -1.2, 1, 0,	// 101
		 1.8, 8.4, -1.2, 1, 1,		// 102
		 1.8, 8.4, -1.6, 0, 1,		// 103
		
	};

	unsigned int indices[] = {
		//dudukan
		0,  1,  2,  0,  2,  3,   // depan
		4,  5,  6,  4,  6,  7,   // belakang
		8,  9,  10, 8,  10, 11,  // kiri
		12, 13, 14, 12, 14, 15,  // kanan
		16, 17, 18, 16, 18, 19,  // atas
		20, 21, 22, 20, 22, 23,  // bawah
		//kaki kanan depan
		24, 25, 26, 24, 26, 27,  // belakang
		28, 29, 30, 28, 30, 31,  // kiri
		32, 33, 34, 32, 34, 35,  // kanan
		36, 37, 38, 36, 38, 39,  // depan
		// kaki kanan belakang
		40, 41, 42, 40, 42, 43,  // belakang
		44, 45, 46, 44, 46, 47,  // kiri
		48, 49, 50, 48, 50, 51,  // kanan
		52, 53, 54, 52, 54, 55,  // depan
		//kaki kiri depan
		56, 57, 58, 56, 58, 59,  // belakang
		60, 61, 62, 60, 62, 63,  // kiri
		64, 65, 66, 64, 66, 67,  // kanan
		68, 69, 70, 68, 70, 71,  // kanan
		//kaki kiri belakang
		72, 73, 74, 72, 74, 75,  // depan
		76, 77, 78, 76, 78, 79,  // belakang
		80, 81, 82, 80, 82, 83,  // kiri
		84, 85, 86, 84, 86, 87,  // kanan
		//sandaran
		88, 89, 90, 88, 90, 91,  // depan
		92, 93, 94, 92, 94, 95,  // belakang
		96, 97, 98, 96, 98, 99,  // kiri
		100, 101, 102, 100, 102, 103,  // kanan

	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define texcoord pointer layout 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Demo::DrawColoredCube()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 0);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glDrawElements(GL_TRIANGLES, 250, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::BuildColoredPlane()
{
	// Load and create a texture 
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height;
	unsigned char* image = SOIL_load_image("marble.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Build geometry
	GLfloat vertices[] = {
		// format position, tex coords
		// bottom
		-50.0, -0.5, -50.0,  0,  0,
		 50.0, -0.5, -50.0, 50,  0,
		 50.0, -0.5,  50.0, 50, 50,
		-50.0, -0.5,  50.0,  0, 50,


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}



void Demo::DrawColoredPlane()
{
	glUseProgram(shaderProgram);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glUniform1i(glGetUniformLocation(this->shaderProgram, "ourTexture"), 1);

	glBindVertexArray(VAO2); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 4.0f;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 4.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 4.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.001f;
	fovy = 45.0f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}

void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}

void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}


int main(int argc, char** argv) {
	RenderEngine& app = Demo();
	app.Start("Camera: Free Camera Implementation", 800, 600, false, false);
}
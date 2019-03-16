glm::mat4 proj = glm::mat4(1.0f), model = glm::mat4(1.0f), view = glm::mat4(1.0f);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

double lastX, lastY;
bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;

void setStartCursorPos(int scrWidth, int scrHeight) {
	lastX = scrWidth / 2.0, lastY = scrHeight / 2.0;
}

void setProj(float fov, int scrWidth, int scrHeight) {
	proj = glm::perspective(fov, (float)scrWidth / (float)scrHeight, 0.1f, 1000.0f);
}
namespace inputType {
	Uint8 storedKeyState[SDL_NUM_SCANCODES];
	Uint8 currentKeyState[SDL_NUM_SCANCODES];

	Sint16 storedJoy1YState;
	Sint16 storedJoy1XState;

	SDL_Event event;
}

bool isHeld(int key) {
	Uint8 currentState = inputType::currentKeyState[key];
	Uint8 storedState = inputType::storedKeyState[key];
	if (currentState && storedState) {
		return true;
	}
	return false;
}

void mouse_callback(double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos, lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX, yoffset = lastY - ypos;
	lastX = xpos, lastY = ypos;

	float sensitivity = 0.08;
	xoffset *= sensitivity, yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

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

void eventPoll(bool &runningBool) {
	using namespace inputType;
	while (SDL_PollEvent(&event) != 0) {
		//Caso termine o programa
		if (inputType::event.type == SDL_QUIT) {
			runningBool = false;
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				runningBool = false;
				break;
			default:
				break;
			}
		}
		else if (event.type == SDL_MOUSEMOTION) {
			static int xpos = lastX; // lastX = 400 to center the cursor in the screen
			static int ypos = lastY; // lastY = 300 to center the cursor in the screen
			xpos += event.motion.xrel;
			ypos += event.motion.yrel;

			mouse_callback(xpos, ypos);
		}
	}
}

void inputHandle(float delta) {
	float velocity;
	if(isHeld(SDL_SCANCODE_LSHIFT))
		velocity = 10.5f * delta;
	else
		velocity = 6.5f * delta;

	if (isHeld(SDL_SCANCODE_W))
		cameraPos += velocity * cameraFront;
	if (isHeld(SDL_SCANCODE_S))
		cameraPos -= velocity * cameraFront;

	cameraPos.y = 0.0f;

	if (isHeld(SDL_SCANCODE_A))
		cameraPos -= velocity * glm::normalize(glm::cross(cameraFront, cameraUp));
	if (isHeld(SDL_SCANCODE_D))
		cameraPos += velocity * glm::normalize(glm::cross(cameraFront, cameraUp));

	if (isHeld(SDL_SCANCODE_O))
		SDL_SetRelativeMouseMode(SDL_FALSE);
	if (isHeld(SDL_SCANCODE_P))
		SDL_SetRelativeMouseMode(SDL_TRUE);
}


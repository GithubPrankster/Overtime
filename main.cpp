#include <iostream>
#include <vector>
#include <string>  
#include <random>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad\glad.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>

#include "headers/meshHandler.h"
#include "headers/quadHandler.h"
#include "headers/textureStuff.h"
#include "headers/shaderLoader.h"
#include "headers/camera.h"
#include "headers/physics.h"
#include "headers/framebuffer.h"
#include "headers/input.h"

const int WIDTH = 1280, HEIGHT = 720;
//const int WIDTH = 1920, HEIGHT = 1080;

SDL_GLContext maincontext;
SDL_Window *window;

struct light {
	glm::vec3 model;
	glm::vec3 color;
};

float deltaTime = 0.0f, lastFrame = 0.0f;
float exposure = 0.1f;

int main(int argc, char* argv[]) {
	std::cout << "Woah, gold." << std::endl;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "nothing was initialized lul" << SDL_GetError() << std::endl;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}

	//Do opengl 4.6 initializing
	SDL_GL_LoadLibrary(nullptr);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	//Plbs gib depth buf
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("Overtime", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == NULL) {
		std::cout << "window could not initialize:" << SDL_GetError() << std::endl;
	}
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	int texWidth, texHeight, texComps;
	unsigned char* pixels = stbi_load("textures/overtimelogo.png", &texWidth, &texHeight, &texComps, 4);
	SDL_Surface *icon = SDL_CreateRGBSurfaceFrom((void*)pixels, texWidth, texHeight, 32, 4 * texWidth, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);
	stbi_image_free(pixels);

	maincontext = SDL_GL_CreateContext(window);
	if (gladLoadGLLoader(SDL_GL_GetProcAddress) == NULL) {
		std::cout << "you done goofed." << std::endl;
	}

	glViewport(0, 0, WIDTH, HEIGHT);
	setStartCursorPos(WIDTH, HEIGHT);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	setProj(45.0f, WIDTH, HEIGHT);

	//TODO: Turn this into a separate header file.

	texture floorTex("textures/floorvga.png", GL_RGBA, GL_NEAREST), shotgunTex("models/sawedoff.png", GL_RGBA, GL_NEAREST), shotgunSpec("models/sawmetal.png", GL_RGBA, GL_NEAREST);
	texture pillarTex("textures/pillar.png", GL_RGBA, GL_NEAREST), woodenTex("textures/woodplank.png", GL_RGBA, GL_NEAREST), cityTex("textures/building.png", GL_RGBA, GL_NEAREST),
		citySpec("textures/buildingSpec.png", GL_RGBA, GL_NEAREST);
	mesh gold("models/gold.obj"), shotgun("models/sawoff.obj"), goldPillar("models/goldpillar.obj"), city("models/city.obj");
	mesh wooden("models/wooden.obj");

	Quad screen;

	Shader diffShader("shaders/diffuse.vs", "shaders/diffuse.fs"), hdrShader("shaders/hdr.vs", "shaders/hdr.fs");

	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_real_distribution<double> distribution(0, 1);

	glm::vec3 colorRand[16];
	glm::vec3 posRand[16];
	for (int z = 0; z < 4; z++) {
		double randVal = distribution(generator);
		double randVal1 = distribution(generator) * 10;

		if (randVal < .33) {
			colorRand[z] = glm::vec3(1.0, 1.0, randVal1);	
		}
		else if (randVal < .66) {
			colorRand[z] = glm::vec3(1.0, randVal1, 1.0);
		}
		else {
			colorRand[z] = glm::vec3(randVal1, 1.0, 1.0);
		}
		std::cout << colorRand[z].x << " " << colorRand[z].y << " " << colorRand[z].z << std::endl;
	}

	posRand[0] = glm::vec3(1.0, 4.0, 1.0);
	posRand[1] = glm::vec3(-5.0, 4.0, 2.0);

	posRand[2] = glm::vec3(5.0, 4.0, 2.0);
	posRand[3] = glm::vec3(-3.0, 4.0, 4.0);

	glEnable(GL_DEPTH_TEST);

	framebuffer hdrBuffer(WIDTH, HEIGHT);
	hdrBuffer.hdrAttach();

	//TODO: Add bloom buffer to be used.

	unevenPhysics defaultPhysics;

	bool isrunning = true;
	while (isrunning) {
		float currentFrame = SDL_GetTicks() / 1000.0f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		std::cout << deltaTime << " " << exposure << std::endl;

		using namespace inputType;
		eventPoll(isrunning);
		inputHandle(deltaTime);

		int numKeys;
		const Uint8* buffer = SDL_GetKeyboardState(&numKeys);
		memcpy(storedKeyState, currentKeyState, numKeys);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		hdrBuffer.bindFramebuffer(true);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		diffShader.use();
		diffShader.setMat4("model", glm::mat4(1.0f));
		diffShader.setMat4("view", view);
		diffShader.setMat4("proj", proj);
		diffShader.setVec3("cool", glm::vec3(1.0f));

		diffShader.setInt("numLights", 4);
		for (int x = 0; x < 3; x++) {
			diffShader.setVec3("lightPos[" + std::to_string(x) +"]", posRand[x]);
			diffShader.setVec3("lightColor[" + std::to_string(x) + "]", colorRand[x]);
		}
		diffShader.setVec3("lightPos[3]", cameraPos);
		diffShader.setVec3("lightColor[3]", colorRand[3]);

		diffShader.setFloat("strength", 0.6f);

		diffShader.setVec3("viewPos", cameraPos);
		diffShader.setFloat("specularStrength", 0.5f);

		textureBind(floorTex.id, 0);
		glEnable(GL_CULL_FACE);
		gold.render();

		textureBind(pillarTex.id, 0);
		goldPillar.render();

		textureBind(woodenTex.id, 0);
		wooden.render();

		textureBind(cityTex.id, citySpec.id);
		city.render();

		double translationSpeed;
		if (isHeld(SDL_SCANCODE_LSHIFT))
			translationSpeed = 6.259;
		else if (isHeld(SDL_SCANCODE_W) || isHeld(SDL_SCANCODE_S))
			translationSpeed = 4.259;
		else
			translationSpeed = 1.259;
		double translation = sin((SDL_GetTicks() / 1000.0f) * translationSpeed) * 0.1;
		glm::vec3 sinPos = glm::vec3(cameraPos.x, cameraPos.y + translation, cameraPos.z);

		shotgun.mat = glm::translate(sinPos);
		shotgun.mat = glm::rotate(shotgun.mat, glm::radians(-yaw), glm::vec3(0, 1, 0));
		diffShader.setMat4("model", shotgun.mat);
		textureBind(shotgunTex.id, shotgunSpec.id);
		shotgun.render();

		hdrBuffer.unBindFramebuffer(WIDTH, HEIGHT);

		if (isHeld(SDL_SCANCODE_1))
			exposure += 0.1 * deltaTime;
		else if(isHeld(SDL_SCANCODE_2))
			exposure -= 0.1 * deltaTime;

		hdrShader.use();
		textureBind(hdrBuffer.hdrAttachment);
		hdrShader.setFloat("exposure", exposure);
		screen.render();

		SDL_GL_SwapWindow(window);
		memcpy(currentKeyState, buffer, numKeys);
	}

	defaultPhysics.terminate();

	citySpec.terminate();
	cityTex.terminate();
	shotgunSpec.terminate();
	shotgunTex.terminate();
	woodenTex.terminate();
	pillarTex.terminate();
	floorTex.terminate();
	
	city.terminator();
	wooden.terminator();
	shotgun.terminator();
	goldPillar.terminator();
	gold.terminator();
	screen.terminator();

	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(window);

	Mix_Quit();
	SDL_Quit();

	return EXIT_SUCCESS;
}


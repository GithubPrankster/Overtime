#ifndef FRAMEHANDLER_H
#define FRAMEHANDLER_H

#include <iostream>
#include <glad\glad.h>

class framebuffer{
public:
	unsigned int fbo, renderbuffer;
	unsigned int hdrAttachment, depthAttachment;
	int width, height;
	framebuffer(int pWidth, int pHeight) {
		glGenFramebuffers(1, &fbo);
		width = pWidth;
		height = pHeight;
	}

	//Attach texture to framebuffer for ADVANCED color
	void hdrAttach() {
		glGenTextures(1, &hdrAttachment);
		glBindTexture(GL_TEXTURE_2D, hdrAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		initiateRenderbuffer();
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void depthAttach() {
		glGenTextures(1, &depthAttachment);
		glBindTexture(GL_TEXTURE_2D, depthAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//Renderbuffer obj for depth/stencil passes
	void initiateRenderbuffer() {
		//Generate rb
		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		// Attach the stuff
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrAttachment, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
		//Get the fbo out.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	//Check for the framebuffer's completion.
	void framebufferCheck() {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Eh, you should have told the framebuffer to complete!!" << std::endl;
		}	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//Attach framebuffer and clear either color or depth.
	void bindFramebuffer(bool colorDepth) {
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if (colorDepth) {
			glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
		}
		else {
			glClear(GL_DEPTH_BUFFER_BIT);
		}
	}

	void unBindFramebuffer(int scrWidth, int scrHeight) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, scrWidth, scrHeight);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	//EXTERMINATEEE
	void terminate() {
		glDeleteFramebuffers(1, &fbo);
		glDeleteRenderbuffers(1, &renderbuffer);
	}
};

#endif
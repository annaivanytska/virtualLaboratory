

// GLuint FBO;
// GLuint RBO;
// GLuint texture_id;

// glGenFramebuffers(1, &FBO);
// glBindFramebuffer(GL_FRAMEBUFFER, FBO);

// glGenTextures(1, &texture_id);
// glBindTexture(GL_TEXTURE_2D, texture_id);
// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

// glGenRenderbuffers(1, &RBO);
// glBindRenderbuffer(GL_RENDERBUFFER, RBO);
// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
// glFramebufferRenderbuffer(GL_makeFRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

// glBindFramebuffer(GL_FRAMEBUFFER, 0);
// glBindTexture(GL_TEXTURE_2D, 0);
// glBindRenderbuffer(GL_RENDERBUFFER, 0);
// glDeleteFramebuffers(1, &FBO);
// glDeleteTextures(1, &texture_id);
// glDeleteRenderbuffers(1, &RBO);
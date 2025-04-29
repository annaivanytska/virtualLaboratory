#ifndef ImguiRenderer_CLASS_H
#define ImguiRenderer_CLASS_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

class ImguiRenderer
{
public:

	ImguiRenderer(GLFWwindow *window);
    void NewFrame();
    void DemoWindow();
    void Render();
    void Delete();

};
#endif
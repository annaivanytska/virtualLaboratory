#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
#include <cmath>
#include <iostream>

#include "../include/stb/stb_image.h"
#include "../include/glm/glm.hpp"
#include "../include/glm/ext/matrix_transform.hpp"
#include "../include/glm/gtc/type_ptr.hpp" 

#include"shaderClass.h"
#include "shader_m.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"camera.h"
#include "model.h"
#include "framebuffer.h"
#include "imgui_renderer.h"
#include "vertices.h"

// ROBO_ARM
#include "roboarm.h"
#include "command.h"

void drawAxes(float length);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, RoboArm& roboArm, Block& block);
bool InverseKinematics4DoF(float targetX, float targetY, float targetZ, 
                           float link1Length, float link2Length, float link3Length,
                           float& theta1, float& theta2, float& theta3, float& theta4);

void WaitForSeconds(float seconds);

#include "values.h"


int main(){
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "WLSR", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);

	// Generates Shader object using shaders defualt.vert and default.frag
	Shader shaderProgram("../src/shader/def.vert", "../src/shader/def.frag");
	ShaderM shaderProgramM("../src/shader/mod_lod.vs", "../src/shader/mod_lod.fs");
	ShaderM roboShader("../src/shader/mod_lod.vs", "../src/shader/mod_lod.fs");
    ShaderM lightingShader("../src/shader/light.vs", "../src/shader/light.fs");

    Block block("../src/model/cube.obj", glm::vec3(0.0f, -4.0f, 0.5f), glm::vec3(0.4f, 0.4f, 0.4f));
    Gripper gripper(
        "../src/model/2-1.stl",
        "../src/model/2-2.stl",
        "../src/model/2-3.stl",
        glm::vec3(0.0f, 0.0f, 0.0f),   // Offset bazy względem ostatniego segmentu
        glm::vec3(0.0f, 0.0f, 0.0f),  // Początkowa pozycja lewego zacisku
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f)   // Początkowa pozycja prawego zacisku
    );

    RoboArm robotArm(gripper, block);

    RoboArm kawasaki(gripper, block);

    // Dodaj segmenty i przeguby ramienia
    robotArm.AddSegment(
        Link("../src/model/00.stl", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        Joint("../src/model/1-0.stl", glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), true)
    );

    robotArm.AddSegment(
        Link("../src/model/1-2.stl", glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        Joint("../src/model/1-1.stl", glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), false)
    );

    robotArm.AddSegment(
        Link("../src/model/1-4b.stl", glm::vec3(0.0f, -0.1f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f)),
        Joint("../src/model/1-3.stl", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 3.8f), true)
    );

    robotArm.AddSegment(
        Link("../src/model/1-4a.stl", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
        Joint("../src/model/1-4a.stl", glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), true)
    );

    robotArm.AddSegment(
        Link("../src/model/1-5.stl", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
        Joint("../src/model/1-5.stl", glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 2.7f), true)
    );
    
    robotArm.AddSegment(
        Link("../src/model/1-6.stl", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
        Joint("../src/model/1-6.stl", glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-0.1f, 0.0f, 0.3f), true)
    );


    //                          model(modelPath), theta(theta), d(d), a(a), alpha(alpha)    
    kawasaki.AddElement(Element("../src/kawasaki/1-0.stl", glm::radians(0.0f), 0.0f, 0.0f, glm::radians(0.0f)));
    kawasaki.AddElement(Element("../src/kawasaki/1.stl", glm::radians(0.0f), 2.15f, 0.52f, glm::radians(0.0f)));
    kawasaki.AddElement(Element("../src/kawasaki/2.stl", glm::radians(0.0f), 0.0f, 0.0f, glm::radians(0.0f)));
    kawasaki.AddElement(Element("../src/kawasaki/3.stl", glm::radians(0.0f), 3.77f, 0.0f, glm::radians(0.0f)));
    kawasaki.AddElement(Element("../src/kawasaki/4a.stl", glm::radians(0.0f), 0.0f, 0.0f, glm::radians(0.0f)));
    kawasaki.AddElement(Element("../src/kawasaki/4b.stl", glm::radians(0.0f), 0.0f, 0.0f, glm::radians(0.0f)));
    kawasaki.AddElement(Element("../src/kawasaki/5.stl", glm::radians(0.0f), 0.0f, 0.0f, glm::radians(0.0f)));
    kawasaki.AddElement(Element("../src/kawasaki/6.stl", glm::radians(0.0f), 0.0f, 0.0f, glm::radians(0.0f)));

	// Generates Vertex BOX
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(LineVertices, sizeof(LineVertices));

	// Links VBO to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();

    	// Generates Vertex WorkingSpace
	VAO VAO2;
	VAO2.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO2(BoardVertices, sizeof(BoardVertices));
	// Generates Element Buffer Object and links it to indices

	// Links VBO to VAO
	VAO2.LinkAttrib(VBO2, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO2.LinkAttrib(VBO2, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO2.Unbind();
	VBO2.Unbind();

    VAO VAO3;
	VAO3.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO3(AxisVertices, sizeof(AxisVertices));
	// Generates Element Buffer Object and links it to indices

	// Links VBO to VAO
	VAO3.LinkAttrib(VBO3, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO3.LinkAttrib(VBO3, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO3.Unbind();
	VBO3.Unbind();


    VAO pointVAO;
	pointVAO.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO pointVBO(AxisVertices, sizeof(AxisVertices));
	// Generates Element Buffer Object and links it to indices

	// Links VBO to VAO
	pointVAO.LinkAttrib(pointVBO, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	pointVAO.LinkAttrib(pointVBO, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	pointVAO.Unbind();
	pointVBO.Unbind();

    lightingShader.use();
    shaderProgram.Activate();
	shaderProgramM.use();
    roboShader.use();

    // FBO.h for ender opengl inside of imgui window //////////////////////////////////////////////////////

    ImguiRenderer ImguiRenderer(window);
    
	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, robotArm, block);

		// Specify the color of the background
		glClearColor(0.00f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // | GL_DEPTH_BUFFER_BIT
        ImguiRenderer.NewFrame();

        // lightingShader.use();
        // //lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        // lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        // lightingShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
        // lightingShader.setVec3("lightPos", cameraPos);

        //         // light properties
        // lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        // lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        // lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // // material properties
        // lightingShader.setFloat("material.shininess", 32.0f);


        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);

        // camera/view transformation
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        
        // lightingShader.setMat4("projection", projection);
        // lightingShader.setMat4("view", view);

		// calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::mat4 world = glm::mat4(1.0f);

		shaderProgramM.use();
        shaderProgramM.setVec3("objectColor", 1.0f, 0.5f, 1.0f);
        shaderProgramM.setMat4("projection", projection);
        shaderProgramM.setMat4("view", view);
		shaderProgramM.setMat4("model", model);

        //robotArm.Draw(shaderProgramM);
        // static float lastUpdateTime = 0.0f;
        // float currentTime = static_cast<float>(glfwGetTime());
        const float totalTime = 5.0f; // Całkowity czas w sekundach
        const int steps = 100;        // Liczba kroków (FPS lub precyzja)
        float timeStep = totalTime / steps;

        if (!robotArm.ikSteps.empty() && robotArm.currentStep < robotArm.ikSteps.size()) {
            // Pobierz aktualny krok
            auto& step = robotArm.ikSteps[robotArm.currentStep];

            // Ustaw kąty przegubów
            robotArm.joints[0].angle = glm::degrees(step.theta0);     // Przegub podstawy
            robotArm.links[1].angle = glm::degrees(step.theta1);      // Segment 1
            robotArm.joints[2].angle = glm::degrees(step.theta2);     // Segment 2
            robotArm.joints[3].angle = glm::degrees(step.theta3);     // Segment 2
            robotArm.joints[4].angle = glm::degrees(step.theta4);
            robotArm.joints[5].angle = glm::degrees(step.theta5);     // Segment 2
            

            // Przejdź do następnego kroku
            robotArm.currentStep++;
            WaitForSeconds(timeStep); 

            // Rysuj ramię
            robotArm.Draw(shaderProgramM);
        } else {
            robotArm.Draw(shaderProgramM);
        }

        //kawasaki.DrawForward(shaderProgramM);
        glm::mat4 roboW = glm::mat4(1.0f);
        
		roboShader.use();
        roboShader.setVec3("objectColor", 1.0f, 0.5f, 1.0f);
        roboShader.setMat4("projection", projection);
        roboShader.setMat4("view", view);
		roboShader.setMat4("model", roboW);
        block.Draw(roboShader, glm::mat4(1.0f));

        shaderProgram.Activate();
		shaderProgram.setMat4("projection", projection);
        shaderProgram.setMat4("view", view);
		shaderProgram.setMat4("model", world);
    
		//Drawing Board
        VAO2.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

		// Drawing Axis
        VAO3.Bind();
        glDrawArrays(GL_LINES, 0, 6);

        VAO1.Bind();
        glDrawArrays(GL_LINES, 0, 6);
        model = glm::translate(model, glm::vec3(x, y, z));
		shaderProgram.setMat4("model", model);
        //lightingShader.setMat4("model", world);

        ImGui::Begin("Menu");
        {
            ImGui::Text("Forward kinematic");
            // Synchronizacja wartości suwaki <-> kąty robotArm
            teta0 = robotArm.joints[0].angle;
            teta1 = robotArm.links[1].angle;
            teta2 = robotArm.joints[2].angle;
            teta3 = robotArm.joints[3].angle;
            teta4 = robotArm.joints[4].angle;
            teta5 = robotArm.joints[5].angle;

            // Suwaki do zmiany pozycji końcowej
            if (ImGui::SliderFloat("Angle 0:", &teta0, -90.0f, 90.0f)) {
                robotArm.joints[0].angle = teta0;
            }
            if (ImGui::SliderFloat("Angle 1:", &teta1, -90.0f, 90.0f)) {
                robotArm.links[1].angle = teta1;
            }
            if (ImGui::SliderFloat("Angle 2:", &teta2, -90.0f, 90.0f)) {
                robotArm.joints[2].angle = teta2;
            }
            if (ImGui::SliderFloat("Angle 3:", &teta3, -90.0f, 90.0f)) {
                robotArm.joints[3].angle = teta3;
            }
            if (ImGui::SliderFloat("Angle 4:", &teta4, -90.0f, 90.0f)) {
                robotArm.joints[4].angle = teta4;
            }
            if (ImGui::SliderFloat("Angle 5:", &teta5, -90.0f, 90.0f)) {
                robotArm.joints[5].angle = teta5;
            }

            ImGui::Separator();

            ImGui::Text(" ");
            ImGui::Text("Program Loading: ");

            if (ImGui::Button("Reset Collision")) {
                robotArm.ResetCollisionState();
                std::cout << "Collision state reset. Robot movement enabled." << std::endl;
            }
            
            ImGui::Text("Robot Commands");
            static char filePath[128] = "../src/commands.txt"; // Domyślna ścieżka
            if (ImGui::InputText("File Path", filePath, sizeof(filePath))) {}

            if (ImGui::Button("Load Commands")) {
            commandQueue.clear();
            if (LoadCommandsFromFile(filePath, commandQueue)) {
                std::cout << "Commands loaded successfully!" << std::endl;
                } else {
                std::cout << "Commands NOT loaded !" << std::endl;
                }
            }


            if (ImGui::Button("Start Execution")) {
                flag = true;
            }
            if (flag) {
                ExecuteCommands(robotArm, commandQueue, flag, block);
            }

        }
        ImGui::End();

        // ImGui::Begin("Inverse Kinematics");

        // static glm::vec3 targetPos = glm::vec3(3.0f, 3.0f, 3.0f); // Pozycja docelowa
        // static  glm::vec targetAngle = glm::vec3(0.0f, 0.0f, 0.0f); // Kąt orientacji chwytaka

        // // Suwaki do zmiany pozycji końcowej
        // ImGui::SliderFloat("Target X", &targetPos.x, -7.0f, 7.0f);
        // ImGui::SliderFloat("Target Y", &targetPos.y, -7.0f, 7.0f);
        // ImGui::SliderFloat("Target Z", &targetPos.z, -7.0f, 7.0f);

        // // Suwak do zmiany kąta chwytaka
        // //ImGui::SliderFloat("Gripper Angle", &targetAngle, -90.0f, 90.0f);

        // //robotArm.SolveIK(targetPos, targetAngle);

        // //Aktualizacja pozycji ramienia
        // if (ImGui::Button("Solve IK")) {
        //     robotArm.SolveIK(targetPos, targetAngle);
        // }

        // ImGui::End();

        ImguiRenderer.Render();

    	glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

    ImguiRenderer.Delete();

	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
    VAO2.Delete();
    VBO2.Delete();
    VAO3.Delete();
    VBO3.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;

}

void drawAxes(float length) {
    glBegin(GL_LINES);

    // Oś X - czerwona
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f); // Początek osi X
    glVertex3f(length, 0.0f, 0.0f); // Koniec osi X

    // Oś Y - zielona
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f); // Początek osi Y
    glVertex3f(0.0f, length, 0.0f); // Koniec osi Y

    // Oś Z - niebieska
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f); // Początek osi Z
    glVertex3f(0.0f, 0.0f, length); // Koniec osi Z

    glEnd();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, RoboArm& robotArm, Block& block)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;


        // Ruch ramienia - przykładowy dla przegubu 1
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            robotArm.RotateJoint(0, 0.5f);
            teta0 = robotArm.joints[0].angle;
    }

        // Ruch ramienia - przykładowy dla przegubu 1
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            robotArm.links[1].Rotate(0.5f);
            teta1 = robotArm.links[1].angle;
    }

        // Ruch ramienia - przykładowy dla przegubu 1
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            robotArm.RotateJoint(2, 0.5f);
            teta2 = robotArm.joints[2].angle;
    }

        // Ruch ramienia - przykładowy dla przegubu 1
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            robotArm.RotateJoint(3, 0.5f);
            teta3 = robotArm.joints[3].angle;
    }
            // Ruch ramienia - przykładowy dla przegubu 1
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            robotArm.RotateJoint(4, 0.5f);
            teta4 = robotArm.joints[4].angle;
    }

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            robotArm.RotateJoint(5, 0.5f);
            teta5 = robotArm.joints[5].angle;
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        robotArm.gripper.CloseGrip(0.01f, &block);
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        robotArm.gripper.OpenGrip(0.01f);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // Sprawdź, czy lewy przycisk myszy jest wciśnięty
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        isRotating = true;  // Włącz obracanie
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
        isRotating = false; // Wyłącz obracanie
        firstMouse = true;  // Reset, aby uniknąć skoku kamery
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}


void WaitForSeconds(float seconds) {
    float startTime = static_cast<float>(glfwGetTime());
    while (static_cast<float>(glfwGetTime()) - startTime < seconds) {
        // Pusta pętla
    }
}



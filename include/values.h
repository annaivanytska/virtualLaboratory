
#include "../include/glm/glm.hpp"

const unsigned int width = 800;
const unsigned int height = 600;

float teta0 = 0.0f;
float teta1 = 0.0;
float teta2 = 0.0;
float teta3 = 0.0;
float teta4 = 0.0;
float teta5 = 0.0;

float x = 0.0;
float y = 0.0;
float z = 0.0;

float positionX = 0.0f;
float positionY = 0.0f;
float positionZ = 0.0f;

 // camera
glm::vec3 cameraPos = glm::vec3(-10.5f, -10.5f, 10.5f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - cameraPos);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);

bool isRotating = false;

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f; //

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


bool flag = false;
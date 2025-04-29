#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb/stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.h>
#include <shader_m.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <roboArm.h>

using namespace std;


enum CommandType { GOTO, OPEN, CLOSE, SAVE_HOME, MOVE_A, WAIT };

struct Command {
    CommandType type;
    glm::vec3 targetPosition; // Używane tylko dla GOTO
    std::vector<float> targetAngles; // Używane tylko dla MOVE_A
    bool moveToHome = false; // Flaga dla moveA homePos
    float waitTime = 0.0f;
};

std::vector<Command> commandQueue; // Kolejka poleceń

bool LoadCommandsFromFile(const std::string& filePath, std::vector<Command>& commands) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return false;
    } else {
        std::cout << "File is open: " << filePath << std::endl;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string commandType;
        stream >> commandType;

        if (commandType == "---") {
            std::cout << "End of program detected in command file." << std::endl;
            break;
        }

        if (commandType == "Goto") {
            float x, y, z;
            stream >> x >> y >> z;
            commands.push_back({GOTO, glm::vec3(x, y, z)});
            std::cout << "Loaded Goto command: (" << x << ", " << y << ", " << z << ")" << std::endl;
        } else if (commandType == "Open") {
            commands.push_back({OPEN, glm::vec3(0.0f)});
            std::cout << "Loaded Open command" << std::endl;
        } else if (commandType == "Close") {
            commands.push_back({CLOSE, glm::vec3(0.0f)});
            std::cout << "Loaded Close command" << std::endl;
        } else if (commandType == "SaveHome") {
            commands.push_back({SAVE_HOME, glm::vec3(0.0f)});
            std::cout << "Loaded SaveHome command" << std::endl;
        } else if (commandType == "MoveA") {
    std::string target;
    stream >> target;
    if (target == "homePos") {
        commands.push_back({MOVE_A, glm::vec3(0.0f), {}, true}); // Flaga moveToHome
        std::cout << "Loaded MoveA to homePos command" << std::endl;
    } else {
        // Cofnij wskaźnik strumienia o długość wczytanego ciągu
        stream.clear();
        stream.seekg(0, std::ios::beg);
        stream >> commandType; // Ponownie wczytaj "MoveA"

        // Wczytaj kąty
        std::vector<float> angles;
        float angle;
        while (stream >> angle) {
            angles.push_back(angle);
        }
        if (angles.size() != 6) { // Sprawdź, czy załadowano dokładnie 6 kątów
            std::cerr << "Error: MoveA command requires 6 angles. Loaded " << angles.size() << "." << std::endl;
            return false;
        }
        commands.push_back({MOVE_A, glm::vec3(0.0f), angles});
        std::cout << "Loaded MoveA command with angles: ";
        for (auto a : angles) std::cout << a << " ";
        std::cout << std::endl;
    }
}
 else if (commandType == "Wait") {
            float waitTime;
            stream >> waitTime;
            if (waitTime >= 0.0f) {
                commands.push_back({WAIT, glm::vec3(0.0f), {}, false, waitTime});
                std::cout << "Loaded Wait command: " << waitTime << " seconds" << std::endl;
            } else {
                std::cerr << "Error: Invalid wait time in command file." << std::endl;
                return false;
            }
        } else {
            std::cerr << "Error: Unknown command type '" << commandType << "' in file." << std::endl;
            return false;
        }
    }

    file.close();
    return true;
}


glm::vec3 InterpolatePosition(const glm::vec3& start, const glm::vec3& end, float t) {
    return start + t * (end - start); // t powinno być w zakresie [0, 1]
}


void ExecuteCommands(RoboArm& robotArm, std::vector<Command>& commands, bool& start, Block& block) {
    if (start) {
        static size_t currentCommand = 0;
        static bool moving = false;
        static bool waiting = false;
        static bool gripperAnimating = false;
        static float waitStartTime = 0.0f;

        static float gripperStartValue = 0.0f;
        static float gripperEndValue = 0.0f;
        static float gripperT = 0.0f;

        if (currentCommand >= commands.size()) {
            std::cout << "All commands executed!" << std::endl;
            commands.clear();
            currentCommand = 0;
            start = false;
            return;
        }
        Command& cmd = commands[currentCommand];

        static std::vector<float> startAngles;
        static std::vector<float> endAngles;
        static glm::vec3 startPosition;
        static glm::vec3 endPosition;
        static float t = 0.0f;

        if (cmd.type == SAVE_HOME) {
    std::cout << "Executing SaveHome" << std::endl;
            robotArm.saveHomeAngle();
            currentCommand++;
        } else if (cmd.type == MOVE_A) {
            if (!moving) {
                startAngles = robotArm.GetCurrentJointAngles();
                endAngles = cmd.moveToHome ? robotArm.homeAngles : cmd.targetAngles;
                if (cmd.moveToHome && robotArm.homeAngles.empty()) {
                    std::cerr << "Error: homePos not set. Use SaveHome before MoveA homePos." << std::endl;
                    currentCommand++;
                    return;
                }
                t = 0.0f;
                moving = true;
            }

                t += 0.01f; // Prędkość ruchu
                if (t >= 1.0f) {
                    t = 1.0f;
                    moving = false;
                    std::cout << "MoveA completed." << std::endl;
                }

                std::vector<float> interpolatedAngles(startAngles.size());
                for (size_t i = 0; i < startAngles.size(); i++) {
                    interpolatedAngles[i] = startAngles[i] + t * (endAngles[i] - startAngles[i]);
                }
                robotArm.SetJointAngles(interpolatedAngles);
                //std::cout << "Interpolated angles: ";
                //for (auto a : interpolatedAngles) std::cout << a << " ";
                //std::cout << std::endl;
        } else if (cmd.type == GOTO) {
            if (!moving) {
                startPosition = robotArm.GetCurrentPosition(); // Funkcja do pobrania aktualnej pozycji
                endPosition = cmd.targetPosition;
                t = 0.0f;
                moving = true;
            }

            t += 0.01f; // Prędkość ruchu
            if (t >= 1.0f) {
                t = 1.0f;
                moving = false;
            }

            glm::vec3 interpolatedPos = InterpolatePosition(startPosition, endPosition, t);
            //robotArm.SolveIK(interpolatedPos, glm::vec3(0.0f, 0.0f, 0.0f));

            // Dodaj logikę, aby sprawdzić, czy ruch jest ukończony (np. interpolacja)
            moving = false; // Jeśli ukończone
        //      } else if (cmd.type == OPEN) {
        //     std::cout << "Executing Open command." << std::endl;
        //     robotArm.gripper.OpenGrip(0.51f);
        //     currentCommand++;
        // } else if (cmd.type == CLOSE) {
        //     std::cout << "Executing Close command." << std::endl;
        //     robotArm.gripper.CloseGrip(1.01f, nullptr);
        //     currentCommand++;
        // } else if (cmd.type == WAIT) {
        } else if (cmd.type == OPEN || cmd.type == CLOSE) {
            if (!gripperAnimating) {
                gripperStartValue = robotArm.gripper.GetGripPosition(); // Pobierz aktualną wartość chwytaka
                gripperEndValue = (cmd.type == CLOSE) ? 1.0f : 0.0f;     // Ustal cel: 1.0 (otwarty) lub 0.0 (zamknięty)
                gripperT = 0.0f;
                gripperAnimating = true;
                moving = true;
                //std::cout << ((cmd.type == OPEN) ? "Opening" : "Closing") << " gripper animation started." << std::endl;
            }

            // Interpolacja chwytaka
            gripperT += 0.01f; //+
            if (gripperT >= 1.0f) { //1.0
                gripperT = 1.0f;  //1.0
                gripperAnimating = false;
                moving = false;
                //std::cout << ((cmd.type == OPEN) ? "Gripper opened." : "Gripper closed.") << std::endl;
        // Przejście do następnego kroku następuje dopiero po zakończeniu animacji
        //currentCommand++;
            }

            float interpolatedGripValue = gripperStartValue + gripperT * (gripperEndValue - gripperStartValue);
            robotArm.gripper.SetGripPosition(interpolatedGripValue, &block); // Ustaw interpolowaną wartość chwytaka
        } else if (cmd.type == WAIT){
            if (!waiting) {
                waitStartTime = static_cast<float>(glfwGetTime());
                waiting = true;
                moving = true;
            }

            float currentTime = static_cast<float>(glfwGetTime());
            if (currentTime - waitStartTime >= cmd.waitTime) {
                waiting = false;
                moving = false;
//                currentCommand++;
            }
            }

        if (!moving) {
            currentCommand++;
            std::cout << "Moving to next command: " << currentCommand << std::endl;
        }

        }
    
}


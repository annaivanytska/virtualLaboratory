// example peace of code

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

        // Nowe zmienne dla sterowania ruchem
        static float acceleration = 0.002f;  // Przyspieszenie
        static float decelerationFactor = 0.8f; // Hamowanie
        static float maxSpeed = 0.03f;  // Maksymalna prędkość
        static float minSpeed = 0.01f;  // Minimalna prędkość
        static float currentSpeed = 0.01f;  // Aktualna prędkość ruchu
        static float timeSinceLastCheck = 0.0f;  // Czas od ostatniego sprawdzenia prędkości
        static float checkInterval = 4.0f;  // Co ile sekund sprawdzamy

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
                currentSpeed = minSpeed;  // Zaczynamy z minimalną prędkością
                std::cout << "MoveA initialized. Start angles: ";
                for (auto a : startAngles) std::cout << a << " ";
                std::cout << " | Target angles: ";
                for (auto a : endAngles) std::cout << a << " ";
                std::cout << std::endl;
            }

            // Co 4 sekundy sprawdzamy pozycję
            float currentTime = static_cast<float>(glfwGetTime());
            if (currentTime - timeSinceLastCheck >= checkInterval) {
                timeSinceLastCheck = currentTime;

                float distanceRemaining = 0.0f;
                for (size_t i = 0; i < startAngles.size(); ++i) {
                    distanceRemaining += fabs(endAngles[i] - startAngles[i]) * (1.0f - t);
                }

                if (distanceRemaining > 1.0f) {  // Jeśli odległość jest duża, zwiększamy prędkość
                    currentSpeed += acceleration;
                    if (currentSpeed > maxSpeed) currentSpeed = maxSpeed;
                    std::cout << "Increasing speed. Current speed: " << currentSpeed << std::endl;
                } else if (distanceRemaining < 0.5f) {  // Jeśli odległość jest mała, zmniejszamy prędkość
                    currentSpeed *= decelerationFactor;
                    if (currentSpeed < minSpeed) currentSpeed = minSpeed;
                    std::cout << "Slowing down. Current speed: " << currentSpeed << std::endl;
                }
            }

            // Aktualizujemy t z uwzględnieniem prędkości
            t += currentSpeed;
            if (t >= 1.0f) {
                t = 1.0f;
                moving = false;
                std::cout << "MoveA completed." << std::endl;
            }

            // Interpolujemy kąty
            std::vector<float> interpolatedAngles(startAngles.size());
            for (size_t i = 0; i < startAngles.size(); i++) {
                interpolatedAngles[i] = startAngles[i] + t * (endAngles[i] - startAngles[i]);
            }
            robotArm.SetJointAngles(interpolatedAngles);

            if (robotArm.gripper.heldBlock != nullptr) {
                glm::vec3 gripperPosition = robotArm.gripper.GetGlobalPosition();
                robotArm.gripper.heldBlock->SetPosition(gripperPosition);
            }
        } else if (cmd.type == OPEN || cmd.type == CLOSE) {
            if (!gripperAnimating) {
                gripperStartValue = robotArm.gripper.GetGripPosition();
                gripperEndValue = (cmd.type == OPEN) ? 0.5f : 0.0f;
                gripperT = 0.0f;
                gripperAnimating = true;
                std::cout << ((cmd.type == OPEN) ? "Opening" : "Closing") << " gripper animation started." << std::endl;
            }

            gripperT += currentSpeed;
            if (gripperT >= 1.0f) {
                gripperT = 1.0f;
                gripperAnimating = false;
                std::cout << ((cmd.type == OPEN) ? "Gripper opened." : "Gripper closed.") << std::endl;
            }

            float interpolatedGripValue = gripperStartValue + gripperT * (gripperEndValue - gripperStartValue);
            robotArm.gripper.SetGripPosition(interpolatedGripValue, &block);
        } else if (cmd.type == WAIT) {
            if (!waiting) {
                waitStartTime = static_cast<float>(glfwGetTime());
                waiting = true;
                std::cout << "Waiting for " << cmd.waitTime << " seconds." << std::endl;
            }

            float currentTime = static_cast<float>(glfwGetTime());
            if (currentTime - waitStartTime >= cmd.waitTime) {
                waiting = false;
                std::cout << "Wait completed." << std::endl;
            }
        }

        if (!moving && !gripperAnimating && !waiting) {
            currentCommand++;
            std::cout << "Moving to next command: " << currentCommand << std::endl;
        }
    }
}



PREDKOSC DLA KAZDEGO KATA

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
        static std::vector<float> speeds;  // Prędkości dla każdego przegubu
        static std::vector<float> maxSpeeds = {0.03f, 0.03f, 0.03f, 0.03f, 0.03f, 0.03f};  // Maksymalne prędkości dla każdego przegubu
        static std::vector<float> minSpeeds = {0.005f, 0.005f, 0.005f, 0.005f, 0.005f, 0.005f};  // Minimalne prędkości
        static std::vector<float> accelerations = {0.002f, 0.002f, 0.002f, 0.002f, 0.002f, 0.002f};  // Przyspieszenie
        static std::vector<float> t = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};  // Stopień interpolacji dla każdego kąta

        static float timeSinceLastCheck = 0.0f;
        static float checkInterval = 4.0f;

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

                speeds.resize(startAngles.size(), 0.01f);  // Początkowe prędkości
                t.assign(startAngles.size(), 0.0f);  // Zresetuj t dla każdego kąta
                moving = true;

                std::cout << "MoveA initialized." << std::endl;
            }

            float currentTime = static_cast<float>(glfwGetTime());
            if (currentTime - timeSinceLastCheck >= checkInterval) {
                timeSinceLastCheck = currentTime;

                for (size_t i = 0; i < startAngles.size(); ++i) {
                    float distanceRemaining = fabs(endAngles[i] - startAngles[i]) * (1.0f - t[i]);

                    if (distanceRemaining > 1.0f) {
                        speeds[i] += accelerations[i];  // Przyspieszaj, jeśli daleko
                        if (speeds[i] > maxSpeeds[i]) speeds[i] = maxSpeeds[i];
                    } else if (distanceRemaining < 0.5f) {
                        speeds[i] *= 0.8f;  // Hamuj, jeśli blisko
                        if (speeds[i] < minSpeeds[i]) speeds[i] = minSpeeds[i];
                    }
                }
            }

            bool allCompleted = true;
            for (size_t i = 0; i < startAngles.size(); ++i) {
                t[i] += speeds[i];
                if (t[i] >= 1.0f) t[i] = 1.0f;

                if (t[i] < 1.0f) {
                    allCompleted = false;  // Jeśli któryś przegub nie zakończył ruchu, nie kończ ruchu całego ramienia
                }
            }

            std::vector<float> interpolatedAngles(startAngles.size());
            for (size_t i = 0; i < startAngles.size(); i++) {
                interpolatedAngles[i] = startAngles[i] + t[i] * (endAngles[i] - startAngles[i]);
            }
            robotArm.SetJointAngles(interpolatedAngles);

            if (robotArm.gripper.heldBlock != nullptr) {
                glm::vec3 gripperPosition = robotArm.gripper.GetGlobalPosition();
                robotArm.gripper.heldBlock->SetPosition(gripperPosition);
            }

            if (allCompleted) {
                moving = false;
                std::cout << "MoveA completed." << std::endl;
            }
        } else if (cmd.type == OPEN || cmd.type == CLOSE) {
            if (!gripperAnimating) {
                gripperStartValue = robotArm.gripper.GetGripPosition();
                gripperEndValue = (cmd.type == OPEN) ? 0.5f : 0.0f;
                gripperT = 0.0f;
                gripperAnimating = true;
                std::cout << ((cmd.type == OPEN) ? "Opening" : "Closing") << " gripper animation started." << std::endl;
            }

            gripperT += 0.01f;
            if (gripperT >= 1.0f) {
                gripperT = 1.0f;
                gripperAnimating = false;
                std::cout << ((cmd.type == OPEN) ? "Gripper opened." : "Gripper closed.") << std::endl;
            }

            float interpolatedGripValue = gripperStartValue + gripperT * (gripperEndValue - gripperStartValue);
            robotArm.gripper.SetGripPosition(interpolatedGripValue, &block);
        } else if (cmd.type == WAIT) {
            if (!waiting) {
                waitStartTime = static_cast<float>(glfwGetTime());
                waiting = true;
                std::cout << "Waiting for " << cmd.waitTime << " seconds." << std::endl;
            }

            float currentTime = static_cast<float>(glfwGetTime());
            if (currentTime - waitStartTime >= cmd.waitTime) {
                waiting = false;
                std::cout << "Wait completed." << std::endl;
            }
        }

        if (!moving && !gripperAnimating && !waiting) {
            currentCommand++;
            std::cout << "Moving to next command: " << currentCommand << std::endl;
        }
    }
}


CZAAS TRWANIA RUCHU 6 SEKUND

void InterpolateJointAnglesWithAcceleration(
    const std::vector<float>& startAngles,
    const std::vector<float>& endAngles,
    std::vector<float>& currentAngles,
    float elapsedTime,  // Czas od początku ruchu
    float totalTime = 6.0f)  // Całkowity czas ruchu (6 sekund)
{
    float t = elapsedTime / totalTime;  // Normalizowany czas w zakresie [0, 1]
    if (t > 1.0f) t = 1.0f;  // Zabezpieczenie przed przekroczeniem czasu

    float phaseTime = totalTime / 3.0f;  // Czas trwania każdej fazy: 2 sekundy

    for (size_t i = 0; i < startAngles.size(); ++i) {
        float angleDiff = endAngles[i] - startAngles[i];

        if (elapsedTime <= phaseTime) {
            // Faza przyspieszania: ruch kwadratowy (przyspieszenie rośnie)
            float accelT = elapsedTime / phaseTime;  // Normalizowany czas w fazie przyspieszania
            currentAngles[i] = startAngles[i] + angleDiff * 0.5f * accelT * accelT;
        } else if (elapsedTime <= 2 * phaseTime) {
            // Faza stałej prędkości
            float steadyT = (elapsedTime - phaseTime) / phaseTime;  // Normalizowany czas w fazie stałej prędkości
            currentAngles[i] = startAngles[i] + angleDiff * (0.5f * steadyT + 0.25f);
        } else {
            // Faza hamowania: ruch kwadratowy (prędkość maleje)
            float decelT = (elapsedTime - 2 * phaseTime) / phaseTime;  // Normalizowany czas w fazie hamowania
            currentAngles[i] = startAngles[i] + angleDiff * (0.75f + 0.25f * (2.0f * decelT - decelT * decelT));
        }
    }
}

else if (cmd.type == MOVE_A) {
    if (!moving) {
        startAngles = robotArm.GetCurrentJointAngles();
        endAngles = cmd.moveToHome ? robotArm.homeAngles : cmd.targetAngles;
        t = 0.0f;
        moving = true;
        std::cout << "MoveA initialized. Start angles: ";
        for (auto a : startAngles) std::cout << a << " ";
        std::cout << " | Target angles: ";
        for (auto a : endAngles) std::cout << a << " ";
        std::cout << std::endl;
    }

    t += deltaTime;  // `deltaTime` to różnica czasu pomiędzy klatkami
    if (t >= 6.0f) {
        t = 6.0f;  // Upewnij się, że czas nie przekroczy całkowitego czasu ruchu
        moving = false;
        std::cout << "MoveA completed." << std::endl;
    }

    InterpolateJointAnglesWithAcceleration(startAngles, endAngles, interpolatedAngles, t, 6.0f);
    robotArm.SetJointAngles(interpolatedAngles);
}


#include "roboArm.h"

void drawAxis(const double origin[3], const double direction[3], double length, const double color[3]) {
    glColor3dv(color);  // Set axis color
    glBegin(GL_LINES);
        glVertex3dv(origin);                             // Start point (origin)
        double endPoint[3] = {
            origin[0] + direction[0] * length,           // End point
            origin[1] + direction[1] * length,
            origin[2] + direction[2] * length
        };
        glVertex3dv(endPoint);
    glEnd();
}


void drawLocalAxes(const glm::mat4& transform, double length = 2) {
    // Extract origin
    double origin[3] = {transform[0][3], transform[1][3], transform[2][3]};

    // Extract axes directions
    double xAxis[3] = {transform[0][0], transform[1][0], transform[2][0]};
    double yAxis[3] = {transform[0][1], transform[1][1], transform[2][1]};
    double zAxis[3] = {transform[0][2], transform[1][2], transform[2][2]};

    // Colors for the axes
    double red[3] = {1.0, 0.0, 0.0};   // X-axis
    double green[3] = {0.0, 1.0, 0.0}; // Y-axis
    double blue[3] = {0.0, 0.0, 1.0};  // Z-axis

    // Draw each axis
    drawAxis(origin, xAxis, length, red);
    drawAxis(origin, yAxis, length, green);
    drawAxis(origin, zAxis, length, blue);
}


RoboArm::RoboArm(const Gripper& gripper, const Block& block) : gripper(gripper), block(block) {}


bool RoboArm::DetectCollision(Block& block, glm::mat4& localTransform) {
        glm::vec3 blockMin = block.GetMinBounds();
        glm::vec3 blockMax = block.GetMaxBounds();

        // Pozycje zacisków
        glm::vec3 robotPos = glm::vec3(localTransform[3]);// + glm::vec3(1.0f, 1.0, 3.0f);;

        // Sprawdzenie, czy lewy zacisk znajduje się w bounding boxie klocka
        bool robotCollision = robotPos.x >= blockMin.x && robotPos.x <= blockMax.x &&
                             robotPos.y >= blockMin.y && robotPos.y <= blockMax.y &&
                             robotPos.z >= blockMin.z && robotPos.z <= blockMax.z;
                             //robotPos.z <= 0.0f;

        // Sprawdzenie, czy gripper jest na poziomie z = 0.0
        bool isAtGroundLevel = (robotPos.z < 0.0f);

        if (robotCollision) {
            //std::cout << "Collision detected!" << std::endl;

            // // Ustawienie blokady rotacji dla przegubów i linków
            // for (auto& joint : joints) {
            //     joint.BlockRotation(); // Blokuje transformację
            // }
            // for (auto& link : links) {
            //     link.BlockRotation(); // Blokuje transformację
            // }

            collisionDetected = true; // Ustaw flagę kolizji
        }

        if (isAtGroundLevel){
            std::cout << "GROUND TOUCHED!" << std::endl;

            // Ustawienie blokady rotacji dla przegubów i linków
            for (auto& joint : joints) {
                joint.BlockRotation(); // Blokuje transformację
            }
            for (auto& link : links) {
                link.BlockRotation(); // Blokuje transformację
            }

            groundCollision = true;
        }

        return robotCollision;
}

void RoboArm::ResetCollisionState() {
 
    joints[0].angle = 0.0f;
    links[1].angle = 0.0f;
    joints[2].angle = 0.0f;
    joints[3].angle = 0.0f;
    joints[4].angle = 0.0f;
    joints[5].angle = 0.0f;

    for (auto& joint : joints) {
        joint.UnblockRotation();
    }
    for (auto& link : links) {
        link.UnblockRotation();
    }
    collisionDetected = false;
    groundCollision = false;
    std::cout << "Return to HOME position!" << std::endl;
}

bool RoboArm::IsCollisionDetected() const {
    return collisionDetected;
}

void RoboArm::SetCollisionDetected(bool state) {
    collisionDetected = state;
}

void RoboArm::AddSegment(const Link& link, const Joint& joint) {
        links.push_back(link);
        joints.push_back(joint);
    }

void RoboArm::Draw(ShaderM& shader) {
    //Jeśli kolizja została wykryta, przerwij rysowanie
        if (groundCollision) {
            std::cout << "Robot stopped due to collision." << std::endl;
            return;
        }
        
        Positions.clear();
        
        glm::mat4 currentTransform = glm::mat4(1.0f); // Macierz globalna
        for (size_t i = 0; i < joints.size(); i++) {

            // Rysowanie przegubu
            joints[i].Draw(shader, currentTransform);

            // Aktualizacja transformacji globalnej o przegub
            currentTransform = currentTransform * joints[i].GetTransform();

            //if(DetectCollision(block, currentTransform)){return;}
            DetectCollision(block, currentTransform);

            // Rysowanie linka
            links[i].Draw(shader, currentTransform);
            
            currentTransform = currentTransform * links[i].GetTransform();
            //if(DetectCollision(block, currentTransform)){return;}
            DetectCollision(block, currentTransform);
        }

        Positions.push_back({currentTransform});
        // Rysowanie chwytaka na końcu ramienia
        gripper.Draw(shader, currentTransform, block);
        robottransform = currentTransform;
        
        DetectCollision(block, currentTransform);

    }

void RoboArm::AddElement(const Element& element) {
        elements.push_back(element);
    }


void RoboArm::DrawForward(ShaderM& shader) {

        glm::mat4 currentTransform = glm::mat4(1.0f); // Macierz globalna
        currentTransform = glm::translate(currentTransform, glm::vec3(-5.0f, -5.0f, 0.0f));

        for (size_t i = 0; i < elements.size(); i++) {
            elements[i].Draw(shader, currentTransform);       // Rysuj element z bieżącą macierzą
            currentTransform *= elements[i].GetTransform();  // Aktualizuj macierz o lokalną transformację
        }

        // Draw local axes for debugging
        drawLocalAxes(currentTransform);

        //elements

    }


void RoboArm::RotateJoint(size_t jointIndex, float angleDelta) 
{
    if (jointIndex < joints.size()) {
        joints[jointIndex].Rotate(angleDelta);
    }
}

void RoboArm::SetJointOffset(size_t jointIndex, const glm::vec3& offset) {
    if (jointIndex < joints.size()) {
        joints[jointIndex].offset = offset;
    }
}

void RoboArm::SetLinkOffset(size_t linkIndex, const glm::vec3& offset) {
    if (linkIndex < links.size()) {
        links[linkIndex].offset = offset;
    }
}

void RoboArm::RotateGripper(float angleDelta) {
    gripper.Rotate(angleDelta);
}

void RoboArm::CloseGripper() {
    gripper.CloseGrip(0.01f, &block);
}

void RoboArm::OpenGripper() {
    gripper.OpenGrip(0.01f);
}

bool RoboArm::CheckCollisionWithBlock(const Block& block) const {
    // Pozycje elementów grippera
    glm::vec3 gripperBasePos = gripper.baseOffset;
    glm::vec3 leftClampPos = gripper.baseOffset + gripper.leftOffset + glm::vec3(gripper.clampDistance, 0.0f, 0.0f);
    glm::vec3 rightClampPos = gripper.baseOffset + gripper.rightOffset - glm::vec3(gripper.clampDistance, 0.0f, 0.0f);

    // Sprawdzenie kolizji z każdą częścią grippera
    return (gripper.IsPointInBoundingBox(gripperBasePos, block) ||
            gripper.IsPointInBoundingBox(leftClampPos, block) ||
            gripper.IsPointInBoundingBox(rightClampPos, block));
}

// void WaitForSeconds(float seconds) {
//     float startTime = static_cast<float>(glfwGetTime());
//     while (static_cast<float>(glfwGetTime()) - startTime < seconds) {
//         // Pusta pętla
//     }
// }

float EaseInOut(float t) {
    return 3 * t * t - 2 * t * t * t; // Sześcienna funkcja Ease-In-Out
}

// bool RoboArm::SolveIK(glm::vec3& targetPos, float targetAngle) {
//     // Długości segmentów ramienia
//     const float L1 = 3.5f; // Długość pierwszego segmentu
    // const float L2 = 3.5f; // Długość drugiego segmentu
    // const float L3 = 0.07f; // Długość trzeciego segmentu (przed gripperem)

    // // Pozycja celu
    // float x = targetPos.x;
    // float y = targetPos.y;
    // float z = targetPos.z;

    // // Obliczanie kąta dla przegubu podstawy (obrót wokół osi Z)
    // float theta0 = atan2(y, x); // x, y

    // // Obliczanie odległości do punktu celu w płaszczyźnie XZ
    // float xy = sqrt(x * x + y * y);

    // if (xy > L1 + L2 + L3){
    //     std::cerr << "Target LENGHT out of reach!" << std::endl;
    //     return false; 
    // }

    // float zPrim = z - L3 * sin(targetAngle); // Odległość w pionie od podstawy
    // float xyPrim = xy - L3 * cos(targetAngle);

    // // Prawo cosinusów dla obliczenia kąta przegubu 2 i 3
    // float d = sqrt(zPrim * zPrim + xyPrim * xyPrim); //(r * r + s * s - L2 * L2 - L3 * L3) / (2 * L2 * L3);
    // float gama = (d * d - L1 * L1 - L2 * L2) / (2 * L1 * L2);

    // // Sprawdź, czy punkt docelowy jest osiągalny
    // if (gama < -1.0f || gama > 1.0f) {
    //     std::cerr << "Target GAMA out of reach!" << std::endl;
    //     return false;
    // }

    // float phi = atan2(zPrim, xyPrim);

    // float e = (d * d + L1 * L1 - L2 * L2) / (2 * d * L1);

    // // Sprawdź, czy punkt docelowy jest osiągalny
    // if (e < -1.0f || e > 1.0f) {
    //     std::cerr << "Target E out of reach!" << std::endl;
    //     return false;
    // }

    // float phi1 = acos(e);

    // float theta1 = phi - phi1; // Kąt przegubu 1
    // float theta2 = acos(gama); // Kąt przegubu 2
    // float theta3 = targetAngle - theta1 - theta2; // Kąt przegubu 3

//     //     // Ograniczenia dla kątów
//     // if (theta1 < glm::radians(0.0f) || theta1 > glm::radians(180.0f)) {
//     //     std::cerr << "Theta1 out of range!" << std::endl;
//     //     return false;
//     // }
//     // if (theta2 < glm::radians(-135.0f) || theta2 > glm::radians(135.0f)) {
//     //     std::cerr << "Theta2 out of range!" << std::endl;
//     //     return false;
//     // }
//     // if (theta3 < glm::radians(-135.0f) || theta3 > glm::radians(135.0f)) {
//     //     std::cerr << "Theta3 out of range!" << std::endl;
//     //     return false;
//     // }

//     // // Aktualizacja rotacji grippera
//     // gripper.angle = glm::degrees(targetAngle);

//         // Odczyt bieżących kątów
//     float currentTheta0 = glm::radians(joints[0].angle);
//     float currentTheta1 = glm::radians(links[1].angle);
//     float currentTheta2 = glm::radians(joints[2].angle);
//     float currentTheta3 = glm::radians(joints[4].angle);

//     // Interpolacja w 10 krokach
//     const int steps = 100;
//     ikSteps.clear();
//     for (int i = 1; i <= steps; ++i) {
//         float t = static_cast<float>(i) / steps;
//         float easedT = EaseInOut(t);             // Modyfikacja za pomocą Ease-In-Out
//         ikSteps.push_back({
//             glm::mix(currentTheta0, theta0, t),
//             glm::mix(currentTheta1, theta1, t),
//             glm::mix(currentTheta2, theta2, t),
//             glm::mix(currentTheta3, theta3, t)
//         });
//         std::cout << "policzyłem" << std::endl;
//     }
//     currentStep = 0;

//     return true;
// }


bool RoboArm::SolveIK(glm::vec3& targetPos, glm::vec3& targetAngle) {
    
    // Step 1: Construct T6 (end-effector's transformation matrix)
    glm::mat4 T6 = glm::mat4(1.0f);
    T6 = glm::translate(glm::mat4(1.0f), targetPos); // Apply translation (target position)
    T6 = glm::rotate(T6, glm::radians(targetAngle.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
    T6 = glm::rotate(T6, glm::radians(targetAngle.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
    T6 = glm::rotate(T6, glm::radians(targetAngle.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll

    // Step 2: Define T5 (translation matrix for last segment d = (0, 0, 3))
    glm::vec3 lastPart = glm::vec3(0.0f, 0.0f, 0.1f); // d5
    glm::mat4 T5 = glm::translate(glm::mat4(1.0f), lastPart);

    // Find the product of T1 * T2 * T3 * T4 * T5
    glm::mat4 T1_to_T5 = glm::inverse(T5) * T6;

    // Extract x and y from the T1_to_T5 matrix
    float x = T1_to_T5[0][3]; // Translation component along x
    float y = T1_to_T5[1][3]; // Translation component along y

    // Calculate theta1
    float teta1 = atan2(y, x);

    // Calculate x' and y' using the equations
    float x_prime = x * cos(teta1) - y * sin(teta1);
    float y_prime = y * cos(teta1) + x * sin(teta1);

    float a1 = 1.0;
    float a2 = 1.0;
    float a3 = 1.0;
    float a4 = 1.0;
    float floor = 0.0;

    if (x_prime == 0){
        floor = x_prime;
    } else if (y_prime == 0)
    {
        floor = y_prime;
    } else {
        std::cerr << "NOT CORRECT" << std::endl;
        return false; 
    }

    float z = T1_to_T5[2][3]; // Translation component along y
    float floor_prime = floor - a2;
    float z_prime = z - a1;

    float phi = atan2(z_prime, floor_prime);

    // Obliczanie odległości do punktu celu w płaszczyźnie XZ
    float b = sqrt(z_prime * z_prime + floor_prime * floor_prime);

    float teta3 = (b * b - a3 * a3 - a4 * a4) / (2 * a3 * a4);

    // Sprawdź, czy punkt docelowy jest osiągalny
    if (teta3 < -1.0f || teta3 > 1.0f) {
        std::cerr << "Target GAMA out of reach!" << std::endl;
        return false;
    }

    float omega = (a3 * a3 + b * b - a4 * a4) / (2 * a3 * b);

    // Sprawdź, czy punkt docelowy jest osiągalny
    if (omega < -1.0f || omega > 1.0f) {
        std::cerr << "Target E out of reach!" << std::endl;
        return false;
    }

    float teta2 = phi + omega;

    glm::mat4 T1 = glm::mat4(1.0f);
    T1 = glm::rotate(T1, glm::radians(teta1), glm::vec3(0.0f, 0.0f, 1.0f)); // Pitch

    T1 = T1 * T1;

    glm::mat4 T2 = glm::mat4(1.0f);
    T2 = glm::rotate(T2, glm::radians(teta2), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch

    glm::mat4 T3 = glm::mat4(1.0f);
    T3 = glm::rotate(T3, glm::radians(teta3), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch

    // Find the product of T1 * T2 * T3 * T4 * T5
    glm::mat4 T1_to_T3 = T1 * T2 * T3;

    
    // Find the product of T1 * T2 * T3 * T4 * T5
    glm::mat4 T3_to_T6 = glm::inverse(T1_to_T3) * T6;

    // Extract the rotation matrix R from T3_to_T6
    glm::mat3 R = glm::mat3(T3_to_T6);

    //Extract individual components
    float r13 = R[0][2];
    float r23 = R[1][2];
    float r33 = R[2][2];
    float r31 = R[2][0];
    float r32 = R[2][1];

    // Calculate theta5
    float teta5 = atan2(r33, sqrt(1.0f - r33 * r33));

    // Handle singularity for theta5
    if (fabs(r33) > 0.9999f) {
        std::cerr << "Singularity detected: theta5 is at its limit.\n";
        if (r33 > 0) {
            teta5 = 0.0f;
        } else {
            teta5 = glm::pi<float>();
        }
    }

    // Calculate theta4
    float teta4 = atan2(r13, r23);

    // Calculate theta6
    float teta6 = atan2(-r31, r32);

        // Odczyt bieżących kątów
    float currentTheta0 = glm::radians(joints[0].angle);
    float currentTheta1 = glm::radians(links[1].angle);
    float currentTheta2 = glm::radians(joints[2].angle);
    float currentTheta3 = glm::radians(joints[3].angle);
    float currentTheta4 = glm::radians(joints[4].angle);
    float currentTheta5 = glm::radians(joints[5].angle);

    // Interpolacja w 10 krokach
    const int steps = 100;
    ikSteps.clear();
    for (int i = 1; i <= steps; ++i) {
        float t = static_cast<float>(i) / steps;
        float easedT = EaseInOut(t);             // Modyfikacja za pomocą Ease-In-Out
        ikSteps.push_back({
            glm::mix(currentTheta0, teta1, t),
            glm::mix(currentTheta1, teta2, t),
            glm::mix(currentTheta2, teta3, t),
            glm::mix(currentTheta3, teta4, t),
            glm::mix(currentTheta4, teta5, t),
            glm::mix(currentTheta5, teta6, t)
        });
    }
    currentStep = 0;

    return true;
}




glm::vec3 RoboArm::GetCurrentPosition() {
    const float L1 = 4.7f; // Długość pierwszego segmentu
    const float L2 = 2.91f; // Długość drugiego segmentu
    const float L3 = 0.07f; // Długość trzeciego segmentu (przed gripperem)

    // Pobierz aktualne kąty przegubów w radianach
    float theta0 = glm::radians(joints[0].angle); // Obrót podstawy
    float theta1 = glm::radians(joints[1].angle); // Unoszenie ramienia
    float theta2 = glm::radians(joints[2].angle); // Unoszenie przedramienia

    // Pozycja końcówki
    float x = 0.0f, y = 0.0f, z = 0.0f;

    // Oblicz pozycję końcówki w układzie globalnym
    float xy = L1 * cos(theta1) + L2 * cos(theta1 + theta2) + L3 * cos(theta1 + theta2);
    z = L1 * sin(theta1) + L2 * sin(theta1 + theta2) + L3 * sin(theta1 + theta2);
    x = xy * cos(theta0);
    y = xy * sin(theta0);

    return glm::vec3(x, y, z);
}

std::vector<std::vector<double>> ComputePseudoInverse(const std::vector<std::vector<double>>& J) {
    size_t rows = J.size();
    size_t cols = J[0].size();

    // Zakładamy, że J jest macierzą 6xN
    std::vector<std::vector<double>> J_pseudo(cols, std::vector<double>(rows, 0.0));

    return J_pseudo;
}


// Zapisz aktualne kąty przegubów jako pozycję domową
void RoboArm::saveHomeAngle() {
    homeAngles.clear();
    for (size_t i = 0; i < joints.size(); ++i) {
        if (i == 1){
            homeAngles.push_back(links[i].angle);
            continue;
        }
        homeAngles.push_back(joints[i].angle);
    }

    if (homeAngles.size() == 6) {
        std::cout << "Home position saved: ";
        for (const auto& angle : homeAngles) {
            std::cout << angle << " ";
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Error: Expected 6 joints, but found " << homeAngles.size() << "." << std::endl;
    }
}

// Przenieś manipulator do podanych kątów lub pozycji domowej
void RoboArm::moveA(const std::vector<float>& targetAngles) {
    if (targetAngles.size() == 6) {
        std::cerr << "Error: Number of target angles does not match the number of joints." << targetAngles.size() << " " << joints.size() <<std::endl;
        return;
    }

    for (size_t i = 0; i < joints.size(); ++i) {
        if (i == 1){
            links[i].angle = targetAngles[i];
            continue;
        }
        joints[i].angle = targetAngles[i];
    }

    std::cout << "Moved to target angles: ";
    for (const auto& angle : targetAngles) {
        std::cout << angle << " ";
    }
    std::cout << std::endl;
}

// Przenieś manipulator do pozycji domowej
void RoboArm::moveHome() {
    if (homeAngles.size() == 6) {
        std::cerr << "Error: Home position not set or does not match joint count." << std::endl;
        return;
    }

    moveA(homeAngles); // Przenieś do zapisanej pozycji domowej
    std::cout << "Moved to home position." << std::endl;
}

void RoboArm::Wait(float seconds){
    float startTime = static_cast<float>(glfwGetTime());
    while ((static_cast<float>(glfwGetTime()) - startTime) < seconds) {
        //
    }
}

std::vector<float> RoboArm::GetCurrentJointAngles() {
    std::vector<float> angles;
    for (size_t i = 0; i < joints.size(); i++) {
        if (i==1){
            angles.push_back(links[i].angle);
            continue;
        }
            angles.push_back(joints[i].angle);
    }
    return angles;
}

void RoboArm::SetJointAngles(const std::vector<float>& angles) {
    for (size_t i = 0; i < joints.size(); i++) {
        if (i < angles.size()) {
        if (i==1){
            links[i].angle = angles[i];
            continue;
        }
            joints[i].angle = angles[i];
        }
    }
}
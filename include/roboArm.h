#ifndef ROBOARM_CLASS_H
#define ROBOARM_CLASS_H

#include "joint.h"
#include "link.h"
#include "gripper.h"
#include "block.h"
#include "element.h"
#include <GLFW/glfw3.h>

struct IKStep {
    float theta0, theta1, theta2, theta3, theta4, theta5;
};

struct Position {
    glm::mat4 jointPosition;
};


class RoboArm
{
public:
    std::vector<Link> links; // Wszystkie linki
    std::vector<Joint> joints; // Wszystkie przeguby
    
    std::vector<Element> elements; // Wszystkie przeguby

    Gripper gripper;           // Chwytak na końcu
    Block block;

    
    std::vector<IKStep> ikSteps;
    int currentStep = 0;
    glm::mat4 robottransform = glm::mat4(1.0f);
    std::vector<Position> Positions;
    
    std::vector<float> homeAngles; // Przechowywanie kątów pozycji domowej

    bool collisionDetected = false;
    bool groundCollision = false;
    std::vector<glm::mat4> lastValidTransforms; // Transformacje każdego elementu (przegub/link)

    RoboArm(const Gripper& gripper, const Block& block);
    bool DetectCollision(Block& block, glm::mat4& localTransform);

    void ResetCollisionState();
    bool IsCollisionDetected() const;
    void SetCollisionDetected(bool state);

	// robotArm
	void AddSegment(const Link& link, const Joint& joint);
	void Draw(ShaderM& shader);

    //kawasaki
    void AddElement(const Element& element);
    void DrawForward(ShaderM& shader);

    glm::vec3 CalculateEndEffectorPosition();
    void MoveToTarget(const glm::vec3& target);
	// Deletes the VBO
	void RotateJoint(size_t jointIndex, float angleDelta);
    void RotateGripper(float angleDelta);

    void SetJointOffset(size_t jointIndex, const glm::vec3& offset);

    void SetLinkOffset(size_t linktIndex, const glm::vec3& offset);

    bool CheckCollisionWithBlock(const Block& block) const;

    void CloseGripper();
    void OpenGripper();

    //bool SolveIK(glm::vec3& targetPos, float targetAngle);
    bool SolveIK(glm::vec3& targetPos, glm::vec3& targetAngle);
    //bool SolveIK(glm::vec3& targetPos, float targetAngle);
    glm::vec3 GetCurrentPosition();
    
    std::vector<float> GetCurrentJointAngles(); // Zwraca aktualne kąty stawów
    void SetJointAngles(const std::vector<float>& angles); // Ustawia kąty stawów

    void saveHomeAngle();
    void moveA(const std::vector<float>& targetAngles);
    void moveHome();
    void Wait(float seconds);

};




#endif
#ifndef JOINT_CLASS_H
#define JOINT_CLASS_H

#include "shader_m.h"
#include "model.h"

class Joint
{
public:
    Model model;            // Model przegubu
    glm::vec3 rotationAxis; // Oś rotacji
    float angle;            // Aktualny kąt rotacji
    glm::vec3 offset;       // Przesunięcie względem poprzedniego elementu
    bool allowsRotation; // Czy przegub może się obracać
    bool blockRot;

	// Constructor that generates a Vertex Buffer Object and links it to vertices
	Joint(const std::string& modelPath, const glm::vec3& axis, const glm::vec3& offset, bool allowsRotation);

    void Draw(ShaderM& shader, const glm::mat4& parentTransform);

	// Unbinds the VBO
	void Rotate(float angleDelta);

    glm::mat4 GetTransform() const;

    void BlockRotation();
    void UnblockRotation();
};

#endif
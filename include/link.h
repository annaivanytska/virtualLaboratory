#ifndef LINK_CLASS_H
#define LINK_CLASS_H

#include "model.h"
#include "shader_m.h"

class Link {
public:
    Model model;
    glm::vec3 rotationAxis; // Oś rotacji
    float angle;            // Kąt rotacji
    glm::vec3 offset;       // Pozycja względem poprzedniego elementu
    bool blockRot;

    Link(const std::string& modelPath, const glm::vec3& offset, const glm::vec3& axis);

    glm::mat4 GetTransform() const;

    void Draw(ShaderM& shader, const glm::mat4& parentTransform);

    void Rotate(float angleDelta);

    void BlockRotation();

    void UnblockRotation();

};


#endif
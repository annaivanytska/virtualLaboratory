#include "joint.h"


Joint::Joint(const std::string& modelPath, const glm::vec3& axis, const glm::vec3& offset, bool allowsRotation = true)
    : model(modelPath), rotationAxis(axis), angle(0.0f), offset(offset),
     allowsRotation(allowsRotation), blockRot(false) {}

void Joint::Draw(ShaderM& shader, const glm::mat4& parentTransform) {
        glm::mat4 globalTransform = parentTransform * GetTransform();

        // Ustawienie globalnej macierzy modelu w shaderze
        shader.setMat4("model", globalTransform);

        // Rysowanie modelu przegubu
        model.Draw(shader);
}

void Joint::Rotate(float angleDelta) {
    angle += angleDelta; // Zaktualizuj kąt rotacji
}

glm::mat4 Joint::GetTransform() const {
    glm::mat4 transform = glm::mat4(1.0f);

    // Przesunięcie
    transform = glm::translate(transform, offset);
    //transform = glm::rotate(transform, glm::radians(angle), rotationAxis);

    if(!blockRot){
        if (this->allowsRotation) {
            transform = glm::rotate(transform, glm::radians(90.0f + angle), rotationAxis);
        }
    }

    return transform;
}

void Joint::BlockRotation() {
    blockRot = true; // Zaktualizuj kąt rotacji
}

void Joint::UnblockRotation() {
    blockRot = false; // Zaktualizuj kąt rotacji
}
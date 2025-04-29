#include "link.h"


Link::Link(const std::string& modelPath, const glm::vec3& offset, const glm::vec3& axis = glm::vec3(0.0f, 0.0f, 0.0f))
        : model(modelPath), offset(offset), rotationAxis(axis), angle(0.0f),
        blockRot(false) {}

void Link::Draw(ShaderM& shader, const glm::mat4& parentTransform) 
{
    glm::mat4 transform = parentTransform * GetTransform();
    //transform = glm::translate(transform, offset);
    shader.setMat4("model", transform);
    model.Draw(shader);
}

glm::mat4 Link::GetTransform() const {
    glm::mat4 transform = glm::mat4(1.0f);

    // Przesunięcie
    transform = glm::translate(transform, offset);

    if(!blockRot){
        // Obrót (jeśli zdefiniowano oś rotacji)
        if (rotationAxis != glm::vec3(0.0f)) {
            transform = glm::rotate(transform, glm::radians(angle), rotationAxis);
        }
    }

    return transform;
}

void Link::Rotate(float angleDelta) {
    angle += angleDelta; // Dodaj rotację
}

void Link::BlockRotation() {
    blockRot = true; // Dodaj rotację
}

void Link::UnblockRotation() {
    blockRot = false; // Odblokuj rotację
}
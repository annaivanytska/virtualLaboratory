#include "block.h"


Block::Block(const std::string& modelPath, const glm::vec3& initialPosition, const glm::vec3& size)
    : model(modelPath), position(initialPosition), size(size), isHeld(false) {}

// Zwraca granice bounding boxa
glm::vec3 Block::GetMinBounds() const {
    return position - size; // Dolne granice (od środka)
}

glm::vec3 Block::GetMaxBounds() const {
    return position + size; // Górne granice (od środka)
}

void Block::Draw(ShaderM& shader, const glm::mat4& transform) {
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
    shader.setMat4("model", transform * modelMatrix);
    shader.setVec3("objectColor", 1.0f, 0.5f, 1.0f);
    //shader.setVec3("modelColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Przykładowy kolor czerwony
    model.Draw(shader);
}

void Block::SetPosition(const glm::vec3& newPosition) {
    position = newPosition;
}

glm::vec3 Block::GetPosition() const {
    return position;
}

void Block::Grab() {
    isHeld = true;
}

void Block::Release() {
    isHeld = false;
}
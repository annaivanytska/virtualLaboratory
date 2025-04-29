#ifndef GRIPPER_CLASS_H
#define GRIPPER_CLASS_H

#include "shader_m.h"
#include "model.h"
#include "block.h"

class Gripper {
public:
    Model base;              // Model bazy chwytaka
    Model clampLeft;         // Model lewego zacisku
    Model clampRight;        // Model prawego zacisku
    glm::vec3 baseOffset;    // Pozycja bazy względem ostatniego segmentu
    glm::vec3 leftOffset;    // Początkowa pozycja lewego zacisku
    glm::vec3 rightOffset;   // Początkowa pozycja prawego zacisku
    float clampDistance;     // Odległość zacisków od środka
    glm::vec3 rotationAxis; // Oś rotacji chwytaka (opcjonalnie)
    float minClampDistance; // Minimalna odległość między zaciskami (nie może przeniknąć klocka)
    float angle;       // Kąt rotacji chwytaka
    glm::vec3 localPosition;   // Początkowa pozycja prawego zacisku

    Block* heldBlock; // Wskaźnik na uchwycony klocek

    Gripper(const std::string& basePath, const std::string& leftPath, const std::string& rightPath, 
            const glm::vec3& baseOffset, const glm::vec3& leftStartOffset, const glm::vec3& rightStartOffset,
            const glm::vec3& axis);

    bool DetectCollision(Block& block);

    // Zwraca lokalną transformację chwytaka
    glm::mat4 GetBaseTransform(const glm::mat4& parentTransform) const;
    glm::mat4 GetClampLeftTransform(const glm::mat4& parentTransform) const;
    glm::mat4 GetClampRightTransform(const glm::mat4& parentTransform) const;

    // Aktualizacja rotacji chwytaka
    void Rotate(float angleDelta);

    void Draw(ShaderM& shader, const glm::mat4& parentTransform, Block& block);

    bool IsPointInBoundingBox(const glm::vec3& point, const Block& block) const;

    void CloseGrip(float step, Block* block);
    void OpenGrip(float step);

    void SetGripPosition(float position, Block* block);
    float GetGripPosition() const;

    glm::vec3 GetGlobalPosition() const;
};


#endif
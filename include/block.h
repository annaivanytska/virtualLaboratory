#ifndef BLOCK_CLASS_H
#define BLOCK_CLASS_H

#include<model.h>

class Block
{
public:
    Model model;           // Model klocka
    glm::vec3 position;    // Pozycja globalna klocka
    glm::vec3 size;        // Rozmiar klocka (bounding box)
    bool isHeld;           // Czy klocek jest uchwycony

    Block(const std::string& modelPath, const glm::vec3& initialPosition, const glm::vec3& size);

    glm::vec3 GetMinBounds() const;
    glm::vec3 GetMaxBounds() const;

    void Draw(ShaderM& shader, const glm::mat4& transform);

    void SetPosition(const glm::vec3& newPosition);
    glm::vec3 GetPosition() const;

    void Grab();

    void Release();
};

#endif
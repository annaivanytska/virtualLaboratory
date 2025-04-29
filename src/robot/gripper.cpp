#include "gripper.h"

Gripper::Gripper(const std::string& basePath, const std::string& leftPath, const std::string& rightPath, 
            const glm::vec3& baseOffset, const glm::vec3& leftStartOffset, const glm::vec3& rightStartOffset,
            const glm::vec3& axis = glm::vec3(0.0f, 0.0f, 0.0f))
        : base(basePath), clampLeft(leftPath), clampRight(rightPath), 
          baseOffset(baseOffset), leftOffset(leftStartOffset), rightOffset(rightStartOffset), 
          clampDistance(0.5f), heldBlock(nullptr), rotationAxis(axis), angle(0.0f), localPosition(glm::vec3(0.0f, 0.0f, 0.0f)) {}

// Wykrywanie kolizji z klockiem
bool Gripper::DetectCollision(Block& block) {
        glm::vec3 blockMin = block.GetMinBounds();
        glm::vec3 blockMax = block.GetMaxBounds();

        // Pozycje zacisków
        glm::vec3 leftClampPos = localPosition + glm::vec3(0.0f, clampDistance, 0.0f);
        glm::vec3 rightClampPos = localPosition - glm::vec3(0.0f, clampDistance, 0.0f);

        // Sprawdzenie, czy lewy zacisk znajduje się w bounding boxie klocka
        bool leftCollision = leftClampPos.x >= blockMin.x && leftClampPos.x <= blockMax.x &&
                             leftClampPos.y >= blockMin.y && leftClampPos.y <= blockMax.y &&
                             leftClampPos.z >= blockMin.z && leftClampPos.z <= blockMax.z;

        // Sprawdzenie, czy prawy zacisk znajduje się w bounding boxie klocka
        bool rightCollision = rightClampPos.x >= blockMin.x && rightClampPos.x <= blockMax.x &&
                              rightClampPos.y >= blockMin.y && rightClampPos.y <= blockMax.y &&
                              rightClampPos.z >= blockMin.z && rightClampPos.z <= blockMax.z;

        if (leftCollision && rightCollision) {std::cout << "COLOSION DETECTED" << std::endl;} 

        return leftCollision && rightCollision;
}

glm::mat4 Gripper::GetBaseTransform(const glm::mat4& parentTransform) const {
    return parentTransform * glm::translate(glm::mat4(1.0f), baseOffset);
}

glm::mat4 Gripper::GetClampLeftTransform(const glm::mat4& parentTransform) const {
    glm::vec3 adjustedOffset = leftOffset + glm::vec3(0.0f, clampDistance, 0.0f); // Zacisk w stronę środka
    return parentTransform * glm::translate(glm::mat4(1.0f), adjustedOffset);
}

glm::mat4 Gripper::GetClampRightTransform(const glm::mat4& parentTransform) const {
    glm::vec3 adjustedOffset = rightOffset - glm::vec3(0.0f, clampDistance, 0.0f); // Zacisk w stronę środka
    return parentTransform * glm::translate(glm::mat4(1.0f), adjustedOffset);
}


// Aktualizacja rotacji chwytaka
void Gripper::Rotate(float angleDelta) {
    angle += angleDelta;
}

void Gripper::Draw(ShaderM& shader, const glm::mat4& parentTransform, Block& block) {

        // Rysowanie bazy i zacisków (jak poprzednio)
    glm::mat4 baseTransform = parentTransform * glm::translate(glm::mat4(1.0f), baseOffset);
    shader.setMat4("model", baseTransform);
    localPosition = glm::vec3(baseTransform[3]);
    base.Draw(shader);

    glm::mat4 leftTransform = baseTransform * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, clampDistance, 0.0f));
    shader.setMat4("model", leftTransform);
    localPosition = glm::vec3(leftTransform[3]);
    clampLeft.Draw(shader);

    glm::mat4 rightTransform = baseTransform * glm::translate(glm::mat4(1.0f), glm::vec3( 0.0f, -clampDistance, 0.0f));
    shader.setMat4("model", rightTransform);
    localPosition = glm::vec3(rightTransform[3]);
    clampRight.Draw(shader);
    
    localPosition = glm::vec3(baseTransform[3]);

    //Jeśli klocek jest uchwycony, rysuj go w przestrzeni lokalnej grippera
    if (heldBlock != nullptr) {
        glm::mat4 blockTransform = baseTransform;
        heldBlock->Draw(shader, blockTransform);
    }
    // if (heldBlock != nullptr) {
    //     glm::vec3 gripperPosition = GetGlobalPosition();
    //     block.SetPosition(gripperPosition); // Aktualizacja pozycji
    //     glm::mat4 blockTransform = baseTransform;
    //     block.Draw(shader, blockTransform);
    // }
}


void Gripper::CloseGrip(float step = 0.01f, Block* block = nullptr) {
    // Jeśli już uchwycono blok, nie zamykaj dalej
    if (heldBlock != nullptr) {
        return;
    }

    // Jeśli wykryto kolizję, ustaw uchwycony blok
    if (block != nullptr && DetectCollision(*block)) {
        std::cout << "Collision detected! Grabbing block." << std::endl;
        block->Grab(); // Oznacz blok jako uchwycony
        heldBlock = block; // Przypisz blok do grippera

                // Ustaw pozycję bloku na środek grippera (lub odpowiednie miejsce)
        glm::vec3 gripperPosition = glm::vec3(baseOffset);
        block->SetPosition(gripperPosition);

        return; // Przerwij dalsze zamykanie
    }

    clampDistance += step;
    if (clampDistance > 0.5f) clampDistance = 0.5f;
}

void Gripper::OpenGrip(float step = 0.01f) {
    
    if (heldBlock != nullptr) {
        glm::vec3 globalPosition = glm::vec3(baseOffset); // Możesz obliczyć globalną pozycję grippera
        heldBlock->SetPosition(glm::vec3(localPosition[0], localPosition[1], 0.5f));
        heldBlock->Release();
        heldBlock = nullptr;
    }

    clampDistance -= step;
    if (clampDistance < 0.0f) clampDistance = 0.0f; // Maksymalne zamknięcie
} 


bool Gripper::IsPointInBoundingBox(const glm::vec3& point, const Block& block) const{
    glm::vec3 blockMin = block.GetMinBounds();
    glm::vec3 blockMax = block.GetMaxBounds();

    return (point.x >= blockMin.x && point.x <= blockMax.x &&
            point.y >= blockMin.y && point.y <= blockMax.y &&
            point.z >= blockMin.z && point.z <= blockMax.z);
}


// void Gripper::SetGripPosition(float position, Block* block) {
//     // Pozycja musi być w zakresie 0.0 (zamknięte) do 0.5 (maksymalnie otwarte)
//     if (position < 0.0f) position = 0.0f;
//     if (position > 0.5f) position = 0.5f;

//     // Otwieranie lub zamykanie chwytaka w zależności od pozycji
//     if (position > clampDistance) { // Otwieranie
//         std::cout << "Opening grip to position: " << position << std::endl;
//         OpenGrip(position); // Otwieranie o różnicę w pozycji
//     } else if (position < clampDistance) { // Zamykanie
//         std::cout << "Closing grip to position: " << position << std::endl;
//         CloseGrip(position, block); // Zamknięcie o różnicę w pozycji
//     }

//     //clampDistance = position; // Aktualizacja pozycji chwytaka
// }

// void Gripper::SetGripPosition(float position, Block* block) {
//     // Clamp position between the allowed range
//     if (position < 0.0f) position = 0.0f;
//     if (position > 0.5f) position = 0.5f;

//     // Update clamp distance
//     clampDistance = position;

//     // If holding a block, update its position to match the gripper
//     if (heldBlock != nullptr) {
//         glm::vec3 gripperPosition = GetGlobalPosition();
//         heldBlock->SetPosition(gripperPosition);
//         std::cout << "Held block position updated to: " << gripperPosition.x << ", "
//                   << gripperPosition.y << ", " << gripperPosition.z << std::endl;
//     }
// }

void Gripper::SetGripPosition(float position, Block* block) {
    // Ograniczenie pozycji do zakresu od 0.0 do 0.5
    if (position < 0.0f) position = 0.0f;
    if (position > 0.5f) position = 0.5f;

    // Sprawdzenie, czy otwieramy czy zamykamy chwytak
    if (position < clampDistance) { 
        // Otwieranie chwytaka
        std::cout << "Opening grip to position: " << position << std::endl;
        OpenGrip(position - clampDistance);
    } else if (position > clampDistance) { 
        // Zamykanie chwytaka
        std::cout << "Closing grip to position: " << position << std::endl;
        CloseGrip(clampDistance - position, block);
    }

    // Aktualizacja odległości zacisków
    clampDistance = position;

    // // Jeśli trzymamy blok, aktualizujemy jego pozycję
    // if (heldBlock != nullptr) {
    //     glm::vec3 gripperPosition = GetGlobalPosition();
    //     heldBlock->SetPosition(gripperPosition);
    //     std::cout << "Held block position updated to: " 
    //               << gripperPosition.x << ", "
    //               << gripperPosition.y << ", "
    //               << gripperPosition.z << std::endl;
    // }
}



float Gripper::GetGripPosition() const {
    // Zwróć aktualną odległość zacisków
    return clampDistance;
}

glm::vec3 Gripper::GetGlobalPosition() const {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), baseOffset);
    transform = transform * glm::translate(glm::mat4(1.0f), localPosition); // Dodaj lokalną pozycję
    return glm::vec3(transform[3]); // Pozycja w macierzy
}

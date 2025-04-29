#include "element.h"

glm::mat4 DHTransform(float theta, float d, float a, float alpha) {
    glm::mat4 transform = glm::mat4(1.0f);

    // Obrót wokół osi Z
    transform = glm::rotate(transform, theta, glm::vec3(0.0f, 0.0f, 1.0f));
    // Przesunięcie wzdłuż osi Z
    transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, d));
    // Przesunięcie wzdłuż osi X
    transform = glm::translate(transform, glm::vec3(a, 0.0f, 0.0f));
    // Obrót wokół osi X
    transform = glm::rotate(transform, alpha, glm::vec3(1.0f, 0.0f, 0.0f));

    return transform;
}


Element::Element(const std::string& modelPath, const float theta, const float d,  const float a, const float alpha)
    : model(modelPath), theta(theta), d(d), a(a), alpha(alpha) {}

glm::mat4 Element::GetTransform() const {
        //glm::mat4 transform = glm::mat4(1.0f);
        //transform *= DHTransform(theta, d, a, alpha);
        return DHTransform(theta, d, a, alpha);
    }

void Element::Draw(ShaderM& shader, const glm::mat4& parentTransform) {
        glm::mat4 localTransform = GetTransform();

        std::cout << "Podane wartości " << theta << " " << d << " " << a << " " << alpha << std::endl;
        std::cout << "Transform Matrix for segment: " << glm::to_string(localTransform) << std::endl;


        glm::mat4 globalTransform = parentTransform * localTransform;

        // Ustawienie globalnej macierzy modelu w shaderze
        shader.setMat4("model", globalTransform);

        // Rysowanie modelu przegubu
        model.Draw(shader);
}



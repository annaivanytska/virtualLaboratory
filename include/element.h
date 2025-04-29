#ifndef ELEMENT_CLASS_H
#define ELEMENT_CLASS_H
#define GLM_ENABLE_EXPERIMENTAL

#include "shader_m.h"
#include "model.h"
#include <glm/gtx/string_cast.hpp>


glm::mat4 DHTransform(float theta, float d, float a, float alpha);

class Element
{
public:
    Model model;            // Model przegubu
	float theta;
	float d;
	float a;
	float alpha;

	// Constructor that generates a Vertex Buffer Object and links it to vertices
	Element(const std::string& modelPath, const float theta, const float d, const float a, const float alpha);
	glm::mat4 GetTransform() const;

    void Draw(ShaderM& shader, const glm::mat4& parentTransform);

};

#endif
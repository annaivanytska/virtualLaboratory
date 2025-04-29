
// #include <dh.h>


// DHParameters::DHParameters(float theta, float d, float a, float alpha)
//     : theta(theta), d(d), a(a), alpha(alpha) {}

// glm::mat4 DHParameters::ComputeDHMatrix(const DHParameters& params) {
//     float theta = glm::radians(params.theta);
//     float alpha = glm::radians(params.alpha);

//     glm::mat4 matrix = glm::mat4(1.0f);

//     matrix[0][0] = cos(theta);
//     matrix[0][1] = -sin(theta) * cos(alpha);
//     matrix[0][2] = sin(theta) * sin(alpha);
//     matrix[0][3] = params.a * cos(theta);

//     matrix[1][0] = sin(theta);
//     matrix[1][1] = cos(theta) * cos(alpha);
//     matrix[1][2] = -cos(theta) * sin(alpha);
//     matrix[1][3] = params.a * sin(theta);

//     matrix[2][0] = 0.0f;
//     matrix[2][1] = sin(alpha);
//     matrix[2][2] = cos(alpha);
//     matrix[2][3] = params.d;

//     matrix[3][0] = 0.0f;
//     matrix[3][1] = 0.0f;
//     matrix[3][2] = 0.0f;
//     matrix[3][3] = 1.0f;

//     return matrix;
// }



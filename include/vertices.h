#ifndef VERTICES_H
#define VERTICES_H

#include <glad/glad.h> 

// Vertices coordinates
GLfloat BoardVertices[] = {
         7.00f,  7.00f, 0.0f, 0.274f, 0.376f, 0.447f,  // top right
         7.00f, -7.00f, 0.0f, 0.274f, 0.376f, 0.447f, // bottom right
        -7.00f,  7.00f, 0.0f, 0.274f, 0.376f, 0.447f, // top le        ft 

         7.00f, -7.00f, 0.0f, 0.274f, 0.376f, 0.447f,  // bottom right
        -7.00f, -7.00f, 0.0f, 0.274f, 0.376f, 0.447f, // bottom left
        -7.00f,  7.00f, 0.0f, 0.274f, 0.376f, 0.447f  // top left 
    };

GLfloat LineVertices[] = {
        -0.5f,  0.5f,  0.0f,    1.0f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.0f,    1.0f, 0.0f, 0.0f
};

GLfloat AxisVertices[] = {
    // X-axis (czerwony)
    -10.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // Punkt początkowy (czerwony)
     10.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // Punkt końcowy (czerwony)

    // Y-axis (zielony)
     0.0f, -10.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Punkt początkowy (zielony)
     0.0f,  10.0f,  0.0f, 0.0f, 1.0f, 0.0f, // Punkt końcowy (zielony)

    // Z-axis (niebieski)
     0.0f,  0.0f, -10.0f, 0.0f, 0.0f, 1.0f, // Punkt początkowy (niebieski)
     0.0f,  0.0f,  10.0f, 0.0f, 0.0f, 1.0f  // Punkt końcowy (niebieski)
};

#endif
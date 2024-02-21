#version 330 core

// Set location of aPos to 0(used in glVertexAttribPointer())
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
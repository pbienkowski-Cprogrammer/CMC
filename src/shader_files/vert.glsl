#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float aSideIndex;

out vec2 texCoords;
out float sideIndex;

uniform mat4 trans;
uniform mat4 view;
uniform mat4 perspe;

void main()
{
    gl_Position = perspe * view * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    texCoords = aTexCoords;
    sideIndex = aSideIndex;
}

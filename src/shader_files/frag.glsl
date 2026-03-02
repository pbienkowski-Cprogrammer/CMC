#version 460 core

out vec4 fragColor;
in vec2 texCoords;

uniform sampler2D texture_atlas;

void main()
{
    fragColor = texture(texture_atlas, texCoords);//mix(texture(texture1, texCoords), texture(texture2, texCoords), 0.2);
}

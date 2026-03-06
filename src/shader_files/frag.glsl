#version 460 core

out vec4 fragColor;
in vec2 texCoords;
in float sideIndex;

uniform sampler2D texture_atlas;

void main()
{
    fragColor = texture(texture_atlas, texCoords);//mix(texture(texture1, texCoords), texture(texture2, texCoords), 0.2);

    if(sideIndex == 4.0f || sideIndex == 6.0f)
    {
        fragColor.rgb *= 0.86f;
    }
    else if(sideIndex == 1.0f || sideIndex == 3.0f)
    {
        fragColor.rgb *= 0.80f;
    }
}

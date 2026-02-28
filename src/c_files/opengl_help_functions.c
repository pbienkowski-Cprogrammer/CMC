#include "opengl_help_functions.h"

#include <glad.h>
#include <stdbool.h>
#include <stb/stb_image.h>

bool readFile(const char* file_location, const char** file_destinity)
{
    FILE* file;
    long file_size;
    char* buffer;

    file = fopen(file_location, "r");
    if(file == NULL)
    {
        printf("Failed to open file %s\n", file_location);
        return false;
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    buffer = (char*)malloc(sizeof(char) * (file_size + 1));
    if(buffer == NULL)
    {
        printf("memory allocation error\n");
        fclose(file);
        return false;
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);

    *file_destinity = buffer;

    return true;
};

bool createShader(GLuint* shader, const char* vertexShaderFileplace, const char* fragmentShaderFileplace)
{
    const char* vertexShaderSource;
    readFile(vertexShaderFileplace, &vertexShaderSource);

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("VERTEX::SHADER::COMPILATION::ERROR%s\n", infoLog);
    }

    const char* fragmentShaderSource;
    readFile(fragmentShaderFileplace, &fragmentShaderSource);

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("FRAGMENT::SHADER::COMPILATION::ERROR%s\n", infoLog);
    }

    *shader = glCreateProgram();

    glAttachShader(*shader, vertexShader);
    glAttachShader(*shader, fragmentShader);
    glLinkProgram(*shader);

    glGetProgramiv(*shader, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(*shader, 512, NULL, infoLog);
        printf("SHADER::PROGRAM::LINK::FAILED::ERROR%s\n", infoLog);
    }

    free((void*)vertexShaderSource);
    free((void*)fragmentShaderSource);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
};

bool createTexture2D(GLuint* texture, const char* Fileplace)
{
    glGenTextures(1, texture);

    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(Fileplace, &width, &height, &nrChannels, 0);

    if(data)
    {
        GLenum format;
        switch(nrChannels)
        {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
                break;
            case 4:
                format = GL_RGBA;
                break;
                break;
            default:
                format = GL_RGB;
        };

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture : %s\n", Fileplace);
        return false;
    }

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
};

void activateTexture2D(GLuint texture, int activeTexture)
{
    glActiveTexture(activeTexture);
    glBindTexture(GL_TEXTURE_2D, texture);
};

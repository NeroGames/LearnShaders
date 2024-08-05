////////////////////////////////////////////////////////////
// Learn OpenGL Shaders (GLSL)
// Copyright (c) 2024 sk-landry
////////////////////////////////////////////////////////////
///////////////////////////HEADERS//////////////////////////
//Engine
#include "ShaderScene.h"
//Boost
#include <boost/filesystem.hpp>
//STB
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//CPP
#include <fstream>
#include <sstream>
////////////////////////////////////////////////////////////
ShaderScene::ShaderScene():
     m_RenderWindow(nullptr)
    ,m_UTime(0.f)
    ,m_UFrame(0)
{
    setSceneName("The Shader Scene");
}

ShaderScene::~ShaderScene()
{
    // dtor
}

void ShaderScene::init()
{
    //create shader program
    m_ShaderProgram = createShaderProgram();

    //texture
    addTexture();

    //build a rectangle
    createRectangle(sf::Vector2f(0.f, 0.f), getResolution());
}

void ShaderScene::addTexture()
{
    //load texture
    addTexture("bin/Debug/data/texture/container.jpg", "texture1");
}

void ShaderScene::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ShaderScene::render()
{
    drawRectangle();
}

void ShaderScene::destroy()
{
    destroyRectangle();
    glDeleteProgram(m_ShaderProgram);
}

void ShaderScene::handleEvent(const sf::Event& event)
{
    // empty
}

void ShaderScene::update(const sf::Time& timeStep)
{
    m_UTime += timeStep.asSeconds();
    m_UFrame++;
}

void ShaderScene::setSceneName(const std::string& name)
{
    m_SceneName = name;
}

std::string ShaderScene::getSceneName()
{
    return m_SceneName;
}

sf::Vector2f ShaderScene::getResolution()
{
    return m_Resolution;
}

sf::Vector2f ShaderScene::getMousePosition()
{
    const auto pos = sf::Mouse::getPosition(*m_RenderWindow);
    return sf::Vector2f(pos.x, pos.y);
}

sf::Vector2f ShaderScene::pixelToNDC(const sf::Vector2f& vertex)
{
    return sf::Vector2f(-1.f + vertex.x/m_Resolution.x * 2.f,
                         1.f - vertex.y/m_Resolution.y * 2.f);
}

std::string ShaderScene::loadShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    if(!stream)
    {
        throw std::runtime_error("failed to load text from " + filePath);
    }

    std::stringstream buffer;
    buffer << stream.rdbuf();
    stream.close();

    return buffer.str();
}

void ShaderScene::checkShader(const unsigned int shader, const std::string& shaderType)
{
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void ShaderScene::checkShaderProgram(const unsigned int shaderProgram)
{
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

 unsigned int ShaderScene::createShaderProgram()
{
    const std::string vShaderStr = loadShader("bin/Debug/data/shader/vertex.vert");
    const std::string fShaderStr = loadShader("bin/Debug/data/shader/fragment.frag");
    const std::string gShaderStr = loadShader("bin/Debug/data/shader/geometry.geom");
    const char *vShaderSource = vShaderStr.c_str();
    const char *fShaderSource = fShaderStr.c_str();
    const char *gShaderSource = gShaderStr.c_str();

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShaderSource, NULL);
    glCompileShader(vShader);
    checkShader(vShader, "VERTEX");

    // geometry shader
    unsigned int gShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(gShader, 1, &gShaderSource, NULL);
    glCompileShader(gShader);
    checkShader(gShader, "GEOMETRY");

    // fragment shader
    unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShaderSource, NULL);
    glCompileShader(fShader);
    checkShader(fShader, "FRAGMENT");

    // shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, gShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    checkShaderProgram(shaderProgram);

    glDeleteShader(vShader);
    glDeleteShader(gShader);
    glDeleteShader(fShader);

    return shaderProgram;
}

void ShaderScene::createRectangle(const sf::Vector2f& origin, const sf::Vector2f& rectSize)
{
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] =
    {
        pixelToNDC(origin).x,  pixelToNDC(origin).y, 0.0f,                  // top left (origin)
        pixelToNDC(origin + rectSize).x, pixelToNDC(origin).y, 0.0f,           // top right
        pixelToNDC(origin + rectSize).x, pixelToNDC(origin + rectSize).y, 0.0f,  // bottom right
        pixelToNDC(origin).x,  pixelToNDC(origin + rectSize).y, 0.0f          // bottom left
    };

    unsigned int indices[] =
    {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    glGenVertexArrays(1, &m_Rectangle.VAO);
    glGenBuffers(1, &m_Rectangle.VBO);
    glGenBuffers(1, &m_Rectangle.EBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(m_Rectangle.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_Rectangle.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Rectangle.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //textures
    std::cout << "textureCount = " << mTextureData.size() << std::endl;
    for(TextureData& textureData : mTextureData)
    {
        glGenTextures(1, &textureData.textureID);
        glBindTexture(GL_TEXTURE_2D, textureData.textureID);
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        int width, height, nrChannels;
        unsigned char *data = stbi_load(textureData.texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void ShaderScene::drawRectangle()
{
    const auto resolution = getResolution();
    const auto mousePosition = getMousePosition();

    //textures
    unsigned int textureCount = 0;
    for(const TextureData& textureData: mTextureData)
    {
        glActiveTexture(GL_TEXTURE0 + textureCount++);
        glBindTexture(GL_TEXTURE_2D, textureData.textureID);
    }
    // draw our first triangle
    glUseProgram(m_ShaderProgram);

    unsigned int uNameCount = 0;
    for(const TextureData& textureData: mTextureData)
    {
       glUniform1i(glGetUniformLocation(m_ShaderProgram, textureData.uName.c_str()), uNameCount++); // set it manually
    }
    glUniform2f(glGetUniformLocation(m_ShaderProgram, "u_resolution"), resolution.x, resolution.y);
    glUniform2f(glGetUniformLocation(m_ShaderProgram, "u_mouse"), mousePosition.x, mousePosition.y);
    glUniform1f(glGetUniformLocation(m_ShaderProgram, "u_time"), m_UTime);
    glUniform1i(glGetUniformLocation(m_ShaderProgram, "u_frame"), m_UFrame);
    // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glBindVertexArray(m_Rectangle.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ShaderScene::destroyRectangle()
{
    glDeleteVertexArrays(1, &m_Rectangle.VAO);
    glDeleteBuffers(1, &m_Rectangle.VBO);
    glDeleteBuffers(1, &m_Rectangle.EBO);
}

void ShaderScene::addTexture(const std::string& texturePath, const std::string& uName)
{
    TextureData textureData;
    textureData.texturePath = texturePath;
    textureData.uName = uName;
    textureData.textureID = 0;
    mTextureData.push_back(textureData);
}




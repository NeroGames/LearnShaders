////////////////////////////////////////////////////////////
// Learn OpenGL Shaders (GLSL)
// Copyright (c) 2024 sk-landry
////////////////////////////////////////////////////////////
#ifndef SHADERSCENE_H
#define SHADERSCENE_H
///////////////////////////HEADERS//////////////////////////
//Glew
#include <GL/glew.h>
//SFML
#include <SFML/OpenGL.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
//CPP
#include <iostream>
#include <memory>
#include <vector>
////////////////////////////////////////////////////////////
class ShaderScene
{
    public:
        typedef std::unique_ptr<ShaderScene> Ptr;

    public:
                                    ShaderScene();
        virtual                    ~ShaderScene();
        std::string                 getSceneName();
        sf::Vector2f                getResolution();
        sf::Vector2f                getMousePosition();

    protected:
        void                        setSceneName(const std::string& name);
        sf::Vector2f                pixelToNDC(const sf::Vector2f& vertex);
        std::string                 loadShader(const std::string& filePath);
        unsigned int                createShaderProgram();
        void                        checkShader(const unsigned int shader, const std::string& shaderType);
        void                        checkShaderProgram(const unsigned int shaderProgram);
        //rectangle
        void                        createRectangle(const sf::Vector2f& origin, const sf::Vector2f& rectSize);
        void                        drawRectangle();
        void                        destroyRectangle();
        void                        addTexture(const std::string& texturePath, const std::string& uName);

    public:
        virtual void                init();
        virtual void                clear();
        virtual void                render();
        virtual void                destroy();
        virtual void                handleEvent(const sf::Event& event);
        virtual void                update(const sf::Time& timeStep);
        virtual void                addTexture();

    private:
        struct TextureData
        {
            std::string texturePath;
            std::string uName;
            unsigned int textureID;
        };
        friend class                Engine;
        std::string                 m_SceneName;
        sf::Vector2f                m_Resolution;
        sf::Window*                 m_RenderWindow;
        //Shader data
        unsigned int                m_ShaderProgram;
        struct                      Rectangle{unsigned int VBO, VAO, EBO;} m_Rectangle;
        // Uniforms
        float                       m_UTime;
        int                         m_UFrame;
        std::vector<TextureData>    mTextureData;
        // Texture

};
#endif // SHADERSCENE_H

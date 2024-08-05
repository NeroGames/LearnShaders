////////////////////////////////////////////////////////////
// Learn OpenGL
// Copyright (c) 2020 sk-landry
////////////////////////////////////////////////////////////
///////////////////////////HEADERS//////////////////////////
#include "Engine.h"
#include <SFML/System/Time.hpp>
////////////////////////////////////////////////////////////
Engine::Engine(const unsigned int& windowWidth, const unsigned int& windowHeight):
     m_OpenGlAvailable(true)
    ,m_EngineRunning(true)
    ,m_WindowResolution(windowWidth, windowHeight)
{
    createWindow();
    initOpenGL();
}

Engine::~Engine()
{
    //dtor
}

void Engine::setScene(ShaderScene::Ptr scene)
{
    m_Scene = std::move(scene);
    m_RenderWindow.setTitle(m_Scene->m_SceneName);
    m_Scene->m_Resolution = m_WindowResolution;
    m_Scene->m_RenderWindow = &m_RenderWindow;
}

void Engine::run()
{
    if(!m_OpenGlAvailable || !m_Scene) return;

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    m_Scene->init();

    while(m_EngineRunning)
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;

         while(timeSinceLastUpdate > sf::seconds(1/60.f))
        {
            //retrieve 1/60 second in the accumulated time
            timeSinceLastUpdate -= sf::seconds(1/60.f);

            // handle events
            sf::Event event;
            while(m_RenderWindow.pollEvent(event))
            {
                if(event.type == sf::Event::Closed)
                {
                    // end the program
                    m_EngineRunning = false;
                }
                else if (event.type == sf::Event::Resized)
                {
                    // adjust the viewport when the window is resized
                    glViewport(0, 0, event.size.width, event.size.height);
                }

                m_Scene->handleEvent(event);
            }

            m_Scene->update(sf::seconds(1/60.f));
        }

        // clear the buffers
        m_Scene->clear();
        // draw scene
        m_Scene->render();

        // end the current frame (internally swaps the front and back buffers)
        m_RenderWindow.display();
    }

    m_Scene->destroy();
}

void Engine::createWindow()
{
    // create the window
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    m_RenderWindow.create(sf::VideoMode(m_WindowResolution.x, m_WindowResolution.y), "OpenGL", sf::Style::Default, settings);
    m_RenderWindow.setVerticalSyncEnabled(true);

    // activate the window
    m_RenderWindow.setActive(true);
}

void Engine::initOpenGL()
{
    // initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        m_OpenGlAvailable = false;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

/*
 * SAMPLE SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "Sample_Scene.hpp"
#include <basics/Canvas>
#include <basics/Director>
#include <basics/Log>
#include <basics/Scaling>
#include <basics/Rotation>
#include <basics/Translation>

using namespace basics;
using namespace std;

namespace example
{

    Sample_Scene::Sample_Scene()
    {
        canvas_width  = 1280;
        canvas_height =  720;
        playerBulletPool = new Proyectile::bulletPool(16, canvas_width, canvas_height);
        playerCharacter = new PlayerRelated::player();
        enemyPoolSpawner = new Enemies::enemyPool(5, canvas_width, canvas_height, playerCharacter, playerBulletPool);
        timerSpawn = 4;
        timerSinceSpawn = 0;
        aspect_ratio_adjusted = false;

        state     = LOADING;
        x         = 640;
        y         = 360;
    }

    bool Sample_Scene::initialize ()
    {
        suspended = false;

        /*
        state     = LOADING;
        x         = 640;
        y         = 360;
         */

        return true;
    }

    void Sample_Scene::suspend ()
    {
        suspended = true;
    }

    void Sample_Scene::resume ()
    {
        suspended = false;
    }

    void Sample_Scene::handle (Event & event)
    {
        if (!suspended && state == RUNNING)
        {
            int id = *event[ID(id)].as< var::Int32 > ();
            float x = *event[ID(x) ].as< var::Float > ();
            float y = *event[ID(y) ].as< var::Float > ();
            //Vector2f touchPosition{x,y};

            switch (event.id)
            {
                case ID(touch-started):
                {
                    if(x > canvas_width * 0.5f)
                    {
                        if(x > shootCircle.Position.coordinates.x() && x < shootCircle.Position.coordinates.x() + shootCircle.Radius*2)
                        {
                            if(y > shootCircle.Position.coordinates.y() && y < shootCircle.Position.coordinates.y() + shootCircle.Radius*2)
                            {
                                shootCircle.Pressed = true;
                                shootCircle.ID = id;

                                if(shotReady)
                                {
                                    playerBulletPool->spawnBullet({playerCharacter->position.coordinates.x() + 50,playerCharacter->position.coordinates.y() + 100}, 90);
                                    shotReady = false;
                                }
                            }
                        }
                    }

                    else
                    {
                        joystickHandler.Position = Vector2f{x,y};
                        joystickHandler.deltaPosition = Vector2f{x,y};
                        joystickHandler.Pressed = true;
                        joystickHandler.ID = id;
                    }

                    break;
                }

                case ID(touch-moved):
                {
                    if(joystickHandler.Pressed)
                    {
                        joystickHandler.deltaPosition = Vector2f(x,y);
                        Vector2f joystickMovement;
                        joystickMovement = Vector2f (joystickHandler.deltaPosition.coordinates.x() - joystickHandler.Position.coordinates.x() , joystickHandler.deltaPosition.coordinates.y()  - joystickHandler.Position.coordinates.y());
                        joystickMovement.normalize();
                        playerCharacter->direction = joystickMovement;
                    }

                    break;
                }

                case ID(touch-ended):
                {
                    if(id == shootCircle.ID) shootCircle.Pressed = false; //else
                    if(id == joystickHandler.ID)
                    {
                        playerCharacter->direction = Vector2f (0, 0);
                        joystickHandler.Pressed = false;

                    }

                    break;
                }
            }
        }
    }

    void Sample_Scene::update (float time)
    {
        switch (state)
        {
            case LOADING: load ();     break;
            case RUNNING: run  (time); break;
        }
    }

    void Sample_Scene::render (basics::Graphics_Context::Accessor & context)
    {
        if (!suspended && state == RUNNING)
        {
            Canvas * canvas = context->get_renderer< Canvas > (ID(canvas));

            if (!canvas)
            {
                canvas = Canvas::create (ID(canvas), context, {{ canvas_width, canvas_height }});
            }

            if (canvas)
            {
                canvas->clear        ();
                Vector3f backgroundColor(0, 0.19921875, 0.3984375);
                canvas->set_color    (backgroundColor[0], backgroundColor[1], backgroundColor[2]);
                canvas->fill_rectangle({ 0, 0 }, { canvas_width / 1.f, canvas_height / 1.f });

                if(shootCircle.Pressed)
                {
                    canvas->set_color    (0.5,0.5,0.5);
                    canvas->fill_rectangle(shootCircle.Position, { 100, 100 });
                }

                else
                    {
                        canvas->set_color    (1, 1, 1);
                        canvas->fill_rectangle(shootCircle.Position, { 100, 100 });
                    }

                if(joystickHandler.Pressed)
                {
                    canvas->fill_rectangle(joystickHandler.Position, { 100, 100 });

                    if(joystickHandler.deltaPosition.coordinates.x() < joystickHandler.Position.coordinates.x() + joystickHandler.OuterRadius &&
                       joystickHandler.deltaPosition.coordinates.y() < joystickHandler.Position.coordinates.y() + joystickHandler.OuterRadius)
                    {
                        canvas->fill_rectangle(joystickHandler.deltaPosition, { 100, 100 });
                    }

                    else
                        {
                            canvas->fill_rectangle(joystickHandler.deltaPosition, { 100, 100 });
                        }
                }

                playerBulletPool->render(*canvas);

                playerCharacter->render(*canvas);

                enemyPoolSpawner->render(*canvas);

                //testEnemy->render(*canvas);

                if(state == GAME_OVER)
                {
                    canvas->fill_rectangle ({ canvas_width / 2.f, canvas_height / 2.f }, { 100, 60 });
                }
            }
        }
    }

    void Sample_Scene::load ()
    {
        if (!suspended)
        {
            Graphics_Context::Accessor context = director.lock_graphics_context ();

            if (context)
            {
                int textureActivationCount = 0;
                texture = Texture_2D::create (ID(test), context, "test.png");
                buttonTexture = Texture_2D::create (ID(test), context, "test.png");
                bulletTexture = Texture_2D::create (ID(test), context, "test.png");
                playerTexture = Texture_2D::create (ID(test), context, "test.png");
                enemyTexture = Texture_2D::create (ID(test), context, "test.png");

                if(buttonTexture)
                {
                    context->add(buttonTexture);
                    ++textureActivationCount;
                }

                if(bulletTexture)
                {
                    context->add(bulletTexture);
                    ++textureActivationCount;
                }

                /*
                if (texture)
                {
                    context->add (texture);
                    ++textureActivationCount;
                }


                if(playerTexture)
                {
                    context->add(playerTexture);
                    //playerCharacter->setTexture(playerTexture);
                    ++textureActivationCount;
                }

                if(enemyTexture)
                {
                    context->add(enemyTexture);
                    ++textureActivationCount;
                }
                 */

                adjustAspectRatio(context);

                if(aspect_ratio_adjusted && textureActivationCount == 2)
                {
                    shootCircle.Position = Vector2f (canvas_width - 150, 20);
                    playerCharacter->position = {canvas_width/2, canvas_height/2};
                    state = RUNNING;
                }
            }
        }
    }

    void Sample_Scene::run (float time)
    {
        playerBulletPool->update(time);
        playerCharacter->update(time);
        enemyPoolSpawner->update(time);

        timerSinceSpawn += time;

        if(timerSinceSpawn >= timerSpawn)
        {
            timerSinceSpawn = 0;
            enemyPoolSpawner->spawnEnemy({500,500}, 0);
            enemyPoolSpawner->spawnEnemy({200,500}, 0);
            enemyPoolSpawner->spawnEnemy({800,500}, 0);
        }

        if(enemyPoolSpawner->checkCollisionWithPlayer())
        {
            playerCharacter->damage();
            basics::log.d(to_string(playerCharacter->life));
        }

        if(!shotReady)
        {
            shotCooldown -= time;

            if(shotCooldown <= 0)
            {
                shotCooldown = 0.8;
                shotReady = true;
            }
        }

        if(playerCharacter->position.coordinates.x() < 0)
        {
            playerCharacter->position.coordinates.x() = canvas_width - 1;
        }

        if(playerCharacter->position.coordinates.y() < 0)
        {
            playerCharacter->position.coordinates.y() = canvas_height - 1;
        }

        else if(playerCharacter->position.coordinates.x() > canvas_width)
        {
            playerCharacter->position.coordinates.x() = 1;
        }

        if(playerCharacter->position.coordinates.y() > canvas_height)
        {
            playerCharacter->position.coordinates.y() = 1;
        }

        if(playerCharacter->life <= 0)
        {
            gameOver();
        }
    }

    void Sample_Scene::gameOver()
    {
        state = GAME_OVER;
    }
}

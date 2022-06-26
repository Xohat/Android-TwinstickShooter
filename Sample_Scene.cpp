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
        //playerCharacter = new player{1};
        playerCharacter = new PlayerRelated::player();
    }

    bool Sample_Scene::initialize ()
    {
        state     = LOADING;
        suspended = false;
        x         = 640;
        y         = 360;

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
                        //basics::log.d("joystickMovement X: " + to_string(joystickMovement.coordinates.x())  + ",Y: " + to_string(joystickMovement.coordinates.y()));
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
                        playerCharacter->position = Vector2f (canvas_width/2, canvas_height/2);
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
                canvas->fill_rectangle({ 0, 0 }, { 1280, 720 });

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

                playerCharacter->position = {canvas_width/2, canvas_height/2};

                if(textureActivationCount == 2)
                {
                    state = RUNNING;
                }
            }
        }
    }

    void Sample_Scene::run (float time)
    {
        playerBulletPool->update(time);
        playerCharacter->update(time);

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

        /*
        if(playerCharacter->life > 0)
        {
            for (int i = 0; i < enemies.size(); ++i)
            {
                bool notCollision;

                if(!notCollision)
                {
                    playerCharacter->damage();
                    playerCharacter->invincibility(time);
                }
            }
        }

        if(playerCharacter->life <= 0)
        {
            gameOver();
        }
         */

        /*
        if(enemies[i].position.coordinates.x() < 0)
        {
            enemies[i].position[0] = canvas_width;
        }

        if(enemies[i].position.coordinates.y() < 0)
        {
            enemies[i].position[1] = canvas_height;
        }

        else if(enemies[i].position.coordinates.x() > canvas_width)
        {
            enemies[i].position[0] = 0;
        }

        if(enemies[i].position.coordinates.y() > canvas_height)
        {
            enemies[i].position[1] = 0;
        }
         */
    }

    void Sample_Scene::gameOver()
    {
        state = GAME_OVER;
    }
}

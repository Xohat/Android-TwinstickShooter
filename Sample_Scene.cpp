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

            switch (event.id)
            {
                case ID(touch-started):
                {
                    if(x > canvas_width * 0.5f)
                    {
                        shootCircle.Position = Point2f{x,y};
                        shootCircle.Pressed = true;
                        shootCircle.ID = id;
                        playerBulletPool->spawnBullet({10,50}, 90);
                    }

                    else
                    {
                        joystickHandler.Position = Vector2f{x,y};
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
                        //Vector2f playerMovement = Vector2f (joystickHandler.deltaPosition - joystickHandler.Position);
                        //playerCharacter.position += playerMovement;
                    }

                    break;
                }

                case ID(touch-ended):
                {
                    if(id == shootCircle.ID) shootCircle.Pressed = false; //else
                    if(id == joystickHandler.ID) joystickHandler.Pressed = false;

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
                canvas->set_color    (1, 1, 1);
                canvas->draw_point   ({ 360, 360 });
                canvas->draw_segment ({   0,   0 }, { 1280, 720 });
                canvas->draw_segment ({   0, 720 }, { 1280,   0 });

                if(shootCircle.Pressed)
                {
                    canvas->fill_rectangle(shootCircle.Position, { 100, 100 });
                }

                if(joystickHandler.Pressed)
                {
                    canvas->fill_rectangle(joystickHandler.Position, { 100, 100 });
                    canvas->fill_rectangle(joystickHandler.deltaPosition, { 100, 100 });
                    //if(joystickHandler.deltaPosition >)
                }

                playerBulletPool->render(*canvas);

                //playerCharacter.render(canvas);

                /*
                if(state == GAME_OVER)
                {
                    canvas->fill_rectangle ({ canvas_width / 2.f, canvas_height / 2.f }, { 100, 60 });
                }
                 */
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
                texture = Texture_2D::create (ID(test), context, "test.png");

                if (texture)
                {
                    context->add (texture);

                    state = RUNNING;
                }
            }
        }
    }

    void Sample_Scene::run (float time)
    {
        playerBulletPool->update(time);
    }

}

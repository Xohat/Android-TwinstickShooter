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
                    ///Si se toca por encima de la mitad de la pantalla se entra en la zona de disparo
                    if(x > canvas_width * 0.5f)
                    {
                        ///Si toca el circulo de disparo
                        if(x > shootCircle.Position.coordinates.x() && x < shootCircle.Position.coordinates.x() + shootCircle.Radius*2)
                        {
                            if(y > shootCircle.Position.coordinates.y() && y < shootCircle.Position.coordinates.y() + shootCircle.Radius*2)
                            {
                                shootCircle.Pressed = true;
                                shootCircle.ID = id;

                                ///Comienza el enfriamiento del disparo
                                if(shotReady)
                                {
                                    playerBulletPool->spawnBullet({playerCharacter->position.coordinates.x() + 50,playerCharacter->position.coordinates.y() + 100}, 90);
                                    shotReady = false;
                                }
                            }
                        }
                    }

                    ///Si no se activa el joystick de movimiento
                    else
                    {
                        joystickHandler.Position = Vector2f{x,y};
                        joystickHandler.deltaPosition = Vector2f{x,y};
                        joystickHandler.Pressed = true;
                        joystickHandler.ID = id;
                    }

                    break;
                }

                ///Al mover el touch, si se esta tocando el joystick, se hace un vector de movimiento que se le aplica al jugador por medio de direction
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

                ///Al acabar el touch elimina el circulo de joystick (Si se estaba tocando el joystick) y para el contacto con el circulo de contacto
                case ID(touch-ended):
                {
                    if(id == shootCircle.ID) shootCircle.Pressed = false;
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
                ///Colorea el canvas azul de fondo y rellena la pantalla con dicho cuadro azul
                canvas->clear        ();
                Vector3f backgroundColor(0, 0.19921875, 0.3984375);
                canvas->set_color    (backgroundColor[0], backgroundColor[1], backgroundColor[2]);
                canvas->fill_rectangle({ 0, 0 }, { canvas_width / 1.f, canvas_height / 1.f });

                ///Si el circulo de disparo esta presionado se colorea de gris para dar feedback
                if(shootCircle.Pressed)
                {
                    canvas->set_color    (0.5,0.5,0.5);
                    canvas->fill_rectangle(shootCircle.Position, { 100, 100 });
                }

                ///Si no el circulo de disparo es blanco
                else
                    {
                        canvas->set_color    (1, 1, 1);
                        canvas->fill_rectangle(shootCircle.Position, { 100, 100 });
                    }

                if(joystickHandler.Pressed)
                {
                    ///Settea el color del joystick a verde para mejor feedback visual
                    canvas->set_color    (0.296875,0.59765625,0);
                    canvas->fill_rectangle(joystickHandler.Position, { 100, 100 });

                    ///Capa el movimiento del joystick si se sale del raido externo
                    if(joystickHandler.deltaPosition.coordinates.x() < joystickHandler.Position.coordinates.x() + joystickHandler.OuterRadius &&
                       joystickHandler.deltaPosition.coordinates.y() < joystickHandler.Position.coordinates.y() + joystickHandler.OuterRadius)
                    {
                        canvas->fill_rectangle(joystickHandler.deltaPosition, { 100, 100 });
                    }

                    ///Si no se sale del radio externo la posicon del joystick arrastrado es la que s epinta
                    else
                        {
                            canvas->fill_rectangle(joystickHandler.deltaPosition, { 100, 100 });
                        }
                }

                ///Si no se presiona el joystick el color del canvas se pone a blanco
                else
                    {
                        canvas->set_color (1, 1, 1);
                    }

                ///Activo los métodos de render de cada clase
                playerBulletPool->render(*canvas);
                playerCharacter->render(*canvas);
                enemyPoolSpawner->render(*canvas);

                ///Si se acaba el juego se pone un rectangulo encima para pintar el final
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
                ///Comprobación de aspect ratio
                adjustAspectRatio(context);

                if(aspect_ratio_adjusted)
                {
                    shootCircle.Position = Vector2f (canvas_width - 150, 20);          ///< Se inicia la posición del circulo de disparo
                                                                                      ///< despues del aspect ratio para poder usar el canvas correcto
                    playerCharacter->position = {canvas_width/2, canvas_height/2};
                    state = RUNNING;
                }
            }
        }
    }

    void Sample_Scene::run (float time)
    {
        ///Se activan los métodos de update de todas las clases
        playerBulletPool->update(time);
        playerCharacter->update(time);
        enemyPoolSpawner->update(time);

        ///Código relacionado con el spawner
        timerSinceSpawn += time;

        if(timerSinceSpawn >= timerSpawn)
        {
            timerSinceSpawn = 0;
            enemyPoolSpawner->spawnEnemy({500,500}, 0);
            enemyPoolSpawner->spawnEnemy({200,500}, 0);
            enemyPoolSpawner->spawnEnemy({800,500}, 0);
        }

        ///Checker de colisiones
        if(enemyPoolSpawner->checkCollisionWithPlayer())
        {
            playerCharacter->damage();
            basics::log.d(to_string(playerCharacter->life));
        }

        ///Función encargada de poner el cooldown del disparo
        if(!shotReady)
        {
            shotCooldown -= time;

            if(shotCooldown <= 0)
            {
                shotCooldown = 0.25;
                shotReady = true;
            }
        }

        ///Ifs que capan la posición del player, para darle mas maniobarbilidad debido a la dificultad del juego
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

        ///Comprueba la vida para terminar el juego
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

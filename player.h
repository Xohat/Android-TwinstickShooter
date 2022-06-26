//
// Created by Game on 26/06/2022.
//

#ifndef BASICS_PROJECT_TEMPLATE_PLAYER_H
#define BASICS_PROJECT_TEMPLATE_PLAYER_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <basics/Atlas>
#include <basics/Canvas>
#include <basics/Scene>
#include <basics/Size>
#include <basics/Vector>
#include <basics/Timer>
#include <basics/Texture_2D>

namespace PlayerRelated
{
    using basics::Atlas;
    using basics::Canvas;
    using basics::Point2f;
    using basics::Size2f;
    using basics::Timer;
    using basics::Vector2f;
    using basics::Texture_2D;

    using std::map;
    using std::shared_ptr;
    using std::string;
    using std::vector;

    class player
    {
    private:

        shared_ptr<basics::Texture_2D> playerTexture;

        bool deathState;
        float invincibilityTime;
        bool pain;
        int speed;

    public:

        float width;
        float height;
        float rotation;
        Vector2f position;
        Vector2f direction;
        int life;

        player()
        {
            position = Vector2f (0,0);
            width = 100;
            height = 100;
            rotation = 0;
            life = 3;
            deathState = false;
            invincibilityTime = 5;
            pain = false;
            speed = 300;
            direction = Vector2f (0,0);
        }

        void update (float delta)
        {
            invincibility(delta);
            position += direction * delta * speed;
        }

        void render (Canvas & canvas)
        {
            Size2f size {width, height};
            canvas.set_color(1,1,1);
            canvas.fill_rectangle (position, size);
        }

        void damage()
        {
            if(pain == false)
            {
                pain = true;    ///< Activa el booleano para no recibir daño
                life -= 1;

                if (life == 0)
                {
                    deathState = true;
                }
            }
        }

        void invincibility(float delta)
        {
            if (pain == true) ///< Se comprueba el booleano
            {
                invincibilityTime -= delta;

                if(invincibilityTime <= 0)
                {
                    invincibilityTime = 4;
                    pain = false;
                }
            }
        }
    };
}

#endif //BASICS_PROJECT_TEMPLATE_PLAYER_H

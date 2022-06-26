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

        int width;
        int height;
        bool deathState;
        float invincibilityTime;
        bool pain;
        int speed;
        float shotCooldown;

    public:

        float rotation;
        Vector2f position;
        Vector2f direction;
        int life;

        player()
        {
            position = Vector2f (0,0);
            width = 0;
            height = 0;
            rotation = 0;
            life = 3;
            deathState = false;
            invincibilityTime = 5;
            pain = false;
            speed = 150;
            direction = Vector2f (0,0);
        }

        void update (float delta)
        {
            invincibility(delta);
            //direction = Vector2f(1, 0);
            position += direction * delta * speed;
        }

        void setTexture (shared_ptr< Texture_2D > & new_texture)
        {
            playerTexture = new_texture;
        }

        void render (Canvas & canvas)
        {
            //Size2f textureSize {playerTexture->get_width(), playerTexture->get_height()};
            Size2f textureSize {100, 100};
            canvas.fill_rectangle (position - Vector2f (textureSize[0]/2,textureSize[1]/2), textureSize);
            /*
            if(playerTexture)
            {

            }
             */
        }

        bool damage()
        {
            if(pain == false)
            {
                pain = true;
                life -= 1;

                if (life == 0)
                {
                    return deathState = true;
                }
            }
        }

        void invincibility(float delta)
        {
            if (pain == true)
            {
                float invTime;
                invTime -= delta;

                if(invTime <= 0)
                {
                    invTime = 4;
                    pain = false;
                }
            }
        }
    };
}

#endif //BASICS_PROJECT_TEMPLATE_PLAYER_H

/*
namespace PlayerData
{

};
 */
//
// Created by Game on 26/06/2022.
//

#include "player.h"
#include <basics/Application>
#include <basics/Director>
#include <basics/Canvas>
#include <basics/Log>
#include <basics/Window>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <basics/Texture_2D>

namespace Player
{
    player::player()
    {
        position = Vector2f (0,0);
        width = 0;
        height = 0;
        //halfWidth = playerTexture->get_width() / 2;
        //halfHeight = playerTexture->get_height() / 2;
        rotation = 0;
        life = 3;
        deathState = false;
        invincibilityTime = 5;
        pain = false;
        speed = 400;
        direction = Vector2f (0,0);
    }

    void player::setTexture (shared_ptr< Texture_2D > & new_texture)
    {
        playerTexture = new_texture;
    }

    void player::update(float delta)
    {
        invincibility(delta);
        position += direction;

        //shotCooldownAux += delta;
    }

    void player::render (Canvas * canvas)
    {
        //Size2f textureSize {playerTexture->get_width(), playerTexture->get_height()};
        Size2f textureSize {300, 300};
        canvas->fill_rectangle (position, textureSize);
        /*
        if(playerTexture)
        {

        }
         */
    }

    bool player::damage()
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

    void player::invincibility(float delta)
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
}
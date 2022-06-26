//
// Created by Game on 26/06/2022.
//

#include "bullet.h"
#include <basics/Application>
#include <basics/Director>
#include <basics/Canvas>
#include <basics/Log>
#include <basics/Window>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

namespace Proyectile
{
    bullet::bullet()
    {
        position = Vector2f (500,500);
        rotation = 0;
        speed = 0.5;
        dmgPower = 1;
        activeState = false;
        width = 10;
        height = 30;
    }

    void bullet::update(float delta)
    {
        if(activeState)
        {
            position.coordinates.y() += sin(rotation) * speed * delta;
        }
    }

    void bullet::render(Canvas & canvas)
    {
        Size2f textureSize {width, height};

        canvas.set_color(0.796875,0.796875,0);
        canvas.fill_rectangle (position, textureSize);
    }

    bulletPool::bulletPool(int initialSize, float canvas_width, float canvas_height)
    {
        this->canvas_width = canvas_width;
        this->canvas_height = canvas_height;

        for (std::size_t i = 0; i < initialSize; i++)
        {
            bullet bulletToStudy = Proyectile::bullet();
            bullets.push_back(bulletToStudy);
        }
    }

    void bulletPool::update(float delta)
    {
        for (int i = 0; i < bullets.size(); ++i)
        {
            if (bullets[i].activeState)
            {
                bullets[i].update(delta);

                if (bullets[i].position.coordinates.y() < 0 || bullets[i].position.coordinates.y() > canvas_height ||
                    bullets[i].position.coordinates.x() < 0 || bullets[i].position.coordinates.x() > canvas_width  )
                {
                    bullets[i].deactivate();
                }
            }
        }
    }

    void bulletPool::render(Canvas & canvas)
    {
        for (int i = 0; i < bullets.size(); ++i)
        {
            if(bullets[i].activeState)
            {
                bullets[i].render(canvas);
            }
        }
    }

    void bullet::activate()
    {
        activeState = true;
    }

    void bullet::deactivate()
    {
        activeState = false;
    }

    bullet & bulletPool::spawnBullet(Vector2f position, float rotation)
    {
        bullet *bulletToReturn;
        bool bulletFound = false;

        for (int i = 0; i < bullets.size() && !bulletFound; ++i)
        {
            if(!bullets[i].activeState)
            {
                bulletToReturn = &bullets[i];
                bulletFound = true;
            }
        }

        if(!bulletFound)
        {
            bullets.push_back(bullet{});
            bulletToReturn = &bullets[bullets.size()-1];
        }

        bulletToReturn->position = position;
        bulletToReturn->rotation = rotation;
        bulletToReturn->speed = 200;
        bulletToReturn->activate();

        return *bulletToReturn;
    }
}
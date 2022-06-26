//
// Created by Game on 26/06/2022.
//

#ifndef BASICS_PROJECT_TEMPLATE_ENEMY_H
#define BASICS_PROJECT_TEMPLATE_ENEMY_H

#include "player.h"
#include "bullet.h"
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <basics/Atlas>
#include <basics/Canvas>
#include <basics/Scene>
#include <basics/Size>
#include <basics/Log>
#include <basics/Vector>
#include <basics/Timer>
#include <basics/Texture_2D>

namespace Enemies
{
    using basics::Vector2f;
    using basics::Texture_2D;
    using basics::Canvas;
    using basics::Size2f;

    class enemy
    {
    public:

        std::shared_ptr<basics::Texture_2D> enemyTexture;
        Vector2f position;
        float rotation;
        int life;
        float width = 80;
        float height = 80;
        bool activeState;
        int dmgPower;
        int speed;
        Proyectile::bulletPool *activeBulletPool;
        PlayerRelated::player *playerCharacter;

    public:

        enemy(PlayerRelated::player *playerReference, Proyectile::bulletPool *bulletPoolReference)
        {
            position = Vector2f (500,500);
            rotation = 0;
            life = 3;
            activeState = false;
            speed = 50;
            playerCharacter = playerReference;
            activeBulletPool = bulletPoolReference;
        }

        virtual void update (float delta)
        {
            Proyectile::bullet* bulletToDelete = checkCollissionBullet();
            if(bulletToDelete != nullptr)
            {
                takeDamage();
                bulletToDelete->deactivate();
            }
        }

        virtual void render (Canvas & canvas)
        {
            canvas.set_color(0.796875,0,0);
            canvas.fill_rectangle(position, {width,height});
        }

        void takeDamage()
        {
            life -= 1;

            if(life <= 0)
            {
                activeState = false;
            }
        }

        Proyectile::bullet* checkCollissionBullet()
        {
            Proyectile::bullet* contactBullet = nullptr;
            bool contact = false;

            for (int i = 0; i < activeBulletPool->bullets.size() && !contact; ++i)
            {
                Proyectile::bullet *bulletToCheck = &activeBulletPool->bullets[i];

                if(bulletToCheck->activeState)
                {
                    contact = (bulletToCheck->position[0] + bulletToCheck->width) < (position[0]) ||
                              (bulletToCheck->position[1] + bulletToCheck->height) < (position[1]) ||
                              (position[0] + width) < (bulletToCheck->position[0]) ||
                              (position[1] + height) < (bulletToCheck->position[1]);

                    contact = !contact;

                    if(contact)
                    {
                        contactBullet = bulletToCheck;
                    }
                }
            }

            return contactBullet;
        }

        bool checkCollissionPlayer()
        {
            bool contact = (playerCharacter->position[0] + playerCharacter->width) < (position[0]) ||
                           (playerCharacter->position[1] + playerCharacter->height) < (position[1]) ||
                           (position[0] + width) < (playerCharacter->position[0]) ||
                           (position[1] + height) < (playerCharacter->position[1]);

            return !contact;
        }

        void activate()
        {
            activeState = true;
        }

        void deactivate()
        {
            activeState = false;
        }

    };

    class enemyFollow : public enemy
    {

    public:

        enemyFollow(PlayerRelated::player *playerReference, Proyectile::bulletPool *bulletPoolReference) : enemy(playerReference, bulletPoolReference)
        {

        }

        void update (float delta) override
        {
            Enemies::enemy::update(delta);

            // follow
            position.coordinates.x() += cos(rotation) * speed * delta;
            position.coordinates.y() += sin(rotation) * speed * delta;

            // rotation
            float incY = playerCharacter->position.coordinates.y() - position.coordinates.y();
            float incX = playerCharacter->position.coordinates.x() - position.coordinates.x();
            rotation = atan2(incY, incX);
        }
    };

    class enemyPool {
    public:

        Proyectile::bulletPool *bulletPoolReference;
        PlayerRelated::player *playerReference;
        std::vector<enemyFollow> enemies;
        float canvas_width;
        float canvas_height;

        enemyPool(int initialSize, float canvas_width, float canvas_height, PlayerRelated::player *playerReference, Proyectile::bulletPool *bulletPoolReference)
        {
            this->bulletPoolReference = bulletPoolReference;
            this->playerReference = playerReference;
            this->canvas_width = canvas_width;
            this->canvas_height = canvas_height;

            for (std::size_t i = 0; i < initialSize; i++)
            {
                enemyFollow enemyToStudy = Enemies::enemyFollow(playerReference, bulletPoolReference);
                enemies.push_back(enemyToStudy);
            }
        }

        void update (float delta)
        {
            for (int i = 0; i < enemies.size(); ++i)
            {
                if (enemies[i].activeState)
                {
                    enemies[i].update(delta);

                    if (enemies[i].position.coordinates.y() < 0 || enemies[i].position.coordinates.y() > canvas_height ||
                        enemies[i].position.coordinates.x() < 0 || enemies[i].position.coordinates.x() > canvas_width  )
                    {
                        enemies[i].deactivate();
                    }
                }
            }
        }

        void render (Canvas & canvas)
        {
            for (int i = 0; i < enemies.size(); ++i)
            {
                if(enemies[i].activeState)
                {
                    enemies[i].render(canvas);
                }
            }
        }

        enemy & spawnEnemy(Vector2f position, float rotation)
        {
            enemy *enemyToReturn;
            bool enemyFound = false;

            for (int i = 0; i < enemies.size() && !enemyFound; ++i)
            {
                if(!enemies[i].activeState)
                {
                    enemyToReturn = &enemies[i];
                    enemyFound = true;
                }
            }

            if(!enemyFound)
            {
                enemies.push_back(enemyFollow{playerReference, bulletPoolReference});
                enemyToReturn = &enemies[enemies.size()-1];
            }

            enemyToReturn->position = position;
            enemyToReturn->rotation = rotation;
            enemyToReturn->speed = 200;
            enemyToReturn->life = 3;
            enemyToReturn->activate();

            return *enemyToReturn;
        }

        bool checkCollisionWithPlayer()
        {
            bool colliding = false;

            for (int i = 0; i < enemies.size() && !colliding; ++i)
            {
                if(enemies[i].activeState)
                {
                    colliding = enemies[i].checkCollissionPlayer();
                }
            }

            return colliding;
        }
    };
};

#endif //BASICS_PROJECT_TEMPLATE_ENEMY_H

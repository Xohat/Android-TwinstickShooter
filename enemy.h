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

        enum typeOfEnemy
        {
            FOLLOW,
            KAMIKAZE,
        };

        typeOfEnemy enemyType;

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
                basics::log.d("BALA HIT");
                takeDamage();
                bulletToDelete->deactivate();
            }
        }

        virtual void render (Canvas & canvas)
        {
            /*
            Size2f textureSize {enemyTexture->get_width(), enemyTexture->get_height()};
            if(enemyTexture)
            {
                canvas->fill_rectangle (position, textureSize, enemyTexture);
            }
             */

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

        //Proyectile::bulletPool *bulletPoolToCheck
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

            //!contact;

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

        //void setTexture (std::shared_ptr< Texture_2D > & new_texture);
        void die(int numberToErase);

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

/*
 * namespace Enemies
{
    void enemy::setTexture (std::shared_ptr< Texture_2D > & new_texture)
    {
        enemyTexture = new_texture;
    }

    void enemy::render(Canvas * canvas)
    {
        Size2f textureSize {enemyTexture->get_width(), enemyTexture->get_height()};
        if(enemyTexture)
        {
            canvas->fill_rectangle (position, textureSize, enemyTexture);
        }
    }

    bool enemy::checkCollission(Proyectile::bullet bulletToHit)
    {
        for (int i = 0; i < enemies.size(); ++i)
        {
            for (int j = 0; j < activeBulletPool->size(); ++j)
            {
                bool collision;

                if(collision)
                {
                    if(enemies[i].life > 0)
                    {
                        takeDamage();
                    }

                    else
                    {
                        die(i);
                    }
                }
            }
        }
    }

    void enemy::die(int numberToErase)
    {
        enemies.erase(enemies, numberToErase);
    }

    bool enemy::takeDamage()
    {
        for (int i = 0; i < enemies.size(); ++i)
        {
            enemies[i].life -= 1;

            if(enemies[i].life <= 0)
            {
                return enemies[i].deathState = true;
            }
        }
    }

    void enemyFollow::update(float delta)
    {

    }

    void enemyKamikaze::update(float delta)
    {
        switch(currentState)
        {
            case looking:
                lookingState(delta);
                break;
            case launch:
                launchState(delta);
                break;
        }
    }

    void enemyKamikaze::lookingState(float delta)
    {
        waitTime -= delta;

        float incY = playerCharacter->position.coordinates.y() - position.coordinates.y();
        float incX = playerCharacter->position.coordinates.x() - position.coordinates.x();

        rotation = atan2(incX, incY);

        if(waitTime <= 0)
        {
            currentState = launch;
        }
    }

    void enemyKamikaze::launchState(float delta)
    {
        waitTime = 1;

        position.coordinates.x() += cos(rotation) * speed * delta;
        position.coordinates.y() += sin(rotation) * speed * delta;

        if(position.coordinates.y() - halfHeight < 0 || position.coordinates.y() + halfHeight > canvas_width||
           position.coordinates.x() - halfWidth < 0 || position.coordinates.x() + halfWidth > canvas_height)
        {
            currentState = looking;
        }
    }
}
 */

#endif //BASICS_PROJECT_TEMPLATE_ENEMY_H

//
// Created by Game on 26/06/2022.
//

#ifndef BASICS_PROJECT_TEMPLATE_BULLET_H
#define BASICS_PROJECT_TEMPLATE_BULLET_H

#include <map>
#include <memory>
#include <vector>
#include <basics/Graphics_Context>
#include <basics/Canvas>
#include <basics/Vector>
#include <basics/Texture_2D>

namespace Proyectile
{
    using basics::Vector2f;
    using basics::Size2f;
    using basics::Texture_2D;
    using basics::Canvas;

    class bullet
    {
    private:

    public:

        std::shared_ptr<basics::Texture_2D> bulletTexture;
        float speed;
        float dmgPower;
        bullet();
        bool activeState;
        Vector2f position;
        float rotation;

        void update (float delta);
        void setTexture (std::shared_ptr< Texture_2D > & new_texture);
        void render (Canvas & canvas);

        void activate();
        void deactivate();
    };

    class bulletPool
    {
    public:
        //bulletPool();
        bulletPool(int initialSize, float canvas_width, float canvas_height);
        std::vector<bullet> bullets;
        float canvas_width;
        float canvas_height;

        void update (float delta);
        void render (Canvas & canvas);
        bullet & spawnBullet(Vector2f position, float rotation);

    private:

    };

};

#endif //BASICS_PROJECT_TEMPLATE_BULLET_H
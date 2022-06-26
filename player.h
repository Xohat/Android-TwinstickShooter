//
// Created by Game on 26/06/2022.
//

#ifndef BASICS_PROJECT_TEMPLATE_PLAYER_H
#define BASICS_PROJECT_TEMPLATE_PLAYER_H

#endif //BASICS_PROJECT_TEMPLATE_PLAYER_H

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

namespace Player
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

        float rotation;
        int width;
        int height;
        bool deathState;
        float invincibilityTime;
        bool pain;
        int speed;
        Vector2f position;
        float shotCooldown;

    public:

        Vector2f direction;
        int life;

        player();
        void update (float delta);
        void setTexture (shared_ptr< Texture_2D > & new_texture);
        void render (Canvas * canvas);

        bool damage();
        void invincibility(float delta);
    };
};
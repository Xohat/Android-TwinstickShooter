/*
 * SAMPLE SCENE
 * Copyright © 2018+ Ángel Rodríguez Ballesteros
 *
 * Distributed under the Boost Software License, version  1.0
 * See documents/LICENSE.TXT or www.boost.org/LICENSE_1_0.txt
 *
 * angel.rodriguez@esne.edu
 */

#include "bullet.h"
#include "player.h"
#include "enemy.h"
#include <memory>
#include <basics/Scene>
#include <basics/Vector>
#include <basics/Texture_2D>

namespace example
{
    using basics::Point2f;
    using basics::Vector2f;

    class Sample_Scene : public basics::Scene
    {
        typedef std::shared_ptr< basics::Texture_2D > Texture_Handle;

    public:

        enum State
        {
            LOADING,
            RUNNING,
            GAME_OVER,
        };

        State          state;
        bool           suspended;

        unsigned       canvas_width;
        unsigned       canvas_height;

        Texture_Handle texture;
        Texture_Handle buttonTexture;
        Texture_Handle enemyTexture;
        Texture_Handle playerTexture;
        Texture_Handle bulletTexture;

        Enemies::enemyPool *enemyPoolSpawner;
        PlayerRelated::player *playerCharacter;
        Proyectile::bulletPool *playerBulletPool;
        bool shotReady = true;
        float shotCooldown = 0.1;
        float timerSpawn;
        float timerSinceSpawn;

        //std::vector<Enemies::enemy> enemies;

        struct shootButton
        {
            Point2f Position = {1100,20};
            bool Pressed;
            static const int Radius = 50;
            int ID;
        };

        struct joystick
        {
            Vector2f Position;
            bool Pressed;
            static const int InnerRadius = 10;
            static const int OuterRadius = 75;
            int ID;
            Vector2f deltaPosition;
        };

        shootButton shootCircle;
        joystick joystickHandler;
        float          x, y;

    public:

        Sample_Scene();

        basics::Size2u get_view_size () override
        {
            return { canvas_width, canvas_height };
        }

        bool initialize () override;
        void suspend    () override;
        void resume     () override;

        void handle     (basics::Event & event) override;
        void update     (float time) override;
        void render     (basics::Graphics_Context::Accessor & context) override;

    private:

        void load ();
        void run  (float time);
        void gameOver();

    };

}

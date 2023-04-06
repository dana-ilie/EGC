#pragma once

#include "components/simple_scene.h"
#include "components/text_renderer.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void DrawScene(glm::mat3 visMatrix, bool flip);
        void RespawnDuck();

     protected:
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float flying_angle;
        float flying_speed;

        glm::ivec2 resolution;
        bool dir;

        float body_width = 150;
        float body_height = 100;
        float head_radius = 40;
        float nose_width = 40;
        float nose_height = 20;
        float wings_speed = 1.5f;
        float wings_rotation;

        float number_of_lives;
        float number_of_bullets;
        
        float max_score;
        float current_score;
        float time_elapsed;
        bool duck_escaped;
        bool duck_dead;
        
        float start_pos_x;
        float start_pos_y;
        float hitbox_cx;
        float hitbox_cy;
        float hitbox_width;
        float hitbox_height;
        float delta_hx;
        float delta_hy;

        int generated_ducks;

        glm::mat3 visMatrix;
        gfxc::TextRenderer* tr;

        GLclampf clear_r;
        GLclampf clear_g;
        GLclampf clear_b;

        bool game_over;
        bool game_started;
        int current_level;
        bool flip_duck;

        std::vector<GLclampf> sky_r = {0.7, 0.9, 0.5};
        std::vector<GLclampf> sky_g = {0.9, 0.9, 0.9};
        std::vector<GLclampf> sky_b = {1, 1, 0.9};
        std::vector<std::string> grounds = {"groundA", "groundB", "groundC"};
        std::vector<std::string> bodies = {"bodyA", "bodyB", "bodyC"};
        std::vector<std::string> wings = {"wingA", "wingB", "wingC"};


        
    };
}   // namespace m1

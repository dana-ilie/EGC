#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/camera.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        struct ViewportArea
        {
            ViewportArea() : x(0), y(0), width(1), height(1) {}
            ViewportArea(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        Tema2();
        ~Tema2();

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
        void RenderScene();
        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
        void MyRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMatrix,  Texture2D* texture);
        void MyRenderScene(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
        void RenderTrees(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
        bool CarIsOnTrack(glm::vec3 position);
        void ComputeTrackPoints();


    protected:
        glm::mat4 modelMatrix;
        float translateX, translateY, translateZ;
        float scaleX, scaleY, scaleZ;
        float angularStepOX, angularStepOY, angularStepOZ;
        GLenum polygonMode;
        ViewportArea miniViewportArea;

        std::vector<std::vector<glm::vec3>> track_path;
        std::vector<glm::vec3> track_path2;
        std::vector<glm::vec3> track_path3;


        float height = 0.01;
        std::vector<glm::vec3> track_mid_points = {
        glm::vec3(16, height, 0),
        glm::vec3(15.5, height, 2),
        glm::vec3(14, height, 4.5),
        glm::vec3(10, height, 6),
        glm::vec3(7, height, 5),
        glm::vec3(4, height, 4.8),
        glm::vec3(0, height, 5),
        glm::vec3(-6, height, 7),
        glm::vec3(-10, height, 8),
        glm::vec3(-14, height, 7),
        glm::vec3(-16.9, height, 4),
        glm::vec3(-18, height, 0),
        glm::vec3(-16.9, height, -4),
        glm::vec3(-14, height, -7),
        glm::vec3(-10, height, -8),
        glm::vec3(-6, height, -7),
        glm::vec3(-4.5, height, -5.8),
        glm::vec3(0, height, -4),
        glm::vec3(4, height, -4),
        glm::vec3(7.5, height, -5),
        glm::vec3(10, height, -6),
        glm::vec3(14, height, -4),
        };

        std::vector<glm::vec3> track_points;
        std::vector<glm::vec3> track_in_points;
        std::vector<glm::vec3> track_out_points;

        mycamera::Camera mini_camera;
        glm::mat4 projectionMatrix_mini;

        mycamera::Camera camera;
        bool renderCameraTarget;
        float FoV = glm::radians(85.0f);
        glm::mat4 projectionMatrix;
        float dist_int;
        float dist_out;
        float track_width;
        float my_car_len;
        float my_car_height;
        float my_car_width;
        float my_car_cx;
        float my_car_cy;
        float my_car_cz;
        glm::vec3 my_car_center;
        float dist_to_car;
        float rotation_angle;
        float camera_cx;
        float camera_cy;
        float camera_cz;

        float angle = 0;

        std::vector<glm::vec3> enemy_pos = std::vector<glm::vec3>(3);
        std::vector<int> pos_index = std::vector<int>(3);
        std::vector<float> enemy_translate = std::vector<float>(3);
        glm::vec3 dir;
        std::vector<float> enemy_rotation = std::vector<float>(3);
        std::vector<glm::vec3> enemy_center = std::vector<glm::vec3>(3);

        float no_enemies;
        std::vector<float> enemy_speed = std::vector<float>(3);

        std::vector<glm::vec3> enemy_positions = {
            glm::vec3(0, 0, 0.4),
            glm::vec3(0, 0, -0.4),
        };

        std::unordered_map<std::string, Texture2D*> mapTextures;

        float enemy_radius;
        float my_car_radius;
    };
}   // namespace m1


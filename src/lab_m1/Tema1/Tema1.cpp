#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    current_level = 1;
    game_started = false;
    game_over = false;
    clear_r = sky_r[current_level - 1];
    clear_g = sky_g[current_level - 1];
    clear_b = sky_b[current_level - 1];


    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;
    delta_hx = 0;
    delta_hy = 0;

    start_pos_x = 400;
    start_pos_y = 400 ;
    hitbox_cx = start_pos_x - body_width / 2;
    hitbox_cy = start_pos_y;
    hitbox_height = body_height;
    hitbox_width = body_width + 2 * head_radius + nose_width;
    flip_duck = false;


    number_of_lives = 3;
    number_of_bullets = 3;
    max_score = 500;
    current_score = 0;
    generated_ducks = 0;


    // Initialize flying angle and speed
    flying_angle = M_PI / 6 ;
    flying_speed = 500;

    if (flying_angle > M_PI / 2) {
        hitbox_cx = start_pos_x - body_width / 2 - body_width;
        hitbox_cy = start_pos_y - body_height / 2;
        hitbox_height = body_height;
        hitbox_width = body_width + 2 * head_radius + nose_width;
        flip_duck = true;
    }

    time_elapsed = 0;
    duck_escaped = false;
    duck_dead = false;

    wings_rotation = 0;

    Mesh* bodyA = object2DCopy::CreateBody("bodyA", glm::vec3(0.4, 0.2, 0), body_width, body_height);
    AddMeshToList(bodyA);
    Mesh* bodyB = object2DCopy::CreateBody("bodyB", glm::vec3(0.3, 0.1, 0.7), body_width, body_height);
    AddMeshToList(bodyB);
    Mesh* bodyC = object2DCopy::CreateBody("bodyC", glm::vec3(0.8, 0.1, 0.1), body_width, body_height);
    AddMeshToList(bodyC);

    glm::vec3 head_position = glm::vec3(body_width, body_height / 2, 0);
    Mesh* head = object2DCopy::CreateCircle("head", head_position, head_radius, glm::vec3(0.1, 0.5, 0));
    AddMeshToList(head);

    glm::vec3 nose_position = head_position + glm::vec3(head_radius, -nose_height / 2, 0);
    Mesh* nose = object2DCopy::CeateNose("nose", glm::vec3(1, 0.8, 0), nose_position, nose_width, nose_height);
    AddMeshToList(nose);

    Mesh* wingA = object2DCopy::CreateWing("wingA", glm::vec3(0.4, 0.2, 0), glm::vec3(body_width / 2, body_height / 2, 0), 40, 80);
    AddMeshToList(wingA);
    Mesh* wingB = object2DCopy::CreateWing("wingB", glm::vec3(0.3, 0.1, 0.7), glm::vec3(body_width / 2, body_height / 2, 0), 40, 80);
    AddMeshToList(wingB);
    Mesh* wingC = object2DCopy::CreateWing("wingC", glm::vec3(0.8, 0.1, 0.1), glm::vec3(body_width / 2, body_height / 2, 0), 40, 80);
    AddMeshToList(wingC);

    Mesh* groundA= object2DCopy::CreateRectangle("groundA", glm::vec3(0, 0, 0), resolution.x, 300, glm::vec3(0.7, 0.8, 0.2), true);
    AddMeshToList(groundA);
    Mesh* groundB = object2DCopy::CreateRectangle("groundB", glm::vec3(0, 0, 0), resolution.x, 300, glm::vec3(0.3, 0.4, 0.9), true);
    AddMeshToList(groundB);
    Mesh* groundC = object2DCopy::CreateRectangle("groundC", glm::vec3(0, 0, 0), resolution.x, 300, glm::vec3(1, 0.9, 0.5), true);
    AddMeshToList(groundC);

    Mesh* life = object2DCopy::CreateCircle("life", glm::vec3(80, resolution.y - 80, 0), 40, glm::vec3(0.8, 0.1, 0));
    AddMeshToList(life);

    Mesh* bullet = object2DCopy::CreateRectangle("bullet", glm::vec3(60, resolution.y - 80 - 40 - 70 - 20, 0), 40, 70, glm::vec3(0.7, 0.4, 0.2), true);
    AddMeshToList(bullet);

    glm::vec3 score_pos = glm::vec3(resolution.x - max_score - 100, resolution.y - 140, 0);
    Mesh* score_frame = object2DCopy::CreateRectangle("score_frame", score_pos, max_score, 80, glm::vec3(1, 1, 1));
    AddMeshToList(score_frame);
    
    Mesh* score = object2DCopy::CreateRectangle("score", score_pos, 100, 80, glm::vec3(1, 0, 0.6), true);
    AddMeshToList(score);

    Mesh* hitbox = object2DCopy::CreateRectangle("hitbox", glm::vec3(hitbox_cx, hitbox_cy, 0), hitbox_width, hitbox_height, glm::vec3(0.1, 1, 0.1), true);
    AddMeshToList(hitbox);

    tr = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
    tr->Load(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf"), 42);
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(clear_r, clear_g, clear_b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
    resolution = window->GetResolution();
    visMatrix = glm::mat3(1);


    // if the number of lives is 0 => the game is over
    if (number_of_lives == 0) {
        // display game over ending screen
        tr->RenderText("Game Over", resolution.x / 4, resolution.y / 3, 5, glm::vec3(1, 1, 1));
        clear_r = 0;
        clear_g = 0;
        clear_b = 0;
        game_over = true;
    } else if (game_started == false) {
        tr->RenderText("Start Game", resolution.x / 4, resolution.y / 3, 5, glm::vec3(1, 1, 1));
        tr->RenderText("Press ENTER to start", resolution.x / 4 + resolution.x / 8, resolution.y / 2, 1, glm::vec3(1, 1, 1));
        clear_r = 0;
        clear_g = 0;
        clear_b = 0;
    } else {
        time_elapsed += deltaTimeSeconds;

        // if enough time has passed => the duck escapes
        if (time_elapsed >= 20) {
            number_of_lives--;
            time_elapsed = 0;
            flying_angle = M_PI / 2;
            duck_escaped = true;
        }

        // if the duck escapes
        if (duck_escaped && start_pos_y + translateY >= resolution.y) {
            // Respawn duck
            RespawnDuck();
            generated_ducks++;
            duck_escaped = false;
        }

        // if the duck is killed
        if (duck_dead && start_pos_y + translateY <= 0) {
            // respawn duck
            RespawnDuck();
            generated_ducks++;
            duck_dead = false;
            // increase score
            current_score++;
        }

        // no more bullets
        if (number_of_bullets == 0) {
            number_of_lives--;
            number_of_bullets = 3;
            // the duck escapes
            time_elapsed = 0;
            flying_angle = M_PI / 2;
            duck_escaped = true;
        }

        // if 5 ducks have been generated => increase the flying speed
        if (generated_ducks == 5) {
            flying_speed += flying_speed / 5;
            generated_ducks = 0;
            current_level++;
            current_score = 0;
        }

        // wings rotation
        if (dir == false) {
            wings_rotation += deltaTimeSeconds * wings_speed;
            if (wings_rotation > 0.3)
                dir = true;
        } else if (dir) {
            wings_rotation -= deltaTimeSeconds * wings_speed;
            if (wings_rotation < -0.7)
                dir = false;
        }

        // duck movement
        delta_hx = flying_speed * deltaTimeSeconds * cos(flying_angle);
        delta_hy = flying_speed * deltaTimeSeconds * sin(flying_angle);

        translateX += flying_speed * deltaTimeSeconds * cos(flying_angle);
        translateY += flying_speed * deltaTimeSeconds * sin(flying_angle);

        hitbox_cx += delta_hx;
        hitbox_cy += delta_hy;

        // duck walls collision
        if (translateY + start_pos_y + 110 >= resolution.y && duck_escaped == false && duck_dead == false) {
            flying_angle = 2 * M_PI - flying_angle;
        } else if (translateX + start_pos_x + 110 >= resolution.x) {
            flying_angle = M_PI - flying_angle;
            flip_duck = true;
            hitbox_cy -= hitbox_height;
            hitbox_cx -= hitbox_width - body_width / 2;
        } else if (translateY + start_pos_y  - 20 <= 0 && duck_escaped == false && duck_dead == false) {
            flying_angle = 2 * M_PI - flying_angle;
        } else if (translateX + start_pos_x  - 20 <= 0) {
            flying_angle = M_PI - flying_angle;
            flip_duck = false;
            hitbox_cx += hitbox_width - body_width / 2;
            hitbox_cy += hitbox_height;
        }

        visMatrix *= transform2D::Translate(translateX, translateY) * transform2D::Rotate(flying_angle);
        // draw the scene according to visMatrix
        DrawScene(visMatrix, flip_duck);
    }
}


void Tema1::FrameEnd()
{
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if(window->KeyHold(GLFW_KEY_ENTER) && game_started == false) {
        game_started = true;
        clear_r = sky_r[(current_level - 1) % 3];
        clear_g = sky_g[(current_level - 1) % 3];
        clear_b = sky_b[(current_level - 1) % 3];
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    glm::ivec2 resolution = window->GetResolution();
    int x = mouseX * 2;
    int y = resolution.y - mouseY * 2;

    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && game_over == false && game_started == true) {
        if (x >= hitbox_cx && x <= hitbox_cx + hitbox_width
            && y >= hitbox_cy && y <= hitbox_cy + hitbox_height) {
            duck_dead = true;
            flying_angle = - M_PI / 2;
        }
        number_of_bullets--;
    } else if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && game_over == true) {
        exit(0);
    } 
}


void Tema1::DrawScene(glm::mat3 visMatrix, bool flip_duck)
{   
    if (flip_duck == true) {
        visMatrix *= transform2D::Translate(0, body_height / 2)
                  * transform2D::Scale(1, -1)
                  * transform2D::Translate(0, -body_height / 2);
    }
    // current level
    tr->RenderText("Level " + std::to_string(current_level), resolution.x - max_score - 100, 160, 1, glm::vec3(1, 1, 1));

    // lives
    for (int i = 0; i < number_of_lives; i++) {
        modelMatrix = glm::mat3(1) * transform2D::Translate(100 * i, 0);
        RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
    }

    // bullets
    for (int i = 0; i < number_of_bullets; i++) {
        modelMatrix = glm::mat3(1) * transform2D::Translate(100 * i, 0);
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
    }
    
    // score
    modelMatrix = glm::mat3(1);
    RenderMesh2D(meshes["score_frame"], shaders["VertexColor"], modelMatrix);

    for (int i = 0; i < current_score; i++) {
        modelMatrix = glm::mat3(1) * transform2D::Translate(100 * i, 0);
        RenderMesh2D(meshes["score"], shaders["VertexColor"], modelMatrix);
    }

    // ground
    modelMatrix = glm::mat3(1);
    RenderMesh2D(meshes[grounds[(current_level - 1) % 3]], shaders["VertexColor"], modelMatrix);

    // head
    modelMatrix = glm::mat3(1) * transform2D::Translate(start_pos_x, start_pos_y);
    modelMatrix *= visMatrix;
    RenderMesh2D(meshes["head"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1) * transform2D::Translate(start_pos_x, start_pos_y);
    modelMatrix *= visMatrix;
    RenderMesh2D(meshes["nose"], shaders["VertexColor"], modelMatrix);
    
    // body
    modelMatrix = glm::mat3(1) * transform2D::Translate(start_pos_x, start_pos_y);
    modelMatrix *= visMatrix;
    RenderMesh2D(meshes[bodies[(current_level - 1) % 3]], shaders["VertexColor"], modelMatrix);

    // wings
    modelMatrix = glm::mat3(1) * transform2D::Translate(start_pos_x, start_pos_y);
    modelMatrix *= visMatrix * transform2D::Translate(body_width / 2, body_height / 2)
                * transform2D::Rotate(wings_rotation)
                * transform2D::Translate(-body_width / 2, - body_height / 2);
    RenderMesh2D(meshes[wings[(current_level - 1) % 3]], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1) * transform2D::Translate(start_pos_x, start_pos_y);
    modelMatrix *= visMatrix * transform2D::Translate(body_width / 2, body_height / 2)
                * transform2D::Rotate(-wings_rotation)
                * transform2D::Translate(-body_width / 2, - body_height / 2);
    modelMatrix *= transform2D::Translate(0, body_height / 2)
                * transform2D::Scale(1, -1)
                * transform2D::Translate(0, -body_height / 2);   
    RenderMesh2D(meshes[wings[(current_level - 1) % 3]], shaders["VertexColor"], modelMatrix);
}


void Tema1::RespawnDuck()
{
    translateX = 0;
    translateY = 0;
    delta_hx = 0;
    delta_hy = 0;
    flying_angle = M_PI / 6;
    time_elapsed = 0;
    flip_duck = false;
    hitbox_cx = start_pos_x - body_width / 2;
    hitbox_cy = start_pos_y;
    number_of_bullets = 3;
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}

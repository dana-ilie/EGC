#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <cmath>


#include "lab_m1/Tema2/transform3D.h"
#include "lab_m1/Tema2/object3D.h"


using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = false;
    polygonMode = GL_FILL;

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Initialize tx, ty and tz (the translation steps)
    translateX = 0;
    translateY = 0;
    translateZ = 0;

    // Sets the resolution of the small viewport
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);

    // track
    {
        glm::vec3 track_color = glm::vec3(0.4, 0.4, 0.5);
        dist_int = 0.8;
        dist_out = 0.8;
        track_width = dist_int + dist_out;

        Mesh* track = object3D::CreateRaceTrack("track", track_color, dist_int, dist_out, track_mid_points);
        AddMeshToList(track);

        ComputeTrackPoints();
        track_path.push_back(track_mid_points);
        track_path.push_back(track_path2);
        track_path.push_back(track_path3);

        Mesh* trackLine = object3D::CreateDottedLine("trackLine");
        AddMeshToList(trackLine);
    }

    // Tree
    {
        glm::vec3 tree_base_color = glm::vec3(0.5, 0.3, 0.1);
        glm::vec3 tree_crown_color = glm::vec3(0.5, 0.6, 0.1);
        glm::vec3 tree_crown_color2 = glm::vec3(0.7, 0.5, 0);
        glm::vec3 tree_crown_color3 = glm::vec3(0.7, 1, 0.3);

        Mesh* tree_base = object3D::CreateRectangle("tree_base", tree_base_color, 0.4, 0.8, 0.4, glm::vec3(0, 0, 0));
        AddMeshToList(tree_base);
        Mesh* tree_crown = object3D::CreateRectangle("tree_crown", tree_crown_color, 1, 1, 1, glm::vec3(-0.3, 0.8, -0.3));
        AddMeshToList(tree_crown);
        Mesh* tree_crown2 = object3D::CreateRectangle("tree_crown2", tree_crown_color2, 1, 1, 1, glm::vec3(-0.3, 0.8, -0.3));
        AddMeshToList(tree_crown2);
        Mesh* tree_crown3 = object3D::CreateRectangle("tree_crown3", tree_crown_color3, 1, 1, 1, glm::vec3(-0.3, 0.8, -0.3));
        AddMeshToList(tree_crown3);
    }

    // my car
    {
        glm::vec3 car_color = glm::vec3(1, 0, 0);
        glm::vec3 my_car_lbc = glm::vec3(0, 0, 4.5);
        my_car_width = track_width / 8;
        my_car_height = my_car_width;
        my_car_len = 2 * my_car_width;
        my_car_cx = my_car_lbc.x + my_car_len / 2;
        my_car_cy = my_car_lbc.y + my_car_height / 2;
        my_car_cz = my_car_lbc.z + my_car_width / 2;
        my_car_center = glm::vec3(my_car_cx, my_car_cy, my_car_cz);
        my_car_radius = my_car_len / 2;
        glm::vec3 my_car_pos = glm::vec3(my_car_cx, my_car_cy, my_car_cz);
        dist_to_car = 0.7;
        rotation_angle = 0;


        Mesh* my_car = object3D::CreateCar("mycar", car_color, my_car_len, my_car_height, my_car_width, glm::vec3(0,0,-0.1));
        AddMeshToList(my_car);
    }

    // enemy car
    no_enemies = 3;

    glm::vec3 enemy_color = glm::vec3(0.6, 0, 0.4);
    
    enemy_radius = my_car_len / 2;
    for (int i = 0; i < no_enemies; i++) {
        enemy_pos[i] = track_path[i][0];
        enemy_center[i] = glm::vec3(enemy_pos[i].x + my_car_len / 2, enemy_pos[i].y, enemy_pos[i].z + my_car_width / 2);
        enemy_translate[i] = 0;
        enemy_rotation[i] = 0;
        pos_index[i] = 0;
    }

    enemy_speed[0] = 3.0f;
    enemy_speed[1] = 2.0f;
    enemy_speed[2] = 1.0f;

    dir = glm::vec3(0, 0, 0);
    Mesh* enemy_car = object3D::CreateCar("enemycar", enemy_color, my_car_len, my_car_height, my_car_width, glm::vec3(0,0,0));
    AddMeshToList(enemy_car);


    // ground
    Mesh* ground = object3D::CreateGround("ground", glm::vec3(0.6, 0.7, 0.2));
    AddMeshToList(ground);

    // camera
    {
        camera = mycamera::Camera();
        camera_cx = 0;
        camera_cy = 0.2;
        camera_cz = 4.5;
        glm::vec3 camera_pos = glm::vec3(camera_cx - 0.3, camera_cy + 0.2, camera_cz);
        //glm::vec3 camera_pos = my_car_pos - glm::vec3(dist_to_car, -2 * my_car_height, 0);
        camera.Set(camera_pos, glm::vec3(camera_cx, camera_cy, camera_cz), glm::vec3(0, 1, 0));
        //camera.Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
        camera.SetDistToTarget(dist_to_car);
    

        // minimap camera
        mini_camera = mycamera::Camera();
        glm::vec3 mini_cam_pos = glm::vec3(camera_cx, camera_cy + 1, camera_cz);
        //mini_camera.Set(mini_cam_pos, glm::vec3(camera_cx, camera_cy, camera_cz), glm::vec3(1, 0, 0));
        mini_camera.Set(camera_pos, glm::vec3(camera_cx, camera_cy, camera_cz), glm::vec3(0, 1, 0));
        mini_camera.RotateThirdPerson_OX(-1.3f);
    }

    projectionMatrix = glm::perspective(FoV, window->props.aspectRatio, 0.01f, 100.0f);
    projectionMatrix_mini = glm::ortho(-2.7f, 2.7f, -2.5f, 2.5f, 0.01f, 100.0f);

    {
        Shader* shader = new Shader("CurveShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "textures", "grass.jpg").c_str(), GL_REPEAT);
        mapTextures["grass"] = texture;
    }
}



void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.6, 0.8, 0.9, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::MyRenderScene(glm::mat4 viewMatrix, glm::mat4 projectionMat)
{
    RenderTrees(viewMatrix, projectionMat);

    for (int i = 0; i < no_enemies; i++) {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, enemy_center[i]);
        //modelMatrix *= transform3D::Translate(enemy_pos[i].x, enemy_pos[i].y, enemy_pos[i].z);
        modelMatrix *= transform3D::RotateOY(enemy_rotation[i]);
        MyRenderMesh(meshes["enemycar"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);
    }
    
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, camera.GetTargetPosition());
    modelMatrix *= transform3D::RotateOY(rotation_angle);
    MyRenderMesh(meshes["mycar"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);

    modelMatrix = glm::mat4(1);
    MyRenderMesh(meshes["track"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);
    modelMatrix = glm::mat4(1);
    MyRenderMesh(meshes["trackLine"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);

    modelMatrix = glm::mat4(1);
    MyRenderMesh(meshes["ground"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, mapTextures["grass"]);

}

void Tema2::RenderTrees(glm::mat4 viewMatrix, glm::mat4 projectionMat)
{
    for (int i = 0; i < track_in_points.size(); i++) {
        float translateZ = track_in_points[i].z + 0.2;
        float translateX = track_in_points[i].x;
        if (translateZ < 0) {
            translateZ -= 0.9;
        }
        if (translateX < 0) {
            translateX -= 0.4;
        }
        if (i % 3 == 0) {
            modelMatrix = glm::mat4(1) * transform3D::Translate(translateX, track_in_points[i].y, translateZ);
            MyRenderMesh(meshes["tree_crown"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);
        } else if (i % 3 == 1) {
            modelMatrix = glm::mat4(1) * transform3D::Translate(translateX, track_in_points[i].y, translateZ);
            MyRenderMesh(meshes["tree_crown2"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);
        } else {
            modelMatrix = glm::mat4(1) * transform3D::Translate(translateX, track_in_points[i].y, translateZ);
            MyRenderMesh(meshes["tree_crown3"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);
        }
        modelMatrix = glm::mat4(1) * transform3D::Translate(translateX, track_in_points[i].y, translateZ);
        MyRenderMesh(meshes["tree_base"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);
    }

    for (int i = 1; i < track_out_points.size(); i++) {
        float translateZ = track_out_points[i].z;
        float translateX = track_out_points[i].x;
        if (translateZ < 0) {
            translateZ += 0.7;
        }
        else {
            translateZ -= 0.8;
        }
        if (translateX < 0) {
            translateX += 0.4;
        }
        else {
            translateX -= 0.4;
        }
        if (i % 3 == 0) {
            modelMatrix = glm::mat4(1) * transform3D::Translate(translateX, track_in_points[i].y, translateZ);
            MyRenderMesh(meshes["tree_crown"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);
        } else if (i % 3 == 1) {
            modelMatrix = glm::mat4(1) * transform3D::Translate(translateX, track_in_points[i].y, translateZ);
            MyRenderMesh(meshes["tree_crown2"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);
        } else {
            modelMatrix = glm::mat4(1) * transform3D::Translate(translateX, track_in_points[i].y, translateZ);
            MyRenderMesh(meshes["tree_crown3"], shaders["CurveShader"], modelMatrix, viewMatrix, projectionMat, NULL);
        }
        modelMatrix = glm::mat4(1) * transform3D::Translate(translateX, track_out_points[i].y, translateZ);
        RenderMesh(meshes["tree_base"], shaders["CurveShader"], modelMatrix);
    }
}

void Tema2::ComputeTrackPoints()
{
    track_mid_points.push_back(track_mid_points[0]);

    float dist_p2 = 0.4;
    float dist_p3 = 0.4;

    glm::vec3 UP = glm::vec3(0, 1, 0);
    for (int i = 0; i < track_mid_points.size() - 1; i++) {
        glm::vec3 P1 = track_mid_points[i];
        glm::vec3 P2 = track_mid_points[i + 1];
        glm::vec3 D = glm::normalize(P2 - P1);
        glm::vec3 P = glm::cross(D, UP);
        glm::vec3 OUT = P1 + dist_out * P;
        glm::vec3 IN = P1 - dist_int * P;
        track_out_points.push_back(OUT);
        track_in_points.push_back(IN);
        
        glm::vec3 path2 = P1 + dist_p2 * P;
        glm::vec3 path3 = P2 - dist_p3 * P;
        track_path2.push_back(path2);
        track_path3.push_back(path3);
    }

    for (int i = 0; i < track_mid_points.size() - 1; i++) {
        track_points.push_back(track_out_points[i]);
        track_points.push_back(track_in_points[i]);
    }
}

float findArea(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    float ax = a.x;
    float az = a.z;

    float bx = b.x;
    float bz = b.z;

    float cx = c.x;
    float cz = c.z;

    float A = sqrt((double)(bx - ax) * (bx - ax) + (bz - az) * (bz - az));

    float B = sqrt((double)(bx - cx) * (bx - cx) + (bz - cz) * (bz - cz));

    float C = sqrt((double)(ax - cx) * (ax - cx) + (az - cz) * (az - cz));

    float s = (A + B + C) / 2;

    float area = sqrt(s * (s - A) * (s - B) * (s - C));

    return area;
}

bool Tema2::CarIsOnTrack(glm::vec3 position)
{
    bool is_on_track = false;
    float error = 0.1;

    for (int i = 0; i <= track_points.size() - 4; i += 2) {
        // first triangle
        glm::vec3 p1t1 = track_points[i];
        glm::vec3 p2t1 = track_points[i + 1];
        glm::vec3 p3t1 = track_points[i + 3];

        float area_t1 = findArea(p1t1, p2t1, p3t1);
        float small_area_t1_1 = findArea(p1t1, p2t1, position);
        float small_area_t1_2 = findArea(p1t1, position, p3t1);
        float small_area_t1_3 = findArea(position, p2t1, p3t1);

        if (small_area_t1_1 + small_area_t1_2 + small_area_t1_3 <= area_t1 + error &&
            small_area_t1_1 + small_area_t1_2 + small_area_t1_3 >= area_t1 - error) {
            is_on_track = true;
        }

        // second triangle
        glm::vec3 p1t2 = track_points[i];
        glm::vec3 p2t2 = track_points[i + 3];
        glm::vec3 p3t2 = track_points[i + 2];

        float area_t2 = findArea(p1t2, p2t2, p3t2);
        float small_area1 = findArea(p1t2, p2t2, position);
        float small_area2 = findArea(p1t2, position, p3t2);
        float small_area3 = findArea(position, p2t2, p3t2);

        if (small_area1 + small_area2 + small_area3 <= area_t2 + error &&
            small_area1 + small_area2 + small_area3 >= area_t2 - error) {
            is_on_track = true;
        }
    }

    // check for the last triangles
    int size = track_points.size();
    glm::vec3 p1t1 = track_points[size - 2];
    glm::vec3 p2t1 = track_points[size - 1];
    glm::vec3 p3t1 = track_points[1];

    float area_t1 = findArea(p1t1, p2t1, p3t1);
    float small_area_t1_1 = findArea(p1t1, p2t1, position);
    float small_area_t1_2 = findArea(p1t1, position, p3t1);
    float small_area_t1_3 = findArea(position, p2t1, p3t1);

    if (small_area_t1_1 + small_area_t1_2 + small_area_t1_3 <= area_t1 + error &&
        small_area_t1_1 + small_area_t1_2 + small_area_t1_3 >= area_t1 - error) {
        is_on_track = true;
    }

    // second triangle
    glm::vec3 p1t2 = track_points[size - 2];
    glm::vec3 p2t2 = track_points[1];
    glm::vec3 p3t2 = track_points[0];

    float area_t2 = findArea(p1t2, p2t2, p3t2);
    float small_area1 = findArea(p1t2, p2t2, position);
    float small_area2 = findArea(p1t2, position, p3t2);
    float small_area3 = findArea(position, p2t2, p3t2);

    if (small_area1 + small_area2 + small_area3 <= area_t2 + error &&
        small_area1 + small_area2 + small_area3 >= area_t2 - error) {
        is_on_track = true;
    }

    return is_on_track;
}

void Tema2::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

   
    for (int i = 0; i < no_enemies; i++) {    
        if (pos_index[i] == track_path[i].size() - 1) {
            glm::vec3 p1 = track_path[i][pos_index[i]];
            glm::vec3 p2 = track_path[i][0];

            float dist = distance(p2, p1);
            dir = glm::normalize(p2 - p1);

            enemy_translate[i] += enemy_speed[i] * deltaTimeSeconds;
            enemy_center[i] = track_path[i][pos_index[i]] + dir * enemy_translate[i];

            glm::vec3 OX = glm::normalize(glm::vec3(1, 0, 0));
            float dotProd = glm::dot(OX, dir);
            enemy_rotation[i] = acos(dotProd);

            if (enemy_translate[i] >= dist) {
                enemy_center[i] = track_path[i][0];

                pos_index[i] = 0;
                enemy_translate[i] = 0;

            }
        } else {
            glm::vec3 p1 = track_path[i][pos_index[i]];
            glm::vec3 p2 = track_path[i][pos_index[i] + 1];

            float dist = distance(p2, p1);
            dir = glm::normalize(p2 - p1);

            enemy_translate[i] += enemy_speed[i] * deltaTimeSeconds;
            enemy_center[i] = track_path[i][pos_index[i]] + dir * enemy_translate[i];

            glm::vec3 OX = glm::normalize(glm::vec3(1, 0, 0));
            float dotProd = glm::dot(OX, dir);
            enemy_rotation[i] = acos(dotProd);

            if (enemy_translate[i] >= dist) {
                pos_index[i] += 1;
                enemy_translate[i] = 0;
            }
        }
    }
    
    MyRenderScene(camera.GetViewMatrix(), projectionMatrix);

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    MyRenderScene(mini_camera.GetViewMatrix(), projectionMatrix_mini);
}

bool CheckCollision(glm::vec3 A, float RA, glm::vec3 B, float RB)
{
    float distance = sqrt(pow((A.x - B.x), 2) + pow((A.y - B.y), 2) + pow((A.z - B.z), 2));

    return distance < (RA + RB);
}

void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera.GetViewMatrix(), projectionMatrix);
}

void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void::Tema2::MyRenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::mat4& viewMatrix, glm::mat4& projectionMat,  Texture2D* texture)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMat));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    int location = glGetUniformLocation(shader->program, "car_pos");
    glUniform3fv(location, 1, glm::value_ptr(camera.GetTargetPosition()));

    if (texture)
    {
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());

        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
        glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 1);
    } 
    else 
    {
        glUniform1i(glGetUniformLocation(shader->program, "has_texture"), 0);
    }

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 2.0f * deltaTime;
    float speed = 2.0f;

    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        if (window->KeyHold(GLFW_KEY_W)) {
            camera.TranslateForward(cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            camera.TranslateRight(-cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            camera.TranslateForward(-cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            camera.TranslateRight(cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            camera.TranslateUpward(-cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            camera.TranslateUpward(cameraSpeed);
        }
    }

    bool collision;
    for (int i = 0; i < no_enemies; i++) {
        collision = CheckCollision(my_car_center, my_car_radius, enemy_center[i], enemy_radius);
        if (collision == true) {
            break;
        }
    }

    if (window->KeyHold(GLFW_KEY_W) &&  collision == false) {
        glm::vec3 next_pos = camera.GetNextPos(speed * deltaTime);

        if (CarIsOnTrack(next_pos)) {
            glm::vec3 forward = camera.GetCameraForward();
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            my_car_center += dir * speed * deltaTime;
            camera.MoveForward(speed * deltaTime);
            mini_camera.MoveForwardWithAngle(speed * deltaTime, M_PI - angle);
        }
    }
    if (window->KeyHold(GLFW_KEY_S) && collision == false) {
        glm::vec3 next_pos = camera.GetNextPos(speed * deltaTime);

        if (CarIsOnTrack(next_pos)) {
            glm::vec3 forward = camera.GetCameraForward();
            glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
            my_car_center += dir * (-speed) * deltaTime;
            camera.MoveForward(-speed * deltaTime);
            mini_camera.MoveForwardWithAngle(-speed * deltaTime, M_PI - angle);
        }
    }
    if (window->KeyHold(GLFW_KEY_A) && collision == false) {
        rotation_angle += deltaTime;
        camera.RotateThirdPerson_OY(deltaTime);
        angle += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_D) && collision == false) {
        rotation_angle -= deltaTime;
        camera.RotateThirdPerson_OY(-deltaTime);
        angle -= deltaTime;
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }

    if (window->KeyHold(GLFW_KEY_I)) {
        miniViewportArea.x += 10;

    }
    if (window->KeyHold(GLFW_KEY_J)) {
        miniViewportArea.x -= 10;

    }
    if (window->KeyHold(GLFW_KEY_K)) {
        miniViewportArea.y += 10;
    }
    if (window->KeyHold(GLFW_KEY_L)) {
        miniViewportArea.y -= 10;
    }
    if (window->KeyHold(GLFW_KEY_U)) {
        miniViewportArea.width += 10;
        miniViewportArea.height += 10;
    }
    if (window->KeyHold(GLFW_KEY_O)) {
        miniViewportArea.height -= 10;
        miniViewportArea.width -= 10;
    }

    if (key == GLFW_KEY_T)
    {
       renderCameraTarget = !renderCameraTarget;
    }
}

void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            camera.RotateFirstPerson_OY(-deltaX * sensivityOX);
            camera.RotateFirstPerson_OX(-deltaY * sensivityOY);

        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            camera.RotateThirdPerson_OX(-sensivityOX * deltaY);
            camera.RotateThirdPerson_OY(-sensivityOY * deltaX);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

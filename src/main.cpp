#include "lab_m1/lab1/lab1.h"
#include "lab_m1/lab2/lab2.h"
#include "lab_m1/lab3/lab3.h"
#include "lab_m1/lab3/lab3_vis2D.h"
#include "lab_m1/lab4/lab4.h"
#include "lab_m1/lab5/lab5.h"
#include "lab_m1/lab6/lab6.h"
#include "lab_m1/lab7/lab7.h"
#include "lab_m1/lab8/lab8.h"
#include "lab_m1/lab9/lab9.h"
#include "lab_m1/Tema1/Tema1.h"
#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"
#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/transform3D.h"
#include "lab_m1/Tema2/object3D.h"
#include "lab_m1/Tema2/camera.h"

#include <ctime>
#include <iostream>

#include "core/engine.h"
#include "components/simple_scene.h"

#if defined(WITH_LAB_M1)
#   include "lab_m1/lab_list.h"
#endif

#if defined(WITH_LAB_M2)
#   include "lab_m2/lab_list.h"
#endif

#if defined(WITH_LAB_EXTRA)
#   include "lab_extra/lab_list.h"
#endif


#ifdef _WIN32
    PREFER_DISCRETE_GPU_NVIDIA;
    PREFER_DISCRETE_GPU_AMD;
#endif


std::string GetParentDir(const std::string &filePath)
{
    size_t pos = filePath.find_last_of("\\/");
    return (std::string::npos == pos) ? "." : filePath.substr(0, pos);
}


int main(int argc, char **argv)
{
    srand((unsigned int)time(NULL));

    // Create a window property structure
    WindowProperties wp;
    wp.resolution = glm::ivec2(1280, 720);
    wp.vSync = true;
    wp.selfDir = GetParentDir(std::string(argv[0]));

    // Init the Engine and create a new window with the defined properties
    (void)Engine::Init(wp);

    // Create a new 3D world and start running it
    World *world = new m1::Tema2();

    world->Init();
    world->Run();

    // Signals to the Engine to release the OpenGL context
    Engine::Exit();

    return 0;
}

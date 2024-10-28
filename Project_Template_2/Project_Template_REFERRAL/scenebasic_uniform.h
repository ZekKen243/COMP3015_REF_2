#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"
#include "helper/torus.h"
#include "helper/plane.h"
#include "helper/teapot.h"
#include "helper/objmesh.h"
#include "helper/cube.h"
#include "helper/skybox.h"
#include "Camera.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SceneBasic_Uniform : public Scene
{
private:
    //Torus torus;
    //Plane plane;
    //Teapot teapot;
    //Cube cube;

    SkyBox sky;
    float rotSpeed;
    std::unique_ptr<ObjMesh> sun;
    GLuint sunTexture;
    float tPrev;
    float angle;
    glm::mat4 view2;

    // Camera stuff
    float lastTime;
    Camera camera;
    GLSLProgram prog;
    GLSLProgram skyboxProg;

    void setMatrices();
    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H

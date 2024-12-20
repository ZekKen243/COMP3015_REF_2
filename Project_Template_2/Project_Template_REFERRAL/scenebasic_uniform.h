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
    std::unique_ptr<ObjMesh> sphere;
    GLuint sunTexture, planetTexture, moonTexture, cloudTexture;

    float rotationSpeed = 0.35f;
    float diagonalAngle;
    glm::mat4 skyboxRotationMatrix;

    // Planet stuff
    float planetAngle = 0.0f; 
    float planetRotSpeed = glm::radians(150.0f);
    float orbitAngle = 0.0f;      
    float orbitSpeed = glm::radians(10.0f);

    bool rotatePlanets = false;
    bool oKeyPressedLastFrame = false;

    // Moon stuff
    float moonAngle, moonOrbitAngle;
    float moonRotSpeed = glm::radians(70.0f); 
    float moonOrbitSpeed = glm::radians(77.0f);

    float tPrev;
    float angle;
    glm::mat4 view2;

    // Camera stuff
    float lastTime;
    Camera camera;
    GLSLProgram prog;
    GLSLProgram skyboxProg;

    GLuint blurFBO[2];        
    GLuint blurTexture[2];    

    GLSLProgram horizontalBlurShader; 
    GLSLProgram verticalBlurShader;

    GLuint quadVAO = 0, quadVBO;
    void renderQuad();

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

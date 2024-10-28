#include "scenebasic_uniform.h"
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

glm::mat4 rotationMatrix;

SceneBasic_Uniform::SceneBasic_Uniform() : 
    tPrev(0),
    angle(90.0f),
    rotSpeed(glm::pi<float>()/8.0f), 
    //torus(1.75f * 0.75f, 0.75f * 0.75f, 50, 50), 
    //plane(50.0f, 50.0f, 10.0f, 1, 1),
    //teapot(14, glm::mat4(1.0f)),
    camera(glm::vec3(0.0f, 0.0f, 5.0f), -90.0f, 0.0f),
    sky(100.0f),
    lastTime(0.0f) {

    sphere = ObjMesh::load("media/sphere.obj", false, true);

}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);

    // Initialise move speed and mouse sens
    camera.setMovementSpeed(10.0f);     
    camera.setMouseSensitivity(0.2f);

    //model = mat4(1.0f); // Initialise the model
    //view = glm::lookAt(vec3(1.0f, 1.25f, 1.25f), vec3(0.0f, 0.2f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    projection = mat4(1.0f);

    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/nebula_hdr/neb");
    sunTexture = Texture::loadTexture("media/texture/sun.jpg");
    planetTexture = Texture::loadTexture("media/texture/makemake.jpg");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, planetTexture);

    // Set light properties in world space, defining its position at the sun's location
    prog.use();
    prog.setUniform("light.Position", glm::vec3(0.0f, 0.0f, 5.0f)); // World-space position of the sun
    prog.setUniform("light.Ld", glm::vec3(1.0f, 0.9f, 0.8f));       // Diffuse intensity
    prog.setUniform("light.La", glm::vec3(0.3f, 0.3f, 0.3f));       // Ambient intensity
    prog.setUniform("light.Ls", glm::vec3(1.0f, 0.9f, 0.8f));       // Specular intensity
   
}

void SceneBasic_Uniform::compile()
{
	try {
        // Compile shader programs here
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();

        // Compile the skybox shaders
        skyboxProg.compileShader("shader/skybox.vert");
        skyboxProg.compileShader("shader/skybox.frag");
        skyboxProg.link();

	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

//////////////////////// UPDATE
void SceneBasic_Uniform::update( float t )
{
    float deltaTime = t - lastTime;
    lastTime = t;

    camera.update(deltaTime, glfwGetCurrentContext());
    view = camera.getViewMatrix();

    planetAngle -= planetRotSpeed * deltaTime;
    orbitAngle += orbitSpeed * deltaTime;
}

///////////////////////////////// RENDER
void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthFunc(GL_LEQUAL);
    skyboxProg.use();
    model = mat4(1.0f);
    setMatrices();
    sky.render();
    glDepthFunc(GL_LESS);

    prog.use(); // Use the object shader program
    prog.setUniform("CameraPos", camera.getPosition());

    // SUN ////////////////////////////////////////////////////////////////////////////////
    prog.setUniform("material.Kd", glm::vec3(1.0f, 1.0f, 1.0f));  // Diffuse color (white)
    prog.setUniform("material.Ka", glm::vec3(1.0f, 1.0f, 1.0f));  // Ambient color
    prog.setUniform("material.Ks", glm::vec3(0.8f, 0.8f, 0.8f));  // Specular color
    prog.setUniform("material.Shininess", 32.0f); // Shininess factor

    // Bind the sun texture to texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sunTexture);      // Assuming sunTexture is loaded elsewhere
    prog.setUniform("Texture", 1);                 // Tell shader that "Texture" is bound to texture unit 1

    // Set model transformation for the sun
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f)); // Position the sun
    model = glm::scale(model, glm::vec3(2.0f));                           // Scale as needed for size
    setMatrices(); // Apply model, view, and projection matrices

    sphere->render();

    // PLANET //////////////////////////////////////////////////////////////////////////
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, planetTexture);
    prog.setUniform("Texture", 2);

    // Set material properties for the planet
    prog.setUniform("material.Kd", glm::vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("material.Ka", glm::vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("material.Ks", glm::vec3(0.3f, 0.3f, 0.3f));
    prog.setUniform("material.Shininess", 16.0f);

    // Set model transformation for the planet
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f)); // Center at the sun's position
    model = glm::rotate(model, orbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));  // Orbit rotation around the Y-axis
    model = glm::translate(model, glm::vec3(7.0f, 0.0f, 0.0f));           // Offset the planet from the sun
    model = glm::rotate(model, planetAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // Spin rotation around the Y-axis
    model = glm::scale(model, glm::vec3(0.5f));
    setMatrices();
    sphere->render();
}


void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;

    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model;
    mat4 viewNoTranslate = glm::mat4(glm::mat3(view)); // Remove translation from the view matrix
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("Model", model);
    skyboxProg.setUniform("MVP", projection * viewNoTranslate * model);

}

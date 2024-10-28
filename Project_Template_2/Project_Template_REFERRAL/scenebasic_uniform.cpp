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

    // CAMERA //////////////////////////////////////////////////////////////
    // Initialise move speed and mouse sens
    camera.setMovementSpeed(10.0f);     
    camera.setMouseSensitivity(0.2f);

    projection = mat4(1.0f);

    // TEXTURES /////////////////////////////////////////////////////////////////
    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/nebula_hdr/neb");
    sunTexture = Texture::loadTexture("media/texture/sun.jpg");
    planetTexture = Texture::loadTexture("media/texture/makemake.jpg");
    moonTexture = Texture::loadTexture("media/texture/eris.jpg");
    cloudTexture = Texture::loadTexture("media/texture/clouds.png");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, planetTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, moonTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, cloudTexture);

    // Set light properties in world space, defining its position at the sun's location
    prog.use();
    prog.setUniform("light.Position", glm::vec3(0.0f, 0.0f, 5.0f)); // World-space position of the sun
    prog.setUniform("light.Ld", glm::vec3(1.0f, 0.9f, 0.8f));       // Diffuse intensity
    prog.setUniform("light.La", glm::vec3(0.3f, 0.3f, 0.3f));       // Ambient intensity
    prog.setUniform("light.Ls", glm::vec3(1.0f, 0.9f, 0.8f));       // Specular intensity
   
    //glGenFramebuffers(2, blurFBO);
    //glGenTextures(2, blurTexture);

    //for (int i = 0; i < 2; i++) {
    //    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[i]);

    //    // Create texture for each blur pass
    //    glBindTexture(GL_TEXTURE_2D, blurTexture[i]);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture[i], 0);

    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //        std::cerr << "Error: Blur framebuffer not complete!" << std::endl;
    //}

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind framebuffer
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

        horizontalBlurShader.compileShader("shader/horizontal_blur.frag");
        horizontalBlurShader.link();

        verticalBlurShader.compileShader("shader/vertical_blur.frag");
        verticalBlurShader.link();


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

    diagonalAngle = t * rotationSpeed;
    skyboxRotationMatrix = glm::rotate(glm::mat4(1.0f), diagonalAngle, glm::normalize(vec3(1.0f, 1.0f, 0.0f)));

    // Toggle rotation on "O" key press
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_O) == GLFW_PRESS) {
        if (!oKeyPressedLastFrame) {
            rotatePlanets = !rotatePlanets; 
        }
        oKeyPressedLastFrame = true;
    }
    else {
        oKeyPressedLastFrame = false;
    }

    // Update planet rotation and orbit if toggled on
    if (rotatePlanets) {
        planetAngle -= planetRotSpeed * deltaTime;
        orbitAngle += orbitSpeed * deltaTime;
    }

    moonAngle -= moonRotSpeed * deltaTime;
    moonOrbitAngle += moonOrbitSpeed * deltaTime;

    prog.setUniform("time", t);

}

///////////////////////////////// RENDER
void SceneBasic_Uniform::render()
{
   /* glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[0]);*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skyboxProg.use();
    model = skyboxRotationMatrix;
    setMatrices();
    sky.render();

    prog.use(); // Use the object shader program
    prog.setUniform("CameraPos", camera.getPosition());

    // SUN ////////////////////////////////////////////////////////////////////////////////
    prog.setUniform("material.Kd", glm::vec3(1.0f, 1.0f, 1.0f));  // Diffuse (white)
    prog.setUniform("material.Ka", glm::vec3(1.0f, 1.0f, 1.0f));  // Ambient 
    prog.setUniform("material.Ks", glm::vec3(0.8f, 0.8f, 0.8f));  // Specular 
    prog.setUniform("material.Shininess", 32.0f); // Shininess 
    prog.setUniform("material.Emissive", glm::vec3(1.0f, 0.9f, 0.8f)); // Make the sun glow

    // Bind the sun texture to texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, sunTexture);      
    prog.setUniform("Texture", 1);                 
    prog.setUniform("useClouds", 0);

    // Set model transformation for the sun
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f)); 
    model = glm::scale(model, glm::vec3(2.0f));                           
    setMatrices(); // Apply model, view, and projection matrices

    sphere->render();

    // PLANET //////////////////////////////////////////////////////////////////////////

    prog.setUniform("roughness", 0.3f);  
    prog.setUniform("metallic", 0.9f);

    // Set material properties for the planet
    prog.setUniform("material.Kd", glm::vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("material.Ka", glm::vec3(0.5f, 0.5f, 0.5f));
    prog.setUniform("material.Ks", glm::vec3(0.3f, 0.3f, 0.3f));
    prog.setUniform("material.Shininess", 16.0f);
    prog.setUniform("material.Emissive", glm::vec3(0.0f, 0.0f, 0.0f)); // Planets don't glow

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, planetTexture);
    prog.setUniform("Texture", 2);
    // Bind the cloud texture to texture unit 4
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, cloudTexture);
    prog.setUniform("CloudTexture", 4);
    prog.setUniform("useClouds", 1);

    // Set model transformation for the planet
    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f)); // Center at the sun's position
    model = glm::rotate(model, orbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));  // Orbit rotation around the Y-axis
    model = glm::translate(model, glm::vec3(7.0f, 0.0f, 0.0f));           // Offset the planet from the sun
    model = glm::rotate(model, planetAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // Spin rotation around the Y-axis
    model = glm::scale(model, glm::vec3(0.5f));
    setMatrices();
    sphere->render();

    // MOON ////////////////////////////////////////////////////////////////////////
    prog.setUniform("material.Emissive", glm::vec3(0.0f, 0.0f, 0.0f));
    prog.setUniform("material.Kd", glm::vec3(0.8f, 0.8f, 0.8f));
    prog.setUniform("material.Ka", glm::vec3(0.4f, 0.4f, 0.4f));
    prog.setUniform("material.Ks", glm::vec3(0.2f, 0.2f, 0.2f));
    prog.setUniform("material.Shininess", 8.0f);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, moonTexture);
    prog.setUniform("Texture", 3);
    prog.setUniform("useClouds", 0);

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f));             
    model = glm::rotate(model, orbitAngle, glm::vec3(0.0f, 1.0f, 0.0f));              
    model = glm::translate(model, glm::vec3(7.0f, 0.0f, 0.0f));                       
    model = glm::rotate(model, moonOrbitAngle, glm::vec3(0.5f, 1.0f, 0.3f));       // Tilted orbit around the planet
    model = glm::translate(model, glm::vec3(1.5f, 0.0f, 0.0f));                       
    model = glm::rotate(model, moonAngle, glm::vec3(0.0f, 1.0f, 0.0f));               
    model = glm::scale(model, glm::vec3(0.25f));                                      
    setMatrices();
    sphere->render();

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Unbind

    //// Horizontal blur pass
    //glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[1]);
    //horizontalBlurShader.use();
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, blurTexture[0]);  // Input texture from the scene pass
    //horizontalBlurShader.setUniform("scene", 0);
    //renderQuad();

    //// Vertical blur pass
    //glBindFramebuffer(GL_FRAMEBUFFER, blurFBO[0]);
    //verticalBlurShader.use();
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, blurTexture[1]);  // Input texture from horizontal pass
    //verticalBlurShader.setUniform("scene", 0);
    //renderQuad();

    //// Render blurred result to the default framebuffer
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glBindTexture(GL_TEXTURE_2D, blurTexture[0]);
    //renderQuad();
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
    mat4 viewNoTranslate = glm::mat4(glm::mat3(view)); 
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("Model", model);
    skyboxProg.setUniform("MVP", projection * viewNoTranslate * model);
}

void SceneBasic_Uniform::renderQuad() {
    if (quadVAO == 0) {
        // Define vertices for a full-screen quad
        float quadVertices[] = {
            // Positions       // Texture Coords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        // Generate and bind the VAO and VBO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Texture coordinate attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    // Render the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


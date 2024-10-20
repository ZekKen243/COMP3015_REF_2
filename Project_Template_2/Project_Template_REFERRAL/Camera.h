// Camera.h
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

// Handles camera movement and orientation in 3D
class Camera {
public:
    // Initialise camera with position, yaw, and pitch
    Camera(glm::vec3 position, float yaw, float pitch);

    // Return view matrix calculated using camera pos and orientation
    glm::mat4 getViewMatrix() const;

    // Keyboard and mouse movement processing
    void processKeyboard(float deltaTime, GLFWwindow* window);
    void processMouseMovement(float xoffset, float yoffset);

    // Update camera pos and orientation based on input
    void update(float deltaTime, GLFWwindow* window);

    // Movement speed and mouse sensitivity
    void setMovementSpeed(float speed) { movementSpeed = speed; }
    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }

private:
    // Camera values 
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Yaw and pitch (rotation on y-axis and x-axis)
    float yaw;
    float pitch;

    // Speed and mouse sens
    float movementSpeed;
    float mouseSensitivity;

    // Initial mouse input flag
    bool firstMouse;

    // Last known X and Y pos of mouse
    float lastX;
    float lastY;

    // Check if left click held down
    bool leftMousePressed;

    void updateCameraVectors();
};
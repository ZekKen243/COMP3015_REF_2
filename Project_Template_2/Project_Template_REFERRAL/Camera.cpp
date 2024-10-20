// Camera.cpp
#include "Camera.h"

Camera::Camera(glm::vec3 position, float yaw, float pitch)
    : position(position), yaw(yaw), pitch(pitch), front(glm::vec3(0.0f, 0.0f, -1.0f)),
    movementSpeed(5.0f), mouseSensitivity(0.1f), firstMouse(true),
    lastX(400), lastY(300), leftMousePressed(false) {
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    updateCameraVectors();
}

// Returns view matrix calculated using camera's current pos and orientation
glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

///////////////////////////////////////////////////////////////// WASD MOVEMENT + UP AND DOWN
void Camera::processKeyboard(float deltaTime, GLFWwindow* window) {
    float velocity = movementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += up * velocity;     
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        position -= up * velocity;     
}

//////////////////////////////////////////////////////////////// MOUSE MOVEMENT
void Camera::processMouseMovement(float xoffset, float yoffset) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain the pitch (prevents flipping)
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateCameraVectors();
}
//////////////////////////////////////////////////////// UPDATE
void Camera::update(float deltaTime, GLFWwindow* window) {
    // Process keyboard input
    processKeyboard(deltaTime, window);

    // Process mouse input if left mouse button is held
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!leftMousePressed) {
            firstMouse = true;
            leftMousePressed = true;
        }

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (firstMouse) {
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            firstMouse = false;
        }

        float xoffset = static_cast<float>(xpos - lastX);
        float yoffset = static_cast<float>(lastY - ypos); // Reversed since y-coordinates go from bottom to top

        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        processMouseMovement(xoffset, yoffset);
    }
    else {
        leftMousePressed = false;
    }
}

void Camera::updateCameraVectors() {
    // Calculate the new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    // Recalculate the right and up vectors
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

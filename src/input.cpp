#include "./input.hpp"

namespace lve {
    void KeyboardMovementController::moveInPlaneXZ(
        GLFWwindow* window, float dt, LveGameObject& gameObject) {
        
        glm::vec3 rotate{0.0f};
        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.0f;
        if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.0f;
        if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.0f;
        if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.0f;

        if(glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
            gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);

        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1 * glm::two_pi<float>() / 4, glm::two_pi<float>() / 4);
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

        float yaw = gameObject.transform.rotation.y;
        float pitch = gameObject.transform.rotation.x;

        const glm::vec3 forwardDir{sin(yaw), -sin(pitch), cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
        const glm::vec3 upDir{0.0f, -1.0f, 0.0f};             

        glm::vec3 moveDir{0.0f};
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

        if(glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
            gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);

    }

    void KeyboardMovementController::moveCamera(LveGameObject& sourceObject, LveGameObject& targetObject) {
 
    }
    void KeyboardMovementController::movePlayer() {}
 
}
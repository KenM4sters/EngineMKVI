#pragma once 
#include "./mesh.hpp"


// libs
#include <glm/gtc/matrix_transform.hpp>

//std
#include <memory>
#include <unordered_map>

namespace lve { 

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        // Matrix corresponds to translate * Ry * Rx * Rz * scale transformation
        // Rotation convention uses tait-bryan angles with axis order Y(1), X(2), Z(3)
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    struct PointLightComponent {
        float lightIntensity = 1.0f;
    };

    class LveGameObject {
        public:
            using id_t = unsigned int;
            using Map = std::unordered_map<id_t, LveGameObject>;

            static LveGameObject createGameObject() {
                static id_t currentId = 0;
                return LveGameObject{currentId++};
            }

            static LveGameObject makePointLight(
                float intensity = 10.0f, 
                float radius = 0.1f,
                glm::vec3 color = glm::vec3(1.0f));

            LveGameObject (const LveGameObject &) = delete;
            LveGameObject &operator=(const LveGameObject &) = delete;
            LveGameObject (LveGameObject &&) = default;
            LveGameObject &operator=(LveGameObject &&) = default;

            id_t getId() {return id;}
            
            glm::vec3 color{};
            TransformComponent transform{};

            std::shared_ptr<LveMesh> mesh{};
            std::unique_ptr<PointLightComponent> pointLight = nullptr;

        private:
            LveGameObject(id_t objId) : id{objId} {}
            id_t id;
    };
}
#pragma once

#include "./window.hpp"
#include "./renderer.hpp"
#include "./game_object.hpp"
#include "./descriptors.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
    class App {

        public:
            static const int WIDTH = 800;
            static const int HEIGHT = 600;

            App();
            ~App();

            App(const LveWindow&) = delete;
            App &operator=(const LveWindow&) = delete;

            void run();

        private:
            void loadGameObjects();

            LveWindow lveWindow{WIDTH, HEIGHT, "EngineMKIV"};
            LveDevice lveDevice{lveWindow};
            LveRenderer lveRenderer{lveWindow, lveDevice};

            // note: order of declarations matters
            std::unique_ptr<LveDescriptorPool> globalPool{};
            LveGameObject::Map gameObjects;

    };
}
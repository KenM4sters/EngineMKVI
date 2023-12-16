#pragma once

#include "./window.hpp"
#include "./pipeline.hpp"

namespace lve {
    class App {

        public:
            static const int WIDTH = 800;
            static const int HEIGHT = 600;

            void run();


        private:
            LveWindow lveWindow{WIDTH, HEIGHT, "EngineMKII"};
            LveDevice lveDevice{lveWindow};
            LvePipeline lvePipeline{
                lveDevice, 
                "shaders/vert.spv", 
                "shaders/frag.spv", 
                LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT
                )};

    };
}
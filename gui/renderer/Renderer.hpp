#ifndef INCLUDED_RENDERER_HPP
    #define INCLUDED_RENDERER_HPP

#include <iostream>
#include "raylib.h"


class Renderer
{
    private:
        int _screenWidth;
        int _screenHeight;
    protected:
    public:
        Renderer(int width, int height);
        void renderWindow();
};

#endif


#include "Header/Application.h"

#define GLW_INCLUDE_VULKAN

#include <iostream>
#include <GLFW/glfw3.h> 


int main()
{
    Application app;

    app.Init();
    app.Run();
    app.Nuke();
  
  

    return 0;

}


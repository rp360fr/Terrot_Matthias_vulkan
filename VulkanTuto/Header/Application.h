#pragma once
#define GLW_INCLUDE_VULKAN

#include <iostream>
#include <GLFW/glfw3.h> 
#include "Renderer.h"
#include <memory>
class Renderer;
class Application
{
	
	
	
	
	
	
public:

	int width = 800;
	int height = 600;

	GLFWwindow* window = nullptr;
	std::unique_ptr<Renderer> renderer;
	
	void Init();
	void Run();
	void Nuke();
	
private:
	
	void InitWidows();
	void InitVulkan();
	void NukeWidows();
	void NukeVulkan();

	
	
	
	
	
	
	
	
};	


#pragma once
#define GLW_INCLUDE_VULKAN

#include <iostream>
#include <GLFW/glfw3.h> 
#include "Renderer.h"
#include "Instance.h"
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "VertexBuffer.h"
#include <memory>
class Renderer;
class Instance;
class Device;
class SwapChain;
class Pipeline;
class VertexBuffer

class Application
{
	
	
	
	
	
	
public:
Application();
	int width = 800;
	int height = 600;

	GLFWwindow* window = nullptr;
	
	Renderer renderer;
	Instance instance;
	Device device;
	SwapChain swapChain;
	Pipeline pipeline;
	VertexBuffer vertexBuffer;
	
	
	void Init();
	void Run();
	void Nuke();
	
private:
	void cleanup();
	void InitWidows();
	void InitVulkan();
	void NukeWidows();
	void NukeVulkan();
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	
	
	
	
	
	
	
	
};	


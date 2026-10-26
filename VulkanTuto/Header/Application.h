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
#include "TextureMap.h"
#include <memory>
class Renderer;
class Instance;
class Device;
class SwapChain;
class Pipeline;
class VertexBuffer;
class TextureMap;

class Application
{
	
	
	
	
	
	
public:
Application()
{
	window = nullptr;
}
	int width = 800;
	int height = 600;

	GLFWwindow* window = nullptr;
	
	Renderer renderer;
	Instance instance;
	Device device;
	SwapChain swapChain;
	Pipeline pipeline;
	VertexBuffer vertexBuffer;
	TextureMap texture;
	
	
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


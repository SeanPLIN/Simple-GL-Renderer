#include "Window.h"

Window* Window::s_WindowInstance = nullptr;

Window::Window(const std::string& title, int width, int height) :
	m_Title(title), m_Width(width), m_Height(height)
{
	ASSERT(s_WindowInstance == nullptr, "Simple GL Renderer only supports one window");
	s_WindowInstance = this;
	
	VERIFY(glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");

	m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (m_Window == nullptr)
	{
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(m_Window);
	glfwSetWindowUserPointer(m_Window, &m_Callback);
	glViewport(0, 0, m_Width, m_Height);

	glfwSetKeyCallback(m_Window, Window::KeyCallback);
	glfwSetCharCallback(m_Window, Window::CharCallback);
	glfwSetCursorPosCallback(m_Window, Window::CursorPosCallback);
	glfwSetMouseButtonCallback(m_Window, Window::MouseButtonCallback);
	glfwSetScrollCallback(m_Window, Window::MouseScrollCallback);
	glfwSetWindowSizeCallback(m_Window, Window::WindowResizeCallback);
	glfwSetWindowCloseCallback(m_Window, Window::WindowCloseCallback);

	m_IsVSync = true;
}

Window::~Window()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Window::StartFrame()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::EndFrame()
{
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Window::SetVSync(bool enable)
{
	m_IsVSync = enable;
	glfwSwapInterval(static_cast<int>(enable));
}

void Window::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(m_Window, title.c_str());
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Event e;
	switch (action)
	{
	case GLFW_PRESS:
		e.type = Event::Type::KeyPressed;
		break;

	case GLFW_RELEASE:
		e.type = Event::Type::KeyReleased;
		break;

	case GLFW_REPEAT:
		e.type = Event::Type::KeyPressed;
		break;

	}

	e.key.key_code = static_cast<Keyboard::KeyCode>(key);
	e.key.shift = mods & GLFW_MOD_SHIFT;
	e.key.ctrl = mods & GLFW_MOD_CONTROL;
	e.key.alt = mods & GLFW_MOD_ALT;
	e.key.option = mods & GLFW_MOD_SUPER;

	CallbackFn callback = *reinterpret_cast<CallbackFn*>(glfwGetWindowUserPointer(window));
	callback(e);
}

void Window::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
	Event e;
	e.type = Event::Type::KeyTyped;

	e.text.character = (char)codepoint;

	CallbackFn callback = *reinterpret_cast<CallbackFn*>(glfwGetWindowUserPointer(window));
	callback(e);
}

void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	Event e;
	e.type = Event::Type::MouseMoved;

	e.mouse.x = xpos;
	e.mouse.y = ypos;

	CallbackFn callback = *reinterpret_cast<CallbackFn*>(glfwGetWindowUserPointer(window));
	callback(e);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Event e;
	switch (action)
	{
	case GLFW_PRESS:
		e.type = Event::Type::MouseButtonPressed;
		break;

	case GLFW_RELEASE:
		e.type = Event::Type::MouseButtonReleased;
		break;
	}

	e.button.button = static_cast<Mouse::Button>(button);
	e.button.shift  = mods & GLFW_MOD_SHIFT;
	e.button.ctrl   = mods & GLFW_MOD_CONTROL;
	e.button.alt    = mods & GLFW_MOD_ALT;
	e.button.option = mods & GLFW_MOD_SUPER;

	CallbackFn callback = *reinterpret_cast<CallbackFn*>(glfwGetWindowUserPointer(window));
	callback(e);
}

void Window::MouseScrollCallback(GLFWwindow* window, double xoff, double yoff)
{
	Event e;
	e.type = Event::Type::MouseScrolled;

	e.scroll.x_offset = xoff;
	e.scroll.y_offset = yoff;

	CallbackFn callback = *reinterpret_cast<CallbackFn*>(glfwGetWindowUserPointer(window));
	callback(e);
}

void Window::WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	Event e;
	e.type = Event::Type::WindowResized;

	e.window.width = width;
	e.window.height = height;

	CallbackFn callback = *reinterpret_cast<CallbackFn*>(glfwGetWindowUserPointer(window));
	callback(e);
}

void Window::WindowCloseCallback(GLFWwindow* window)
{
	Event e;
	e.type = Event::Type::WindowClosed;

	CallbackFn callback = *reinterpret_cast<CallbackFn*>(glfwGetWindowUserPointer(window));
	callback(e);
}
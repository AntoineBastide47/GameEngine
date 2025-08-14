//
// Window.cpp
// Author: Antoine Bastide
// Date: 03.08.2025
//

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "Window.hpp"
#include "LevelEditor.hpp"

namespace Editor {
  Window::Window()
    : window(nullptr), mouseDown(false) {
    if (instance)
      throw std::runtime_error("ERROR::WINDOW: There can only be one instance of Editor::Window running.");
    instance = this;
  }

  void Window::Run() {
    if (!instance) {
      std::cout << "ERROR::WINDOW::Run(): Editor window not initialized!" << std::endl;
      exit(EXIT_FAILURE);
    }

    initialize();

    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      ImGui_ImplGlfw_NewFrame();
      ImGui_ImplOpenGL3_NewFrame();
      ImGui::NewFrame();

      if (LevelEditor::loadedGame) {
        if (LevelEditor::IsViewportFocused())
          LevelEditor::loadedGame->processInput();
        LevelEditor::loadedGame->updateGame();
      }

      LevelEditor::Render();
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // Update and Render additional Platform Windows
      if (const ImGuiIO &io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
      }

      glfwSwapBuffers(instance->window);
    }

    quit();
  }

  bool Window::Initialized() {
    return instance;
  }

  float Window::WindowWidth() {
    return static_cast<float>(instance->width);
  }

  float Window::WindowHeight() {
    return static_cast<float>(instance->height);
  }

  void Window::Quit() {
    if (!instance)
      return;
    glfwSetWindowShouldClose(instance->window, true);
  }

  void Window::initialize() {
    // Initialize and Configure GLFW
    if (!glfwInit()) {
      std::cerr << "ERROR: GLFW could not initialize" << std::endl;
      exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Set window hints
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    // Create the window
    window = glfwCreateWindow(mode->width, mode->height, "Editor Window", nullptr, nullptr);
    if (!window) {
      std::cerr << "ERROR: Failed to create window" << std::endl;
      exit(EXIT_FAILURE);
    }

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Set window callbacks
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);

    glfwSwapInterval(1);

    // The official code for "Setting Your Raster Position to a Pixel Location" (i.e., set up a camera for 2D screen)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Make some OpenGL properties better for 2D and enable blending
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(32768.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (const GLenum err = glewInit(); err != GLEW_OK) {
      std::cerr << "ERROR: Failed to initialize GLEW: " << err << std::endl;
      exit(EXIT_FAILURE);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const auto glsl_version = "#version 330 core";
    ImGui_ImplOpenGL3_Init(glsl_version);

    LevelEditor::initialize();
  }

  void Window::quit() {
    LevelEditor::unloadProject();

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destroy the window and terminate all OpenGL processes
    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();

    instance = nullptr;
  }

  void Window::framebuffer_size_callback(GLFWwindow *, const int width, const int height) {
    instance->width = width;
    instance->height = height;

    glViewport(0, 0, width, height);
  }

  void Window::window_close_callback(GLFWwindow *) {
    Quit();
  }
} // Editor

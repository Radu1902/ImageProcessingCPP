#include "image_loader.h"
#include "gui.h"



int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = decide_gl_glsl_version();

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init(glsl_version);




    bool show_demo_window = false;
    bool show_another_window = false;

    bool show_imgproc = false;
    bool show_menu = false;

    int my_image_width = 0;
    int my_image_height = 0;
    GLuint my_image_texture = 0;
    bool ret = LoadTextureFromFile("input/Zoro.jpg", &my_image_texture, &my_image_width, &my_image_height);
    IM_ASSERT(ret);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            ImGui::Begin("Image Processing", &show_imgproc, ImGuiWindowFlags_MenuBar);
            ImGui::Text("Hello");
            if (ImGui::Button("Upload"))
            {
                show_menu = true;
            }
            if (show_menu)
            {
                if (ImGui::BeginMenuBar())
                {
                    
                    ImGui::EndMenuBar();
                }
            }
            
            ImGui::End();
        }

        {
            ImGui::Begin("OpenGL Texture Text");
            ImGui::Text("pointer = %x", my_image_texture);
            ImGui::Text("size = %d x %d", my_image_width, my_image_height);
            ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
            ImGui::End();
        }
        

        if (show_menu)
        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Main menu"))
                {
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Pointwise operations"))
                {
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Thresholdings"))
                {
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Filters"))
                {
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        }
        

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

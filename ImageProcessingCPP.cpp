#include "image_loader.h"
#include "gui.h"
#include "ImGuiFileDialog.h"


const char* drawGui(bool* keep_open) {
    // open Dialog Simple
    IGFD::FileDialogConfig config;
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".jpg,.png", config);
    const char* path_string = nullptr;
    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
            const char* path_string = filePathName.c_str();
            // *keep_open = false;
            ImGuiFileDialog::Instance()->Close();
        }

        // close
        *keep_open = false;
        ImGuiFileDialog::Instance()->Close();
        return path_string;
    }
}

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




    bool show_demo_window = true;


    const char* input_path = NULL;
    bool show_path_chooser = false;

    bool input_image_exists = false;
    bool output_image_exists = false;
    bool show_input_image = false;
    bool show_output_image = false;

    bool show_imgproc_operations = false;

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
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Main menu"))
                {
                    ImGui::MenuItem("Choose file", NULL, &show_path_chooser);
                    ImGui::EndMenu();
                }
                if (show_imgproc_operations)
                {
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
                }
                ImGui::EndMainMenuBar();
            }
        }

        if (show_path_chooser)
        {
            input_path = drawGui(&show_path_chooser);
            printf("%s\n", input_path);
        }

        {
            ImGui::Begin("Input Image");
            ImGui::Text("pointer = %x", my_image_texture);
            ImGui::Text("size = %d x %d", my_image_width, my_image_height);
            if (input_path != nullptr)
            {
                ImGui::Text("file path: %s", *input_path);
            }
            if (show_input_image)
            {
                ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
            }
            ImGui::End();
        }
        {
            ImGui::Begin("Output Image");
            ImGui::Text("pointer = %x", my_image_texture);
            ImGui::Text("size = %d x %d", my_image_width, my_image_height);
            if (show_output_image)
            {
                ImGui::Image((void*)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
            }
            ImGui::End();
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

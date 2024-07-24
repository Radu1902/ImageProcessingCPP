#include "image_loader.h"
#include "gui.h"
#include "ImGuiFileDialog.h"
#include "Image.h"

void drawGui(char** path, bool* keep_open) {
    // open Dialog Simple
    IGFD::FileDialogConfig config;
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".jpg,.png,.bmp,.jpeg", config);
    char* path_string = nullptr;
    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
            path_string = new char[filePathName.length() + 1];
            strcpy_s(path_string, filePathName.length() + 1, filePathName.c_str());
            *path = path_string;
            *keep_open = false;
            ImGuiFileDialog::Instance()->Close();
        }

        // close
        *keep_open = false;
        ImGuiFileDialog::Instance()->Close();
    }
}

void writeAndDisplayOutput(GLuint* output_image_texture, int* output_image_width, int* output_image_height, int *output_image_channels, Image out_img)
{
    unsigned char* out_texture;
    out_img.getTexture(&out_texture);
    *output_image_width = out_img.getWidth();
    *output_image_height = out_img.getHeight();
    *output_image_channels = out_img.getChannels();

    writeTextureToFile("output/outputty.bmp", *output_image_width, *output_image_height, *output_image_channels, out_texture);
    bool output_ret = loadTextureFromFile("output/outputty.bmp", output_image_texture, output_image_width, output_image_height);
    IM_ASSERT(output_ret);
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


    bool input_image_exists = false;
    bool output_image_exists = false;
    bool show_input_image = false;
    bool show_output_image = false;

    bool show_imgproc_operations = false;

    bool show_path_chooser = false;

    char* input_path = NULL;

    int input_image_width = 0;
    int input_image_height = 0;
    GLuint input_image_texture = 0;
    bool input_ret = false; 

    int output_image_width = 0;
    int output_image_height = 0;
    GLuint output_image_texture = 0;
    bool output_ret = false;
    int output_image_channels = 0;

    Image in_img;
    Image out_img;

    static float size_coefficient = 1.0f;

    int thresh = 255;
    bool show_thresh_dialog = false;

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
                    if (ImGui::BeginMenu("Basic operations"))
                    {
                        if (ImGui::MenuItem("Convert to grayscale"))
                        {
                            out_img = in_img.convert2Gray();

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Pointwise operations"))
                    {
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Thresholdings"))
                    {

                        if (ImGui::MenuItem("Manual"))
                        {
                            show_thresh_dialog = true;
                        }
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
            drawGui(&input_path, &show_path_chooser);
        }

        if (show_thresh_dialog)
        {
            ImGui::Begin("Manual thresholding");
            ImGui::SliderInt("Choose Threshold", &thresh, 0, 255);
            if (ImGui::Button("Choose"))
            {
                out_img = in_img.threshold(thresh);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_thresh_dialog = false;
            }
            ImGui::End();
        }

        {
            ImGui::Begin("Input Image");
            ImGui::Text("pointer = %x", input_image_texture);
            ImGui::Text("size = %d x %d", input_image_width, input_image_height);
            ImGui::SliderFloat("resize  (VISUAL ONLY)", &size_coefficient, 0.1f, 10.0f);
            if (input_path != nullptr)
            {
                ImGui::Text("file path: %s", input_path);
                if (ImGui::Button("Load image"))
                {
                    input_ret = loadTextureFromFile(input_path, &input_image_texture, &input_image_width, &input_image_height);
                    in_img.loadImage(input_path, 3);

                    IM_ASSERT(input_ret);
                    show_input_image = true;
                    show_imgproc_operations = true;
                }
            }
            if (show_input_image)
            {
                ImGui::Image((void*)(intptr_t)input_image_texture, ImVec2(input_image_width * size_coefficient, input_image_height * size_coefficient));
            }
            ImGui::End();
        }
        {
            ImGui::Begin("Output Image");
            ImGui::Text("pointer = %x", output_image_texture);
            ImGui::Text("size = %d x %d", output_image_width, output_image_height);
            if (show_output_image)
            {
                ImGui::Image((void*)(intptr_t)output_image_texture, ImVec2(output_image_width * size_coefficient, output_image_height * size_coefficient));
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

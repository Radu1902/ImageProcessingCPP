#include "image_loader.h"
#include "implot.h"
#include "implot_internal.h"
#include "gui.h"
#include "ImGuiFileDialog.h"
#include "Image.h"
#include "Operations.h"

void drawPathSelector(char** path, bool* keep_open) {
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

void writeAndDisplayOutput(GLuint* output_image_texture, int* output_image_width, int* output_image_height, int* output_image_channels, Image out_img)
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

void saveOutputAsInput(GLuint* output_image_texture, int* output_image_width, int* output_image_height, int* output_image_channels, Image out_img)
{
    // literally only differs in the name of the saved bmp (outputInput.bmp instead of outputty.bmp)

    unsigned char* out_texture;
    out_img.getTexture(&out_texture);
    *output_image_width = out_img.getWidth();
    *output_image_height = out_img.getHeight();
    *output_image_channels = out_img.getChannels();

    writeTextureToFile("output/outputInput.bmp", *output_image_width, *output_image_height, *output_image_channels, out_texture);
    bool output_ret = loadTextureFromFile("output/outputInput.bmp", output_image_texture, output_image_width, output_image_height);
    IM_ASSERT(output_ret);



}

int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = decide_gl_glsl_version();

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Image processing", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init(glsl_version);

    static ImU32 colorDataRGB[3] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
    int customRGBMap = ImPlot::AddColormap("RGBColors", colorDataRGB, 32);

    bool show_demo_window = true;


    bool input_image_exists = false;
    bool output_image_exists = false;
    bool show_input_image = false;
    bool show_output_image = false;

    bool show_imgproc_operations = false;

    bool show_path_selector = false;

    char* input_path = nullptr;

    int input_image_width = 0;
    int input_image_height = 0;
    GLuint input_image_texture = 0;
    bool input_ret = false;
    int input_image_channels = 3;

    int output_image_width = 0;
    int output_image_height = 0;
    GLuint output_image_texture = 0;
    bool output_ret = false;
    int output_image_channels = 0;

    Image in_img;
    Image out_img;

    static float size_coefficient = 1.0f;

    // histogram

    unsigned int* chan1Histogram = nullptr;
    unsigned int* chan2Histogram = nullptr;
    unsigned int* chan3Histogram = nullptr;
    bool show_histogram = false;

    // Pointwise ops

    int brightnessOffset = 0;
    bool show_brightness_dialog = false;

    float contrastFactor = 1.0f;
    bool show_contrast_dialog = false;

    bool show_linear_dialog = false;

    float gamma = 1;
    bool show_gamma_dialog = false;

    int r1 = 1;
    int s1 = 1;
    int r2 = 254;
    int s2 = 254;
    bool show_piecewise_dialog = false;

    float e;
    int m;
    bool show_em_dialog = false;

    char x1, y1, x2, y2, x3, y3, x4, y4, x5, y5;
    bool show_spline_dialog = false;

    // thresholdings

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
                    ImGui::MenuItem("Choose file", NULL, &show_path_selector);


                    ImGui::EndMenu();
                }

                if (show_imgproc_operations)
                {
                    if (ImGui::BeginMenu("Basic operations"))
                    {
                        if (ImGui::MenuItem("Convert to grayscale"))
                        {
                            out_img = convert2Gray(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (show_input_image) // if show_input_image is true, it means in_img is not null
                        {
                            if (ImGui::MenuItem("Show input image histogram"))
                            {
                                getHistogram(in_img, 0, &chan1Histogram);
                                if (in_img.getType() == PixelType::RGB)
                                {
                                    getHistogram(in_img, 1, &chan2Histogram);
                                    getHistogram(in_img, 2, &chan3Histogram);
                                }
                                else // if img isn't rgb, free the other histograms so that they don't remain loaded with data from previous images
                                {
                                    delete[] chan2Histogram;
                                    delete[] chan3Histogram;
                                    chan2Histogram = nullptr;
                                    chan3Histogram = nullptr;
                                }
                                show_histogram = true;
                            }
                        }
                        if (show_output_image) // it means out_img is not null
                        {
                            if (ImGui::MenuItem("Show output image histogram"))
                            {
                                getHistogram(out_img, 0, &chan1Histogram);
                                if (out_img.getType() == PixelType::RGB)
                                {
                                    getHistogram(out_img, 1, &chan2Histogram);
                                    getHistogram(out_img, 2, &chan3Histogram);
                                }
                                else
                                {
                                    delete[] chan2Histogram;
                                    delete[] chan3Histogram;
                                    chan2Histogram = nullptr;
                                    chan3Histogram = nullptr;
                                }
                                show_histogram = true;
                            }
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Pointwise operations"))
                    {
                        if (ImGui::MenuItem("Modify brightness"))
                        {
                            show_brightness_dialog = true;
                        }
                        if (ImGui::MenuItem("Modify contrast"))
                        {
                            show_contrast_dialog = true;
                        }
                        if (ImGui::MenuItem("Modify brightness and contrast"))
                        {
                            show_linear_dialog = true;
                        }
                        if (ImGui::MenuItem("Logarithmic operator"))
                        {
                            out_img = logOperator(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Gamma operator"))
                        {
                            show_gamma_dialog = true;
                        }
                        if (ImGui::MenuItem("Piecewise linear contrast"))
                        {
                            show_piecewise_dialog = true;
                        }
                        if (ImGui::MenuItem("EM - operator"))
                        {
                            show_em_dialog = true;
                        }
                        if (ImGui::MenuItem("Spline tool"))
                        {
                            show_spline_dialog = true;
                        }
                        if (ImGui::MenuItem("Color contrast stretching"))
                        {

                        }
                        if (ImGui::MenuItem("Histogram equalization"))
                        {

                        }
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

        if (show_histogram)
        {
            ImGui::Begin("Histogram", &show_histogram);

            if (chan1Histogram != nullptr && chan2Histogram != nullptr && chan3Histogram != nullptr)
            {
                ImPlot::PushColormap(customRGBMap);
                if (ImPlot::BeginPlot("RGB Histogram"))
                {
                    ImPlot::PlotBars("Channel 1", chan1Histogram, 256);
                    ImPlot::PlotBars("Channel 2", chan2Histogram, 256);
                    ImPlot::PlotBars("Channel 3", chan3Histogram, 256);
                    ImPlot::EndPlot();
                    ImPlot::PopColormap();
                }
            }
            else
            {
                if (chan1Histogram != nullptr)
                {
                    if (ImPlot::BeginPlot("Histogram"))
                    {
                        ImPlot::PlotBars("Pixel values", chan1Histogram, 256);
                        ImPlot::EndPlot();
                    }
                }
            }

            ImGui::End();
        }

        // dialogs

        if (show_path_selector)
        {
            drawPathSelector(&input_path, &show_path_selector);
        }

        // pointwise operations

        if (show_brightness_dialog)
        {
            ImGui::Begin("Brightness offset", &show_brightness_dialog);
            ImGui::SliderInt("Choose offset", &brightnessOffset, -255, 255);
            if (ImGui::Button("Choose"))
            {
                out_img = modifyBrightess(in_img, brightnessOffset);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_brightness_dialog = false;
            }
            ImGui::End();
        }
        if (show_contrast_dialog)
        {
            ImGui::Begin("Modify contrast", &show_contrast_dialog);
            ImGui::InputFloat("Choose contrast factor", &contrastFactor);
            if (ImGui::Button("Choose"))
            {
                if (contrastFactor < 0)
                {
                    ImGui::Text("Contrast factor must be positive.");
                }
                else
                {
                    out_img = modifyContrast(in_img, contrastFactor);
                    writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                    show_output_image = true;
                    show_contrast_dialog = false;
                }
            }
            ImGui::End();
        }
        if (show_linear_dialog)
        {
            ImGui::Begin("Modify contrast and brightness ", &show_linear_dialog);
            ImGui::InputInt("Choose offset", &brightnessOffset);
            ImGui::InputFloat("Choose contrast factor", &contrastFactor);
            if (ImGui::Button("Choose"))
            {
                if (contrastFactor < 0)
                {
                    ImGui::Text("Contrast factor must be positive.");
                }
                else
                {
                    out_img = modifyContrastAndBrightness(in_img, contrastFactor, brightnessOffset);
                    writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                    show_output_image = true;
                    show_linear_dialog = false;
                }
            }
            ImGui::End();
        }
        if (show_gamma_dialog)
        {
            ImGui::Begin("Gamma operator", &show_gamma_dialog);
            ImGui::InputFloat("Choose contrast factor", &gamma);
            if (ImGui::Button("Choose"))
            {
                if (gamma < 0)
                {
                    ImGui::Text("Contrast factor must be positive.");
                }
                else
                {
                    out_img = gammaOperator(in_img, gamma);
                    writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                    show_output_image = true;
                    show_gamma_dialog = false;
                }
            }
            ImGui::End();
        }
        if (show_piecewise_dialog)
        {
            ImGui::Begin("Piecewise contrast", &show_piecewise_dialog);
            ImGui::SliderInt("r1", &r1, 1, r2 - 1);
            ImGui::SliderInt("s1", &s1, 1, s2 - 1);
            ImGui::SliderInt("r2", &r2, r1 + 1, 254);
            ImGui::SliderInt("s2", &s2, s1 + 1, 254);
            if (ImGui::Button("Choose"))
            {
                {
                    out_img = piecewiseLinearContrast(in_img, r1, s1, r2, s2);
                    writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                    show_output_image = true;
                    show_piecewise_dialog = false;
                }
            }
            ImGui::End();
        }
        if (show_em_dialog)
        {
            ImGui::Begin("EM contrast", &show_piecewise_dialog);
            ImGui::InputFloat("s1", &e);
            ImGui::InputInt("r1", &m);
            if (ImGui::Button("Choose"))
            {
                {
                    out_img = emOperator(in_img, e, m);
                    writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                    show_output_image = true;
                    show_piecewise_dialog = false;
                }
            }
            ImGui::End();
        }

        if (show_thresh_dialog)
        {
            ImGui::Begin("Manual thresholding", &show_thresh_dialog);
            ImGui::SliderInt("Choose Threshold", &thresh, 0, 255);
            if (ImGui::Button("Choose"))
            {
                out_img = threshold(in_img, thresh);
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
                if (ImGui::Button("Save output image as input image"))
                {
                    in_img = out_img;
                    saveOutputAsInput(&input_image_texture, &input_image_width, &input_image_height, &input_image_channels, in_img);
                    // input_ret = loadTextureFromFile(input_path, &input_image_texture, &input_image_width, &input_image_height);

                }
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
    ImPlot::DestroyContext();
    ImGui::DestroyContext();


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

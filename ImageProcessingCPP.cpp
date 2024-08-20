#include "image_loader.h"
#include "implot.h"
#include "implot_internal.h"
#include "gui.h"
#include "ImGuiFileDialog.h"
#include "Image.h"
#include "Operations.h"
#include <vector>

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

    static ImU32 colorDataGray[1] = { 0xAAAAAAAA };
    static ImU32 colorDataRGB[3] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
    static ImU32 colorDataHSV[3] = { 0xFF00FFFF, 0xFFFFFF00, 0xFFFF00FF };
    int GrayMap = ImPlot::AddColormap("GrayColor", colorDataGray, 32);
    int RGBMap = ImPlot::AddColormap("RGBColors", colorDataRGB, 32);
    int HSVMap = ImPlot::AddColormap("HSVColors", colorDataHSV, 32);

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
    int output_image_channels = 3;

    Image in_img;
    Image out_img;

    static float size_coefficient = 1.0f;

    int padding_size = 1;
    int mirror_padding_size = 1;
    bool show_padding_dialog = false;
    bool show_mirror_dialog = false;

    // histogram

    unsigned int inChan1Histogram[256] = { 0 };
    unsigned int inChan2Histogram[256] = { 0 };
    unsigned int inChan3Histogram[256] = { 0 };
    unsigned int outChan1Histogram[256] = { 0 };
    unsigned int outChan2Histogram[256] = { 0 };
    unsigned int outChan3Histogram[256] = { 0 };
    bool show_in_histogram = false;
    bool show_out_histogram = false;

    // Pointwise ops

    int brightnessOffset = 0;
    bool show_brightness_dialog = false;

    float contrastFactor = 1.0f;
    bool show_contrast_dialog = false;

    bool show_linear_dialog = false;

    float gamma = 1.0f;
    bool show_gamma_dialog = false;

    int r1 = 1;
    int s1 = 1;
    int r2 = 254;
    int s2 = 254;
    bool show_piecewise_dialog = false;

    float e = 1.0f;
    float m = 1.0f;
    bool show_em_dialog = false;
    
    int splinePointsX[7];
    splinePointsX[0] = 0;
    for (size_t i = 1; i < 6; i++)
        splinePointsX[i] = -1;
    int splinePointsY[7];
    splinePointsY[0] = 0;
    for (size_t i = 1; i < 6; i++)
        splinePointsY[i] = -1;
    int splinePointsIndex = 1;
    float splineLine[256];
    for (size_t i = 0; i < 256; i++)
        splineLine[i] = i;
    bool show_spline_dialog = false;

    // thresholdings

    int thresh = 255;
    bool show_thresh_dialog = false;

    int thresh1 = 128;
    int thresh2 = 255;
    bool show_double_thresh_dialog = false;

    float minColor[3] = { 0.0f, 0.0f, 0.0f };
    float maxColor[3] = { 0.0f, 0.0f, 0.0f };
    bool show_color_thresh_dialog = false;

    float backgroundPercentage = 1.0f;
    bool show_quantile_dialog = false;

    int ksize = 1;
    bool show_bernsen_dialog = false;

    float color[3] = { 0.0f, 0.0f, 0.0f };
    int hue_range = 0;
    bool show_hsv_thresh_dialog = false;

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
                        if (ImGui::MenuItem("Convert to RGB"))
                        {
                            out_img = convert2RGB(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Convert to HSV"))
                        {
                            out_img = convert2HSV(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Invert"))
                        {
                            out_img = invert(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Zero paddding"))
                        {
                            show_padding_dialog = true;
                        }
                        if (ImGui::MenuItem("Mirror paddding"))
                        {
                            show_mirror_dialog = true;
                        }
                        if (show_input_image) // if show_input_image is true, it means in_img is not null
                        {
                            if (ImGui::MenuItem("Show input image histogram"))
                            {
                                memset(inChan1Histogram, 0, sizeof(inChan1Histogram));
                                memset(inChan2Histogram, 0, sizeof(inChan2Histogram));
                                memset(inChan3Histogram, 0, sizeof(inChan3Histogram));
                                getHistogram(in_img, 0, inChan1Histogram);
                                if (in_img.getType() != PixelType::GRAY)
                                {
                                    getHistogram(in_img, 1, inChan2Histogram);
                                    getHistogram(in_img, 2, inChan3Histogram);
                                }
                                show_in_histogram = true;
                            }
                        }
                        if (show_output_image) // it means out_img is not null
                        {
                            if (ImGui::MenuItem("Show output image histogram"))
                            {
                                memset(outChan1Histogram, 0, sizeof(outChan1Histogram));
                                memset(outChan2Histogram, 0, sizeof(outChan2Histogram));
                                memset(outChan3Histogram, 0, sizeof(outChan3Histogram));
                                getHistogram(out_img, 0, outChan1Histogram);
                                if (out_img.getType() != PixelType::GRAY)
                                {
                                    getHistogram(out_img, 1, outChan2Histogram);
                                    getHistogram(out_img, 2, outChan3Histogram);
                                }
                                show_out_histogram = true;
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
                        if (ImGui::MenuItem("Histogram stretching"))
                        {
                            out_img = histogramStretchingOperator(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Histogram equalization"))
                        {
                            out_img = histogramEqualizationOperator(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Thresholdings"))
                    {

                        if (ImGui::MenuItem("Manual"))
                        {
                            show_thresh_dialog = true;
                        }
                        if (ImGui::MenuItem("Manual double thresholding"))
                        {
                            show_double_thresh_dialog = true;
                        }
                        if (ImGui::MenuItem("Manual color thresholding"))
                        {
                            show_color_thresh_dialog = true;
                        }
                        if (ImGui::MenuItem("Mid range thresholding"))
                        {
                            out_img = midRangeThreshold(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Mean thresholding"))
                        {
                            out_img = meanThreshold(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Quantile thresholding"))
                        {
                            show_quantile_dialog = true;
                        }
                        if (ImGui::MenuItem("Intermeans (ISODATA) thresholding"))
                        {
                            out_img = intermeansThreshold(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Otsu thresholding"))
                        {
                            out_img = otsuThreshold(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Minimum error thresholding"))
                        {
                            out_img = minErrorThreshold(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Otsu two classes thresholding"))
                        {
                            out_img = otsuDoubleThreshold(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Maximum entropy thresholding"))
                        {
                            out_img = maxEntropyThresholding(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Triangle method thresholding"))
                        {
                            out_img = triangleThresholding(in_img);

                            writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                            show_output_image = true;
                        }
                        if (ImGui::MenuItem("Bernsen adaptive thresholding"))
                        {
                            show_bernsen_dialog = true;
                        }
                        if (ImGui::MenuItem("HSV thresholding"))
                        {
                            show_hsv_thresh_dialog = true;
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

        // histogram window

        if (show_in_histogram)
        {
            ImGui::Begin("Input histogram", &show_in_histogram);

            switch (in_img.getType())
            {
            case PixelType::GRAY:
                ImPlot::PushColormap(GrayMap);
                if (ImPlot::BeginPlot("Histogram"))
                {
                    ImPlot::PlotBars("Pixel values", inChan1Histogram, 256);
                    ImPlot::EndPlot();
                    ImPlot::PopColormap();
                }
                break;
            case PixelType::RGB:
                ImPlot::PushColormap(RGBMap);
                if (ImPlot::BeginPlot("RGB histogram"))
                {
                    ImPlot::PlotBars("Red channel values", inChan1Histogram, 256);
                    ImPlot::PlotBars("Green channel values", inChan2Histogram, 256);
                    ImPlot::PlotBars("Blue channel values", inChan3Histogram, 256);
                    ImPlot::EndPlot();
                    ImPlot::PopColormap();
                }
                break;
            case PixelType::HSV:
                ImPlot::PushColormap(HSVMap);
                if (ImPlot::BeginPlot("HSV histogram"))
                {
                    ImPlot::PlotBars("Hue", inChan1Histogram, 256);
                    ImPlot::PlotBars("Saturation", inChan2Histogram, 256);
                    ImPlot::PlotBars("Value", inChan3Histogram, 256);
                    ImPlot::EndPlot();
                    ImPlot::PopColormap();
                }
                break;
            default:
                ImPlot::PushColormap(RGBMap);
                if (ImPlot::BeginPlot("Multiple channel histogram"))
                {
                    ImPlot::PlotBars("Red channel values", inChan1Histogram, 256);
                    ImPlot::PlotBars("Green channel values", inChan2Histogram, 256);
                    ImPlot::PlotBars("Blue channel values", inChan3Histogram, 256);
                    ImPlot::EndPlot();
                    ImPlot::PopColormap();
                }
                break;
            }
            ImGui::End();
        }
        if (show_out_histogram)
        {
            ImGui::Begin("Output histogram", &show_out_histogram);

            switch (out_img.getType())
            {
            case PixelType::GRAY:
                ImPlot::PushColormap(GrayMap);
                if (ImPlot::BeginPlot("Histogram"))
                {
                    ImPlot::PlotBars("Pixel values", outChan1Histogram, 256);
                    ImPlot::EndPlot();
                    ImPlot::PopColormap();
                }
                break;
            case PixelType::RGB:
                ImPlot::PushColormap(RGBMap);
                if (ImPlot::BeginPlot("RGB histogram"))
                {
                    ImPlot::PlotBars("Red channel values", outChan1Histogram, 256);
                    ImPlot::PlotBars("Green channel values", outChan2Histogram, 256);
                    ImPlot::PlotBars("Blue channel values", outChan3Histogram, 256);
                    ImPlot::EndPlot();
                    ImPlot::PopColormap();
                }
                break;
            case PixelType::HSV:
                ImPlot::PushColormap(HSVMap);
                if (ImPlot::BeginPlot("HSV histogram"))
                {
                    ImPlot::PlotBars("Hue", outChan1Histogram, 256);
                    ImPlot::PlotBars("Saturation", outChan2Histogram, 256);
                    ImPlot::PlotBars("Value", outChan3Histogram, 256);
                    ImPlot::EndPlot();
                    ImPlot::PopColormap();
                }
                break;
            default:
                ImPlot::PushColormap(RGBMap);
                if (ImPlot::BeginPlot("Multiple channel histogram"))
                {
                    ImPlot::PlotBars("Red channel values", outChan1Histogram, 256);
                    ImPlot::PlotBars("Green channel values", outChan2Histogram, 256);
                    ImPlot::PlotBars("Blue channel values", outChan3Histogram, 256);
                    ImPlot::EndPlot();
                    ImPlot::PopColormap();
                }
                break;
            }
            ImGui::End();
        }

        // dialogs

        if (show_path_selector)
        {
            drawPathSelector(&input_path, &show_path_selector);
        }
        if (show_padding_dialog)
        {
            ImGui::Begin("Image zero padding", &show_padding_dialog);
            ImGui::InputInt("Choose padding size", &padding_size);
            if (ImGui::Button("Choose") && padding_size > 0)
            {
                out_img = zeroPadding(in_img, padding_size);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_padding_dialog = false;
            }
            ImGui::End();
        }
        if (show_mirror_dialog)
        {
            ImGui::Begin("Image mirror padding", &show_mirror_dialog);
            ImGui::SliderInt("Choose padding size", &mirror_padding_size, 0, MIN(input_image_width, input_image_height));
            if (ImGui::Button("Choose") && mirror_padding_size > 0)
            {
                out_img = mirrorPadding(in_img, mirror_padding_size);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_mirror_dialog = false;
            }
            ImGui::End();
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
                out_img = piecewiseLinearContrast(in_img, r1, s1, r2, s2);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_piecewise_dialog = false;
            }
            ImGui::End();
        }
        if (show_em_dialog)
        {
            ImGui::Begin("EM contrast", &show_em_dialog);
            ImGui::InputFloat("e", &e);
            ImGui::SliderFloat("m", &m, 0, 255);
            if (ImGui::Button("Choose"))
            {
                out_img = emOperator(in_img, e, m);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_em_dialog = false;
            }
            ImGui::End();
        }
        if (show_spline_dialog)
        {
            ImPlot::SetNextAxesLimits(0, 255, 0, 255, ImPlotCond_Always);
            ImGui::Begin("Spline tool", &show_spline_dialog);
            if (ImPlot::BeginPlot("Choose points"))
            {
                if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && splinePointsIndex < 6)
                {
                    ImPlotPoint mousePos = ImPlot::GetPlotMousePos();

                    if (splinePointsIndex == 0 || mousePos.x > splinePointsX[splinePointsIndex - 1])
                    {
                        splinePointsX[splinePointsIndex] = (int)mousePos.x;
                        splinePointsY[splinePointsIndex] = (int)mousePos.y;

                        splinePointsIndex++;
                    }
                    if (splinePointsIndex == 6)
                    {
                        splinePointsX[6] = 255;
                        splinePointsY[6] = 255;
                    }

                }

                ImPlot::PlotScatter("Pinned Points", splinePointsX, splinePointsY, 7);
                ImPlot::EndPlot();
            }
            if (ImGui::Button("Choose"))
            {
                out_img = splineOperator(in_img, splinePointsX, splinePointsY);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_spline_dialog = false;

                splinePointsX[0] = 0;
                for (size_t i = 1; i < 6; i++)
                    splinePointsX[i] = -1;
                splinePointsY[0] = 0;
                for (size_t i = 1; i < 6; i++)
                    splinePointsY[i] = -1;

                for (size_t i = 0; i < 256; i++)
                    splineLine[i] = i;
                splinePointsIndex = 1;
            }
            ImGui::End();
        }

        // thresholdings

        if (show_thresh_dialog)
        {
            ImGui::Begin("Manual thresholding", &show_thresh_dialog);
            ImGui::SliderInt("Choose Threshold", &thresh, 0, 255);
            if (ImGui::Button("Apply"))
            {
                out_img = threshold(in_img, thresh);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_thresh_dialog = false;
            }
            ImGui::End();
        }
        if (show_double_thresh_dialog)
        {
            ImGui::Begin("Manual thresholding", &show_double_thresh_dialog);
            ImGui::SliderInt("Choose first threshold", &thresh1, 0, thresh2 - 1);
            ImGui::SliderInt("Choose second threshold", &thresh2, thresh1 + 1, 255);
            if (ImGui::Button("Apply"))
            {
                out_img = doubleThreshold(in_img, thresh1, thresh2);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_double_thresh_dialog = false;
            }
            ImGui::End();
        }
        if (show_color_thresh_dialog)
        {
            ImGui::Begin("Manual thresholding", &show_color_thresh_dialog);
            ImGui::ColorEdit3("Min color", minColor);
            ImGui::ColorEdit3("Max color", maxColor);
            if (ImGui::Button("Apply"))
            {
                out_img = colorThreshold(in_img, (unsigned char)(minColor[0] * 255), (unsigned char)(minColor[1] * 255), (unsigned char)(minColor[2] * 255), (unsigned char)(maxColor[0] * 255), (unsigned char)(maxColor[1] * 255), (unsigned char)(maxColor[2] * 255));
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_color_thresh_dialog = false;
            }
            ImGui::End();
        }
        if (show_quantile_dialog)
        {
            ImGui::Begin("Quantile Thresholding", &show_quantile_dialog);
            ImGui::SliderFloat("Choose background percentage", &backgroundPercentage, 0, 1);
            if (ImGui::Button("Apply"))
            {
                out_img = quantileThreshold(in_img, backgroundPercentage);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_quantile_dialog = false;
            }
            ImGui::End();
        }
        if (show_bernsen_dialog)
        {
            ImGui::Begin("Bernsen adaptive thresholding", &show_bernsen_dialog);
            ImGui::InputInt("Choose mask sizes", &ksize, 2);
            //ImGui::SliderInt("Choose mask size", &ksize, 1, 999);
            if (ImGui::Button("Apply") && ksize % 2 == 1 && ksize > 0 && ksize < 999)
            {
                out_img = bernsenThresholding(in_img, ksize);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_bernsen_dialog = false;
            }
            ImGui::End();
        }
        if (show_hsv_thresh_dialog)
        {
            ImGui::Begin("hsv thresholding", &show_hsv_thresh_dialog);
            ImGui::ColorEdit3("Color", color);
            ImGui::SliderInt("Hue Range", &hue_range, 0, 255);
            if (ImGui::Button("Apply"))
            {
                out_img = hsvThresholding(in_img, (unsigned char)(maxColor[0] * 255), (unsigned char)(maxColor[1] * 255), (unsigned char)(maxColor[2] * 255), (unsigned char)hue_range);
                writeAndDisplayOutput(&output_image_texture, &output_image_width, &output_image_height, &output_image_channels, out_img);
                show_output_image = true;
                show_hsv_thresh_dialog = false;
            }
            ImGui::End();
        }

        // Image display

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

        ImGuiIO io = ImGui::GetIO();
        {
            ImGui::SetNextWindowPos(ImVec2(0.0f, 18.0f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, 82.0f), ImGuiCond_Always);
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoCollapse;
            window_flags |= ImGuiWindowFlags_NoMove;

            ImGui::Begin("Menu", nullptr, window_flags);
            ImGui::SliderFloat("resize  (VISUAL ONLY)", &size_coefficient, 0.1f, 10.0f);
            if (ImGui::Button("Choose file"))
            {
                show_path_selector = true;
            }
            if (input_path != nullptr)
            {
                if (ImGui::Button("Load image"))
                {
                    input_ret = loadTextureFromFile(input_path, &input_image_texture, &input_image_width, &input_image_height);
                    in_img.loadImage(input_path, 3);

                    IM_ASSERT(input_ret);
                    show_input_image = true;
                    show_imgproc_operations = true;
                }
                ImGui::SameLine();
                ImGui::Text("file path: %s", input_path);
            }
            if (show_output_image)
            {
                ImGui::SameLine(io.DisplaySize.x - 240.0f);
                if (ImGui::Button("Save output image as input image"))
                {
                    in_img = out_img;
                    saveOutputAsInput(&input_image_texture, &input_image_width, &input_image_height, &input_image_channels, in_img);
                }
            }
            ImGui::End();
        }
        {
            ImGui::SetNextWindowPos(ImVec2(0.0f, 100.0f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 2.0f, io.DisplaySize.y - 100.0f), ImGuiCond_Always);
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoCollapse;
            window_flags |= ImGuiWindowFlags_NoMove;
            ImGui::Begin("Input Image", nullptr, window_flags);
            ImGui::Text("size = %d x %d", input_image_width, input_image_height);

            if (show_input_image)
            {
                ImGui::Image((void*)(intptr_t)input_image_texture, ImVec2(input_image_width * size_coefficient, input_image_height * size_coefficient));
            }
            ImGui::End();
        }
        {
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x / 2.0f, 100.0f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x / 2.0f, io.DisplaySize.y - 100.0f), ImGuiCond_Always);
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoCollapse;
            window_flags |= ImGuiWindowFlags_NoMove;
            ImGui::Begin("Output Image", nullptr, window_flags);
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
    ImPlot::DestroyContext();
    ImGui::DestroyContext();


    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

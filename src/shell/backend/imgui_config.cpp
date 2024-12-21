#include <backend/imgui.h>

#include <imgui/imgui.h>

#include <string>

#include <backend/font.h>
#include <hydra/config.h>

namespace hydra::shell {
  void ImGuiLoadConfig(ImGuiStyle& style) {
    ImGuiIO& io = ImGui::GetIO();

    io.IniFilename = nullptr;

    std::string font_path = *FontConfig::get().match_font(std::string(Config::Get().FONT));
    io.Fonts->AddFontFromFileTTF(font_path.c_str(), Config::Get().FONT_HEIGHT);

    // Bootstrap Dark style by Madam-Herta from ImThemes
    style.Alpha = Config::Get().ALPHA;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 4.199999809265137f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 7.300000190734863f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 10.19999980926514f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 4.0f;
    style.TabBorderSize = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4431372582912445f, 0.4431372582912445f, 0.4431372582912445f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.196078434586525f, 0.2274509817361832f, 0.1411764770746231f, 0.8500000238418579f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1098039224743843f, 0.1098039224743843f, 0.1372549086809158f, 0.9200000166893005f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 0.5f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4274509847164154f, 0.3899999856948853f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.07450980693101883f, 0.7568627595901489f, 0.572549045085907f, 0.4000000059604645f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2784313857555389f, 1.0f, 0.7490196228027344f, 0.6899999976158142f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.3882353007793427f, 0.5333333611488342f, 0.3411764800548553f, 0.8299999833106995f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.3019607961177826f, 0.572549045085907f, 0.2980392277240753f, 0.8700000047683716f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.4392156898975372f, 0.800000011920929f, 0.4000000059604645f, 0.2000000029802322f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2901960909366608f, 0.3803921639919281f, 0.2745098173618317f, 0.800000011920929f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2980392277240753f, 0.6000000238418579f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3921568691730499f, 0.9137254953384399f, 0.5843137502670288f, 0.300000011920929f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645f, 0.800000011920929f, 0.5803921818733215f, 0.4000000059604645f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3882353007793427f, 0.800000011920929f, 0.4117647111415863f, 0.6000000238418579f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5137255191802979f, 0.5882353186607361f, 0.3372549116611481f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.239215686917305f, 0.8784313797950745f, 0.4941176474094391f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.196078434586525f, 0.4901960790157318f, 0.2588235437870026f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.5254902243614197f, 0.8039215803146362f, 0.6470588445663452f, 0.6200000047683716f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3960784375667572f, 0.6352941393852234f, 0.47843137383461f, 0.7900000214576721f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1764705926179886f, 0.2862745225429535f, 0.168627455830574f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.3882353007793427f, 0.501960813999176f, 0.3098039329051971f, 0.449999988079071f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4705882370471954f, 0.6392157077789307f, 0.5333333611488342f, 0.800000011920929f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.5686274766921997f, 0.7843137383460999f, 0.5607843399047852f, 0.800000011920929f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 0.6000000238418579f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.2078431397676468f, 0.3294117748737335f, 0.2705882489681244f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.294117659330368f, 0.4509803950786591f, 0.3764705955982208f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.1000000014901161f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4156862795352936f, 0.9921568632125854f, 0.5843137502670288f, 0.6000000238418579f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.3568627536296844f, 0.5372549295425415f, 0.43529412150383f, 0.8999999761581421f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.2352941185235977f, 0.3450980484485626f, 0.2666666805744171f, 0.7860000133514404f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1098039224743843f, 0.2313725501298904f, 0.1294117718935013f, 0.800000011920929f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.5764706134796143f, 1.0f, 0.5803921818733215f, 0.3819742202758789f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.2274509817361832f, 0.4039215743541718f, 0.2235294133424759f, 0.8212000131607056f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.3960784375667572f, 0.772549033164978f, 0.5921568870544434f, 0.8371999859809875f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.2823529541492462f, 0.3764705955982208f, 0.2666666805744171f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.4470588266849518f, 0.3254902064800262f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2588235437870026f, 0.2588235437870026f, 0.2784313857555389f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.07000000029802322f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2627451121807098f, 0.6352941393852234f, 0.364705890417099f, 0.3499999940395355f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.3686274588108063f, 0.501960813999176f, 0.2431372553110123f, 0.800000011920929f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3499999940395355f);
  }
}

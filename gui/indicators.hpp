#ifndef INDICATORS_HPP
#define INDICATORS_HPP

#include <cstdio>
#include "config.hpp"

#include "../imgui/dearimgui.hpp"

#include "../hacks/tickbase/tickbase.hpp"
#include "../hacks/spectator_list/spectator_list.hpp"

static void draw_tickbase_indicator(void) {
  if (config.misc.exploits.tickbase_indicator == false || config.misc.exploits.tickbase == false) return;

  ImGuiStyle* style = &ImGui::GetStyle();
  style->WindowTitleAlign = ImVec2(0.5f, 0.5f);

  ImGui::SetNextWindowSize(ImVec2(235, 68), ImGuiCond_Always);
  if (ImGui::Begin("Command Queue Telemetry", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
    const int max_ticks = 24;
    int current_ticks = choked_ticks;
    float fraction = (float)current_ticks / (float)max_ticks;
    if (fraction < 0.0f) fraction = 0.0f;
    if (fraction > 1.0f) fraction = 1.0f;

    char overlay_buf[36];
    std::snprintf(overlay_buf, sizeof(overlay_buf), "%d / %d Buffered Ticks", current_ticks, max_ticks);

    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.52f, 0.88f, 0.76f, 0.85f));
    ImGui::ProgressBar(fraction, ImVec2(-1.0f, 22.0f), overlay_buf);
    ImGui::PopStyleColor();
  }

  ImGui::End();
}

#endif

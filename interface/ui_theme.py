import dearpygui.dearpygui as dpg

#creating theme
def applyCustomTheme(): 
    with dpg.theme() as blue_theme:
        with dpg.theme_component(dpg.mvAll):
            # Backgrounds and base
            dpg.add_theme_color(dpg.mvThemeCol_WindowBg, (15, 40, 60), category=dpg.mvThemeCat_Core)        # Deep blue window bg
            dpg.add_theme_color(dpg.mvThemeCol_ChildBg, (25, 55, 85), category=dpg.mvThemeCat_Core)         # Child window bg
            dpg.add_theme_color(dpg.mvThemeCol_PopupBg, (25, 55, 85), category=dpg.mvThemeCat_Core)
            
            # Text
            dpg.add_theme_color(dpg.mvThemeCol_Text, (240, 255, 255), category=dpg.mvThemeCat_Core)         # Near white
            dpg.add_theme_color(dpg.mvThemeCol_TextDisabled, (180, 180, 180), category=dpg.mvThemeCat_Core)

            # Buttons
            dpg.add_theme_color(dpg.mvThemeCol_Button, (30, 80, 120), category=dpg.mvThemeCat_Core)         # Blue button
            dpg.add_theme_color(dpg.mvThemeCol_ButtonHovered, (40, 100, 140), category=dpg.mvThemeCat_Core)
            dpg.add_theme_color(dpg.mvThemeCol_ButtonActive, (20, 60, 100), category=dpg.mvThemeCat_Core)

            # Borders & Highlights
            dpg.add_theme_color(dpg.mvThemeCol_Border, (50, 100, 160), category=dpg.mvThemeCat_Core)
            dpg.add_theme_color(dpg.mvThemeCol_FrameBg, (20, 60, 100), category=dpg.mvThemeCat_Core)
            dpg.add_theme_color(dpg.mvThemeCol_FrameBgHovered, (35, 90, 130), category=dpg.mvThemeCat_Core)
            dpg.add_theme_color(dpg.mvThemeCol_FrameBgActive, (15, 45, 75), category=dpg.mvThemeCat_Core)

            # Scrollbars and sliders
            dpg.add_theme_color(dpg.mvThemeCol_ScrollbarBg, (15, 35, 50), category=dpg.mvThemeCat_Core)
            dpg.add_theme_color(dpg.mvThemeCol_SliderGrab, (70, 200, 140), category=dpg.mvThemeCat_Core)     # Green accent
            dpg.add_theme_color(dpg.mvThemeCol_SliderGrabActive, (90, 255, 160), category=dpg.mvThemeCat_Core)

            # Checkboxes and progress bars
            dpg.add_theme_color(dpg.mvThemeCol_CheckMark, (100, 255, 150), category=dpg.mvThemeCat_Core)

            # Style
            dpg.add_theme_style(dpg.mvStyleVar_FrameRounding, 6, category=dpg.mvThemeCat_Core)
            dpg.add_theme_style(dpg.mvStyleVar_WindowPadding, 10, 10, category=dpg.mvThemeCat_Core)
        with dpg.theme_component(dpg.mvChildWindow):
            dpg.add_theme_style(dpg.mvStyleVar_ChildRounding, 12)


    dpg.bind_theme(blue_theme)

def loadFonts():
    #setting font and font sizes
    with dpg.font_registry():
        DEFAULT_FONT = dpg.add_font("assets/Roboto/static/Roboto-Regular.ttf", 18)
        LARGE_FONT   = dpg.add_font("assets/Roboto/static/Roboto-Regular.ttf", 22)

        dpg.bind_font(DEFAULT_FONT)

    return DEFAULT_FONT, LARGE_FONT
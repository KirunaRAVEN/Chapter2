import dearpygui.dearpygui as dpg
from ui_interface import buildInterface
from ui_theme import applyCustomTheme, loadFonts
from telemetry_manager import updateFrame
dpg.create_context()

#loading the theme and fonts
applyCustomTheme()
DEFAULT_FONT, LARGE_FONT = loadFonts()

dpg.create_viewport(title="Raven Interface", width=1920, height=1080)

# Window
with dpg.window(label="Sensor Plots", tag="main_window", width=1920, height=1080):

    buildInterface(LARGE_FONT)
    
# Setup and start
dpg.setup_dearpygui()
dpg.set_primary_window("main_window", True)
dpg.show_viewport()

# Main loop

while dpg.is_dearpygui_running():
    updateFrame()
    dpg.render_dearpygui_frame()

dpg.destroy_context()

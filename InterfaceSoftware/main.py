import dearpygui.dearpygui as dpg
from ui_interface import build_fueling_interface
from ui_theme import apply_custom_theme, load_fonts
from data_updates import update_frame
import time
dpg.create_context()

#loading the theme and fonts
apply_custom_theme()
default_font, Large_font = load_fonts()

dpg.create_viewport(title="Raven Interface", width=1920, height=1080)

# Window
with dpg.window(label="Sensor Plots", tag="main_window", width=1920, height=1080):

    build_fueling_interface(Large_font)
    
# Setup and start
dpg.setup_dearpygui()
dpg.set_primary_window("main_window", True)
dpg.show_viewport()

# Main loop
target_frame_time = 1.0 / 60.0 

while dpg.is_dearpygui_running():
    start = time.perf_counter()
    update_frame()
    dpg.render_dearpygui_frame()

dpg.destroy_context()

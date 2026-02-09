import dearpygui.dearpygui as dpg
from ui_common import addDataTable

#function to build the entire interface
def buildInterface(largeFont):
    with dpg.group(tag="Interface", show=True):
        with dpg.group(horizontal=True):
            buildOx1TempPanel()
            buildOx1PressurePanel()
            buildDataDisplayPanel(largeFont)
            buildSoftwareLogPanel(largeFont)
        
        with dpg.group(horizontal=True):
            buildOx2TempPanel()
            buildOx2PressurePanel()
            with dpg.group():
                buildIndicatorPanel(largeFont)
                buildSoftwareModePanel(largeFont)
            with dpg.group():
                buildHeatingHelperPanel(largeFont)
                buildSoftwareSubstratePanel(largeFont)
        with dpg.group(horizontal=True):
            buildLinePressurePanel()
            buildN2PressurePanel()
            buildPipingPanel(largeFont)

# building the temp panel for ox1
def buildOx1TempPanel():
    with dpg.group():
        with dpg.plot(label="Ox. Bottle 1 Temperature", width=600, height=320, tag="Ox1Temp"):
            
            # X-axis: time 
            with dpg.plot_axis(dpg.mvXAxis, label="Time [s]", tag="Ox1TempXAxis", no_gridlines=True, no_tick_marks=True, no_tick_labels=True):
                pass

            # Y-axis: temperature
            with dpg.plot_axis(dpg.mvYAxis, label="Temperature [°C]", tag="Ox1TempYAxis") as y_axis:
                dpg.set_axis_limits(y_axis, -15, 40)
                dpg.add_line_series([], [], label="Ox. Bottle 1 Temp", tag="Ox1TempPlot")


# building the temp panel for ox2
def buildOx2TempPanel():
    with dpg.group():
        with dpg.plot(label="Ox. Bottle 2 Temperature", width=600, height=320, tag="Ox2Temp"):
            
            # X-axis: time 
            with dpg.plot_axis(dpg.mvXAxis, label="Time [s]", tag="Ox2TempXAxis", no_gridlines=True, no_tick_marks=True, no_tick_labels=True):
                pass  

            # Y-axis: temperature
            with dpg.plot_axis(dpg.mvYAxis, label="Temperature [°C]", tag="Ox2TempYAxis") as y_axis:
                dpg.set_axis_limits(y_axis, -15, 40)
                dpg.add_line_series([], [], label="Ox. Bottle 2 Temp", tag="Ox2TempPlot")


# building the ox 1 pressure panel
def buildOx1PressurePanel():
    with dpg.group():
        with dpg.plot(label="Ox. Bottle 1 Pressure", width=600, height=320, tag="Ox1Pressure"):

            # X-axis: time 
            with dpg.plot_axis(dpg.mvXAxis, label="Time [s]", tag="Ox1PresXAxis", no_gridlines=True, no_tick_marks=True, no_tick_labels=True):
                pass  
            
             #  Y-axis: Pressure
            with dpg.plot_axis(dpg.mvYAxis, label="Pressure [Bar]", tag="Ox1PressYAxis") as y_axis:
                dpg.set_axis_limits(y_axis, 0, 80)
                dpg.add_line_series([], [], label="Ox. bottle 1 Pressure", tag="Ox1PressurePlot")

# building the ox 2 pressure panel
def buildOx2PressurePanel():
    with dpg.group():
        with dpg.plot(label="Ox. Bottle 2 Pressure", width=600, height=320, tag="Ox2Pressure"):
            
            # X-axis: time 
            with dpg.plot_axis(dpg.mvXAxis, label="Time [s]", tag="Ox2PresXAxis", no_gridlines=True, no_tick_marks=True, no_tick_labels=True):
                pass  
            
             #  Y-axis: Pressure
            with dpg.plot_axis(dpg.mvYAxis, label="Pressure [Bar]", tag="Ox2PressYAxis") as y_axis:
                dpg.set_axis_limits(y_axis, 0, 80)
                dpg.add_line_series([], [], label="Ox. bottle 2 Pressure", tag="Ox2PressurePlot")

# building the line pressure panel
def buildLinePressurePanel():
    with dpg.group():
        with dpg.plot(label="Line Pressure", width=600, height=320, tag="LinePressure"):
            
            # X-axis: time 
            with dpg.plot_axis(dpg.mvXAxis, label="Time [s]", tag="LinePresXAxis", no_gridlines=True, no_tick_marks=True, no_tick_labels=True):
                pass  
            
             #  Y-axis: Pressure
            with dpg.plot_axis(dpg.mvYAxis, label="Pressure [Bar]", tag="LinePressYAxis") as y_axis:
                dpg.set_axis_limits(y_axis, 0, 80)
                dpg.add_line_series([], [], label="Line Pressure", tag="LinePressurePlot")

# building the N2 pressure panel
def buildN2PressurePanel():
    with dpg.group():
        with dpg.plot(label="Nitrogen Pressure", width=600, height=320, tag="N2Pressure"):
            
            # X-axis: time 
            with dpg.plot_axis(dpg.mvXAxis, label="Time [s]", tag="N2PresXAxis", no_gridlines=True, no_tick_marks=True, no_tick_labels=True):
                pass  
            
             #  Y-axis: Pressure
            with dpg.plot_axis(dpg.mvYAxis, label="Pressure [Bar]", tag="N2PressYAxis") as y_axis:
                dpg.set_axis_limits(y_axis, 0, 80)
                dpg.add_line_series([], [], label="Nitrogen Pressure", tag="N2PressurePlot")

# building the piping panel
def buildPipingPanel(largeFont):
    with dpg.group():
        pipingDiagramTitle = dpg.add_text("Piping Diagram")
        dpg.bind_item_font(pipingDiagramTitle, largeFont) 
        with dpg.group(horizontal=True):
            with dpg.group():
                with dpg.drawlist(width=400, height=300, tag="piping_diagram"):

                    # Line Pressure - vertical center line
                    dpg.draw_line((200, 60), (200, 200), color=(128, 128, 128, 255), thickness=6, tag="line_pressure_line")
                    dpg.draw_circle(center=(200, 60), radius=6, color=(200, 200, 200, 255), fill=(128, 128, 128, 255))
                    dpg.draw_text((210, 40), "Line Pressure", size=14, color=(200, 255, 255, 255))

                    # Ox. Bottle 1 Pressure - left horizontal
                    dpg.draw_line((50, 125), (200, 125), color=(128, 128, 128, 255), thickness=6, tag="ox1_pressure_line")
                    dpg.draw_circle(center=(50, 125), radius=6, color=(200, 200, 200, 255), fill=(128, 128, 128, 255))
                    dpg.draw_text((50, 90), "Ox. Bottle 1\nPressure", size=14, color=(200, 255, 255, 255))

                    # Ox. Bottle 2 Pressure - downward vertical
                    dpg.draw_line((200, 125), (200, 250), color=(128, 128, 128, 255), thickness=6, tag="ox2_pressure_line")
                    dpg.draw_circle(center=(200, 250), radius=6, color=(200, 200, 200, 255), fill=(128, 128, 128, 255))
                    dpg.draw_text((210, 240), "Ox. Bottle 2\nPressure", size=14, color=(200, 255, 255, 255))

                    # N2 Pressure - right horizontal
                    dpg.draw_line((200, 125), (350, 125), color=(128, 128, 128, 255), thickness=6, tag="n2_pressure_line")
                    dpg.draw_circle(center=(350, 125), radius=6, color=(200, 200, 200, 255), fill=(128, 128, 128, 255))
                    dpg.draw_text((280, 100), "N2 Pressure", size=14, color=(200, 255, 255, 255))

                    dpg.draw_circle(center=(200, 125), radius=6, color=(200, 200, 200, 255), fill=(128, 128, 128, 255))

            with dpg.group():
                # Output message
                dpg.add_text("", tag="pipe_warning_msg")

# building the sw panel
def buildSoftwareLogPanel(largeFont):
    with dpg.group():
        with dpg.child_window(tag = "MainSWLog", auto_resize_x=True, auto_resize_y=True):
            with dpg.group():
                softwareLogTitle = dpg.add_text("Software Log")
                dpg.bind_item_font(softwareLogTitle, largeFont) 
                dpg.add_input_text(tag="software_log", multiline=True, readonly=True, default_value="", width=300, height=267)

# building the data display
def buildDataDisplayPanel(largeFont):
    with dpg.child_window(tag="DataBox1", border=True, auto_resize_x=True, auto_resize_y=True):
        with dpg.group(horizontal=True):
            with dpg.group():
                addDataTable("Data Display:", {
                    "Ox. bottle 1 Pressure:": "N/A bar", "Ox. bottle 2 Pressure:": "N/A bar", "Line Pressure:": "N/A bar", "Nitrogen Pressure:": "N/A bar", "Chamber Pressure:": "N/A bar", "Plume Temperature:": "N/A °C", "Nozzle Temperature:": "N/A °C", "Load Cell:": "N/A N", "Mega Time:": "N/A s"
                }, largeFont)

def buildSoftwareModePanel(largeFont):
    with dpg.child_window(tag="ModeBox1", border=True, auto_resize_x=True, auto_resize_y=True):
        modeTitle = dpg.add_text("Software Mode:")
        with dpg.child_window(tag="SoftwareModeDisplay", border=True, width=180, auto_resize_y=True):
            modeDisplay = dpg.add_text("Current Mode",tag="softwareModeText")
            dpg.bind_item_font(modeDisplay, largeFont)

def buildSoftwareSubstratePanel(largeFont):
    with dpg.child_window(tag="SubstrateBox1", border=True, auto_resize_x=True, auto_resize_y=True):
        substrateTitle = dpg.add_text("Software Substrate:")
        with dpg.child_window(tag="SoftwareSubstrateDisplay", border=True, width=200, auto_resize_y=True):
            substrateDisplay = dpg.add_text("Current Substrate",tag="softwareSubstrateText")
            dpg.bind_item_font(substrateDisplay, largeFont)


def buildIndicatorPanel(largeFont):
    with dpg.child_window(tag="RelayBox1", border=True, auto_resize_x=True, auto_resize_y=True):
        relayTitle = dpg.add_text("Relays:")
        dpg.bind_item_font(relayTitle, largeFont)
        with dpg.table(header_row=False, resizable=False, borders_innerH=False, borders_outerH=False,
                       borders_innerV=False, borders_outerV=False, policy=dpg.mvTable_SizingFixedFit):

            dpg.add_table_column()
            dpg.add_table_column()
            dpg.add_table_column()

            # Row 1
            with dpg.table_row():
                dpg.add_text("Heating Relay 1:", tag="HeatingRelay1")
                dpg.add_text("ON", tag="HeatingRelay1StateText")
                with dpg.drawlist(width=30, height=30):
                    dpg.draw_circle(center=(15, 15), radius=10,
                                    color=(0, 255, 0, 255), fill=(0, 255, 0, 255),
                                    tag="HeatingRelay1StatusCircle")

            # Row 2
            with dpg.table_row():
                dpg.add_text("Heating Relay 2:", tag="HeatingRelay2")
                dpg.add_text("ON", tag="HeatingRelay2StateText")
                with dpg.drawlist(width=30, height=30):
                    dpg.draw_circle(center=(15, 15), radius=10,
                                    color=(0, 255, 0, 255), fill=(0, 255, 0, 255),
                                    tag="HeatingRelay2StatusCircle")

            # Row 3
            with dpg.table_row():
                dpg.add_text("Ox. Valve 1:", tag="OxRelay1")
                dpg.add_text("ON", tag="OxRelay1StateText")
                with dpg.drawlist(width=30, height=30):
                    dpg.draw_circle(center=(15, 15), radius=10,
                                    color=(0, 255, 0, 255), fill=(0, 255, 0, 255),
                                    tag="OxRelay1StatusCircle")

            # Row 4
            with dpg.table_row():
                dpg.add_text("Ox. Valve 2:", tag="OxRelay2")
                dpg.add_text("ON", tag="OxRelay2StateText")
                with dpg.drawlist(width=30, height=30):
                    dpg.draw_circle(center=(15, 15), radius=10,
                                    color=(0, 255, 0, 255), fill=(0, 255, 0, 255),
                                    tag="OxRelay2StatusCircle")

            # Row 5
            with dpg.table_row():
                dpg.add_text("Ignition Relay:", tag="IgnRelay")
                dpg.add_text("ON", tag="IgnRelayStateText")
                with dpg.drawlist(width=30, height=30):
                    dpg.draw_circle(center=(15, 15), radius=10,
                                    color=(0, 255, 0, 255), fill=(0, 255, 0, 255),
                                    tag="IgnRelayStatusCircle")

def buildHeatingHelperPanel(largeFont):
    with dpg.child_window(tag="HeatHelpBox1", border=True, auto_resize_x=True, auto_resize_y=True):
        heatingTitle = dpg.add_text("Heating Helper:")
        dpg.bind_item_font(heatingTitle, largeFont)
        with dpg.table(header_row=False, resizable=False, borders_innerH=False, borders_outerH=False,
                borders_innerV=False, borders_outerV=False, policy=dpg.mvTable_SizingFixedFit):
            
            dpg.add_table_column()
            dpg.add_table_column()
            dpg.add_table_column()
            dpg.add_table_column()
            dpg.add_table_column()
            dpg.add_table_column()

            with dpg.table_row():
                dpg.add_text("Ox. bottle 1 Temperature:", tag="Ox1TempForPrintout")
                dpg.add_text("25.0 °C", tag="Ox1TempPrintout")

            with dpg.table_row():
                dpg.add_text("Ox. bottle 2 Temperature:", tag="Ox2TempForPrintout")
                dpg.add_text("25.0 °C", tag="Ox2TempPrintout")

            with dpg.table_row():
                dpg.add_text("Assumed Pressure in Ox. Bottle 1:", tag="AssumedPressureOx1")
                dpg.add_text("10 Bar", tag="AssumedPressureOx1PrintOut")

            with dpg.table_row():
                dpg.add_text("Assumed Pressure in Ox. Bottle 2:", tag="AssumedPressureOx2")
                dpg.add_text("10 Bar", tag="AssumedPressureOx2PrintOut")

            with dpg.table_row():
                dpg.add_text("Delta Temperature Ox. Bottle 1:", tag="DeltaTempOx1")
                dpg.add_text("5 °C/min", tag="DeltaTempOx1PrintOut")

            with dpg.table_row():
                dpg.add_text("Delta Temperature Ox. Bottle 2:", tag="DeltaTempOx2")
                dpg.add_text("5 °C/min", tag="DeltaTempOx2PrintOut")

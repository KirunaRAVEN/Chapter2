import dearpygui.dearpygui as dpg


#a function for creating data displays for the interface
def addDataTable(title, dataDict, largeFont):
    # Creates a border around the data display
    with dpg.child_window(width=300, height=300, border=True, no_scrollbar=True, no_scroll_with_mouse=True):
        # Adding the title to the data display
        titleWidget = dpg.add_text(title)
        dpg.bind_item_font(titleWidget, largeFont)  
        # Creating the table with two columns one for names and one for data
        with dpg.table(header_row=False, borders_innerH=True, borders_innerV=True, 
                       policy=dpg.mvTable_SizingFixedFit):
            dpg.add_table_column()
            dpg.add_table_column()
            # Add one row per entry in the provided data dict
            for label, value in dataDict.items():
                with dpg.table_row():
                    dpg.add_text(label) # First column lable
                    dpg.add_text(value, tag=f"{title}_{label}".lower().replace(" ", "_")) # Second column data
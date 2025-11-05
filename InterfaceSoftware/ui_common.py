import dearpygui.dearpygui as dpg


#a function for creating data displays for the interface
def add_data_table(title, data_dict, Large_font):
    # Creates a border around the data display
    with dpg.child_window(width=300, height=300, border=True, no_scrollbar=True, no_scroll_with_mouse=True):
        # Adding the title to the data display
        Title = dpg.add_text(title)
        dpg.bind_item_font(Title, Large_font)  
        # Creating the table with two columns one for names and one for data
        with dpg.table(header_row=False, borders_innerH=True, borders_innerV=True, 
                       policy=dpg.mvTable_SizingFixedFit):
            dpg.add_table_column()
            dpg.add_table_column()
            # Add one row per entry in the provided data dict
            for label, value in data_dict.items():
                with dpg.table_row():
                    dpg.add_text(label) # First column lable
                    dpg.add_text(value, tag=f"{title}_{label}".lower().replace(" ", "_")) # Second column data

#function that centers windows !!!MIGHT BE UNUSED FOR THE TEST BENCH INTERFACE, PLEASE CHECK!!!
def center_func(Item1, Item2, horizontal, vertical):
    #getting position
    Position_of_main_item = dpg.get_item_pos(Item1)
    Size_of_main_item = dpg.get_item_rect_size(Item1)
    Position_of_center_item = dpg.get_item_pos(Item2)
    Size_of_center_item = dpg.get_item_rect_size(Item2)
    print(Position_of_center_item)
    print(Size_of_center_item)
    #calculating new position
    if horizontal:
        New_pos_of_center_item_X = (
            Position_of_main_item[0] + (Size_of_main_item[0] / 2) - (Size_of_center_item[0] / 2)
        )
    else:
        New_pos_of_center_item_X = Position_of_center_item[0]

    if vertical:
        New_pos_of_center_item_Y = (
            Position_of_main_item[1] + (Size_of_main_item[1] / 2) - (Size_of_center_item[1] / 2)
        )
    else:
        New_pos_of_center_item_Y = Position_of_center_item[1]
    #setting new position
    dpg.set_item_pos(Item2, [New_pos_of_center_item_X, New_pos_of_center_item_Y])

# Ensures the centering happens after all items have been generated !!!CHECK FOR USE WHEN IT COMES TO THE TEST BENCH INTERFACE!!!
def Center_func_with_set_frame(Item1Array, Item2Array, horizontalArray, verticalArray):
    base_frame = dpg.get_frame_count()
    
    def delayed_center_func(i1, i2, h, v, delay_frame):
        dpg.set_frame_callback(delay_frame, lambda: center_func(i1, i2, h, v))

    for n, (Item1, Item2, horizontal, vertical) in enumerate(zip(Item1Array, Item2Array, horizontalArray, verticalArray)):
        delayed_center_func(Item1, Item2, horizontal, vertical, base_frame + 3 + n)
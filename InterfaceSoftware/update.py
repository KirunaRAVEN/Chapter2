def update(i):
    global x_data
    global y_data
    global nData
    global artists
    print(i)
    x_data.append(i)
    y_data.append(1 / (i+1))
    for i in range(nData):
        artists[i].set_xdata(x_data)
        artists[i].set_ydata(y_data)
    # remove first elements
    if frameCounter > 10:
        x_data.pop(0)
        y_data.pop(0)
    return artists
    
    
    # global frameCounter

    # # keep track of frame number
    # frameCounter += 1

    # # ================================ #
    # # === FETCH DATA FROM CSV FILE === #
    # # ================================ #

    # # csvData = fetchData()

    # # ====================== #
    # # === UPDATE ARTISTS === #
    # # ====================== #

    # # ------------------- #
    # # --- DATA GRAPHS --- #
    # # ------------------- #
    
    # # --------------------- #
    # # --- DATA DISPLAYS --- #
    # # --------------------- #

    # for i in range(nDataDisplays):
    #     dataDisplayValue[i] = frameCounter # rollingAverage()
    #     dataDisplays[i].setValue(dataDisplayValue[i])

    # # ------------------------ #
    # # --- INDICATOR LIGHTS --- #
    # # ------------------------ #

    # # fetch state value and set state of indicator
    # for i in range(nIndicators):
    #     indicatorStates[i] = 0
    #     indicators[i].setState(indicatorStates[i])

    # return artists
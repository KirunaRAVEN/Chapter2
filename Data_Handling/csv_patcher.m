data = readtable("data.csv");
patchedTable = data;

pointCount = height(data);

firstTimeIndex = 0;
lastTimeIndex = 0;

i = 2;
while i < pointCount
    
    if data.ArduinoTime(i-1) == data.ArduinoTime(i) 
        firstTimeIndex = i-1;
        while data.ArduinoTime(i) == data.ArduinoTime(firstTimeIndex)
            i = i+1;
        end
        lastTimeIndex = i;

        timeVector = linspace(data.ArduinoTime(firstTimeIndex), data.ArduinoTime(lastTimeIndex), lastTimeIndex - firstTimeIndex + 1);

        patchedTable.ArduinoTime(firstTimeIndex:lastTimeIndex) = timeVector;
    else
        i = i+1;
    end

end

writetable(patchedTable, "patched_data.csv");

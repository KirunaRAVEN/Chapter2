
%   CSV patcher
%   Linearly interpolates the missing timesteps from the data
%   
%

copyfile data.csv patched_data.csv

data = readtable("data.csv");
patchedTable = readtable("patched_data.csv");

pointCount = height(data);

percentSkip = floor(pointCount / 100);

firstTimeIndex = 0;
lastTimeIndex = 0;

i = 2;

while i < pointCount
    if mod(i, percentSkip) == 0
        fprintf("Patching time skips: <strong>%.1f %%</strong> completed \n", 100 * i / pointCount);
    end
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




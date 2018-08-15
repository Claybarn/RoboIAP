function [data, indices] = rawExtractDrinkingData(filePull, filePush, numIndexes, numSensors)

%% Imports data from the file on the SD card called DATALOG.TXT and aligns
% the data to the first numSensors button pushes. For example, if you have 
% 4 sensors (numSensors=4) the data will be aligned to the first four
% button pushes. Data prior to the button push will be ignored. 

%% Inputs: 
%  filePull= path and name of the file where dataa is help (DATALOG.TXT)
%  numIndexes=The time you would like to extract after the botton push in seconds
%  numSensors= the number of sensors you recorded from on the arduino
%  filePush= name of the .csv that holds the data in sinlge quotes. Example 'data1.csv'

%% Outputs:
%  data = the voltage values in a numIndexes(time) x numSensors matrix
%  indices= the time of the button press relative to when the Arudino was turned on


fileData = csvread(filePull);           % imports data
indices = find(fileData(:,5)==1);       % finds button presses and ignores "double presses"
data = zeros(numIndexes,4);             % preallocate for speed
for i = 1:numSensors
        data(:,i)=fileData(indices(i):numIndexes+indices(i)-1,4+1-i);
end
csvwrite(filePush,data);                % exports data to user defined name/path

function [data, normData, indices] = rawExtractDrinkingData(filename, numIndexes, numRats)
%aligns fluid levels to trial time
%   values in column 5 are used as the start point when trimming the
%   data to the number of specified indexes
fileData = csvread(filename);
clear rawNormData;
for i = 1:(numRats+1)
	rawNormData(:,i) = fileData(:,i)./fileData(1,i);
end
rawIndices = find(fileData(:,5)>=1);
indices = rawIndices;
toDelete = [];
for i = 2:length(rawIndices)
	if rawIndices(i)-rawIndices(i-1) > 0.95
		if rawIndices(i)-rawIndices(i-1) < 1.05
			toDelete = [toDelete, i];
        end
	end
end
indices(toDelete)=[];
data = zeros(numIndexes,4);
normData = zeros(numIndexes,4);

for i = 1:numRats
    %try
        data(:,i)=fileData(indices(i):numIndexes+indices(i)-1,4+1-i);
        normData(:,i) = rawNormData(indices(i):numIndexes+indices(i)-1,4+1-i);
    %end
end
end


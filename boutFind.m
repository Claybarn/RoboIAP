function [ putativeBouts ] = boutFind( data, maxError, minVoltDrop )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
%        wind = 400;
%        maxError = 5.0*10^-5;
%        minVoltDrop = 0.04;

    wind = length(data);

    % preallocation
    normErrorMat = zeros(wind);
    voltDropMat = zeros(wind);

    %data prep, smooth with savit-golay filter, polynomial degree 3, bin size 11
    d=sgolayfilt(data,3,11);
    d = d';
    for i = 1:wind
        for j = 1:wind
            if abs(i-j)<5
                normErrorMat(i,j) = 1000;
                voltDropMat(i,j) = 0;
            else
                if i>j
                    normErrorMat(i,j) = 1000;
                    voltDropMat(i,j) = 0;
                else
                    y=d(i:j);
                    [P,S] = polyfit(i:j,y,1);
                    slopeMat(i,j) = P(1);
                    linearSlope((i-1)*wind+j) = P(1);
                    f = polyval(P,i:j);
                    normErrorMat(i,j)=sum((y-f).^2)/length(i:j);
                    voltDropMat(i,j) = d(i)-d(j);
                end
            end
        end
    end
    localMins = imregionalmin(normErrorMat);
    targetsMat = voltDropMat.*localMins;
    firstPass = true;
    putativeBouts = [];

    remaining = find(targetsMat>0);
    while remaining > 0
        [start,finish]=find(targetsMat==max(max(targetsMat))); % find largets voltage drop
        if length(start) > 1 % may cause bug if not all points are the same. find() will return more than one index if equivalent voltage drops
            break % no more to be found
        end
        selectedPointError=normErrorMat(start,finish); % error of selected best fit line
        selectedPointVolt=voltDropMat(start,finish); % voltage drop of max voltage drop
        targetsMat(start,finish)=0; % since already found, set to 0 so dont find again
        [len,~] = size(putativeBouts);
        print = false;
        if start == 23
            if finish == 71
                print = true;
            end
        end
        shouldAdd = true;
        % check to see if overlaps with previously added bout
        for i = 1:len
            if putativeBouts(i,1:2) == [start,finish]
                shouldAdd = false;
            else 

                if start <= putativeBouts(i,1) && finish >= putativeBouts(i,1) % if start before another bout start and finish after another bout start 
                    shouldAdd = false;
                end

                if start >= putativeBouts(i,1) && start <= putativeBouts(i,2) % if putative start is after known start but not after known end, remove
                    shouldAdd = false;

                end
            end
        end
        if selectedPointError > maxError
            shouldAdd = false;
        end
        if selectedPointVolt < minVoltDrop
            shouldAdd = false;
        end
        if shouldAdd  
            putativeBouts(end+1,:)=[start, finish];
        end
        remaining = find(targetsMat>0);
    end
end


%%
numIndices = 24*60*60;
close
experiment = 2;
rat =1;

day = 11;

if experiment == 1;
    ratsPerShelf = 3;
else
    ratsPerShelf = 4;
end
arduino = round((rat+1)/ratsPerShelf,0);
r = rat-(arduino-1)*ratsPerShelf

rawFilename = ['Box Sync/IAP/Experiment' int2str(experiment) '/Day' int2str(day) '/ard' int2str(arduino) '.csv'];
[data, normData, ind] = rawExtractDrinkingData(rawFilename,numIndices,ratsPerShelf);


%% run bout finding algorithm. Feed data in chunks. Finds the best best-fit line to match the data with the largest voltage drops

bouts = []
wind = 200;
chunks = numIndices/wind;
overlapChunks = chunks*2-1;
waitbar(0)
bouts=zeros(overlapChunks,200);
%hbar = parfor_progressbar(overlapChunks,'Please wait...'); %create the progress bar for parfor loob 
tic
parfor w =1:overlapChunks
    subBouts = boutFind(data((w-1)*wind/2+1:(w-1)*wind/2+wind,r),5.0*10^-5,0.03);
    [len,~] = size(subBouts);
    adjustedBouts = subBouts+(w-1)*wind/2;
    vectToAdd = bouts(w,:);
    for b=1:len
        vectToAdd((b-1)*2+1)=adjustedBouts(b,1);
        vectToAdd((b-1)*2+2)=adjustedBouts(b,2);
    end
    bouts(w,:)=vectToAdd;
    %hbar.iterate(1); % update progress by one iteration 
end
toc
%close(hbar); % close the progress bar



%% converts the bout times from above into an easier to use format and plots it

d = sgolayfilt(data(:,r),3,21);
linearizedBouts = [];
for row = 1:overlapChunks
    for col = 1:200
        if bouts(row,col)
            linearizedBouts(end+1)=bouts(row,col);
        end
    end
end

len = length(linearizedBouts);
poBouts = [len/2,2];
for i = 1:len/2
    poBouts(i,1)=linearizedBouts((i-1)*2+1);
    poBouts(i,2)=linearizedBouts((i-1)*2+2);
end

binaryBouts=zeros(numIndices,1);
for i = 1:len/2
    binaryBouts(poBouts(i,1):poBouts(i,2)) = 1;
end 
realBouts=[];
inBout = false;
for i =1:numIndices
    if not(inBout)
        if binaryBouts(i) == 1
            realBouts(end+1,1)=i;
            inBout = true;
        end
    else
        if binaryBouts(i) == 0
            realBouts(end,2)=i;
            inBout = false;
        end
    end
end       

dat=zeros(numIndices,1);
for i = 1:len
    dat(realBouts(i,1):realBouts(i,2)) = d(realBouts(i,1));
end  

hold on;
plot(d)
plot(dat)

%%
d = sgolayfilt(data(:,r),3,11);
figure
dat = zeros((w-1)*wind/2+wind,1);
[len,~] = size(bouts);
for i = 1:len
    dat(bouts(i,1):bouts(i,2))=d(bouts(i,1));
end
hold on;
plot(d(1:(w-1)*wind/2+wind))
plot(dat)

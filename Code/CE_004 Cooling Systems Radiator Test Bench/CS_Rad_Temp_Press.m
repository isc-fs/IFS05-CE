%  -----------------------------------------------------------------------------
%  Author       :   Juan Gracia & Raúl Morán
%  Created      :   25/01/2023
%  Modified     :   -
%  Name         :   Cooling Systems Radiator Bench Temperature and Pressure
%  Description  :   This script plots in real time the values gathered by different
%  pressure and temperature sensors
%  Dependencies: MATLAB Support Package for Arduino Hardware and 1-Wire
%  Add-On
% -----------------------------------------------------------------------------

%% Connect to Arduino
% Create arduino connection and add OneWire library

a = arduino('COM3','UNO','Libraries', 'PaulStoffregen/OneWire');

% Create a One-Wire object for the sensors connected on digital port 9

temp_sens = addon(a, 'PaulStoffregen/OneWire', 'D9'); % Returns the address of each sensor connected to this port

temp_addr = temp_sens.AvailableAddresses{1};

% Reset the device before any operation
reset(temp_sens);

% Start temperature measure and conversion

write(temp_sens, temp_addr, hex2dec('44'), true);

%% Take a single temperature measurement

% Read the sensors' memory (8 data bytes and another CRC byte) and check if
% there´s any error

reset(temp_sens);
write(temp_sens, temp_addr, hex2dec('BE')); 
temp_data = read(temp_sens, temp_addr, 9);
temp_crc = temp_data(9);

sprintf('Data -  %x %x %x %x %x %x %x %x  CRC = %x\n', temp_data(1), temp_data(2), temp_data(3), temp_data(4), temp_data(5), temp_data(6), temp_data(7), temp_data(8), temp_crc)

if ~checkCRC(temp_sens, temp_data(1:8), temp_crc, 'crc8')
    error('Invalid data read.');
end

% Get raw value for temperature (unsigned 16-bit)
temp_raw = bitshift(temp_data(2),8)+temp_data(1);

% Get the R0 and R1 bits in the config register, which is the fifth byte in scratchpad data. R0 and R1 together determines the resolution configuration.
cfg = bitshift(bitand(temp_data(5), hex2dec('60')), -5);
switch cfg
    case bin2dec('00')  % 9-bit resolution, 93.75 ms conversion time
        raw = bitand(raw, hex2dec('fff8'));
    case bin2dec('01')  % 10-bit resolution, 187.5 ms conversion time
        raw = bitand(raw, hex2dec('fffC'));
    case bin2dec('10')  % 11-bit resolution, 375 ms conversion time
        raw = bitand(raw, hex2dec('fffE'));
    case bin2dec('11')  % 12-bit resolution, 750 ms conversion time
    otherwise
        error('Invalid resolution configuration');
end
% Convert temperature reading from unsigned 16-bit value to signed 16-bit.
temp_raw = typecast(uint16(temp_raw), 'int16');

% Convert to the actual floating point value
temp_celsius = double(temp_raw) / 16.0;
sprintf('Temperature -> %.4f Celsius', temp_celsius)



%% Record and plot 10 seconds of temperature data

ii = 0;
TempC = zeros(1e4,1);
t = zeros(1e4,1);

tic
while toc < 10
    ii = ii + 1;
    reset(temp_sens);
    write(temp_sens, temp_addr, hex2dec('44'), true);
    reset(temp_sens);
    write(temp_sens, temp_addr, hex2dec('BE')); 
    temp_data = read(temp_sens, temp_addr, 9);
    temp_raw = bitshift(temp_data(2),8)+temp_data(1);
    temp_raw = typecast(uint16(temp_raw), 'int16');

    temp_celsius = double(temp_raw) / 16.0;
    TempC(ii) = temp_celsius;
    % Get time since starting
    t(ii) = toc;
end

% Post-process and plot the data. First remove any excess zeros on the
% logging variables.
TempC = TempC(1:ii);
t = t(1:ii);
% Plot temperature versus time
figure
plot(t,TempC,'-*')
xlabel('Elapsed time (sec)')
ylabel('Temperature (\circC)')
title('Ten Seconds of Temperature Data')
set(gca,'xlim',[t(1) t(ii)])

%% Sampling rate

timeBetweenDataPoints = diff(t);
averageTimePerDataPoint = mean(timeBetweenDataPoints);
dataRateHz = 1/averageTimePerDataPoint;
fprintf('Acquired one data point per %.3f seconds (%.f Hz)\n',...
    averageTimePerDataPoint,dataRateHz)


%% Acquire and display live data

figure
h = animatedline;
ax = gca;
ax.YGrid = 'on';
ax.YLim = [20 50];

stop = false;
startTime = datetime('now');
while ~stop
    reset(temp_sens);
    write(temp_sens, temp_addr, hex2dec('44'), true);
    reset(temp_sens);
    write(temp_sens, temp_addr, hex2dec('BE')); 
    temp_data = read(temp_sens, temp_addr, 9);
    temp_raw = bitshift(temp_data(2),8)+temp_data(1);
    temp_raw = typecast(uint16(temp_raw), 'int16');
    temp_celsius = double(temp_raw) / 16.0;

    t =  datetime('now') - startTime;
    % Add points to animation
    addpoints(h,datenum(t),temp_celsius)
    % Update axes
    ax.XLim = datenum([t-seconds(15) t]);
    datetick('x','keeplimits')
    drawnow
    % Check stop condition (push button)
    stop = 0;
end

%% Plot the recorded data

[timeLogs,tempLogs] = getpoints(h);
timeSecs = (timeLogs-timeLogs(1))*24*3600;
figure
plot(timeSecs,tempLogs)
xlabel('Elapsed time (sec)')
ylabel('Temperature (\circC)')

%% Smooth out readings with moving average filter

smoothedTemp = smooth(tempLogs,25);
tempMax = smoothedTemp + 2*9/5;
tempMin = smoothedTemp - 2*9/5;

figure
plot(timeSecs,tempLogs, timeSecs,tempMax,'r--',timeSecs,tempMin,'r--')
xlabel('Elapsed time (sec)')
ylabel('Temperature (\circC)')
hold on 

%%
% Plot the original and the smoothed temperature signal, and illustrate the
% uncertainty.

plot(timeSecs,smoothedTemp,'r')

%% Save results to a file

T = table(timeSecs',tempLogs','VariableNames',{'Time_sec','Temp_F'});
filename = 'Temperature_Data.xlsx';
% Write table to file 
writetable(T,filename)
% Print confirmation to command line
fprintf('Results table with %g temperature measurements saved to file %s\n',...
    length(timeSecs),filename)

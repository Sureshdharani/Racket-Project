function [ acc, gyro, mag, t ] = readSensData( path, accFileName,...
                                               gyroFileName,...
                                               magFileName,...
                                               dt, range, type )
% Reads sensor data from giben file on given range
% path - path to the data files
% accFileName, gyroFileName and magFileName  - appropriate file name
% dt - sampling time
% range - an 2x1 vector with begin and end range
% type - data type to read
%
% Returns sensro data and time vector

% Read acc:
file = fopen([path, accFileName]);
acc = fread(file, range, type);
fclose(file);

file = fopen([path, gyroFileName]);
gyro = fread(file, range, type);
fclose(file);

file = fopen([path, magFileName]);
mag = fread(file, range, type);
fclose(file);

N = min([length(acc(1,:)), length(acc(2,:)), length(acc(3,:)),...
         length(gyro(1,:)), length(gyro(2,:)), length(gyro(3,:)),...
         length(mag(1,:)), length(mag(2,:)), length(mag(3,:))]);
t = linspace(0,dt*N,N);
acc = acc(:,1:N);
gyro = gyro(:,1:N);
mag = mag(:,1:N);
end


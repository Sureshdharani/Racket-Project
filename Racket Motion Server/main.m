% Racket motion server application for matlab.
% Created by Alexander Kozhinov
% Date: 18.05.2017

clc;
close all;
clear variables;

%% Begin processing data here:
% 1) Read IMU data:
path = '../Offline Sensor Data/test3/';
range = [3, inf];
type = 'float';

dt = 1/100;
[acc, gyro, mag, t] = readSensData(path,...
                                   'test3_acc.pcm',...
                                   'test3_gyro.pcm',...
                                   'test3_mag.pcm',...
                                   dt, range, type);

% 2) Plot raw data:
plotSensData( 'RAW Data', t, acc, gyro, mag);


% Racket motion server application for matlab.
% Created by Alexander Kozhinov
% Date: 18.05.2017

clc;
close all;
clear variables;

%% Begin processing data here:
% 1) Read IMU data:

testName = 'test4';
path = ['../Offline Sensor Data/', testName, '/'];
range = [3, inf];
type = 'float';

dt = 1/100;
[acc_dat, gyro_dat, mag_dat, t_dat] = readSensData(path,...
                                   [testName, '_acc.pcm'],...
                                   [testName, '_gyro.pcm'],...
                                   [testName, '_mag.pcm'],...
                                   dt, range, type);

% 2) Iterate over data plot it and fit it:
figName = 'RAW Data';
hFig = figure('Name', figName); 
% plotSensData(hFig, t_dat, acc_dat, gyro_dat, mag_dat);

% t = [];
% acc = [];
% gyro = [];
% mag = [];
% for k=1:length(t_dat)
% %     t = [t, t_dat(k)];
% %     acc = [acc, acc_dat(:,k)];
% %     gyro = [gyro, gyro_dat(:,k)];
% %     mag = [mag, mag_dat(:,k)];
%     
%     plotSensData(hFig, t_dat(k), acc_dat(:,k), gyro_dat(:,k), mag_dat(:,k));
%     
%     pause(dt);
% end

acc_x = acc_dat(1,:);

idx_beg = 520;
idx_end = idx_beg + 150;
acc_x = acc_x(idx_beg:idx_end);
t = t_dat(idx_beg:idx_end);








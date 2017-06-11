% Racket motion server application for matlab.
% Created by Alexander Kozhinov
% Date: 18.05.2017

clc;
close all;
clear variables;

%% Begin processing data here:
% 1) Read IMU data:

% Read from presaved file:
isPresaved = 0;
dt = 1/100;
if isPresaved == 1
    testName = 'test4';
    path = ['../Offline Sensor Data/', testName, '/'];
    range = [3, inf];
    type = 'float';

    [acc_dat, gyro_dat, mag_dat, t_dat] = readSensData(path,...
                                            [testName, '_acc.pcm'],...
                                            [testName, '_gyro.pcm'],...
                                            [testName, '_mag.pcm'],...
                                            dt, range, type);
else
%     connector('off');
%     connector('on', '12345');
%     m = mobiledev;
%     m.Logging = 1;
end
% Read from android mobile phone

% 2) Iterate over data plot it and fit it:
figName = 'RAW Data';
hFig = figure('Name', figName); 
% plotSensData(hFig, t_dat, acc_dat, gyro_dat, mag_dat);

t = [];
acc = [];
gyro = [];
mag = [];
ax = [];

% N = length(t_dat);
N = 10;
tt = [];

u = udp('127.0.0.1', 'LocalPort', 5555);
fopen(u);
for k=1:N
    t_beg = tic;
%     t = [t, t_dat(k)];
%     acc = [acc, acc_dat(:,k)];
%     gyro = [gyro, gyro_dat(:,k)];
%     mag = [mag, mag_dat(:,k)];
%     acc = [acc, m.Acceleration'];
%     gyro = [gyro, m.AngularVelocity'];
%     mag = [mag, m.MagneticField'];
%     
%     plotSensData(hFig, dt, acc, gyro, mag);
%     t_samp = toc(t_beg);
%     
%     tt = [tt, t_samp];

    dat = fgetl(u);
    if ~isempty(dat)
        disp([num2str(k) , ': ', num2str(dat)]);
    end
end
fclose(u);







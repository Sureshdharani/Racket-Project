% Racket motion server application for matlab.
% Created by Alexander Kozhinov
% Date: 18.05.2017

clc;
close all;
clear variables;

%% Begin processing data here:
% 1) Read IMU data:

u = udp('127.0.0.1', 'LocalPort', 5554);
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







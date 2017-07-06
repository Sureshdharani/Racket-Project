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
while(1)
    dat = fgetl(u);
    if ~isempty(dat)
        disp([num2str(dat)]);
    end
end
fclose(u);







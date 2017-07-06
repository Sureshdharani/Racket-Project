clc;
close all;
clear variables;

x=3;
y=3;

files = dir('*acc.pcm');
for file = files'
    acc_file = fopen(file.name);
    acc_Data = fread(acc_file, [3,9999], 'float');
    fclose(acc_file);
    
hold on;
    subplot(x,y,1), plot(acc_Data(1,:)), ylabel('a_x, m/s^2');
    hold off;
    hold on;
    subplot(x,y,4), plot(acc_Data(2,:)), ylabel('a_y, m/s^2');
    hold off;
    hold on;
    subplot(x,y,7), plot(acc_Data(3,:)), ylabel('a_z, m/s^2');
    hold off;
end

files = dir('*gyro.pcm');
for file = files'
  gyro_file = fopen(file.name);
  gyro_Data = fread(gyro_file, [3,9999], 'float');
  fclose(gyro_file);
    
hold on;
    subplot(x,y,2), plot(gyro_Data(1,:)), ylabel('\omega_x, rad/s');
    hold off;
    hold on;
    subplot(x,y,5), plot(gyro_Data(2,:)), ylabel('\omega_x, rad/s');
    hold off;
    hold on;
    subplot(x,y,8), plot(gyro_Data(3,:)), ylabel('\omega_x, rad/s');
    hold off;
end

files = dir('*mag.pcm');
for file = files'
mag_file = fopen(file.name);
mag_Data = fread(mag_file, [3,9999], 'float');
fclose(mag_file);
    
hold on;
    subplot(x,y,3), plot(mag_Data(1,:)), ylabel('m_x, {\muT}');
    hold off;
    hold on;
    subplot(x,y,6), plot(mag_Data(2,:)), ylabel('m_x, {\muT}');
    hold off;
    hold on;
    subplot(x,y,9), plot(mag_Data(3,:)), ylabel('m_x, {\muT}');
    hold off;
end
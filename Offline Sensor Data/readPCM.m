clc;
close all;
clear variables;

%% Read line from file:
acc_filename = 'test2_acc.pcm';
gyro_filename = 'test2_gyro.pcm';
magnetic_f_filename = 'test2_magnetic_f.pcm';
rotation_v_filename = 'test2_rotation_v.pcm';

acc_file = fopen(acc_filename);
acc_Data = fread(acc_file, [3,9999], 'float');
fclose(acc_file);

gyro_file = fopen(gyro_filename);
gyro_Data = fread(gyro_file, [3,9999], 'float');
fclose(gyro_file);

mag_file = fopen(magnetic_f_filename);
mag_Data = fread(mag_file, [3,9999], 'float');
fclose(mag_file);

rot_file = fopen(rotation_v_filename);
rot_Data = fread(rot_file, [3,9999], 'float');
fclose(rot_file);

figure('Name', 'Read IMU Sensor binary');
x = 3;
y = 4;
subplot(x,y,1), plot(acc_Data(1,:)), ylabel('a_x, m/s^2');
subplot(x,y,5), plot(acc_Data(2,:)), ylabel('a_y, m/s^2');
subplot(x,y,9), plot(acc_Data(3,:)), ylabel('a_z, m/s^2');

subplot(x,y,2), plot(gyro_Data(1,:)), ylabel('g_x, rad/s');
subplot(x,y,6), plot(gyro_Data(2,:)), ylabel('g_y, rad/s');
subplot(x,y,10), plot(gyro_Data(3,:)), ylabel('g_z, rad/s');

subplot(x,y,3), plot(mag_Data(1,:)), ylabel('m_x, ?T');
subplot(x,y,7), plot(mag_Data(2,:)), ylabel('m_y, ?T');
subplot(x,y,11), plot(mag_Data(3,:)), ylabel('m_z, ?T');

subplot(x,y,4), plot(rot_Data(1,:)), ylabel('r_x, ');
subplot(x,y,8), plot(rot_Data(2,:)), ylabel('r_y, ');
subplot(x,y,12), plot(rot_Data(3,:)), ylabel('r_z, ');
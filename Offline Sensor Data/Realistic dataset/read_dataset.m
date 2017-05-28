clc;
close all;
clear variables;

%% Read line from file:
acc_filename = 'acc.pcm';
gyro_filename = 'gyr.pcm';
magnetic_f_filename = 'mag.pcm';
rotation_v_filename = 'rot_v.pcm';
linear_acc_filename = 'li_acc.pcm';
orientation_filename = 'orientation.pcm';

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

li_acc_file = fopen(linear_acc_filename);
li_acc_Data = fread(li_acc_file, [3,9999], 'float');
fclose(li_acc_file);

orientation_file = fopen(orientation_filename);
orientation_Data = fread(orientation_file, [3,9999], 'float');
fclose(orientation_file);

figure('Name', 'Read IMU Sensor binary');
x = 3;
y = 6;
subplot(x,y,1), plot(acc_Data(1,:)), ylabel('a_x, m/s^2');
subplot(x,y,7), plot(acc_Data(2,:)), ylabel('a_y, m/s^2');
subplot(x,y,13), plot(acc_Data(3,:)), ylabel('a_z, m/s^2');

subplot(x,y,2), plot(gyro_Data(1,:)), ylabel('\omega_x, rad/s');
subplot(x,y,8), plot(gyro_Data(2,:)), ylabel('\omega_y, rad/s');
subplot(x,y,14), plot(gyro_Data(3,:)), ylabel('\omega_z, rad/s');

subplot(x,y,3), plot(mag_Data(1,:)), ylabel('m_x, {\muT}');
subplot(x,y,9), plot(mag_Data(2,:)), ylabel('m_y, {\muT}');
subplot(x,y,15), plot(mag_Data(3,:)), ylabel('m_z, {\muT}');

subplot(x,y,4), plot(rot_Data(1,:)), ylabel('r_x, ');
subplot(x,y,10), plot(rot_Data(2,:)), ylabel('r_y, ');
subplot(x,y,16), plot(rot_Data(3,:)), ylabel('r_z, ');

subplot(x,y,5), plot(orientation_Data(1,:)), ylabel('orie_x, � ');
subplot(x,y,11), plot(orientation_Data(2,:)), ylabel('orie_y, � ');
subplot(x,y,17), plot(orientation_Data(3,:)), ylabel('orie_z, � ');

subplot(x,y,6), plot(li_acc_Data(1,:)), ylabel('li-a_x, m/s^2');
subplot(x,y,12), plot(li_acc_Data(2,:)), ylabel('li-a_y, m/s^2');
subplot(x,y,18), plot(li_acc_Data(3,:)), ylabel('li-a_z, m/s^2');
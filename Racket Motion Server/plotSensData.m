function [ hFig ] = plotSensData( figName, t, acc, gyro, mag)
% Plots sensor data on separate figure
% Returns figure handle

hFig = figure('Name', figName);

% acceleration plot:
subplot(3,3,1), plot(t, acc(1,:));
ylabel('a_x, m/s^2');
subplot(3,3,4), plot(t, acc(2,:));
ylabel('a_y, m/s^2');
subplot(3,3,7), plot(t, acc(3,:));
ylabel('a_z, m/s^2');
xlabel('t, sec.');

% gyroscope plot:
subplot(3,3,2), plot(t, gyro(1,:));
ylabel('\omega_x, rad/s');
subplot(3,3,5), plot(t, gyro(2,:));
ylabel('\omega_y, rad/s');
subplot(3,3,8), plot(t, gyro(3,:));
ylabel('\omega_z, rad/s');
xlabel('t, sec.');

% magnetic plot:
subplot(3,3,3), plot(t, mag(1,:));
ylabel('M_x, \muT');
subplot(3,3,6), plot(t, mag(2,:));
ylabel('M_y, \muT');
subplot(3,3,9), plot(t, mag(3,:));
ylabel('M_z, \muT');
xlabel('t, sec.');

end


function [ hFig ] = plotSensData( hFig, t, acc, gyro, mag)
% Plots sensor data in separate given figure
% Returns figure handle

if ~ishandle(hFig)
    return;
end

figure(hFig);
ax = get(gcf,'children');  % set of axeses

if isempty(ax)  % create axises
    ax(1) = subplot(3,3,1);
    ax(2) = subplot(3,3,2);
    ax(3) = subplot(3,3,3);
    ax(4) = subplot(3,3,4);
    ax(5) = subplot(3,3,5);
    ax(6) = subplot(3,3,6);
    ax(7) = subplot(3,3,7);
    ax(8) = subplot(3,3,8);
    ax(9) = subplot(3,3,9);
end
ax = flipud(ax);

% Acceleration plot:
col = 'b';
plot(ax(1), t, acc(1,:), col);
ylabel(ax(1), 'a_x, m/s^2');
hold on;

plot(ax(4), t, acc(2,:), col);
ylabel(ax(4), 'a_y, m/s^2');
hold on;

plot(ax(7), t, acc(3,:), col);
ylabel(ax(7), 'a_z, m/s^2');
xlabel(ax(7), 't, sec.');
hold on;

% Gyroscope plot:
col = 'g';
plot(ax(2), t, gyro(1,:), col);
ylabel(ax(2), '\omega_x, rad/s');
hold on;

plot(ax(5), t, gyro(2,:), col);
ylabel(ax(5), '\omega_y, rad/s');
hold on;

plot(ax(8), t, gyro(3,:), col);
ylabel(ax(8), '\omega_z, rad/s');
xlabel(ax(8), 't, sec.');
hold on;

% Magnetic plot:
col = 'r';
plot(ax(3), t, mag(1,:), col);
ylabel(ax(3), 'M_x, \muT');
hold on;

plot(ax(6), t, mag(2,:), col);
ylabel(ax(6), 'M_y, \muT');
hold on;

plot(ax(9), t, mag(3,:), col);
ylabel(ax(9), 'M_z, \muT');
xlabel(ax(9), 't, sec.');
hold on;

drawnow;
end


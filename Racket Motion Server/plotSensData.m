function [ hFig ] = plotSensData( hFig, dt, acc, gyro, mag)
% Plots sensor data in separate given figure
% Returns figure handle

if ~ishandle(hFig)
    return;
end

if isempty(acc)
    return;
end

figure(hFig);

t = 0:dt:(length(acc(1,:))-1)*dt;
subplot(3,3,1), plot(t, acc(1,:));
ylabel('a_x, m/s^2');
subplot(3,3,4), plot(t, acc(2,:));
ylabel('a_y, m/s^2');
subplot(3,3,7), plot(t, acc(3,:));
ylabel('a_z, m/s^2');
xlabel('t, sec.');

subplot(3,3,2), plot(t, gyro(1,:));
ylabel('\omega_x, rad/s');
subplot(3,3,5), plot(t, gyro(2,:));
ylabel('\omega_y, rad/s');
subplot(3,3,8), plot(t, gyro(3,:));
ylabel('\omega_z, rad/s');
xlabel('t, sec.');

subplot(3,3,3), plot(t, mag(1,:));
ylabel('M_x, \muT');
subplot(3,3,6), plot(t, mag(2,:));
ylabel('M_y, \muT');
subplot(3,3,9), plot(t, mag(3,:));
ylabel('M_z, \muT');
xlabel('t, sec.');

% ax = get(gcf,'children');  % set of axeses
% 
% if isempty(ax)  % create axises
%     ax(1) = subplot(3,3,1);
%     hold on;
%     ax(2) = subplot(3,3,2);
%     hold on;
%     ax(3) = subplot(3,3,3);
%     hold on;
%     ax(4) = subplot(3,3,4);
%     hold on;
%     ax(5) = subplot(3,3,5);
%     hold on;
%     ax(6) = subplot(3,3,6);
%     hold on;
%     ax(7) = subplot(3,3,7);
%     hold on;
%     ax(8) = subplot(3,3,8);
%     hold on;
%     ax(9) = subplot(3,3,9);
%     hold on;
% end
% ax = flipud(ax);

% % Acceleration plot:
% col = 'b';
% appendToAxis(ax(1), t, acc(1,:), '', 'a_x, m/s^2', 'on', col);
% appendToAxis(ax(4), t, acc(2,:), '', 'a_y, m/s^2', 'on', col);
% appendToAxis(ax(7), t, acc(3,:), 't, sec.', 'a_z, m/s^2', 'on', col);
% 
% % Gyroscope plot:
% col = 'g';
% appendToAxis(ax(2), t, gyro(1,:), '', '\omega_x, rad/s', 'on', col);
% appendToAxis(ax(5), t, gyro(2,:), '', '\omega_y, rad/s', 'on', col);
% appendToAxis(ax(8), t, gyro(3,:), 't, sec.', '\omega_z, rad/s', 'on', col);
% 
% % Magnetic plot:
% col = 'r';
% appendToAxis(ax(3), t, mag(1,:), '', 'M_x, \muT', 'on', col);
% appendToAxis(ax(6), t, mag(2,:), '', 'M_y, \muT', 'on', col);
% appendToAxis(ax(9), t, mag(3,:), 't, sec.', 'M_z, \muT', 'on', col);

drawnow;
end


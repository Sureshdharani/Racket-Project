clc;
close all;
clear variables;

rec = importdata("SomeRecord.txt");
rec = rec.data;
t = rec(:,1);
aX = rec(:,2);
aY = rec(:,3);
aZ = rec(:,4);
gX = rec(:,5);
gY = rec(:,6);
gZ = rec(:,7);
oX = rec(:,8);
oY = rec(:,9);
oZ = rec(:,10);

nR = 0;  % 150;
nL = 0;  % 20;
t = t(1+nL:end-nR);
oZ = oZ(1+nL:end-nR);

% Provide FFT of orientation Z to find better model:
dt = mean(diff(t)) / 1000;
Fs = 1 / dt;
L = length(t);
f = Fs * (0:(L/2)) / L;
f = f';
fZMag = abs(fft(oZ)/L);
fZMag = fZMag(1:L/2+1);
fZMag(2:end-1) = 2 * fZMag(2:end-1);

% Plot FFT:
figure('Name', 'FFT of Angle_Z');
stem(f, fZMag);
xlabel('f, Hz');
ylabel('|oZ|');
grid on;




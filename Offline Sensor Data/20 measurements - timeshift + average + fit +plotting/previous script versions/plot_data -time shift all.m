clc;
close all;
clear variables;

x = 3;
y = 3;

files = dir('*acc.pcm');

ACC = cell(1, length(files));
idx = 1;

for file = files'
    disp('new file')
    acc_file = fopen(file.name);
    acc_Data = fread(acc_file, [3, 9999], 'float');
    fclose(acc_file);
    %     Putting x and y axis data
    xx = 1:length(acc_Data(1, :));
    yy = acc_Data(1, :);
    trimmed = 0;
    datapointsbeforepeak = 30;
    shifting_step = 1;
    window = 10;
    hold on;
    begin = 1;
    endd = begin + window;
    limit = length(acc_Data(1, :)) - window;
    disp(limit)
    while begin < limit
        coefficients = polyfit(xx(begin:endd), yy(begin:endd), 1);
        slope = coefficients(1);
        disp(slope)
        if (slope > 0.2 && trimmed == 0)
            
            acc_Data = acc_Data(:, (begin - datapointsbeforepeak):end);
            ACC{idx} = acc_Data;
            idx = idx + 1;
            trimmed = 1;
         
            hold on;
            subplot(x, y, 1), plot(acc_Data(1, :)), ylabel('a_x, m/s^2');
            hold off;
            hold on;
            subplot(x, y, 4), plot(acc_Data(2, :)), ylabel('a_y, m/s^2');
            hold off;
            hold on;
            subplot(x, y, 7), plot(acc_Data(3, :)), ylabel('a_z, m/s^2');
            hold off;
        end
     
        begin = begin + shifting_step;
        endd = endd + shifting_step;
    end
    hold off;
end

files = dir('*gyro.pcm');

GYR = cell(1, length(files));
idx = 1;

for file = files'
    disp('new file')
    gyro_file = fopen(file.name);
    gyro_Data = fread(gyro_file, [3, 9999], 'float');
    fclose(gyro_file);
    %     Putting x and y axis data
    xx = 1:length(gyro_Data(1, :));
    yy = gyro_Data(1, :);
    trimmed = 0;
    datapointsbeforepeak = 15;
    shifting_step = 1;
    window = 10;
    hold on;
    begin = 1;
    endd = begin + window;
    limit = length(gyro_Data(1, :)) - window;
    disp(limit)
    while begin < limit
        coefficients = polyfit(xx(begin:endd), yy(begin:endd), 1);
        slope = coefficients(1);
        disp(slope)
        if (slope < -0.08 && trimmed == 0)
            disp(begin - datapointsbeforepeak);
            gyro_Data = gyro_Data(:, (begin - datapointsbeforepeak):end);
            GYR{idx} = gyro_Data;
            idx = idx + 1;
            trimmed = 1;
         
            hold on;
            subplot(x, y, 2), plot(gyro_Data(1, :)), ylabel('\omega_x, rad/s');
            hold off;
            hold on;
            subplot(x, y, 5), plot(gyro_Data(2, :)), ylabel('\omega_x, rad/s');
            hold off;
            hold on;
            subplot(x, y, 8), plot(gyro_Data(3, :)), ylabel('\omega_x, rad/s');
            hold off;
        end
     
        begin = begin + shifting_step;
        endd = endd + shifting_step;
    end
    hold off;
end

files = dir('*mag.pcm');

MAG = cell(1, length(files));
idx = 1;

for file = files'
    disp('new file')
    mag_file = fopen(file.name);
    mag_Data = fread(mag_file, [3, 9999], 'float');
    fclose(mag_file);
    %     Putting x and y axis data
    xx = 1:length(mag_Data(1, :));
    yy = mag_Data(1, :);
    trimmed = 0;
    datapointsbeforepeak = 30;
    shifting_step = 1;
    window = 10;
    hold on;
    begin = 1;
    endd = begin + window;
    limit = length(mag_Data(1, :)) - window;
    disp(limit)
    while begin < limit
        coefficients = polyfit(xx(begin:endd), yy(begin:endd), 1);
        slope = coefficients(1);
        disp(slope)
        if (slope < -0.6 && trimmed == 0)
           
            mag_Data = mag_Data(:, (begin - datapointsbeforepeak):end);
            MAG{idx} = mag_Data;
            idx = idx + 1;
            trimmed = 1;
         
            hold on;
            subplot(x, y, 3), plot(mag_Data(1, :)), ylabel('m_x, {\muT}');
            hold off;
            hold on;
            subplot(x, y, 6), plot(mag_Data(2, :)), ylabel('m_x, {\muT}');
            hold off;
            hold on;
            subplot(x, y, 9), plot(mag_Data(3, :)), ylabel('m_x, {\muT}');
            hold off;
        end
     
        begin = begin + shifting_step;
        endd = endd + shifting_step;
    end
    hold off;
end
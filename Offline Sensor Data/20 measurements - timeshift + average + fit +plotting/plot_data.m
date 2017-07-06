clc;
close all;
clear variables;

x = 3;
y = 3;

files = dir('*acc.pcm');
% declaring cell array [populated later but not used]
ACC = cell(1, length(files));
% counter to fill ACC, MAG, GYR + calculate average
idx = 1;

for file = files'
    % extract data
    acc_file = fopen(file.name);
    acc_Data = fread(acc_file, [3, 9999], 'float');
    fclose(acc_file);
    % preparing x and y axis data
    xx = 1:length(acc_Data(1, :));
    yy = acc_Data(1, :);
    % trimmed flag, to avoid trimming a data set more than once
    trimmed = 0;
    % data points to keep before the peek
    datapointsbeforepeak = 30;
    % window shifting step
    shifting_step = 1;
    % window to calculate the slop  
    window = 10;
    hold on;
    % start from first data point
    beginn = 1;
    % end is begin + window
    endd = beginn + window;
    % limit of slope calculation
    limit = length(acc_Data(1, :)) - window;
    % while we're still in the curve boundaries
    while beginn < limit
        % calculate the slope
        coefficients = polyfit(xx(beginn:endd), yy(beginn:endd), 1);
        slope = coefficients(1);
        % slope was determined to be 0,2 in the slope determining script
        if (slope > 0.2 && trimmed == 0)
            % trim the beginning of the dataset, leaving only X amount:
            % [datapointsbeforepeak] before the slope was detected
          
            acc_Data = acc_Data(:, (beginn - datapointsbeforepeak):end);
            %trim the end of the dataset
            acc_Data = acc_Data(:, 1:140);
            % calculating rolling average:
            % Prepare x,y,z column vectors
            col_v_x=acc_Data(1,:)';
            col_v_y=acc_Data(2,:)';
            col_v_z=acc_Data(3,:)';
            if(idx~=1)
                % rest of iterations:calculating the rolling average
                avg_x=(avg_x*(idx-1)+col_v_x)/(idx);
                avg_y=(avg_y*(idx-1)+col_v_y)/(idx);
                avg_z=(avg_z*(idx-1)+col_v_z)/(idx);
            else
                % first iteration, average is the first data set
                avg_x=acc_Data(1,:)';
                avg_y=acc_Data(2,:)';
                avg_z=acc_Data(3,:)';
            end
            
            ACC{idx} = acc_Data;
            idx = idx + 1;
            trimmed = 1;
            % plotting data set
            hold on;
            subplot(x, y, 1), plot(acc_Data(1, :)), ylabel('a_x, m/s^2');
            hold off;
            hold on;
            subplot(x, y, 4), plot(acc_Data(2, :)), ylabel('a_y, m/s^2');
            hold off;
            hold on;
            subplot(x, y, 7), plot(acc_Data(3, :)), ylabel('a_z, m/s^2');
            hold off;
        end % end if slope
        % increment window
        beginn = beginn + shifting_step;
        endd = endd + shifting_step;
    end % end going through the data
    hold off;
    
end% end loop through files
% plot the averages
hold on;
subplot(x, y, 1), plot(avg_x,'*'), ylabel('a_x, m/s^2');
subplot(x, y, 4), plot(avg_y,'*'), ylabel('a_x, m/s^2');
subplot(x, y, 7), plot(avg_z,'*'), ylabel('a_x, m/s^2');
hold off;
% calculate the fit of the average & plot it
x_axis= 1:140;
[fitx, gofx] = fit(x_axis(:), avg_x(:),'gauss2');
[fity, gofy] = fit(x_axis(:), avg_y(:),'gauss2');
[fitz, gofz] = fit(x_axis(:), avg_z(:),'gauss2');
hold on;
subplot(x, y, 1), plot(fitx), ylabel('a_x, m/s^2');
subplot(x, y, 4), plot(fity), ylabel('a_x, m/s^2');
subplot(x, y, 7), plot(fitz), ylabel('a_x, m/s^2');
hold off;
% display coefficients
disp(fitx);
disp(gofx);





files = dir('*gyro.pcm');

GYR = cell(1, length(files));
idx = 1;

for file = files'
    % disp('new file')
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
    beginn = 1;
    endd = beginn + window;
    limit = length(gyro_Data(1, :)) - window;
    % disp(limit)
    while beginn < limit
        coefficients = polyfit(xx(beginn:endd), yy(beginn:endd), 1);
        slope = coefficients(1);
        % disp(slope)
        if (slope < -0.08 && trimmed == 0)
            % disp(beginn - datapointsbeforepeak);
            gyro_Data = gyro_Data(:, (beginn - datapointsbeforepeak):end);
            gyro_Data = gyro_Data(:, 1:140);
            
            %             calculating rolling average
            col_v_x=gyro_Data(1,:)';
            col_v_y=gyro_Data(2,:)';
            col_v_z=gyro_Data(3,:)';
            if(idx~=1)
                avg_x=(avg_x*(idx-1)+col_v_x)/(idx);
                avg_y=(avg_y*(idx-1)+col_v_y)/(idx);
                avg_z=(avg_z*(idx-1)+col_v_z)/(idx);
            else
                avg_x=gyro_Data(1,:)';
                avg_y=gyro_Data(2,:)';
                avg_z=gyro_Data(3,:)';
            end
            
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
     
        beginn = beginn + shifting_step;
        endd = endd + shifting_step;
    end
    hold off;
end
hold on;
subplot(x, y, 2), plot(avg_x,'*'), ylabel('\omega_x, rad/s');
subplot(x, y, 5), plot(avg_y,'*'), ylabel('\omega_x, rad/s');
subplot(x, y, 8), plot(avg_z,'*'), ylabel('\omega_x, rad/s');
hold off;

[fitx, gofx] = fit(x_axis(:), avg_x(:),'gauss2');
[fity, gofy] = fit(x_axis(:), avg_y(:),'gauss2');
[fitz, gofz] = fit(x_axis(:), avg_z(:),'gauss2');
hold on;
subplot(x, y, 2), plot(fitx), ylabel('\omega_x, rad/s');
subplot(x, y, 5), plot(fity), ylabel('\omega_x, rad/s');
subplot(x, y, 8), plot(fitz), ylabel('\omega_x, rad/s');
hold off;



files = dir('*mag.pcm');

MAG = cell(1, length(files));
idx = 1;

for file = files'
    % disp('new file')
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
    beginn = 1;
    endd = beginn + window;
    limit = length(mag_Data(1, :)) - window;
    % disp(limit)
    while beginn < limit
        coefficients = polyfit(xx(beginn:endd), yy(beginn:endd), 1);
        slope = coefficients(1);
        % disp(slope)
        if (slope < -0.6 && trimmed == 0)
           
            mag_Data = mag_Data(:, (beginn - datapointsbeforepeak):end);
            mag_Data = mag_Data(:, 1:140);
            
            %             calculating rolling average
            col_v_x=mag_Data(1,:)';
            col_v_y=mag_Data(2,:)';
            col_v_z=mag_Data(3,:)';
            if(idx~=1)
                avg_x=(avg_x*(idx-1)+col_v_x)/(idx);
                avg_y=(avg_y*(idx-1)+col_v_y)/(idx);
                avg_z=(avg_z*(idx-1)+col_v_z)/(idx);
            else
                avg_x=mag_Data(1,:)';
                avg_y=mag_Data(2,:)';
                avg_z=mag_Data(3,:)';
            end
            
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
     
        beginn = beginn + shifting_step;
        endd = endd + shifting_step;
    end
    hold off;
end
hold on;
subplot(x, y, 3), plot(avg_x,'*'), ylabel('m_x, {\muT}');
subplot(x, y, 6), plot(avg_y,'*'), ylabel('m_x, {\muT}');
subplot(x, y, 9), plot(avg_z,'*'), ylabel('m_x, {\muT}');
hold off;

[fitx, gofx] = fit(x_axis(:), avg_x(:),'sin1');
[fity, gofy] = fit(x_axis(:), avg_y(:),'sin2');
[fitz, gofz] = fit(x_axis(:), avg_z(:),'gauss2');
hold on;
subplot(x, y, 3), plot(fitx), ylabel('m_x, {\muT}');
subplot(x, y, 6), plot(fity), ylabel('m_x, {\muT}');
subplot(x, y, 9), plot(fitz), ylabel('m_x, {\muT}');
hold off;
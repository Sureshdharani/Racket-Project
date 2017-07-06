clc;
close all;
clear variables;

x = 3;
y = 3;

files = dir('*acc.pcm');

ACC = cell(1, length(files));
idx = 1;

avg_x = ones([1,140]);
avg_x=avg_x';

avg_y = ones([1,140]);
avg_y=avg_y';

avg_z = ones([1,140]);
avg_z=avg_z';

for file = files'
    % disp('new file')
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
    beginn = 1;
    endd = beginn + window;
    limit = length(acc_Data(1, :)) - window;
    % disp(limit)
    while beginn < limit
        coefficients = polyfit(xx(beginn:endd), yy(beginn:endd), 1);
        slope = coefficients(1);
        % disp(slope)
        if (slope > 0.2 && trimmed == 0)
            
            acc_Data = acc_Data(:, (beginn - datapointsbeforepeak):end);
            acc_Data = acc_Data(:, 1:140);
%             calculating rolling average
            col_v_x=acc_Data(1,:)';
            col_v_y=acc_Data(2,:)';
            col_v_z=acc_Data(3,:)';
            if(idx~=1)
                avg_x=(avg_x*(idx-1)+col_v_x)/(idx);
                avg_y=(avg_y*(idx-1)+col_v_y)/(idx);
                avg_z=(avg_z*(idx-1)+col_v_z)/(idx);
            else
                avg_x=acc_Data(1,:)';
                avg_y=acc_Data(2,:)';
                avg_z=acc_Data(3,:)';
            end
            
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
     
        beginn = beginn + shifting_step;
        endd = endd + shifting_step;
    end
    hold off;
    
end
hold on;
    subplot(x, y, 1), plot(avg_x,'*'), ylabel('a_x, m/s^2');
    subplot(x, y, 4), plot(avg_y,'*'), ylabel('a_x, m/s^2');
    subplot(x, y, 7), plot(avg_z,'*'), ylabel('a_x, m/s^2');
    hold off;
% disp(ACC(1,1));
% disp (avg);

% 
% files = dir('*gyro.pcm');
% 
% GYR = cell(1, length(files));
% idx = 1;
% 
% for file = files'
%     % disp('new file')
%     gyro_file = fopen(file.name);
%     gyro_Data = fread(gyro_file, [3, 9999], 'float');
%     fclose(gyro_file);
%     %     Putting x and y axis data
%     xx = 1:length(gyro_Data(1, :));
%     yy = gyro_Data(1, :);
%     trimmed = 0;
%     datapointsbeforepeak = 15;
%     shifting_step = 1;
%     window = 10;
%     hold on;
%     beginn = 1;
%     endd = beginn + window;
%     limit = length(gyro_Data(1, :)) - window;
%     % disp(limit)
%     while beginn < limit
%         coefficients = polyfit(xx(beginn:endd), yy(beginn:endd), 1);
%         slope = coefficients(1);
%         % disp(slope)
%         if (slope < -0.08 && trimmed == 0)
%             % disp(beginn - datapointsbeforepeak);
%             gyro_Data = gyro_Data(:, (beginn - datapointsbeforepeak):end);
%             gyro_Data = gyro_Data(:, 1:140);
%             GYR{idx} = gyro_Data;
%             idx = idx + 1;
%             trimmed = 1;
%          
%             hold on;
%             subplot(x, y, 2), plot(gyro_Data(1, :)), ylabel('\omega_x, rad/s');
%             hold off;
%             hold on;
%             subplot(x, y, 5), plot(gyro_Data(2, :)), ylabel('\omega_x, rad/s');
%             hold off;
%             hold on;
%             subplot(x, y, 8), plot(gyro_Data(3, :)), ylabel('\omega_x, rad/s');
%             hold off;
%         end
%      
%         beginn = beginn + shifting_step;
%         endd = endd + shifting_step;
%     end
%     hold off;
% end
% 
% files = dir('*mag.pcm');
% 
% MAG = cell(1, length(files));
% idx = 1;
% 
% for file = files'
%     % disp('new file')
%     mag_file = fopen(file.name);
%     mag_Data = fread(mag_file, [3, 9999], 'float');
%     fclose(mag_file);
%     %     Putting x and y axis data
%     xx = 1:length(mag_Data(1, :));
%     yy = mag_Data(1, :);
%     trimmed = 0;
%     datapointsbeforepeak = 30;
%     shifting_step = 1;
%     window = 10;
%     hold on;
%     beginn = 1;
%     endd = beginn + window;
%     limit = length(mag_Data(1, :)) - window;
%     % disp(limit)
%     while beginn < limit
%         coefficients = polyfit(xx(beginn:endd), yy(beginn:endd), 1);
%         slope = coefficients(1);
%         % disp(slope)
%         if (slope < -0.6 && trimmed == 0)
%            
%             mag_Data = mag_Data(:, (beginn - datapointsbeforepeak):end);
%             mag_Data = mag_Data(:, 1:140);
%             MAG{idx} = mag_Data;
%             idx = idx + 1;
%             trimmed = 1;
%          
%             hold on;
%             subplot(x, y, 3), plot(mag_Data(1, :)), ylabel('m_x, {\muT}');
%             hold off;
%             hold on;
%             subplot(x, y, 6), plot(mag_Data(2, :)), ylabel('m_x, {\muT}');
%             hold off;
%             hold on;
%             subplot(x, y, 9), plot(mag_Data(3, :)), ylabel('m_x, {\muT}');
%             hold off;
%         end
%      
%         beginn = beginn + shifting_step;
%         endd = endd + shifting_step;
%     end
%     hold off;
% end

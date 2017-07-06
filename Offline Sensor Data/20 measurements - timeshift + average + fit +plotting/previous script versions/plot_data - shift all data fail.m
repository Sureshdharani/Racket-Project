clc;
close all;
clear variables;

x=3;
y=3;

files = dir('*acc.pcm');

ACC = cell(1, length(files));
MAG = cell(1, length(files));
GYRO = cell(1, length(files));
idx = 1;

% loop through all ACC files
for file = files'
    acc_file = fopen(file.name);
    acc_Data = fread(acc_file, [3,9999], 'float');
    fclose(acc_file);
    
    mag_filename = strrep(file.name,'acc','mag');
    gyro_filename = strrep(file.name,'acc','gyro');
    
    mag_file = fopen(mag_filename);
    mag_Data = fread(mag_file, [3,9999], 'float');
    fclose(mag_file);

    gyro_file = fopen(gyro_filename);
    gyro_Data = fread(gyro_file, [3,9999], 'float');
    fclose(gyro_file);
    
%     Putting x and y axis data
    xx=1:length(acc_Data(1,:));
    yy=acc_Data(1,:);
%     disp(length(acc_Data(1,:)));
   trimmed=0;
   datapointsbeforepeak=30;
   shifting_step=1;
   window=12;
%    plot(acc_Data(1,:)), ylabel('a_x, m/s^2');
   hold on;
   begin=1;
   endd=begin+window;
   limit=length(acc_Data(1,:))-window;
%    disp(limit)
    while begin<limit
%         disp(acc_Data(1,begin))
        
        coefficients = polyfit(xx(begin:endd), yy(begin:endd), 1);
        slope = coefficients(1);
%         disp(slope)
%         l_b=line ([begin begin],[-5 20]);
%         l_e=line ([endd endd],[-5 20]);
        if(slope>0.2 && trimmed ==0)
%            current_data= acc_Data(1,:);
%            current_data= current_data((begin-datapointsbeforepeak):end);
           acc_Data=acc_Data(:,(begin-datapointsbeforepeak):end);
           shifted_points=begin-datapointsbeforepeak
           mag_Data=mag_Data(:,(begin-datapointsbeforepeak):end);
           gyro_Data=gyro_Data(:,(begin-datapointsbeforepeak):end);
           
           ACC{idx} = acc_Data;
           MAG{idx} = mag_Data;
           GYRO{idx} = gyro_Data;
           idx = idx + 1;
           trimmed=1;
           
%            disp(mag_filename);
%            disp(gyro_filename);
%            disp(file.name);
%            hold on
%            plot(current_data), ylabel('a_x, m/s^2');
%            hold off
           
    hold on;
    subplot(x,y,1), plot(acc_Data(1,:)), ylabel('a_x, m/s^2');
    hold off;
    hold on;
    subplot(x,y,4), plot(acc_Data(2,:)), ylabel('a_y, m/s^2');
    hold off;
    hold on;
    subplot(x,y,7), plot(acc_Data(3,:)), ylabel('a_z, m/s^2');
    hold off;
    
    hold on;
    subplot(x,y,2), plot(gyro_Data(1,:)), ylabel('\omega_x, rad/s');
    hold off;
    hold on;
    subplot(x,y,5), plot(gyro_Data(2,:)), ylabel('\omega_x, rad/s');
    hold off;
    hold on;
    subplot(x,y,8), plot(gyro_Data(3,:)), ylabel('\omega_x, rad/s');
    hold off;
    
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
        
%         input('press enter');
        begin=begin+shifting_step;
        endd=endd+shifting_step;
%         delete(l_b);
%         delete(l_e);
        
%         if slope>0.1
%             l_b=line ([c c],[-20 20]);
%             disp('good')
%             break
%         end
    end
     hold off;
 end    
% hold on;
%     subplot(x,y,1), plot(acc_Data(1,:)), ylabel('a_x, m/s^2');
%     hold off;
%     hold on;
%     subplot(x,y,4), plot(acc_Data(2,:)), ylabel('a_y, m/s^2');
%     hold off;
%     hold on;
%     subplot(x,y,7), plot(acc_Data(3,:)), ylabel('a_z, m/s^2');
%     hold off;
%     
%     ACC{idx} = acc_Data;
%     idx = idx + 1;
% end
% 
% files = dir('*gyro.pcm');
% 
% GYR = cell(1, length(files));
% idx = 1;
% 
% for file = files'
%   gyro_file = fopen(file.name);
%   gyro_Data = fread(gyro_file, [3,9999], 'float');
%   fclose(gyro_file);
%     
% hold on;
%     subplot(x,y,2), plot(gyro_Data(1,:)), ylabel('\omega_x, rad/s');
%     hold off;
%     hold on;
%     subplot(x,y,5), plot(gyro_Data(2,:)), ylabel('\omega_x, rad/s');
%     hold off;
%     hold on;
%     subplot(x,y,8), plot(gyro_Data(3,:)), ylabel('\omega_x, rad/s');
%     hold off;
%     GYR{idx} = gyro_Data;
%     idx = idx + 1;
% end
% 
% files = dir('*mag.pcm');
% 
% MAG = cell(1, length(files));
% idx = 1;
% 
% for file = files'
% mag_file = fopen(file.name);
% mag_Data = fread(mag_file, [3,9999], 'float');
% fclose(mag_file);
%     
% hold on;
%     subplot(x,y,3), plot(mag_Data(1,:)), ylabel('m_x, {\muT}');
%     hold off;
%     hold on;
%     subplot(x,y,6), plot(mag_Data(2,:)), ylabel('m_x, {\muT}');
%     hold off;
%     hold on;
%     subplot(x,y,9), plot(mag_Data(3,:)), ylabel('m_x, {\muT}');
%     hold off;
%     
%     MAG{idx} = mag_Data;
%     idx = idx + 1;
% end
% 
% 
% % Let's find the slope
% % Once slope found ==> l_b=line ([x x],[-20 20]);

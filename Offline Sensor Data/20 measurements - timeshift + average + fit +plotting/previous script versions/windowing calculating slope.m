clc;
close all;
clear variables;

x=3;
y=3;

files = dir('*acc.pcm');

ACC = cell(1, length(files));
idx = 1;


for file = files'
    disp('new file')
    acc_file = fopen(file.name);
    acc_Data = fread(acc_file, [3,9999], 'float');
    fclose(acc_file);
%     Putting x and y axis data
    xx=1:length(acc_Data(1,:));
    yy=acc_Data(1,:);
%     disp(length(acc_Data(1,:)));

    
   shifting_step=1;
   window=10;
   plot(acc_Data(1,:)), ylabel('a_x, m/s^2');
   hold on;
   begin=1;
   endd=begin+window;
   limit=length(acc_Data(1,:))-shifting_step;
   disp(limit)
    while begin<limit
%         disp(acc_Data(1,begin))
        
        coefficients = polyfit(xx(begin:endd), yy(begin:endd), 1);
        slope = coefficients(1);
        disp(slope)
        l_b=line ([begin begin],[-5 20]);
        l_e=line ([endd endd],[-5 20]);
        
        input('press enter');
       
        begin=begin+shifting_step;
        endd=endd+shifting_step;
        delete(l_b);
        delete(l_e);
        
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

clc;
close all;
clear variables;

x=1;
y=3;
acc_filename = 'acc.pcm';
    acc_file = fopen(acc_filename);
    acc_Data = fread(acc_file, [3,9999], 'float');
    fclose(acc_file);
    data_length=length(acc_Data(1,:));
    x_begin=1;
    x_end=130;
    shifting_step=10;
    
   acc_Data(1,:) = acc_Data(1,:) - min(acc_Data(1,:));
   plot (acc_Data(1,:));
   ylim([min(acc_Data(1,:)) max(acc_Data(1,:))]);
   xlim([1 1400]);
      while x_begin<data_length
        x_axis= x_begin:x_end;
        y_axis_data=acc_Data(1,x_begin:x_end);
        hold on;
        plot(x_axis(:),y_axis_data(:));
        [fit2, gof] = fit(x_axis(:), y_axis_data(:),'gauss2');
        p=plot(fit2);
        disp(fit2);
        l_b=line ([x_begin x_begin],[-20 20]);
        l_e=line ([x_end x_end],[-20 20]);
        hold off;
        input('press enter');
       
        x_begin=x_begin+shifting_step;
        x_end=x_end+shifting_step;
        delete(p);
        delete(l_b);
        delete(l_e);
      end
function [ ] = appendToAxis( hAxis, x, y, xlbl, ylbl,...
                             holdState, col)
% Appends data to given axis:
%   hAxis - axis handle to append
%   x - x data
%   y - y data
%   holdState - on or off
%   col - color

if ~exist('col', 'var')
    color = 'b';
else
    color = col;
end

if ~exist('holdState', 'var')
    holdSt = 'on';
else
    holdSt = holdState;
end

h = findobj(hAxis, 'Type', 'line');
x_prev = get(h, 'Xdata');
y_prev = get(h, 'Ydata');

if isempty(x_prev)
    xx = x;
    yy = y;
else
    if iscell(x_prev)
        xx = [x_prev{1}(end), x];
        yy = [y_prev{1}(end), y];
    else
        xx = [x_prev, x];
        yy = [y_prev, y];
    end
end
plot(hAxis, xx, yy, col);
hold(holdSt);

if exist('ylbl', 'var'), ylabel(hAxis, ylbl); end
if exist('xlbl', 'var'), xlabel(hAxis, xlbl); end

end


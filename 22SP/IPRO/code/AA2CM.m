function output = AA2CM(params,input,cp)
if min(diff(input)) ~= max(diff(input))
    error('time vector not evenly spaced')
end
temp = params(1)*conv(cp,exp(-params(2)*input))*min(diff(input));
output = temp(1:numel(input));

function [ d ] = rscode_decode( bits )
%RSCODE_DECODE Summary of this function goes here
%   Detailed explanation goes here

k = 12;
n = k + 2*round(k/4);

data = rsdec(bits, n, k);


data = data.x;

d = [];
for i = data
  if (i >= 38)
    d = [d char('a' + i-38)];
  elseif (i == 37)
    d = [d ' '];
  elseif (i >= 26)
    d = [d char('0' + i-26)];
  else
    d = [d char('a' + i)]; 
  end 
end


end


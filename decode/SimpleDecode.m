function [ labels ] = SimpleDecode(img, mean_x)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

bit_size = 1.7;
labels = zeros(15, 15);
wind_size = 25;
for i = 1:15
  row = round(mean_x + (i-8)* bit_size/17*1000);
  for j = 1:15
    col = round(mean_x + (j-8)* bit_size/17*1000);
        
    labels(i, j) = mean(mean(img(row-wind_size:row+wind_size, ...
        col-wind_size:col+wind_size)));
  end
end

mask = logical([zeros(5, 5), ones(5, 5), zeros(5, 5); ones(5, 15); ...
    zeros(5, 5), ones(5, 5), zeros(5, 5)]);

[bits, c] = kmeans(labels(mask), 2);

if (c(1) < c(2))
    bits = bits - 1;
else
    bits = 2 - bits;
end

labels(mask) = bits;
labels(~mask) = 0.5;
%labels = imresize(labels, 50, 'nearest');

end


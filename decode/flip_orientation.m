function [ res ] = flip_orientation( im, pos )
%FLIP_ORIENTATION Summary of this function goes here
%   Detailed explanation goes here

key_points = [6,6; 6,10; 10,6; 10,10];

for i = 1:4
    im_part = im( pos(key_points(i,1), key_points(i,2),1) - 20: pos(key_points(i,1), key_points(i,2),1)+20, ...
        pos(key_points(i,1), key_points(i,2),2) - 20: pos(key_points(i,1), key_points(i,2),2)+20 );
    d(i) = mean(im_part(:));
end

[a, idx] = sort(d);

if (idx(1) == 4)
  res = im;
elseif(idx(1) == 1)
  res = rot90(im,2);
elseif(idx(1) == 2)
  res = rot90(im,3);
elseif(idx(1) == 3)
  res = rot90(im,1);
end

res = imadjust(res);

end


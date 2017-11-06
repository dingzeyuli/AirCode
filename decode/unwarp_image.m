function [ B, mean_x ] = unwarp_image(im_cr, centers )
%UNWARP_IMAGE Summary of this function goes here
%   Detailed explanation goes here
[tmp, I1] = sort(centers(:,1));
[tmp, I1] = sort(I1);
[tmp, I2] = sort(centers(:,2));
[tmp, I2] = sort(I2);

for i = 1:4
  if I1(i) <= 2 && I2(i) <= 2 
    centers_(1, :) = centers(i, :);
  elseif I1(i) <= 2 && I2(i) > 2
    centers_(2, :) = centers(i, :);
  elseif I1(i) > 2 && I2(i) > 2
    centers_(3, :) = centers(i, :);
  else
    centers_(4, :) = centers(i, :);
  end
end


centers = centers_;

captured_points = centers;

min_x = floor(600 + 0);
max_x = floor(600 + 1000);
mean_x = min_x/2+max_x/2;
fixed_points = [...
  min_x,  min_x, max_x, max_x;...
  min_x, max_x, max_x, min_x];
tform = fitgeotrans(captured_points, fixed_points', 'projective');

ra = imref2d([2200 2200]);
B = imwarp(im_cr, tform, 'OutputView', ra);
B = (B - min(B(:))) / (max(B(:)) - min(B(:)));

B = imrotate(B, -90);

end


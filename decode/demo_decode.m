close all
clear

im = im2double(imread('./data/trial198-red255/Lg.png'));

% Take the transpose/mirror since the code is printed in the back side of 
% the 3D object and we captured in the front direction.
im = im';


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 0. Select the ROI (region of interests) %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

figure
imshow(im,[])

% This is to select the ROI, basically only run the
% filtering algorithm on the selected region with AirCode.
% Uncomment the following ginput() code to manually select region for new
% images.
x = [788; 1346];
y = [736; 1306];

% [x, y] = ginput(2);
%  x = floor(x);
%  y = floor(y);

%%%%%%%%%%%%%%%%%%%%
% 1. Crop the ROI  %
%%%%%%%%%%%%%%%%%%%%

IM = im(y(1):y(2), x(1):x(2), :);
IM = ( IM - min(IM(:))) / range(IM(:));

clf
subplot(2,2,1)
imshow(IM);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 2. Enhance the image and find the corners %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

im_cr = improve_image_quality(IM);
subplot(2,2,2)
imshow(im_cr);

[ centers, tmp] = locate_markers(im_cr, [0.3 0.4 0.5 0.7 0.9]);

hold on
scatter(centers(:,1), centers(:,2), 300, 'r','filled')


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 3. Identify the bit locations and run SVM %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

[B, mean_x]  = unwarp_image(im_cr, centers );


subplot(2,2,3)
imshow(B, [])

hold on
bit_size = 1.7;
for i = 1:15
  for j = 1:15
    scatter(mean_x + (i-8)* bit_size/17*1000  , mean_x + (j-8)* bit_size/17*1000, 50,'filled' );
    pos(i,j,1) = mean_x + (i-8)* bit_size/17*1000;
    pos(i,j,2) = mean_x + (j-8)* bit_size/17*1000;
  end 
end
B = flip_orientation(B, pos);


levels = 200;
level_range = 4:1:levels;
im = cat(3,B,B,B);

SVMModel = bitwise_SVM_model_training(im, B, mean_x, bit_size, level_range);
[label, score] = bitwise_SVM_model_testing(im, B, mean_x, bit_size, level_range, levels, SVMModel);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 4. Decode the embedded information %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

[im_label, msg] = generate_image_label(label, B, mean_x);

disp(msg)
subplot(2,2,4)
imshow(im_label)

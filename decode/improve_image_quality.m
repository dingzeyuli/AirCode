function [ im_cr ] = improve_image_quality( IM )
%REMOVE_FITTED_PLANE Summary of this function goes here
%   Detailed explanation goes here

[X,Y] = meshgrid(1:size(IM,2),1:size(IM,1));
X_v = X(:);
Y_v = Y(:);
f_v = IM(:);
sf = fit([X_v, Y_v], f_v, 'poly22');
z = feval(sf, [X_v, Y_v]);
z = reshape(z, size(IM));

IM = (IM - z);
IM = ( IM - min(IM(:))) / range(IM(:));

im_cr = (IM - min(IM(:))) / (max(IM(:)) - min(IM(:)));

num_tiles = 16;
clip_limit = 0.01; 
num_bins = 256;
range_ = 'full';
dist = 'uniform';

im_cr = adapthisteq(im_cr, 'NumTiles', [num_tiles num_tiles],...
    'ClipLimit', clip_limit, 'NBins', num_bins, 'Range', range_,...
    'Distribution', dist);

im_cr = bfilter2(im_cr, 5, [2.5, 0.1]);

end


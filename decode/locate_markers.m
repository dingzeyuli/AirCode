function [ centers, center_candidates ] = locate_markers( IM, ranges )
%LOCATE_MARKERS Summary of this function goes here
%   Detailed explanation goes here
%close all

rim = IM;
center_candidates = [];

% find circle marker in all the scale listed in ranges vector
for scale = ranges
  rim_crop = imresize(rim, scale);

  E=[];
  for i = 0.5:0.25:1
    rim1 = imgaussfilt(rim_crop, i);
    rim1 = imadjust(rim1);
    rim1 = medfilt2(rim1, [5, 5]);
    E_i = FindEllipses(rim1);
    E = [E; E_i];
  end

  %figure; imshow(rim_crop, []); hold on
  for i=1:size(E,1)
      center_candidates = [center_candidates; DrawConic(E(i,:), 0)/scale];
  end
end

centers = ransac_mean_selection(center_candidates, 10);
centers = ransac_point_selection(centers, 10);
[IDX, centers] = kmeans(center_candidates, 4);


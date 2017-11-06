function [ sorted_pts_4x2 ] = sort_points( center_pts_nx2 )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

num_pts = size(center_pts_nx2, 1);
if (and(num_pts ~= 3, num_pts ~= 4))
    disp('Only 3 or 4 points supported.');
    return;
end

left_check = (repmat(center_pts_nx2(:, 1), 1, num_pts) - ...
    repmat(center_pts_nx2(:, 1)', num_pts, 1));
left_check = sum(left_check / max(abs(left_check)), 2);
top_check = (repmat(center_pts_nx2(:, 2), 1, num_pts) - ...
    repmat(center_pts_nx2(:, 2)', num_pts, 1));
top_check = sum(top_check / max(abs(top_check)), 2);

threshold = 0.15;
sorted_pts_4x2 = -1 * ones(4, 2);
tl_idx = find(and(left_check < -threshold, top_check < -threshold));
if (tl_idx)
    sorted_pts_4x2(1, :) = center_pts_nx2(tl_idx(1), :);
end
tr_idx = find(and(left_check >threshold, top_check < -threshold));
if (tr_idx)
    sorted_pts_4x2(2, :) = center_pts_nx2(tr_idx(1), :);
end
bl_idx = find(and(left_check < -threshold, top_check > threshold));
if (bl_idx)
    sorted_pts_4x2(3, :) = center_pts_nx2(bl_idx(1), :);
end
br_idx = find(and(left_check > threshold, top_check > threshold));
if (br_idx)
    sorted_pts_4x2(4, :) = center_pts_nx2(br_idx(1), :);
end
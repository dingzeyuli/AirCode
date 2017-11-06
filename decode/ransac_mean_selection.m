function [ center_pts_rx2 ] = ransac_mean_selection( center_candidates_nx2, dist_thresh )
% Separate points into clusters based on dist_thresh and find the mean of each cluster
%   Detailed explanation goes here

    i = 0;
    while size(center_candidates_nx2, 1)
        i = i + 1;
        n = size(center_candidates_nx2, 1);
        ransac_iter = n;
        inlier_indices = false(n);
        for j = 1:ransac_iter
            dist = center_candidates_nx2 - repmat(center_candidates_nx2(j, :), n, 1);
            dist = sqrt(sum(dist.^2, 2));
            inlier_indices(j, :) = (dist < dist_thresh)';
        end
        num_inliers = sum(inlier_indices, 2);
        max_indices = find(num_inliers == max(num_inliers));
        sel_indices = inlier_indices(max_indices(1), :);
        center_pts_rx2(i, :) = mean(center_candidates_nx2(sel_indices, :), 1);
        center_candidates_nx2 = center_candidates_nx2(~sel_indices, :);
    end

end

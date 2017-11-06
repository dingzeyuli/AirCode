function [ center_pts_4x2 ] = ransac_point_selection( center_pts_nx2, err_thresh )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

    n = size(center_pts_nx2, 1);
    sel_indices = combntns(1:n, 3);
    iter = size(sel_indices, 1);

    for i=1:iter
        centers = center_pts_nx2(sel_indices(i, :), :);
        
        sorted_pts = sort_points(centers);
        
        min_x = floor(600 + 0);
        max_x = floor(600 + 1000);
        fixed_points = [min_x,  max_x, min_x, max_x; ...
            min_x, min_x, max_x, max_x]';
        ommited_pt = fixed_points(sorted_pts(:, 1) < 0, :);
        fixed_points = fixed_points(sorted_pts(:, 1) > 0, :);
        captured_points = sorted_pts(sorted_pts(:, 1) > 0, :);
        if (size(captured_points, 1) < 3)
            continue;
        end
        tform = fitgeotrans(captured_points, fixed_points, 'affine');
        
        Xt = inv(tform.T') * [ommited_pt 1]';
        Xt(1) = Xt(1) ./ Xt(3);
        Xt(2) = Xt(2) ./ Xt(3);
        Xt = Xt(1:2);
        dist = sqrt(sum((repmat(Xt', n, 1) - center_pts_nx2).^2, 2));
        if (sum(dist < err_thresh) ~= 0)
            corner_pt = center_pts_nx2(dist < err_thresh, :);
            sorted_pts(sorted_pts(:, 1) < 0, :) = corner_pt;
            break;
        end
    end
    center_pts_4x2 = sorted_pts;

end


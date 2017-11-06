function [ label, score ] = bitwise_SVM_model_testing( im, B, mean_x, bit_size,  level_range, levels, SVMModel)
%BITWISE_SVM_MODEL_TRAINING Summary of this function goes here
%   Detailed explanation goes here
index =1;

for i = 1:15
  row = round(mean_x + (i-8)* bit_size/17*1000);
  for j = 1:15
    col = round(mean_x + (j-8)* bit_size/17*1000);
        
    level_index = 1;
    for level = level_range
      new_feature(index, level_index) = ( ...
        sum(B(row-level+1:row+level-1, col-level)) + ...
        sum(B(row-level+1:row+level-1, col+level)) + ...
        sum(B(row-level, col-level+1:col+level-1)) + ...
        sum(B(row+level, col-level+1:col+level-1)) ) / ...
        (4* (2*level-1));
      level_index = level_index + 1;
    end
    
    im_crop = B(row-levels:row+levels, col-levels:col+levels);
    var_matrix(i,j) = var(im_crop(:));
    new_feature(index, :) = imadjust(new_feature(index, :)); 
    index = index + 1;
  end
end
[label, score] = predict(SVMModel,new_feature);

end

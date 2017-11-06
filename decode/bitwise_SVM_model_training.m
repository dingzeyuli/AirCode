function [ SVMModel ] = bitwise_SVM_model_training( im, B, mean_x, bit_size,  level_range)
%BITWISE_SVM_MODEL_TRAINING Summary of this function goes here
%   Detailed explanation goes here
index = 1;


for i = 1:15
  row = round(mean_x + (i-8)* bit_size/17*1000);
  for j = 1:15
    col = round(mean_x + (j-8)* bit_size/17*1000);
      
    if ~(( i==6 && j == 6) || (i== 6 && j ==10) || ( i==10 && j == 10) || (i== 10 && j ==6) || ( i==8&& j == 8))
      continue
    end
 
    im(row, col,:) = [1;0;0];
 
    level_index=1;
    for level = level_range
      im(row-level+1:row+level-1, col-level-1:col-level+1,2) = 1;
      im(row-level+1:row+level-1, col+level,2) = 1;
      im(row-level, col-level+1:col+level-1,2) = 1;
      im(row+level, col-level+1:col+level-1,2) = 1;
      
      feature(index, level_index) = ( ...
        sum(B(row-level+1:row+level-1, col-level)) + ...
        sum(B(row-level+1:row+level-1, col+level)) + ...
        sum(B(row-level, col-level+1:col+level-1)) + ...
        sum(B(row+level, col-level+1:col+level-1)) ) / ...
        (4* (2*level-1))  ;
      
      level_index = level_index + 1;
    end
    feature(index, :) = imadjust(feature(index, :));
    if ( (i == 6 && j == 6) ||...
         (i == 6 && j == 10) ||...
         (i == 10 && j == 6))
      Y(index) = 1;
    else
      Y(index) = 0;
    end
    index = index + 1;
  end
end

SVMModel = fitcsvm(feature,Y);


end


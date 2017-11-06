function [status, bit] = check_position(row_index, col_index, code_mode)

total_code_size = 2*5 + code_mode ;

code_mode  =5;

max_region = total_code_size + 1 - 5;

if ( ( row_index <= code_mode  && col_index <= code_mode) ||...
     ( row_index >= max_region && col_index <= code_mode) ||...
     ( row_index >= max_region && col_index >= max_region)||...
     ( row_index <= code_mode  && col_index >= max_region) ) 
  status = 0;
  bit = 0;
elseif (row_index >= (max_region-1) && col_index >= (max_region-1))
  status = 1;
  bit = 1;
elseif ( (row_index == (code_mode + 1) && col_index == (code_mode + 1)) ||...
         (row_index == (code_mode + 1) && col_index == (max_region -1)) ||...
         (row_index == (max_region -1) && col_index == (code_mode + 1)))
  status = 2;
  bit = 0;
elseif ( (row_index == (max_region -1) && col_index == (max_region -1)) ||...
         (row_index == (total_code_size+1)/2 && col_index == (total_code_size+1)/2))
  status = 2;
  bit = 1;
else
  status = 3;
  bit = [];
end
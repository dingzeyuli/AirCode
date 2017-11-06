function [ im_label, a ] = generate_image_label( label, B, mean_x)
%GENERATE_IMAGE_LABEL Summary of this function goes here
%   Detailed explanation goes here

label = reshape(label, [15, 15])';
label(1:5,1:5) = 0.5;
label(1:5,11:15) = 0.5;
label(11:15,1:5) = 0.5;
label(11:15,11:15) = 0.5;


%label2 = SimpleDecode(B, mean_x);

index = 1;
for i = 1:15
  for j = 1:15
      [status, bit] = check_position(i, j, 5);
      if (status == 3)
        bits(index) = label(i,j);
        index = index + 1;
      end
  end
end

bits(end-2:end-1) = [];
bits = 1 -bits;

a = reshape(bits,6,[])';
b = int2str(a);
c = bin2dec(b)';

msg_in = gf(c, 6);

a = rscode_decode(msg_in);


im_label = imresize(label, 50, 'nearest');

end


function Ellipses = FindEllipses(img)

%works with gray images, although you could accumulate gradient from the
%three channel for the same fitting.
if size(img,3)>1
    img = rgb2gray(img); 
end

%derivative kernel
d = [0.2707 0.6065 0 -0.6065 -0.2707];
g = [0.1353 0.6065 1 0.6065 0.1353];
opx=g'*d;
opy=d'*g;
dx = filter2(opx,img);
dy = filter2(opy,img);

grad = sqrt(dx.*dx + dy.*dy);
%remove boundary gradient
grad(:,1:4) = 0;
grad(1:4,:) = 0;
grad(:,end-3:end) = 0;
grad(end-3:end,:) = 0;


sImg = size(img);
[X,Y] = meshgrid(1:sImg(2),1:sImg(1));

%will only use strong gradient, simple way to segment image in calibration
%context  (stronger than 3x mean value
% 3*grad
mask = grad>1.5*mean(grad(:));
% figure;imshow(mask, []); ginput;
[label,nblabel] = bwlabel(mask);
%this will speed up the search for labeled area
ind = find(label);
indlabel = label(ind);
Ellipses = [];

for i=1:nblabel

  indLoc = ind(find(indlabel==i));
  indLoc = indLoc(:);
  if length(indLoc)<10
      continue;
  end
  [dC,stdCenter,angleIncertitude]  = DualConicFitting(dx(indLoc),dy(indLoc),X(indLoc),Y(indLoc));

  if stdCenter(1)==-1  | stdCenter(1)> 0.15 ;%0.075
      continue
  end
  %dual ellipses are ellipse inverses
  C = inv(dC);
  C = C./C(3,3);
  Ellipses = [Ellipses;debuildConic(C)];
end





function par = debuildConic(Q)

%direct
par(1)=Q(1,1);
par(2)=Q(1,2)*2;
par(3)=Q(2,2);
par(4)=Q(1,3)*2; 
par(5)=Q(3,2)*2;
par(6)=Q(3,3);

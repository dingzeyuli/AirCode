function [dC,stdCenter,angleIncertitude]  = DualConicFitting(Ix,Iy,x,y)
%Fit a dual ellipse to the image gradient. 
%Input: Ix,Iy  vector containing the horizontal and vertical image gradient
%       x,y    corresponding coordinates of the image gradient in Ix,Iy
%Output: dC    the fitted dual ellipse in the form L'*dC*L = 0 where L is a
%               line on the dual conic L = [a b c]'
%        stdCenter  estimated uncertainty on the center of the dual ellipse
%                   (in pixel)
%        angleIncertitude   angle of the center uncertainty covariance
%                           ellipse
%                           
% JNO 2008

Normalisation = 1;
if nargin==1
    Normalisation = 0;
end


%data normalisation
%point le plus pres des droites
%translation
a = Ix;
b = Iy;
c = -(Ix.*x+Iy.*y);

if Normalisation == 1
    M = [-b a];
    B = -c;
    mpts = M\B;

    %no scaling
    H = [1 0    mpts(1)
         0 1    mpts(2)
         0 0    1];
    Lnorm = ((H)'*[a b c]')';
    a = Lnorm(:,1);
    b = Lnorm(:,2);
    c = Lnorm(:,3);
end







a2 = a.^2;
ab = a.*b;
b2 = b.^2;
ac = a.*c;
bc = b.*c;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
%% solution par least-square
% %Ax=b impose F=1
AA=[sum(a2.^2),  sum(a2.*ab), sum(a2.*b2), sum(a2.*ac), sum(a2.*bc)
    sum(a2.*ab), sum(ab.^2),  sum(ab.*b2), sum(ab.*ac), sum(ab.*bc)
    sum(a2.*b2), sum(ab.*b2), sum(b2.^2),  sum(b2.*ac), sum(b2.*bc)
    sum(a2.*ac), sum(ab.*ac), sum(b2.*ac), sum(ac.^2),  sum(ac.*bc)
    sum(a2.*bc), sum(ab.*bc), sum(b2.*bc), sum(ac.*bc), sum(bc.^2)];


BB = [ sum(-(c.^2).*a2)
       sum(-(c.^2).*ab)
       sum(-(c.^2).*b2)
       sum(-(c.^2).*ac)
       sum(-(c.^2).*bc)];

if det(AA)<10e-10
    %not well conditioned, inversion not stable. no solution
    dC = ones(3)*-1;
    stdCenter = -1;
    angleIncertitude = -1;
    return
end

%inversion with svd decomposition
[U,S,V] = svd(AA,0);
S = diag(S);
Sinv = zeros(size(S));
Sinv(abs(S)>10e-12) = 1./S(abs(S)>10e-12);
Sinv = diag(Sinv);
iAA = V*Sinv*U';
sol = (iAA*BB)';
sol = [sol 1];

%denormalisation
dCnorm = rebuildConic(sol);
if Normalisation ==1
    dC = (H)*dCnorm*(H)';
else
    dC = dCnorm;
end


%error estimation
s = sol(1:5)';
BTB = sum(c.^4);
R = (s'*AA*s-2*s'*BB+BTB)/(length(a)-5);
cvar2_constantVariance = R*inv(AA);
vD = cvar2_constantVariance(4,4);
vDE = cvar2_constantVariance(4,5);
vE = cvar2_constantVariance(5,5);

[U,S,V] = svd(cvar2_constantVariance(4:5,4:5));

%divide by 4 (2^2), dual conic center is d,e divided by 2
stdCenter = [sqrt(diag(S))/4]';
angleIncertitude = atan2(V(2,1),V(1,1));






function Q = rebuildConic(par)

%direct
Q(1,1)=par(1);
Q(1,2)=par(2)/2;
Q(1,3)=par(4)/2; 
Q(2,1)=par(2)/2;
Q(2,2)=par(3);
Q(2,3)=par(5)/2;
Q(3,1)=par(4)/2; 
Q(3,2)=par(5)/2;
Q(3,3)=par(6);
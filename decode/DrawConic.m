function center = DrawConic(C, draw)
%DRAWELLIPSE	Draw ellipse
%
%	drawellipse (z, a, b, alpha, pat{'-'}, OPTIONS{[]})
%	draws ellipse into current figure.
%
%	z, a, b, alpha: parameters of ellipse
%	pat: pattern to be used

%color
lineWidth = 1;

mc = max(size(C));
if mc==6
    %equation implicite
    [centrex, centrey, axea, axeb, angle] = convertConicToParametric(C);
elseif mc==3
    par = debuildConic(C);
    [centrex, centrey, axea, axeb, angle] = convertConicToParametric(par);
elseif mc==5
    centrex = C(1);
    centrey = C(2);
    axea = C(3);
    axeb = C(4);
    angle = C(5);
else
    disp('Error in draw conic, format parametres non conforme')
    return
end

center = [];

if axea < 35 || axeb < 35 || axea > 100 || axeb > 100
  return
end

if axea / axeb > 1.8 || axeb / axea > 1.8
  return
end

%fprintf('%f, %f, %f, %f, %f\n', centrex, centrey, axea, axeb, angle);

center = [centrex, centrey];

s = sin(angle); c = cos(angle);
Q =[c -s; s c];
theta = [0:0.02:2*pi]';
xx = axea*cos(theta);
yy = axeb*sin(theta);

if (draw == 1)
    x = Q(1,:)*[xx yy]' + centrex;
    y = Q(2,:)*[xx yy]' + centrey;
	plot(x,y, '-','LineWidth',lineWidth);
end


function [centrex, centrey, axea, axeb, angle] = convertConicToParametric(par)
%this function will convert parameters from the form
%Ax^2 + Bxy + Cy^2 + Dx + Ey + F = 0   
%to
%centrex centrey axea axeb angle
%
%take as input :
%par = [A B C D E F]


%assure forme compatible
if size(par,1)==3
    par = debuildConic(par);
end

% Convert to geometric radii, and centers

thetarad = 0.5*atan2(par(2),par(1) - par(3));
cost = cos(thetarad);
sint = sin(thetarad);
sin_squared = sint.*sint;
cos_squared = cost.*cost;
cos_sin = sint .* cost;

Ao = par(6);
Au =   par(4) .* cost + par(5) .* sint;
Av = - par(4) .* sint + par(5) .* cost;
Auu = par(1) .* cos_squared + par(3) .* sin_squared + par(2) .* cos_sin;
Avv = par(1) .* sin_squared + par(3) .* cos_squared - par(2) .* cos_sin;


if(Auu==0 | Avv==0)
    %problem.  this is not a valid ellipse
    %make sure param are invalid and be easy to spot
    centrex = -1;
    centrey = -1;
    axea = 0;
    axeb = 0;
    angle = 0;
    return;   
end


% ROTATED = [Ao Au Av Auu Avv]

tuCentre = - Au./(2.*Auu);
tvCentre = - Av./(2.*Avv);
wCentre = Ao - Auu.*tuCentre.*tuCentre - Avv.*tvCentre.*tvCentre;

uCentre = tuCentre .* cost - tvCentre .* sint;
vCentre = tuCentre .* sint + tvCentre .* cost;

Ru = -wCentre./Auu;
Rv = -wCentre./Avv;

Ru = sqrt(abs(Ru)).*sign(Ru);
Rv = sqrt(abs(Rv)).*sign(Rv);


centrex = uCentre;
centrey = vCentre;
axea = Ru;
axeb = Rv;
angle = thetarad;




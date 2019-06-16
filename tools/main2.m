p1 = [0.000000000 1.00000000 0.000000000];
r1 = 2;
p2 = [2.00000000 3.232 0.000000000];
r2 = 1;

n = [.667359 .74473 -.0028850];
d = .000856;

figure(1);clf;
hold on;axis equal;
xlabel('x');
ylabel('y');
zlabel('z');

[x,y,z] = sphere(16);
mesh(x*r1+p1(1),y*r1+p1(2),z*r1+p1(3), 'FaceColor', 'none');
mesh(x*r2+p2(1),y*r2+p2(2),z*r2+p2(3), 'FaceColor', 'none');

shift = n;

quiver3(p2(1), p2(2), p2(3), shift(1), shift(2), shift(3));

pause

p2 = p2 + n*d;

figure(1);clf;
hold on;axis equal;
xlabel('x');
ylabel('y');
zlabel('z');

[x,y,z] = sphere(16);
mesh(x*r1+p1(1),y*r1+p1(2),z*r1+p1(3), 'FaceColor', 'none');
mesh(x*r2+p2(1),y*r2+p2(2),z*r2+p2(3), 'FaceColor', 'none');

shift = n*d;

quiver3(p2(1), p2(2), p2(3), shift(1), shift(2), shift(3));
origin = [0;0;0];
[hit, a, b,c, d] = gjk(origin);

if hit
    [n, d] = EPA(a, b, c, d, origin)
    %origin = origin + (n*d*1.05);
    %[hit, a, b, c, d] = gjk(origin)
end
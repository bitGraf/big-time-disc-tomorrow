function [hit, a, b, c, d] = gjk(off)
    hit = false;
    origin1 = [0;1;0] + off;
    origin2 = [2; 2.746; 0];
    drawSum(origin1, origin2);
    
    search_dir = [1;2;3];
    
    b = support( search_dir, origin1, origin2);
    a = support(-search_dir, origin1, origin2);
    
    search_dir = cross(a, b-a);
    
    c = support(search_dir, origin1, origin2);
   
    numIts = 0;
    maxIts = 30;
    
    R = [0;0;0];
    Rprev = [1000;1000;1000];
    
    while numIts < maxIts
        %pause(.5)
        search_dir = planeNormal(a, b, c)
        d = support(search_dir, origin1, origin2);
        Rprev = R;
        R = d;
        
        drawSum(origin1, origin2);
        drawSimplex(a, b, c, d);
        
        if containsOrigin(a, b, c, d)
            fprintf('Contains origin.\n');
            hit = true;
            break;
        end
        
        differ = norm(Rprev-R);
        fprintf('deltaR = %.3f\n', differ);
        
        if differ < .15
            fprintf('Converged at iteration %d\n', numIts);
            break;
        end
        
        [a,b,c,pass] = collapseSimplex(a,b,c,d);
        if pass == 0
            fprintf('nonconverging\n');
        end
        
        numIts = numIts + 1;
    end
end

function [v] = support(search_dir, origin1, origin2)
    d = search_dir/norm(search_dir);
    p1 =  d*2 + origin1;
    p2 = -d*1 + origin2;
    
    v = p1 - p2;
end

function [] = drawSimplex(a, b, c, d)    
    drawTri(a, b, c);
    drawTri(a, b, d);
    drawTri(b, c, d);
    drawTri(c, a, d);
end

function [] = drawTri(a, b, c)
    fill3([a(1), b(1), c(1)], ...
          [a(2), b(2), c(2)], ...
          [a(3), b(3), c(3)], 'r');
end

function [] = drawSum(origin1, origin2)
    [X,Y,Z] = sphere(12);
    O3 = origin1-origin2;
    R3 = 2 + 1;
    
    figure(1);
    clf;hold on;grid on;axis equal;
    mesh(X*R3+O3(1), Y*R3+O3(2), Z*R3+O3(3), 'FaceColor', 'none');
    plot3(0,0,0, 'k+', 'MarkerSize', 10);
end

function [na,nb,nc,pass] = collapseSimplex(a,b,c,d)
    [~,idx] = farthest(a, b, c, d);
    
    pass = 1;
    if idx == 1
        na = d;
        nb = b;
        nc = c;
    elseif idx == 2
        na = d;
        nb = a;
        nc = c;
    elseif idx == 3
        na = d;
        nb = a;
        nc = b;
    else
        na = d;
        nb = b;
        nc = c;
        pass = 0;
    end
end

function [v,idx] = farthest(a,b,c,d)
    [~,idx] = max([norm(a), norm(b), norm(c), norm(d)]);
    
    v = [0;0;0];
end

function [v] = planeNormal(a, b, c)
    n = cross(b - a, c - a);
    k = (a + b + c) /3.0;

    if (dot(k, n) > 0)
        n = -n;
    end
    
    v = n;
end

function [result] = testTri(a, b, c)
    np = [0;0;0] - a;
    na = a - a;
    nb = b - a;
    nc = c - a;
    
    X = (nb-na)/norm(nb-na);
    Y = cross(nb-na, nc-na);
    Z = cross(X, Y);
    
    tp = [dot(X, np), dot(Y, np), dot(Z, np)];
    tp(2) = 0;
    ta = [dot(X, na), dot(Y, na), dot(Z, na)];
    tb = [dot(X, nb), dot(Y, nb), dot(Z, nb)];
    tc = [dot(X, nc), dot(Y, nc), dot(Z, nc)];
    
    result = triContainsPoint(ta, tb, tc, tp);
end

function [result] = triContainsPoint(a, b, c, p)
    v0 = b-a;
    v1 = c-a;
    v2 = p-a;
    
    d00 = dot(v0, v0);
    d01 = dot(v0, v1);
    d11 = dot(v1, v1);
    d20 = dot(v2, v0);
    d21 = dot(v2, v1);
    
    denom = d00 * d11 - d01 * d01;

    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0 - v - w;
    
    result = (u < 1 && v < 1 && w < 1) && (u > 0 && v > 0 && w > 0);
end

function [result] = containsOrigin(a, b, c, d)
    if ~testTri(a, b, c)
        result = false;
    elseif ~testTri(a, b, d)
        result = false;
    elseif ~testTri(b, c, d)
        result = false;
    elseif ~testTri(c, a, d)
        result = false;
    else
        result = true;
    end
end
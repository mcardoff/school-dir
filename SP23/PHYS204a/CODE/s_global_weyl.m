function y=s_global_weyl(r,a,N)

if r==1
    y=s_weyl(a);
else
    y=speye(2);
end

for i=2:N
    if i==r
        y=kron(y,s_weyl(a));
    else
        y=kron(y,speye(2));
    end
end

y=sparse(y);

end

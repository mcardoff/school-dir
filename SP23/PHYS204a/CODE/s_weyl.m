function y=s_weyl(a)

X=[0 1; 1 0];
Z=[1 0; 0 -1];

y=X^a(1)*Z^a(2);

y=sparse(y);

end

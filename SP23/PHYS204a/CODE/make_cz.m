function y=make_cz(r,s,n)
% r is control, s is target

I=speye(2^n);
Zs=s_global_weyl(s,[0 1],n);
Zr=s_global_weyl(r,[0 1],n);

y=(1/2)*(I+Zr)+(1/2)*(I-Zr)*Zs;

end
function y=make_cnot(r,s,n)
% r is control, s is target

I=speye(2^n);
Xs=s_global_weyl(s,[1 0],n);
Zr=s_global_weyl(r,[0 1],n);

y=(1/2)*(I+Zr)+(1/2)*(I-Zr)*Xs;

end
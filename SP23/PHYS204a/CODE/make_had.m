function y=make_had(r,n)

X=s_global_weyl(r,[1 0],n);
Z=s_global_weyl(r,[0 1],n);

y=(1/sqrt(2))*(X+Z);

end




% random seed
rng('shuffle');

% number of spins 
n=2;
D=2^n;

% Pauli matrices
X=cell(n,1);
Y=cell(n,1);
Z=cell(n,1);
for r=1:n
    X{r}=s_global_weyl(r,[1 0],n);
    Y{r}=1i*s_global_weyl(r,[1 1],n);
    Z{r}=s_global_weyl(r,[0 1],n);
end

"many-body operators"
full(Z{1})
full(X{1})
full(Y{2})

"sample commutators"
full(X{1}*Y{1}-Y{1}*X{1})
full(X{1}*Y{2}-Y{2}*X{1})

H1=(1/4)*(X{1}*X{2}+Y{1}*Y{2}+Z{1}*Z{2});
"spectrum of S1*S2: "
eig(H1)









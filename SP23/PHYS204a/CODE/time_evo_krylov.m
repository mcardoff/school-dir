function y=time_evo_krylov(psi,H,dt,M)

% M = number of iterations
% for N=18 spins, M=20 seems quite adequate for time step of .1 and pretty
% good for time step of .5

[Qtmp,Htmp]=arnoldi(H,psi,M);

Htmp=Htmp(1:M,1:M);
if norm(Htmp-Htmp')>=1e-8
    norm(Htmp-Htmp')
end
Htmp=(Htmp+Htmp')/2;

Utmp=expm(-1i*dt*Htmp);

e1=zeros(M,1);
e1(1)=1;

y=Qtmp(:,1:M)*Utmp*e1;

end


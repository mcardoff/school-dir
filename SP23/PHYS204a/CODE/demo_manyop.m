% random seed
rng('shuffle');

% number of spins 
n=8;
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

% Hamiltonian
H=sparse(D,D);
J=1;
hx=1.2;
hz=.5;
for r=1:n-1
    H=H-J*Z{r}*Z{r+1};
end
for r=1:n
    H=H-hx*X{r}-hz*Z{r};
end

t=linspace(0,10,100);
dt=t(2)-t(1);
dU=expm(-1i*dt*full(H));

psi_1=zeros(D,1);
psi_1(1)=1;
for r=1:n
    psi_1=psi_1+Y{r}*psi_1;
    psi_1=psi_1/norm(psi_1);
end
psi_2=Z{1}*psi_1;

psi_1_evo=zeros(D,length(t));
psi_2_evo=zeros(D,length(t));
psi_1_evo(:,1)=psi_1;
psi_2_evo(:,1)=psi_2;

for ii=1:length(t)-1
    psi_1_evo(:,ii+1)=dU*psi_1_evo(:,ii);
    psi_2_evo(:,ii+1)=dU*psi_2_evo(:,ii);
end

psi_1_final=psi_1_evo(:,length(t));
psi_2_final=psi_2_evo(:,length(t));
delta_ev=zeros(4^n,1);
for k=0:4^n-1
    delta_ev(k+1)=real(psi_1_final'*num_2_op(k,n)*psi_1_final-psi_2_final'*num_2_op(k,n)*psi_2_final);
end

c1=sum(delta_ev)/4^n;
c2=sum((delta_ev-c1).^2)/4^n;

% figure()
% histogram(delta_ev);
% xlabel('$\langle O\rangle_{\psi_1} - \langle O\rangle_{\psi_2}$','interpreter','latex');
% ylabel('count');
% title('all possible simple measurements (Pauli strings)');
% set(gca,'fontsize',24);

sigma2=2/(D+1);
figure()
histogram(delta_ev,'BinWidth',sqrt(sigma2)/20);
xlabel('$\langle O\rangle_{\psi_1} - \langle O\rangle_{\psi_2}$','interpreter','latex');
ylabel('count');
title('all possible simple measurements (Pauli strings)');
set(gca,'fontsize',24);
hold on
x=linspace(-4*sqrt(2/(D+1)),4*sqrt(2/(D+1)),100);
plot(x,4^n*exp(-x.^2/(2*sigma2))./sqrt(2*pi*sigma2)*(sqrt(sigma2)/20),'linewidth',2)




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

% timing
t=linspace(0,20,200);
dt=t(2)-t(1);
dU=expm(-1i*dt*full(H)); % time evolution operator for time step dt

% initial states
psi_1=zeros(D,1);
psi_1(1)=1;
for r=1:n
    psi_1=psi_1+Y{r}*psi_1; % all +y
    psi_1=psi_1/norm(psi_1);
end
psi_2=Z{1}*psi_1; % first spin is -y

% setup time evolution and data structures
psi_1_evo=zeros(D,length(t));
psi_2_evo=zeros(D,length(t));
psi_1_evo(:,1)=psi_1;
psi_2_evo(:,1)=psi_2;

Y1_1=zeros(length(t),1);
Yn_1=zeros(length(t),1);
Y1_2=zeros(length(t),1);
Yn_2=zeros(length(t),1);

% compute
for ii=1:length(t)
    % read out y spins at site 1 and n
    Y1_1(ii)=psi_1_evo(:,ii)'*Y{1}*psi_1_evo(:,ii);
    Yn_1(ii)=psi_1_evo(:,ii)'*Y{n}*psi_1_evo(:,ii);
    Y1_2(ii)=psi_2_evo(:,ii)'*Y{1}*psi_2_evo(:,ii);
    Yn_2(ii)=psi_2_evo(:,ii)'*Y{n}*psi_2_evo(:,ii);
    
    % update    
    if ii<length(t)
        psi_1_evo(:,ii+1)=dU*psi_1_evo(:,ii);
        psi_2_evo(:,ii+1)=dU*psi_2_evo(:,ii);
    end
end

% plot, Ys should be real but have small imaginary parts

figure()
plot(t,real(Y1_1),'linewidth',2); 
ylim([-1,1]); ylabel('$\langle \sigma^y_1\rangle$','interpreter','latex'); 
xlabel('time ($ tJ/\hbar$)','interpreter','latex');
title('dynamics of initially polarized state')
set(gca,'fontsize',24);

figure()
plot(t,real(Y1_1),t,real(Y1_2),'linewidth',2);
ylim([-1,1]); ylabel('$\langle \sigma^y_1\rangle$','interpreter','latex');
xlabel('time ($ tJ/\hbar$)','interpreter','latex'); legend({'$+$y','$-$y'},'interpreter','latex');
title('comparing states')
set(gca,'fontsize',24);

figure()
plot(t(t>5),real(Y1_1(t>5)),'k-',t(t>5),real(Y1_2(t>5)),'k--','linewidth',2); 
ylim([-1,1]); ylabel('$\langle \sigma^y_1\rangle$','interpreter','latex'); 
xlabel('time ($ tJ/\hbar$)','interpreter','latex');
title('guessing game')
set(gca,'fontsize',24);

figure()
plot(t,real(Yn_1),t,real(Yn_2),'linewidth',2);
ylim([-1,1]); ylabel('$\langle \sigma^y_n\rangle$','interpreter','latex'); 
xlabel('time ($ tJ/\hbar$)','interpreter','latex');
title('propagation effects')
set(gca,'fontsize',24);


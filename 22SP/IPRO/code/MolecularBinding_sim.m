clearvars;close all;clc;

%% Similate the 2-compartment model Cp <-> Cf <-> Cb (K1, k2: for cp->cf and cf->cp, respectively; k3, k4: for cf->cb and cb-> cf, respectively

t = transpose(0:5:3*60);%column vector of time in min
cp_param = [0.5,5,0.5,60];%coefficients of the biexponential: 0.5*exp(-t/5)+0.5*exp(-t/60); % plasma input function
K1 = 0.08;% min^(-1)
k2 = 0.04;% min^(-1)
k3 = 0.6;% min^(-1) ! This is the most important parameter because it is directly proportional to concentration of the targeted biological molecular
k4 = 0.1;% min^(-1)
CT_0 = [0 0];
cp = cp_param(1)*exp(-t/cp_param(2))+cp_param(3)*exp(-t/cp_param(4));


% dcf/dt = K1cp-(k2+k3)cf+k4cb
% dcb/dt = k3cf-k4cb

[~,C_output] = ode45(@comp2mod, t, CT_0,[],K1,k2,k3,k4,cp_param);
cf = C_output(:,1);
cb = C_output(:,2);
% CT = vpcp+Cf+Cb
vp = 0.05; %corresponds to 5% of the tissue is the blood volume
CT = vp*cp+cf+cb;

% figure;plot(t,CT/sum(CT),'b*',t,cp/sum(cp),'r*',t,cf/sum(cf),'g*',t,cb/sum(cb),'c*')
% figure;plot(t,CT,'b*',t,cf,'g*',t,cb,'c*')

%% Add noise to data (Assumes "shot-noise" or noise that can be modeled by a Poisson Distribution & 16-bit detector)

% scale the signal to 10% of full dynamic range of the system

ST = poissrnd(0.1*2^16/max(CT)*CT);
figure;plot(t,ST,'k*',t,0.1*2^16/max(CT)*CT,'r')
cp_n = poissrnd(0.1*2^16/max(cp)*cp)/(0.1*2^16/max(cp));

%% Interpolate to help with fitting
step = 0.5;
t_i = transpose(t(1):step:t(end));
ST_i = interp1(t,ST,t_i,'spline');
cp_i = interp1(t,cp,t_i,'spline');

%% Fitting the data with the model we developed (adiabatic approximation to the 2-compartment model) ST = eta*K1*conv(cp,exp(-k2/(1+BP)*t)); BP = k3/k4

Starting = [1 1];
options = optimset('display','off');
[newParams,error] = lsqcurvefit(@AA2CM,Starting,t_i,ST_i,[0 0],[],options,cp_i);

%% test ftting
figure;plot(t_i, AA2CM(newParams,t_i,cp_i),'r',t,ST,'k.')
k2/(1+k3/k4)
newParams(2)





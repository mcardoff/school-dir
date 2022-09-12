clearvars;close all;clc;

%% Set some parameters

L = 1; % cm
D = 1e-2; % cm^2/s
dx = 0.1; % cm
nx = L/dx+1; % unitless integer
dur = 200; % s
C0 = 1e6; % concentration of leukocytes upstream of the prosthetic

%% Stability Concern

dt = dx^2/(2*D);
nt = ceil(dur/dt)+1;

%% Initialize the result matrix

C_out = zeros(nx,nt);

%% Initial conditions

C_out(1,1) = C0;
C_out(2:end,1) = 0;

%% Fill in C_out

A = D*dt/(dx^2); C = A;
B = 1-2*D*dt/(dx^2);
figure;plot(1:nx,C_out(:,1),'r')
for n = 1:nt-1
    for i = 2:nx-1
        C_out(i,n+1) = A*C_out(i+1,n)+B*C_out(i,n)+C*C_out(i-1,n);
        C_out(nx,n+1) = C_out(nx-1,n+1);
        C_out(1,n+1) = C0;%C_out(2,n+1);
    end
    plot(1:nx,C_out(:,n+1),'r');ylim([0,C0]);pause(0.3)% Changed location
end


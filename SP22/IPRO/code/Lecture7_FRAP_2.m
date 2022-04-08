clearvars;close all;clc;

%% Set some parameters

L = 30; % um
D = 2e-2; % um^2/s
dx = 0.2; % um
nx = L/dx+1; % unitless integer
dur = 200; % s
C0 = 1e6;% concentration of fluorophore in cell before photobleaching

%% Stability Concern

dt = dx^2/(4*D);
nt = ceil(dur/dt)+1;
%% Initialize the result matrix

C_out = zeros(nx,nx,nt);

%% Create the initial condition

C_out(:,:,1) = ones(nx,nx)*C0;

%% simulate a region of photobleaching
% photobleach a circle in the middle of the cell with radius r
r = 2; % microns
x0 = floor(nx/2)*dx;%multiply dx here
y0 = x0;
x = transpose(0:dx:(nx-1)*dx);
y = x;
counter = 0;
for i = 1:nx
    for j = 1:nx
        if ((x(i)-x0)^2+(y(j)-y0)^2) <= r^2
            C_out(i,j,1) = 0;
            counter = counter+1;
        end
    end
end

figure;imagesc(C_out(:,:,1));axis image;caxis([0 C0])

%% Fill in C_out

A = D*dt/(dx^2); C = A;
B = 1-4*D*dt/(dx^2);
figure;
subplot(121);imagesc(C_out(:,:,1));axis image;caxis([0 C0])
subplot(122);plot(x,squeeze(C_out(floor(nx/2),:,1)),'k')
C_FRAP = zeros(nt-1,1);
for n = 1:nt-1
    for i = 2:nx-1
        for j = 2:nx-1
            C_out(i,j,n+1) = A*(C_out(i+1,j,n)+C_out(i-1,j,n))+B*C_out(i,j,n)+C*(C_out(i,j+1,n)+C_out(i,j-1,n));
            if ((x(i)-x0)^2+(y(j)-y0)^2) <= r^2
                C_FRAP(n) = C_FRAP(n) + C_out(i,j,n+1);
            end
            % Boundary conditions
        end
    end
    C_out(nx,:,n+1) = C0;
    C_out(1,:,n+1) = C0;
    C_out(:,1,n+1) = C0;
    C_out(:,nx,n+1) = C0;
    pause(0.01)
    subplot(121);imagesc(C_out(:,:,n));axis image;caxis([0 C0])
    subplot(122);plot(x,squeeze(C_out(floor(nx/2),:,n)),'k');ylim([0 C0])
end
t = transpose(dt:dt:(nt-1)*dt);
figure;plot(dt:dt:(nt-1)*dt,C_FRAP/counter,'r')
[~,pos] = min(abs(C_FRAP/counter-C0/2));

D_fit = 0.22*r^2/t(pos)

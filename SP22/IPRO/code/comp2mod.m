function dydt = comp2mod(t,y,K1,k2,k3,k4,cp_param)

dydt = zeros(2,1);
cp = cp_param(1)*exp(-t/cp_param(2))+cp_param(3)*exp(-t/cp_param(4));
dydt(1) = K1*cp-(k2+k3)*y(1)+k4*y(2);
odydt(2) = k3*y(1)-k4*y(2);

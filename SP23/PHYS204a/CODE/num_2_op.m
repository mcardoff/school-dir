function y=num_2_op(k,n)

ind=zeros(n,1);
for ii=1:n
    ind(ii)=mod(k,4);
    k=(k-ind(ii))/4;
end

op=speye(2^n,2^n);
for ii=1:n
    if ind(ii)==1
        op=s_global_weyl(ii,[1 0],n)*op;
    end
    if ind(ii)==2
        op=1i*s_global_weyl(ii,[1 1],n)*op;
    end
    if ind(ii)==3
        op=s_global_weyl(ii,[0 1],n)*op;
    end
end

y=op;

end
    
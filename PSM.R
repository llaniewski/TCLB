


# nu*u''-ku
# u''=(k/nu)*u
# u = Ae^sqrt(k/nu)x + Be^-sqrt(k/nu)x
# u = 0 for x = 0
# u = w for x = 1
# A + B = 0
# Ae^sqrt(k/nu) + Be^-sqrt(k/nu) = w
# A(e^sqrt(k/nu) - e^-sqrt(k/nu)) = w
# A = w/(e^sqrt(k/nu) - e^-sqrt(k/nu))
# s = sqrt(k/nu)
# u  = A*(exp(s*x) - exp(-s*x))
# u' = A*s*(exp(s*x) + exp(-s*x))
# u'(1) = A*s*(exp(s) + exp(-s))
# u'(1) = w*s*(exp(s) + exp(-s))/(exp(s) - exp(-s))
#
# u'(1)

w = 1
k = 0.1
nu = 1
s = sqrt(k/nu)
A = w/(exp(s) - exp(-s))

x = seq(0,1,len=100)
u = A*(exp(s*x) - exp(-s*x))
plot(x,u)


k = exp(seq(-5,10,len=100))
s = sqrt(k/nu)
plot(s/tanh(s),k)


plot(1-tanh(s)/s,k,log="y")

plot(1-tanh(s)/s,log(k),log="")
p = seq(-10,10,len=300)
lines((tanh(p/3-0.5)+1)/2,p)

plot(1-tanh(s)/s,k,log="")

plot(log(k),log(p/(1-p)))
plot(k,p/(1-p))

plot(log(k),p)

p = 1-tanh(s)/s
plot(log(p/(1-p)),log(k))
#abline(0,1)
#abline(0,2)
p = seq(-10,10,len=300)
lines(p,log(exp(p)))
lines(p,log(exp(p)^2))
lines(p,log(3*exp(p)+exp(p)^2))


p = 1-tanh(s)/s
plot(log(p/(1-p)),log(k))

plot(log(s/tanh(s)-1),log(k))
lines(p,log(3*exp(p)+exp(p)^2))

#p=log(s/tanh(s)-1)
#log(k) = log(3*exp(p)+exp(p)^2)

#p = s/tanh(s)-1
#k = 3*p+p^2

k = exp(seq(-5,10,len=100))
s = sqrt(k/nu)
p = s/tanh(s)-1
plot(p,k)
lines(p,3*p+p^2)
k = 3*p+p^2

## s = sqrt(k/nu)
## p = s/tanh(s)-1
## k ~= 3*p+p^2

## vf = 1-tanh(s)/s
## vf = 1-tanh(s)/s
vf = 1-tanh(s)/s


k = exp(seq(-5,10,len=100))
s = sqrt(k/nu)
plot(1-tanh(s)/s,k,log="y",type="l")
vf = seq(0,1,len=200)
p = vf/(1-vf)
lines(vf,3*p+p^2)

plot(vf,(3*vf*(1-vf)+vf^2)/(1-vf)^2,log="y")
plot(vf,nu*vf*(3-2*vf)/(1-vf)^2,log="y")

plot(1-tanh(s)/s,k,log="")

vf = seq(0,1,len=200)
plot(vf,1/(1-vf),log="y")


# s/tanh(s) = 1/(1-vf)
# tanh(s)/s = (1-vf)
# 1-tanh(s)/s = vf

  
  
  
s = seq(-3,3,len=300)
plot(s,s*(exp(s) + exp(-s))/(exp(s) - exp(-s)))
plot(s,s/tanh(s))

# s/tanh(s) = vf




# f = -ku
# u = uhat + f/2
# u = uhat -k/2u
# u(1+k/2) = uhat
# u(1+k/2) = uhat
# u = uhat
# f = -u*k/(1+k/2)

plot(vf,nu*vf*(3-2*vf)/(1-vf)^2,log="y")

k = nu*vf*(3-2*vf)/(1-vf)^2
khat = nu*vf*(3-2*vf)/(1-vf)^2/(1+nu*vf*(3-2*vf)/(1-vf)^2/2)
khat = k/(1+k/2)
khat = 1/(1/k+1/2)


khat = 2*nu*vf*(3-2*vf)/(2*(1-vf)^2 + nu*vf*(3-2*vf))
plot(vf, khat)


khat = outer(vf, seq(0,1,len=10), function(vf, nu) 2*nu*vf*(3-2*vf)/(2*(1-vf)^2 + nu*vf*(3-2*vf)))
matplot(vf, khat)


khat = outer(vf, seq(0,1/6,len=10), function(vf, nu) 2*nu*vf*(3-2*vf)/(2+(3*nu-4)*vf+2*(1-nu)*vf^2))
matplot(vf, khat)


nu = 1/6
k = nu*vf*(3-2*vf)/((1-vf)*(1-vf));
khat = 1/(2.0/k+1);
# 2.0/k+1 = 2.0*((1-vf)*(1-vf))/(nu*vf*(3-2*vf)) + 1
# 2.0/k+1 = (2.0*((1-vf)*(1-vf)) + (nu*vf*(3-2*vf)))/(nu*vf*(3-2*vf))
khat = nu*vf*(3-2*vf)/(2.0*(1-vf)*(1-vf) + nu*vf*(3-2*vf))
matplot(vf, khat)


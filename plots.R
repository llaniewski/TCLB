setwd("~/repo/TCLB/cfdgo/develop/")

# N = kg*m/s2
# N = kg*m/s2
# kg/m3 = 1 >= kg=m^3

tab=read.csv("output/parts.csv")

#plot(tab$p1_x,tab$p1_fx)
#plot(tab$p1_x,tab$p0_fx)

# roughness (rms) = 0.1-0.5 e-6
# diameter 5 e-6

matplot(tab$p1_x-2,cbind(tab$p0_fx,-tab$p1_fx),type="l",lty=1)


tab=read.csv("output/parts.csv")
matplot(tab$p1_x-2,-cbind(tab$p0_fx,-tab$p1_fx)/8^3,type="l",lty=1,log="xy")

tab=read.csv("output/parts16.csv")
matplot(tab$p1_x-2,-cbind(tab$p0_fx,-tab$p1_fx)/16^3,type="l",lty=1,add=TRUE)

x = exp(seq(-5,0,len=100))
nu=1; udiff=0.1; r=0.5
lines(x,6*pi*nu*udiff/x*(r/2)^2,log="xy",col=4)



tab=read.csv("output/lub_4_96_sp.csv")
matplot(tab$p1_x-tab$p0_x-1,-cbind(tab$p0_fx,-tab$p1_fx),type="l",lty=1,log="xy",xlim=c(0.01,20))
tab=read.csv("output/lub_8_192_sp.csv")
matplot(tab$p1_x-tab$p0_x-1,-cbind(tab$p0_fx,-tab$p1_fx),type="l",lty=1,add=TRUE)
tab=read.csv("output/lub_16_384_sp.csv")
matplot(tab$p1_x-tab$p0_x-1,-cbind(tab$p0_fx,-tab$p1_fx),type="l",lty=1,add=TRUE)
tab=read.csv("output/lub_32_768_sp.csv")
matplot(tab$p1_x-tab$p0_x-1,-cbind(tab$p0_fx,-tab$p1_fx),type="l",lty=1,add=TRUE)



tab=read.csv("output/lub_4_96_sp.csv")
matplot(tab$p1_x-tab$p0_x-1,-cbind(tab$p0_fx,-tab$p1_fx),type="l",lty=1,log="xy",xlim=c(0.01,20))
tab=read.csv("output/lub_8_384_sp.csv")
matplot(tab$p1_x-tab$p0_x-1,-cbind(tab$p0_fx,-tab$p1_fx),type="l",lty=1,add=TRUE)
tab=read.csv("output/lub_16_1536_sp.csv")
matplot(tab$p1_x-tab$p0_x-1,-cbind(tab$p0_fx,-tab$p1_fx),type="l",lty=1,add=TRUE)
tab=read.csv("output/lub_32_768_sp.csv")
matplot(tab$p1_x-tab$p0_x-1,-cbind(tab$p0_fx,-tab$p1_fx),type="l",lty=1,add=TRUE)




x = exp(seq(-5,3,len=100))
nu=1; udiff=1; v=udiff/2; r=0.5
drag = rep(6*pi*nu*r*v,length(x))
lub = 6*pi*nu*udiff/x*(r*r/(r+r))^2
lines(x,lub,col=4)
lines(x,drag,col=4)
lines(x,lub+drag,col=4)


lines(x,6*pi*nu*udiff/x*(r/2)^2,log="xy",col=4)

# kg/m3 * m2/s * m/s / m * m^2
# kg/s2 * m







tab=read.csv("output/lub_4_96_sp.csv")
plot(tab$p1_x-tab$p0_x-1,tab$p1_fx,type="l",lty=1,log="xy",xlim=c(0.01,20),ylim=c(5,50),col=2)
tab=read.csv("output/lub_8_192_sp.csv")
lines(tab$p1_x-tab$p0_x-1,tab$p1_fx,col=3)
tab=read.csv("output/lub_16_384_sp.csv")
lines(tab$p1_x-tab$p0_x-1,tab$p1_fx,col=4)
x = exp(seq(-5,3,len=100))
nu=1; udiff=1; v=udiff/2; r=0.5
delta = 1/50
drag = rep(6*pi*nu*r*v,length(x))
lub = 6*pi*nu*udiff/x*(r*r/(r+r))^2
lub2 = 6*pi*nu*udiff/(x+delta)*(r*r/(r+r))^2
lines(x,lub+drag,lwd=2)
lines(x,lub2+drag,lwd=2,lty=2)
abline(v=delta,lty=2)
#abline(v=1/4, lty=2,col=2)
#abline(v=1/8, lty=2,col=3)
#abline(v=1/16,lty=2,col=4)


tab=read.csv("output/lub2_4_96_sp.csv")
plot(tab$p1_x-tab$p0_x-1,tab$p1_fx,type="l",lty=1,log="xy",xlim=c(0.01,20),ylim=c(5,50),col=2)
tab=read.csv("output/lub2_8_192_sp.csv")
lines(tab$p1_x-tab$p0_x-1,tab$p1_fx,col=3)
tab=read.csv("output/lub2_16_384_sp.csv")
lines(tab$p1_x-tab$p0_x-1,tab$p1_fx,col=4)
x = exp(seq(-5,3,len=100))
nu=1; udiff=1; v=udiff/2; r=0.5
delta = 1/10
drag = rep(6*pi*nu*r*v,length(x))
lub = 6*pi*nu*udiff/x*(r*r/(r+r))^2
lub2 = 6*pi*nu*udiff/(x+delta)*(r*r/(r+r))^2
lines(x,lub+drag,lwd=2)
lines(x,lub2+drag,lwd=2,lty=2)
abline(v=delta,lty=2)



tab=read.csv("output/lub3_4_96_sp.csv")
plot(tab$p1_x-tab$p0_x-1,tab$p1_fx,type="l",lty=1,log="xy",xlim=c(0.01,20),ylim=c(5,50),col=2)
#plot(tab$p1_x-tab$p0_x-1,tab$p1_fx,type="l",lty=1,log="y",xlim=c(0.0,2),ylim=c(5,50),col=2)
tab=read.csv("output/lub3_8_192_sp.csv")
lines(tab$p1_x-tab$p0_x-1,tab$p1_fx,col=3)
tab=read.csv("output/lub3_16_384_sp.csv")
lines(tab$p1_x-tab$p0_x-1,tab$p1_fx,col=4)
x = exp(seq(-5,3,len=100))
nu=1; udiff=1; v=udiff/2; r=0.5
delta = 1/10
drag = rep(6*pi*nu*r*v,length(x))
lub = 6*pi*nu*udiff/x*(r*r/(r+r))^2
lines(x,lub+drag,lwd=2)
delta = 1/4
lub2 = 6*pi*nu*udiff/(x+delta)*(r*r/(r+r))^2
lines(x,lub2+drag,lwd=2,lty=2,col=2)
delta = 1/8
lub2 = 6*pi*nu*udiff/(x+delta)*(r*r/(r+r))^2
lines(x,lub2+drag,lwd=2,lty=2,col=3)
delta = 1/16
lub2 = 6*pi*nu*udiff/(x+delta)*(r*r/(r+r))^2
lines(x,lub2+drag,lwd=2,lty=2,col=4)



tab=read.csv("output/lub_16_384_sp.csv")
plot(tab$p1_x-tab$p0_x-1,-tab$p0_fx,log="xy")

tab=read.csv("output/lub_s_16_384_sp.csv")
lines(2*tab$p0_x-1,tab$p0_fx*4,log="xy")


tab=read.csv("output/lub_8_192_sp.csv")
tab2=read.csv("output/drag_8_192_sp.csv")

plot(tab$p1_x-tab$p0_x-1,tab$p1_fx,type="l",lty=1,log="xy",xlim=c(0.005,20),ylim=c(1,100),col=2)
lines(tab$p1_x-tab$p0_x-1,-tab2$p0_fx)
lines(x,lub+drag,lwd=2)
lines(x,drag,lwd=2)

lines(tab$p1_x-tab$p0_x-1,tab$p1_fx+tab2$p0_fx)
lines(x,lub,lwd=2)


plot(tab$p0_x)
plot(tab$p1_x)

tab=read.csv("output/lub_1_0.02_s_4_96_0.02_sp.csv")
plot( tab$p0_x-0.5,4*tab$p0_fx,type="l",lty=1,log="xy",col=2,xlim=c(0.005,10),ylim=c(5,50))
tab=read.csv("output/lub_1_0.02_s_8_192_0.02_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=3)
tab=read.csv("output/lub_1_0.02_s_16_384_0.02_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=4)
tab=read.csv("output/lub_1_0.02_s_32_768_0.02_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=5)

tab=read.csv("output/lub_1_0.02_4_96_0.02_sp.csv")
plot( tab$p0_x-0.5,tab$p0_fx,type="l",lty=1,log="xy",col=2,xlim=c(0.005,10),ylim=c(5,50))
tab=read.csv("output/lub_1_0.02_8_192_0.02_sp.csv")
lines(tab$p0_x-0.5,tab$p0_fx,col=3)
tab=read.csv("output/lub_1_0.02_16_384_0.02_sp.csv")
lines(tab$p0_x-0.5,tab$p0_fx,col=4)
tab=read.csv("output/lub_1_0.02_32_768_0.02_sp.csv")
lines(tab$p0_x-0.5,tab$p0_fx,col=5)

tab=read.csv("output/lub_1_0.1_s_4_96_0.1_sp.csv")
plot( tab$p0_x-0.5,4*tab$p0_fx,type="l",lty=1,log="xy",col=2,xlim=c(0.005,10),ylim=c(5,50))
tab=read.csv("output/lub_1_0.1_s_8_192_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=3)
tab=read.csv("output/lub_1_0.1_s_16_384_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=4)
tab=read.csv("output/lub_1_0.1_s_32_768_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=5)
x = exp(seq(-5,3,len=100))
nu=1; udiff=1; v=udiff/2; r=0.5
drag = rep(6*pi*nu*r*v,length(x))
lub = 6*pi*nu*udiff/x*(r*r/(r+r))^2
lines(x,drag*1.12+lub*0.7,lwd=2)
abline(v=1/4, lty=2,col=2)
abline(v=1/8, lty=2,col=3)
abline(v=1/16,lty=2,col=4)

lub = 6*pi*nu*udiff/(x+0.03)*(r*r/(r+r))^2
lines(x,drag*1.12+lub*0.7,lwd=2)



tab=read.csv("output/lub_1_0.1_s_4_96_0.1_sp.csv")
plot( tab$p0_x-0.5,4*tab$p0_fx,type="l",lty=1,log="xy",col=2,xlim=c(0.005,10),ylim=c(5,50))
tab=read.csv("output/lub_1_0.1_s_8_384_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=3)
tab=read.csv("output/lub_1_0.1_s_16_1536_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=4)
tab=read.csv("output/lub_1_0.1_s_32_6144_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=5)
x = exp(seq(-5,3,len=100))
nu=1; udiff=1; v=udiff/2; r=0.5
drag = rep(6*pi*nu*r*v,length(x))
lub = 6*pi*nu*udiff/x*(r*r/(r+r))^2
lines(x,drag+lub,lwd=2)
abline(v=1/4, lty=2,col=2)
abline(v=1/8, lty=2,col=3)
abline(v=1/16,lty=2,col=4)




tab=read.csv("output/lub_1_0.1_s_4_96_0.1_sp.csv")
plot( tab$p0_x-0.5,4*tab$p0_fx,type="l",lty=1,log="xy",col=2,xlim=c(0.005,10),ylim=c(5,50))
#                     lub_1_0.1_s_8_384_0.1
tab=read.csv("output/lub_1_0.1_s_8_384_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=3)
tab=read.csv("output/lub_1_0.1_s_16_1536_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=4)
tab=read.csv("output/lub_1_0.1_s_32_6144_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=5)



tab=read.csv("output/drag_4_96_sp.csv")
plot(tab$p0_x,-tab$p0_fx,type="l",lty=1,log="y",xlim=c(0.01,20),col=2)
tab=read.csv("output/drag_8_192_sp.csv")
lines(tab$p0_x,-tab$p0_fx,col=3)
tab=read.csv("output/drag_16_384_sp.csv")
lines(tab$p0_x,-tab$p0_fx,col=3)

x = exp(seq(-5,3,len=100))
nu=1; udiff=1; v=udiff/2; r=0.5
drag = rep(6*pi*nu*r*v,length(x))
lub = 6*pi*nu*udiff/x*(r*r/(r+r))^2
lines(x,drag,lwd=2)
abline(v=1/4, lty=2,col=2)
abline(v=1/8, lty=2,col=3)
abline(v=1/16,lty=2,col=4)



tab=read.csv("output/lub_4_96_sp.csv")
write.csv(tab,"output/drag_4_96_sp.csv",row.names=FALSE)
tab=read.csv("output/lub_8_192_sp.csv")
write.csv(tab,"output/drag_8_192_sp.csv",row.names=FALSE)
tab=read.csv("output/lub_16_384_sp.csv")
write.csv(tab,"output/drag_16_384_sp.csv",row.names=FALSE)




d = seq(0.8,1.2,len=200)
r = 10
d = d*r
plot(d, pmax(0,pmin(1,ifelse((d - r) < -1,1,(r -  0.084/r + 0.5 - d)))))
abline(v=r+c(-0.5,0.5),lty=2)





tab=read.csv("output/lub_1_0.1_s_4_96_0.1_sp.csv")
plot( tab$p0_x-0.5,4*tab$p0_fx,type="l",lty=1,log="xy",col=2,xlim=c(0.005,10),ylim=c(5,50))
tab=read.csv("output/lub_1_0.1_s_8_384_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=3)
tab=read.csv("output/lub_1_0.1_s_16_1536_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=4)
tab=read.csv("output/lub_1_0.1_s_32_6144_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=5)


tab=read.csv("output/lub_2_0.1_s_4_96_0.1_sp.csv")
plot( tab$p0_x-0.5,4*tab$p0_fx,type="l",lty=1,log="xy",col=2,xlim=c(0.005,10),ylim=c(5,50))
tab=read.csv("output/lub_2_0.1_s_8_192_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=3)
tab=read.csv("output/lub_2_0.1_s_16_384_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=4)
tab=read.csv("output/lub_2_0.1_s_32_768_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=5)


tab=read.csv("output/lub_3_0.1_s_4_96_0.1_sp.csv")
plot( tab$p0_x-0.5,4*tab$p0_fx,type="l",lty=1,log="xy",col=2,xlim=c(0.005,10),ylim=c(5,50))
tab=read.csv("output/lub_3_0.1_s_8_192_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=3)
tab=read.csv("output/lub_3_0.1_s_16_384_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=4)
tab=read.csv("output/lub_3_0.1_s_32_768_0.1_sp.csv")
lines(tab$p0_x-0.5,4*tab$p0_fx,col=5)


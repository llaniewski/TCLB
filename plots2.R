r = 1/2
nu = 1
u = 1
vpart = u/2
stokes_drag = 6*pi*nu*r*vpart
ref = stokes_drag
de = exp(seq(-7,3,len=100))
#1/a = 1/r+1/r
a = r*r/(r+r)
eta = 0.0170*2
x=exp(seq(-5,5,len=300))

pdf(file="lub_conv_smooth.pdf")
plot(NA,log="xy",xlab="distance / d", ylab="F / Fstokes",xlim=c(0.001,20),ylim=c(1,25),main="Smooth particle")
tab=read.csv("output/lub_big_good_0.1_s_4_768_sp.csv")
lines(tab$p0_x-r,4*tab$p0_fx/ref,col=2)
tab=read.csv("output/lub_big_good_0.1_s_8_1536_sp.csv")
lines(tab$p0_x-r,4*tab$p0_fx/ref,col=3)
tab=read.csv("output/lub_big_good_0.1_s_16_3072_sp.csv")
lines(tab$p0_x-r,4*tab$p0_fx/ref,col=4)
tab=read.csv("output/lub_big_good_0.1_s_32_6144_sp.csv")
lines(tab$p0_x-r,4*tab$p0_fx/ref,col=5)
lub = u/(x/r)*(a/r)^2/vpart
#lines(x,1+lub,lwd=2)

#lub = u/(x/r)^0.85*(a/r)^2/vpart
#lines(x,1.05+lub,lwd=2)
abline(-0.1,-0.6)
legend("topright",legend = c(paste("d =",c(4,8,16,32),"el."),"Analitical"),col=c(2:5,1),lty=1,lwd=c(rep(1,4),2),bg = "white")
dev.off()

pdf(file="lub_conv_rough.pdf")
plot(NA,log="xy",xlab="distance / d", ylab="F / Fstokes",xlim=c(0.001,20),ylim=c(1,25),main="Synthetic rough particle (RMS/d=0.00761)")
tab=read.csv("output/lub_big_rough_0.1_s_4_768_sp.csv")
lines(tab$p0_x-r,4*tab$p0_fx/ref,col=2)
tab=read.csv("output/lub_big_rough_0.1_s_8_1536_sp.csv")
lines(tab$p0_x-r,4*tab$p0_fx/ref,col=3)
tab=read.csv("output/lub_big_rough_0.1_s_16_3072_sp.csv")
lines(tab$p0_x-r,4*tab$p0_fx/ref,col=4)
tab=read.csv("output/lub_big_rough_0.1_s_32_6144_sp.csv")
lines(tab$p0_x-r,4*tab$p0_fx/ref,col=5)
lub = u/((x+eta)/r)*(a/r)^2/vpart
lines(x,1+lub,lwd=2)
legend("topright",legend = c(paste("d =",c(4,8,16,32),"el."),paste0("Analitical (eta=",eta,")")),col=c(2:5,1),lty=1,lwd=c(rep(1,4),2),bg = "white")
dev.off()

pdf(file="lub_d10.pdf")
plot(NA,log="xy",xlab="distance / d", ylab="F / Fstokes",xlim=c(0.001,20),ylim=c(1,25),main="Present work")
tab=read.csv("output/lub_big_base_0.1_10_1920_sp.csv")
lines(tab$p0_x-r,tab$p0_fx/ref,col=1)
lub = u/((x+eta)/r)*(a/r)^2/vpart
lines(x,1+lub,lwd=1,lty=2)
legend("topright",legend = c("smooth particle - d=10",paste0("rough particle - Analitical (eta=",eta,")")),col=c(2:5,1),lty=1,lwd=c(rep(1,4),2),bg = "white")
dev.off()


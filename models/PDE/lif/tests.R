
library(polyAlgebra)
rowMax = function(x) apply(x,1,max)


tab = read.table("lebedev_015.txt")
a = tab[,1:2]*pi/180
sph_weights = tab[,3]
sph_points = cbind(
    sin(a[,2])*sin(a[,1]),
    sin(a[,2])*cos(a[,1]),
    cos(a[,2])
)
sph_points[abs(sph_points) < 1e-14] = 0

plot(sph_points,asp=1)


sel = apply(sph_points>=0,1,all)

v = sph_points[sel,]

a = c(0.3,0.4,0.5)
a = c(0.3,0.3,0.3)


A=seq(0,pi/2,len=200)
dt = 1/sqrt(3)
plot(cos(A),sin(A),asp=1,type="l")
points(exp(1i*a),pch=16,cex=2)
points(exp(dt*1i*(v%*%a)),cex=2)
points(1+dt*v %*% (exp(1i*a) - 1),cex=2,pch=2)

points(exp(1i*(v%*%a)))

exp(1i*c(0,a[1],a[2],a[1]+a[2],a[3],a[1]+a[3],a[2]+a[3],a[1]+a[2]+a[3]))

P = as.matrix(expand.grid(x=1:2-1,y=1:2-1,z=1:2-1))
P %*% a

get_polys = function(v) {
    ret = lapply(seq_len(nrow(P)), function(i) {
        w = 1
        if (P[i,1] == 1) w = w * v[,1]
        if (P[i,1] == 0) w = w * (1-v[,1])
        if (P[i,2] == 1) w = w * v[,2]
        if (P[i,2] == 0) w = w * (1-v[,2])
        if (P[i,3] == 1) w = w * v[,3]
        if (P[i,3] == 0) w = w * (1-v[,3])
        w
    })
    do.call(cbind,ret)
}

dt = 1/sqrt(3)
lam1 = exp(dt*1i*(v%*%a))
lam2 = 1+dt*v %*% (exp(1i*a) - 1)
lam3 = get_polys(dt*v) %*% exp(1i*(P %*% a))

plot(cos(A),sin(A),asp=1,type="l")
points(exp(1i*a),pch=16,cex=2)
points(lam1,cex=2)
points(lam2,cex=2,pch=2)
points(lam3,cex=2,pch=3)

plot(NA, xlim=c(0.8,1.1), ylim=c(-0.05,0.05))
abline(h=0)
abline(v=1)
dt = 1/sqrt(3)
lam1 = exp(dt*1i*(v%*%a))
lam2 = 1+dt*v %*% (exp(1i*a) - 1)
lam3 = get_polys(dt*v) %*% exp(1i*(P %*% a))
points((lam2/lam1)^(1/dt),cex=2,pch=1,col=2)
points((lam3/lam1)^(1/dt),cex=2,pch=2,col=2)
dt = 1
lam1 = exp(dt*1i*(v%*%a))
lam2 = 1+dt*v %*% (exp(1i*a) - 1)
lam3 = get_polys(dt*v) %*% exp(1i*(P %*% a))
points((lam2/lam1)^(1/dt),cex=2,pch=1,col=3)
points((lam3/lam1)^(1/dt),cex=2,pch=2,col=3)
dt = 0.1
lam1 = exp(dt*1i*(v%*%a))
lam2 = 1+dt*v %*% (exp(1i*a) - 1)
lam3 = get_polys(dt*v) %*% exp(1i*(P %*% a))
points((lam2/lam1)^(1/dt),cex=2,pch=1,col=4)
points((lam3/lam1)^(1/dt),cex=2,pch=2,col=4)



library(polyAlgebra)
rowMax = function(x) apply(x,1,max)


tab = read.table(findFileInDirs("lebedev_015.txt",include.dir))
a = tab[,1:2]*pi/180
sph_weights = tab[,3]
sph_points = cbind(
    sin(a[,2])*sin(a[,1]),
    sin(a[,2])*cos(a[,1]),
    cos(a[,2])
)
sph_points[abs(sph_points) < 1e-14] = 0

N = 2
I = expand.grid(x = 0:N, y = 0:N, z = 0:N)
I = I[rowSums(I) <= N,]
I = I[order(rowSums(I),-rowMax(I),-I[,1],-I[,2],-I[,3]),]

ret = 1
for (i in 1:3) {
    ret = ret * outer(sph_points[,i],I[,i],"^")
}
M = diag(sqrt(sph_weights)) %*% ret

Mqr = qr(M)
R = qr.R(Mqr)

sel = abs(diag(R)) > 1e-14
K = sum(sel)
pick = Mqr$pivot[sel]
G = solve(R[sel,sel]) %*% diag(diag(R)[sel])

monomials = PV("x")^I$x * PV("y")^I$y * PV("z")^I$z
polies = monomials[pick] %*% G

dirs = data.frame(x=c(1,-1,0,0,0,0), y=c(0,0,1,-1,0,0), z=c(0,0,0,0,1,-1))
tab = expand.grid(poly=1:K, dir=seq_len(nrow(dirs)))
tab$moment = paste("m",tab$poly,sep="_")
tab$field = paste(tab$moment,tab$dir,sep="_")
tab$dx = dirs$x[tab$dir]
tab$dy = dirs$y[tab$dir]
tab$dz = dirs$z[tab$dir]

AddDensity( name=tab$field, dx=tab$dx, dy=tab$dy, dz=tab$dz, group="f")

AddQuantity(name="J", unit="1")
AddQuantity(name="C", unit="1")
AddQuantity(name="U", unit="1", vector=T)


AddSetting(name="Source", default=0, zonal=T)
AddSetting(name="Concentration", default=0, zonal=T)
AddSetting(name="Absorption", default=0)
AddSetting(name="Emission", default=0)

#Node types for boundaries
AddNodeType(name="Wall", group="BOUNDARY")
AddNodeType(name="Collision", group="COLLISION")

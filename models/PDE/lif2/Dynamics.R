
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


M = nrow(sph_points)


dirs = data.frame(
    x=c(0,1,-1,0, 0,0, 0),
    y=c(0,0, 0,1,-1,0, 0),
    z=c(0,0, 0,0, 0,1,-1)
)
tab = data.frame(sph_points)
names(tab) = paste0("d",c("x","y","z"))
tab$weight = sph_weights
tab$field = paste0("f[",seq_len(nrow(tab))-1,"]")


AddDensity( name=tab$field, group="f")
AddField( name=tab$field, dx=c(-1,1), dy=c(-1,1), dz=c(-1,1))

AddQuantity(name="J", unit="1")
AddQuantity(name="C", unit="1")
AddQuantity(name="U", unit="1", vector=T)


AddSetting(name="Source", default=0, zonal=T)
AddSetting(name="SpeedOfLight", default=1/sqrt(3), zonal=T)
AddSetting(name="Concentration", default=0, zonal=T)
AddSetting(name="Absorption", default=0)
AddSetting(name="Emission", default=0)

#Node types for boundaries
AddNodeType(name="Wall", group="BOUNDARY")
AddNodeType(name="Collision", group="COLLISION")

source("lib/lattice.R")

U = d2q9

xy = c("x","y")
xyz = c("x","y","z")
i9 = 1:9-1

AddDensity( name=paste0("g",i9), dx= U[,1], dy= U[,2], group="g")
AddDensity( name=paste0("h",i9), dx= U[,1], dy= U[,2], group="h")

AddQuantity(name="H",unit="1")
AddQuantity(name="P",unit="1Pa")
AddQuantity(name="VF",unit="1")
AddQuantity(name="U", unit="m/s",vector=T)
AddQuantity(name="Uf",unit="m/s",vector=T)
AddQuantity(name="Us",unit="m/s",vector=T)

AddSetting(name="Viscosity", default=0.16666666, comment='fluid viscosity')
AddSetting(name="SlotVisc", default=1, unit="1m2/s", comment='fluid viscosity')
AddSetting(name="VelocityX", default=0, comment='inlet/outlet/init velocity', zonal=T)
AddSetting(name="VelocityY", default=0, comment='inlet/outlet/init velocity', zonal=T)
AddSetting(name="Pressure", default=0, comment='inlet/outlet/init density', zonal=T)
AddSetting(name="VolumeFraction", default=0, comment='solid phase volume fraction', zonal=T)



AddSetting(name="SatVolumeFraction",unit="1")
AddSetting(name="Width",unit="1m")
AddSetting(name="PartDiam",unit="1m")
AddSetting(name="Friction",unit="1/s")
AddSetting(name="Rho",unit="1kg/m3")
AddSetting(name="Rho_s",unit="1kg/m3")

AddSetting(name="AccelX", default=0, unit="1m/s2")
AddSetting(name="AccelY", default=0, unit="1m/s2")
AddSetting(name="NablaPX", default=0, unit="1Pa/m")
AddSetting(name="NablaPY", default=0, unit="1Pa/m")

AddNodeType(name="WPressure", group="BOUNDARY")
AddNodeType(name="EPressure", group="BOUNDARY")
AddNodeType(name="WVelocity", group="BOUNDARY")
AddNodeType(name="EVelocity", group="BOUNDARY")

AddNodeType(name="Wall", group="COLLISION")
AddNodeType(name="MRT",  group="COLLISION")

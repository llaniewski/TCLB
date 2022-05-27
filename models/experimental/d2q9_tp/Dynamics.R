# Density - table of variables of LB Node to stream
#  name - variable name to stream
#  dx,dy,dz - direction of streaming
#  comment - additional comment

source("lib/lattice.R")
U = d2q9

AddDensity( name=paste0('fa',1:9-1), dx= U[,1], dy= U[,2], group="fa")
AddDensity( name=paste0('fb',1:9-1), dx= U[,1], dy= U[,2], group="fb")

AddField( name="phi", dx= c(-1,1), dy= c(-1,1), group="PF")

# Quantities - table of fields that can be exported from the LB lattice (like density, velocity etc)
#  name - name of the field
#  type - C type of the field, "real_t" - for single/double float, and "vector_t" for 3D vector single/double float
# Every field must correspond to a function in "Dynamics.c".
# If one have filed [something] with type [type], one have to define a function:
# [type] get[something]() { return ...; }

AddQuantity(name="Rho",unit="kg/m3")
AddQuantity(name="U",unit="m/s",vector=T)
AddQuantity(name="Phi",unit="1")
AddQuantity(name="GradPhi",unit="1/m",vector=T)

# Settings - table of settings (constants) that are taken from a .xml file
#  name - name of the constant variable
#  comment - additional comment
# You can state that another setting is 'derived' from this one stating for example: RelaxationRate='1.0/(3*Viscosity + 0.5)'

AddSetting(name="Viscosity", default=0.16666666, comment='viscosity')
AddSetting(name="VelocityX", default=0, comment='inlet/outlet/init velocity', zonal=T)
AddSetting(name="VelocityY", default=0, comment='inlet/outlet/init velocity', zonal=T)
AddSetting(name="Pressure", default=0, comment='inlet/outlet/init density', zonal=T)
AddSetting(name="PhaseF", default=0, comment='inlet/outlet/init phase field', zonal=T)
AddSetting(name="W", default=0, comment='interface width', zonal=T)

AddSetting(name="GravitationX")
AddSetting(name="GravitationY")
# Globals - table of global integrals that can be monitored and optimized

AddGlobal(name="PressureLoss", comment='pressure loss', unit="1mPa")
AddGlobal(name="OutletFlux", comment='pressure loss', unit="1m2/s")
AddGlobal(name="InletFlux", comment='pressure loss', unit="1m2/s")

#Node types for boundaries
AddNodeType(name="EPressure", group="BOUNDARY")
AddNodeType(name="WPressure", group="BOUNDARY")

AddNodeType(name="NVelocity", group="BOUNDARY")
AddNodeType(name="SVelocity", group="BOUNDARY")
AddNodeType(name="WVelocity", group="BOUNDARY")
AddNodeType(name="EVelocity", group="BOUNDARY")

AddNodeType(name="Inlet", group="OBJECTIVE")
AddNodeType(name="Outlet", group="OBJECTIVE")
AddNodeType(name="Solid", group="BOUNDARY")
AddNodeType(name="Wall", group="BOUNDARY")
AddNodeType(name="MRT", group="COLLISION")



	AddStage("BaseInit" , "Init",    save=!Fields$group %in% c("PF"), load=FALSE)
	AddStage("BaseIter" , "Run",     save=!Fields$group %in% c("PF"), load=TRUE)
	AddStage("CalcPhi"  , "CalcPhi", save= Fields$group %in% c("PF"), load=TRUE)
    AddAction("Init"     , c("BaseInit", "CalcPhi"))
    AddAction("Iteration", c("BaseIter", "CalcPhi"))



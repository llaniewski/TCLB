# Density - table of variables of LB Node to stream
#  name - variable name to stream
#  dx,dy,dz - direction of streaming
#  comment - additional comment

AddDensity( name="f[0]", dx= 0, dy= 0, group="f",shift=4/9)
AddDensity( name="f[1]", dx= 1, dy= 0, group="f",shift=2/18)
AddDensity( name="f[2]", dx= 0, dy= 1, group="f",shift=2/18)
AddDensity( name="f[3]", dx=-1, dy= 0, group="f",shift=2/18)
AddDensity( name="f[4]", dx= 0, dy=-1, group="f",shift=2/18)
AddDensity( name="f[5]", dx= 1, dy= 1, group="f",shift=1/36)
AddDensity( name="f[6]", dx=-1, dy= 1, group="f",shift=1/36)
AddDensity( name="f[7]", dx=-1, dy=-1, group="f",shift=1/36)
AddDensity( name="f[8]", dx= 1, dy=-1, group="f",shift=1/36)

AddField( name="UX", dy=c(-1,1))
AddField( name="UY", dx=c(-1,1))


# Quantities - table of fields that can be exported from the LB lattice (like density, velocity etc)
#  name - name of the field
#  type - C type of the field, "real_t" - for single/double float, and "vector_t" for 3D vector single/double float
# Every field must correspond to a function in "Dynamics.c".
# If one have filed [something] with type [type], one have to define a function: 
# [type] get[something]() { return ...; }

AddQuantity(name="Rho",unit="kg/m3")
AddQuantity(name="U",unit="m/s",vector=T)



# Settings - table of settings (constants) that are taken from a .xml file
#  name - name of the constant variable
#  comment - additional comment
# You can state that another setting is 'derived' from this one stating for example: RelaxationRate='1.0/(3*Viscosity + 0.5)'

AddSetting(
           name="RelaxationRate", 
           S2='1-RelaxationRate',       
           comment='one over relaxation time'
            )
AddSetting(name="Viscosity", RelaxationRate='1.0/(3*Viscosity + 0.5)', default=0.16666666, comment='viscosity')
AddSetting(name="VelocityX", default=0, comment='inlet/outlet/init velocity', zonal=T)
AddSetting(name="VelocityY", default=0, comment='inlet/outlet/init velocity', zonal=T)
AddSetting(name="Pressure", default=0, comment='inlet/outlet/init density', zonal=T)

AddSetting(name="S2", default="0", comment='MRT Sx')

AddNodeType(name="EPressure", group="BOUNDARY")
AddNodeType(name="WPressure", group="BOUNDARY")

AddNodeType(name="NVelocity", group="BOUNDARY")
AddNodeType(name="SVelocity", group="BOUNDARY")
AddNodeType(name="WVelocity", group="BOUNDARY")
AddNodeType(name="EVelocity", group="BOUNDARY")

AddNodeType(name="NSymmetry", group="BOUNDARY")
AddNodeType(name="SSymmetry", group="BOUNDARY")

AddNodeType(name="Inlet", group="OBJECTIVE")
AddNodeType(name="Outlet", group="OBJECTIVE")
AddNodeType(name="Solid", group="BOUNDARY")
AddNodeType(name="Wall", group="BOUNDARY")
AddNodeType(name="MRT", group="COLLISION")

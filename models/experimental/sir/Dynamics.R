
AddDensity( name="S" )  ##  S
AddDensity( name="I" )  ##    I
AddDensity( name="R" )  ##      R - model

AddDensity( name="W0", dx= 0, dy= 0, group="W")
AddDensity( name="W1", dx= 1, dy= 0, group="W")
AddDensity( name="W2", dx= 0, dy= 1, group="W")
AddDensity( name="W3", dx=-1, dy= 0, group="W")
AddDensity( name="W4", dx= 0, dy=-1, group="W")
AddDensity( name="W5", dx= 1, dy= 1, group="W")
AddDensity( name="W6", dx=-1, dy= 1, group="W")
AddDensity( name="W7", dx=-1, dy=-1, group="W")
AddDensity( name="W8", dx= 1, dy=-1, group="W")

AddQuantity( name="Suspect" )
AddQuantity( name="Infected" )
AddQuantity( name="Recovered" )
AddQuantity( name="Spread" )

AddSetting(name="InfectionRate", comment='Infection rate', unit="1/s")
AddSetting(name="SpreadRate", comment='Spread relaxation rate', unit="1/s")
AddSetting(name="SpreadRadius", comment='Spread radius', unit = "m")
AddSetting(name="RecoveryRate", comment='Recovery rate', unit="1/s")
AddSetting(name="MinimalViralLoad", comment='Minimal viral load', unit="1")

AddSetting(name="InitInfected", comment='Initial Infected', unit="1", zonal=TRUE)
AddSetting(name="Immunisation", comment='Percent of people immunised', unit="1", zonal=TRUE)

AddGlobal(name="TotalSuspect")
AddGlobal(name="TotelInfected")
AddGlobal(name="TotalRecovered")
AddGlobal(name="Total")

AddNodeType(name="MRT", group="COLLISION")
AddNodeType(name="WallD", group="COLLISION")
AddNodeType(name="WallN", group="COLLISION")


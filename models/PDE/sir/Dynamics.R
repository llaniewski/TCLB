
AddDensity(name="N")
AddDensity(name="I")
AddDensity(name="fI")
AddDensity(name="R")
AddField(name="fI", dx=c(-1,1), dy=c(-1,1)) 

# 	Outputs:
AddQuantity(name="Infected")
AddQuantity(name="Recovered")

AddSetting(name="Beta", zonal=TRUE)
AddSetting(name="Alpha", zonal=TRUE)
AddSetting(name="Population", zonal=TRUE)
AddSetting(name="Sick", zonal=TRUE)

AddGlobal(name="TotalPopulation")
AddGlobal(name="TotalRecovered")
AddGlobal(name="TotalSick")

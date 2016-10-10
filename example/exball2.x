<?xml version="1.0"?>
<CLBConfig version="2.0" output="output/">
        <Geometry nx="1024" ny="100">
		<MRT><Box/></MRT>
		<WVelocity name="Inlet"><Inlet/></WVelocity>
		<EPressure name="Outlet"><Outlet/></EPressure>
		<Inlet nx='1' dx='5'><Box/></Inlet>
		<Outlet nx='1' dx='-5'><Box/></Outlet>
		<Wall mask="ALL">
			<Channel/>
			<Wedge dx="120" nx="20" dy="50" ny="20" direction="LowerRight"/>
			<Wedge dx="120" nx="20" dy="30" ny="20" direction="UpperRight"/>
			<Wedge dx="140" nx="20" dy="50" ny="20" direction="LowerLeft"/>
			<Wedge dx="140" nx="20" dy="30" ny="20" direction="UpperLeft"/>
		</Wall>
	</Geometry>
	<Model>
		<Params Velocity="0.01"/>
		<Params nu="0.02"/>
		<Params InletTemperature="1"
			InitTemperature="0"
			FluidAlfa="0.01"
			SolidAlfa="0.166"
			Coriolis="0.001"
			Tension="0.01"
			EOSScale="0.05"
			PressDiffInObj="1"
			Smag="0.16"
		/>
	</Model>
	<VTK Iterations="1000"/>
	<Solve Iterations="10000"/>
</CLBConfig>

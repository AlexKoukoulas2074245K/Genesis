dtAccum = 0.0
function Update(dt)
	dtAccum = dtAccum + dt;
	print(dtAccum)
	monkeyEntity = FindEntity("monkey")
	x,y,z = GetEntityPosition(monkeyEntity)
	SetEntityPosition(monkeyEntity, math.sin(dtAccum), 0.0, math.cos(dtAccum))
	
end

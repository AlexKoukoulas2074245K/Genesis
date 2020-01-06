-- Native functions exported:
-- CreateEntity([optional]entityName) -> entityId
-- FindEntity(entityName) -> entityId
-- DestroyEntity(entityId) -> void
-- GetEntityPosition(entityId) -> x,y,z
-- SetEntityPosition(entityId, x, y, z) -> void

dtAccum = 0.0
function Update(dt)
	dtAccum = dtAccum + dt;	
	monkeyEntity = FindEntity("monkey")
	x,y,z = GetEntityPosition(monkeyEntity)
	SetEntityPosition(monkeyEntity, math.sin(dtAccum), 0.0, math.cos(dtAccum))
	
end

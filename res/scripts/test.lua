-- Native functions exported:
-- CreateEntity([optional]entityName) -> entityId
-- FindEntity(entityName) -> entityId
-- DestroyEntity(entityId) -> void
-- GetEntityPosition(entityId) -> x,y,z
-- SetEntityPosition(entityId, x, y, z) -> void

function Update(entityId, dt, dtAccum)	
	x,y,z = GetEntityPosition(entityId)
	SetEntityPosition(entityId, math.sin(dtAccum), 0.0, math.cos(dtAccum))
	
end

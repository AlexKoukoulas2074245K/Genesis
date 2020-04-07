-- Native functions exported:
-- CreateEntity([optional]entityName) -> entityId
-- FindEntity(entityName) -> entityId
-- DestroyEntity(entityId) -> void
-- GetEntityPosition(entityId) -> x,y,z
-- SetEntityPosition(entityId, x, y, z) -> void
-- GetEntityRotation(entityId) -> rx,ry,rz
-- SetEntityRotation(entityId, rx, ry, rz) -> void

function Update(entityId, dt, dtAccum)
	rotX, rotY, rotZ = GetEntityRotation(entityId)
	SetEntityRotation(entityId, rotX, rotY + dt, rotZ)
end

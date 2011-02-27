function InitMap(environment)
	Spawn_1 = Spawn(1)
	Spawn_1:SetPosX(62)
	Spawn_1:SetPosY(2)
	Spawn_1:SetPosZ(59.5)
	Spawn_1:SetName("spawning1")
	environment:AddSpawn(Spawn_1)

	MapObject = ActorObjectInfo(1)
	MapObject:SetRenderType(1)
	MapObject.DisplayDesc.ModelName = "Worlds/Lba1Original/Grids/Map58.osgb"
	MapObject.DisplayDesc.UseLight = true
	MapObject.DisplayDesc.CastShadow = false
	MapObject:SetModelState(1)
	MapObject.PhysicDesc.Pos.X = 0
	MapObject.PhysicDesc.Pos.Y = 0
	MapObject.PhysicDesc.Pos.Z = 0
	MapObject.PhysicDesc.Pos.Rotation = 0
	MapObject:SetPhysicalActorType(1)
	MapObject:SetPhysicalShape(5)
	MapObject.PhysicDesc.Collidable = true
	MapObject.PhysicDesc.Filename = "Worlds/Lba1Original/Grids/Map58.phy"
	MapObjectH = ActorHandler(MapObject)
	environment:AddActorObject(MapObjectH)

	Trigger_1_info = TriggerInfo(1, "exit1", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 1, 5.5, 3, true)
	Trigger_1:SetPosition(63.5, 2, 59.5)
	Trigger_1_act1 = TeleportAction()
	Trigger_1_act1:SetMapName("Map013_1")
	Trigger_1_act1:SetSpawning(4)
	Trigger_1:SetAction1(Trigger_1_act1)
	environment:AddTrigger(Trigger_1)

end


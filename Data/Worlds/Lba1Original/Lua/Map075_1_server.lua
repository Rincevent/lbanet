function InitMap(environment)
	Spawn_1 = Spawn(1)
	Spawn_1:SetPosX(50)
	Spawn_1:SetPosY(3)
	Spawn_1:SetPosZ(61)
	Spawn_1:SetName("spawning1")
	environment:AddSpawn(Spawn_1)

	Spawn_2 = Spawn(2)
	Spawn_2:SetPosX(46)
	Spawn_2:SetPosY(16)
	Spawn_2:SetPosZ(30)
	Spawn_2:SetName("spawning2")
	environment:AddSpawn(Spawn_2)

	MapObject = ActorObjectInfo(1)
	MapObject:SetRenderType(1)
	MapObject.DisplayDesc.ModelName = "Worlds/Lba1Original/Grids/Map75.osgb"
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
	MapObject.PhysicDesc.Filename = "Worlds/Lba1Original/Grids/Map75.phy"
	MapObjectH = ActorHandler(MapObject)
	environment:AddActorObject(MapObjectH)

	Trigger_1_info = TriggerInfo(1, "exit0", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 2, 3.5, 1, true)
	Trigger_1:SetPosition(50, 3, 63.5)
	Trigger_1_act1 = TeleportAction()
	Trigger_1_act1:SetMapName("Map077_1")
	Trigger_1_act1:SetSpawning(1)
	Trigger_1:SetAction1(Trigger_1_act1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "exit2", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 1, 3.5, 4, true)
	Trigger_2:SetPosition(44.5, 16, 30)
	Trigger_2_act1 = TeleportAction()
	Trigger_2_act1:SetMapName("Map079_1")
	Trigger_2_act1:SetSpawning(1)
	Trigger_2:SetAction1(Trigger_2_act1)
	environment:AddTrigger(Trigger_2)

end


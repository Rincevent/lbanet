function InitMap(environment)
	Spawn_1 = Spawn(1)
	Spawn_1:SetPosX(37)
	Spawn_1:SetPosY(0)
	Spawn_1:SetPosZ(51.5)
	Spawn_1:SetName("spawning1")
	environment:AddSpawn(Spawn_1)

	Spawn_2 = Spawn(2)
	Spawn_2:SetPosX(62)
	Spawn_2:SetPosY(6)
	Spawn_2:SetPosZ(20)
	Spawn_2:SetName("spawning2")
	environment:AddSpawn(Spawn_2)

	Spawn_3 = Spawn(3)
	Spawn_3:SetPosX(62)
	Spawn_3:SetPosY(0)
	Spawn_3:SetPosZ(42.5)
	Spawn_3:SetName("spawning3")
	environment:AddSpawn(Spawn_3)

	MapObject = ActorObjectInfo(1)
	MapObject:SetRenderType(1)
	MapObject.DisplayDesc.ModelName = "Worlds/Lba1Original/Grids/Map68.osgb"
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
	MapObject.PhysicDesc.Filename = "Worlds/Lba1Original/Grids/Map68.phy"
	MapObjectH = ActorHandler(MapObject)
	environment:AddActorObject(MapObjectH)

	Trigger_1_info = TriggerInfo(1, "exit0", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 1, 3.5, 5, true)
	Trigger_1:SetPosition(63.5, 6, 20.5)
	Trigger_1_act1 = TeleportAction()
	Trigger_1_act1:SetMapName("Map067_1")
	Trigger_1_act1:SetSpawning(1)
	Trigger_1:SetAction1(Trigger_1_act1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "exit1", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 1, 3.5, 9, true)
	Trigger_2:SetPosition(63.5, 0, 42.5)
	Trigger_2_act1 = TeleportAction()
	Trigger_2_act1:SetMapName("Map067_1")
	Trigger_2_act1:SetSpawning(2)
	Trigger_2:SetAction1(Trigger_2_act1)
	environment:AddTrigger(Trigger_2)

	Trigger_3_info = TriggerInfo(3, "exit14", true, true, false)
	Trigger_3 = ZoneTrigger(Trigger_3_info, 1, 3.5, 1, true)
	Trigger_3:SetPosition(39.5, 3, 63.5)
	Trigger_3_act1 = TeleportAction()
	Trigger_3_act1:SetMapName("Map015_1")
	Trigger_3_act1:SetSpawning(1)
	Trigger_3:SetAction1(Trigger_3_act1)
	environment:AddTrigger(Trigger_3)

end


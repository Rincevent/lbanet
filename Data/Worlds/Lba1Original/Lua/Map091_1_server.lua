function InitMap(environment)
	Spawn_1 = Spawn(1)
	Spawn_1:SetPosX(2)
	Spawn_1:SetPosY(2)
	Spawn_1:SetPosZ(59)
	Spawn_1:SetName("spawning1")
	environment:AddSpawn(Spawn_1)

	Spawn_2 = Spawn(2)
	Spawn_2:SetPosX(35)
	Spawn_2:SetPosY(2)
	Spawn_2:SetPosZ(41)
	Spawn_2:SetName("spawning2")
	environment:AddSpawn(Spawn_2)

	Spawn_3 = Spawn(3)
	Spawn_3:SetPosX(15)
	Spawn_3:SetPosY(2)
	Spawn_3:SetPosZ(51)
	Spawn_3:SetName("spawning3")
	environment:AddSpawn(Spawn_3)

	Spawn_4 = Spawn(4)
	Spawn_4:SetPosX(18)
	Spawn_4:SetPosY(3)
	Spawn_4:SetPosZ(61)
	Spawn_4:SetName("spawning4")
	environment:AddSpawn(Spawn_4)

	Spawn_5 = Spawn(5)
	Spawn_5:SetPosX(14)
	Spawn_5:SetPosY(2)
	Spawn_5:SetPosZ(51)
	Spawn_5:SetName("spawning5")
	environment:AddSpawn(Spawn_5)

	Spawn_6 = Spawn(6)
	Spawn_6:SetPosX(34)
	Spawn_6:SetPosY(2)
	Spawn_6:SetPosZ(42)
	Spawn_6:SetName("spawning6")
	environment:AddSpawn(Spawn_6)

	Spawn_7 = Spawn(7)
	Spawn_7:SetPosX(14)
	Spawn_7:SetPosY(2)
	Spawn_7:SetPosZ(51)
	Spawn_7:SetName("spawning7")
	environment:AddSpawn(Spawn_7)

	MapObject = ActorObjectInfo(1)
	MapObject:SetRenderType(1)
	MapObject.DisplayDesc.ModelName = "Worlds/Lba1Original/Grids/Map91.osgb"
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
	MapObject.PhysicDesc.Filename = "Worlds/Lba1Original/Grids/Map91.phy"
	MapObjectH = ActorHandler(MapObject)
	environment:AddActorObject(MapObjectH)

	Trigger_1_info = TriggerInfo(1, "exit0", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 2, 4, 1, true)
	Trigger_1:SetPosition(18, 3, 62.5)
	Trigger_1_act1 = TeleportAction()
	Trigger_1_act1:SetMapName("Map081_1")
	Trigger_1_act1:SetSpawning(2)
	Trigger_1:SetAction1(Trigger_1_act1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "exit1", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 1, 4, 10, true)
	Trigger_2:SetPosition(0.5, 2, 59)
	Trigger_2_act1 = TeleportAction()
	Trigger_2_act1:SetMapName("Map073_1")
	Trigger_2_act1:SetSpawning(1)
	Trigger_2:SetAction1(Trigger_2_act1)
	environment:AddTrigger(Trigger_2)

	Trigger_3_info = TriggerInfo(3, "exit10", true, true, false)
	Trigger_3 = ZoneTrigger(Trigger_3_info, 2, 3, 2, true)
	Trigger_3:SetPosition(5, 3, 1)
	Trigger_3_act1 = TeleportAction()
	Trigger_3_act1:SetMapName("Map100_1")
	Trigger_3_act1:SetSpawning(3)
	Trigger_3:SetAction1(Trigger_3_act1)
	environment:AddTrigger(Trigger_3)

	Trigger_4_info = TriggerInfo(4, "exit6", true, true, false)
	Trigger_4 = ZoneTrigger(Trigger_4_info, 2, 3, 2, true)
	Trigger_4:SetPosition(63, 3, 1)
	Trigger_4_act1 = TeleportAction()
	Trigger_4_act1:SetMapName("Map097_1")
	Trigger_4_act1:SetSpawning(4)
	Trigger_4:SetAction1(Trigger_4_act1)
	environment:AddTrigger(Trigger_4)

	Trigger_5_info = TriggerInfo(5, "exit7", true, true, false)
	Trigger_5 = ZoneTrigger(Trigger_5_info, 2, 3, 2, true)
	Trigger_5:SetPosition(61, 3, 1)
	Trigger_5_act1 = TeleportAction()
	Trigger_5_act1:SetMapName("Map074_1")
	Trigger_5_act1:SetSpawning(4)
	Trigger_5:SetAction1(Trigger_5_act1)
	environment:AddTrigger(Trigger_5)

	Trigger_6_info = TriggerInfo(6, "exit8", true, true, false)
	Trigger_6 = ZoneTrigger(Trigger_6_info, 2, 3, 2, true)
	Trigger_6:SetPosition(1, 3, 1)
	Trigger_6_act1 = TeleportAction()
	Trigger_6_act1:SetMapName("Map097_1")
	Trigger_6_act1:SetSpawning(5)
	Trigger_6:SetAction1(Trigger_6_act1)
	environment:AddTrigger(Trigger_6)

	Trigger_7_info = TriggerInfo(7, "exit9", true, true, false)
	Trigger_7 = ZoneTrigger(Trigger_7_info, 2, 3, 2, true)
	Trigger_7:SetPosition(3, 3, 1)
	Trigger_7_act1 = TeleportAction()
	Trigger_7_act1:SetMapName("Map078_1")
	Trigger_7_act1:SetSpawning(2)
	Trigger_7:SetAction1(Trigger_7_act1)
	environment:AddTrigger(Trigger_7)

end


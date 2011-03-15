function InitMap(environment)
	Spawn_1 = Spawn(1)
	Spawn_1:SetPosX(5)
	Spawn_1:SetPosY(1)
	Spawn_1:SetPosZ(8)
	Spawn_1:SetName("from_baldinocell")
	environment:AddSpawn(Spawn_1)

	Spawn_2 = Spawn(2)
	Spawn_2:SetPosX(11)
	Spawn_2:SetPosY(1)
	Spawn_2:SetPosZ(4)
	Spawn_2:SetName("from_tech")
	environment:AddSpawn(Spawn_2)

	Spawn_3 = Spawn(3)
	Spawn_3:SetPosX(17)
	Spawn_3:SetPosY(1)
	Spawn_3:SetPosZ(35)
	Spawn_3:SetName("from_circle")
	environment:AddSpawn(Spawn_3)

	Spawn_4 = Spawn(4)
	Spawn_4:SetPosX(47)
	Spawn_4:SetPosY(1)
	Spawn_4:SetPosZ(5)
	Spawn_4:SetName("from_triangle")
	environment:AddSpawn(Spawn_4)

	Actor_1 = ActorObjectInfo(1)
	Actor_1:SetRenderType(1)
	Actor_1.DisplayDesc.ModelId = 750
	Actor_1.DisplayDesc.ModelName = "Worlds/Lba2Original/Grids/Map14.osgb"
	Actor_1.DisplayDesc.Outfit = ""
	Actor_1.DisplayDesc.Weapon = ""
	Actor_1.DisplayDesc.Mode = ""
	Actor_1.DisplayDesc.UseLight = true
	Actor_1.DisplayDesc.CastShadow = false
	Actor_1.DisplayDesc.ColorR = 1
	Actor_1.DisplayDesc.ColorG = 1
	Actor_1.DisplayDesc.ColorB = 2.10195e-044
	Actor_1.DisplayDesc.ColorA = 5.03608e-024
	Actor_1.DisplayDesc.TransX = 0
	Actor_1.DisplayDesc.TransY = 0
	Actor_1.DisplayDesc.TransZ = 0
	Actor_1.DisplayDesc.ScaleX = 1
	Actor_1.DisplayDesc.ScaleY = 1
	Actor_1.DisplayDesc.ScaleZ = 1
	Actor_1.DisplayDesc.RotX = 0
	Actor_1.DisplayDesc.RotY = 0
	Actor_1.DisplayDesc.RotZ = 0
	Actor_1:SetModelState(1)
	Actor_1.PhysicDesc.Pos.X = 0
	Actor_1.PhysicDesc.Pos.Y = 0
	Actor_1.PhysicDesc.Pos.Z = 0
	Actor_1.PhysicDesc.Pos.Rotation = 0
	Actor_1.PhysicDesc.Density = 0
	Actor_1.PhysicDesc.Collidable = true
	Actor_1.PhysicDesc.SizeX = 0
	Actor_1.PhysicDesc.SizeY = 0
	Actor_1.PhysicDesc.SizeZ = 0
	Actor_1.PhysicDesc.Filename = "Worlds/Lba2Original/Grids/Map14.phy"
	Actor_1:SetPhysicalActorType(1)
	Actor_1:SetPhysicalShape(5)
	Actor_1.ExtraInfo.Name = ""
	Actor_1.ExtraInfo.NameColorR = 0
	Actor_1.ExtraInfo.NameColorG = 7.64534e-031
	Actor_1.ExtraInfo.NameColorB = 0
	Actor_1H = ActorHandler(Actor_1)
	environment:AddActorObject(Actor_1H)

	Trigger_1_info = TriggerInfo(1, "to_tech", true, true, false)
	Trigger_1 = ZoneTrigger(Trigger_1_info, 4, 8, 1, true)
	Trigger_1:SetPosition(11, 1, 0.6)
	Trigger_1_act1 = TeleportAction()
	Trigger_1_act1:SetMapName("Map024_2")
	Trigger_1_act1:SetSpawning(3)
	Trigger_1:SetAction1(Trigger_1_act1)
	environment:AddTrigger(Trigger_1)

	Trigger_2_info = TriggerInfo(2, "to_baldinocell", true, true, false)
	Trigger_2 = ZoneTrigger(Trigger_2_info, 1, 8, 2, true)
	Trigger_2:SetPosition(1.5, 1, 8)
	Trigger_2_act1 = TeleportAction()
	Trigger_2_act1:SetMapName("Map046_2")
	Trigger_2_act1:SetSpawning(1)
	Trigger_2:SetAction1(Trigger_2_act1)
	environment:AddTrigger(Trigger_2)

	Trigger_4_info = TriggerInfo(4, "to_circle", true, true, false)
	Trigger_4 = ZoneTrigger(Trigger_4_info, 1, 8, 2, true)
	Trigger_4:SetPosition(19.5, 1, 35)
	Trigger_4_act1 = TeleportAction()
	Trigger_4_act1:SetMapName("Map044_2")
	Trigger_4_act1:SetSpawning(2)
	Trigger_4:SetAction1(Trigger_4_act1)
	environment:AddTrigger(Trigger_4)

	Trigger_5_info = TriggerInfo(5, "to_triangle", true, true, false)
	Trigger_5 = ZoneTrigger(Trigger_5_info, 1, 8, 2, true)
	Trigger_5:SetPosition(49.5, 1, 5)
	Trigger_5_act1 = TeleportAction()
	Trigger_5_act1:SetMapName("Map112_2")
	Trigger_5_act1:SetSpawning(1)
	Trigger_5:SetAction1(Trigger_5_act1)
	environment:AddTrigger(Trigger_5)

end

